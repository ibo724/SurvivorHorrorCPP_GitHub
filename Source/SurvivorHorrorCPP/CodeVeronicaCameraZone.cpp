// Copyright Epic Games, Inc. All Rights Reserved.

#include "CodeVeronicaCameraZone.h"

#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "SurvivorHorrorPlayerController.h"

ACodeVeronicaCameraZone::ACodeVeronicaCameraZone()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.TickGroup = TG_PostPhysics;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	TriggerVolume->SetupAttachment(SceneRoot);
	TriggerVolume->SetBoxExtent(FVector(500.0f, 500.0f, 200.0f));
	TriggerVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerVolume->SetCollisionObjectType(ECC_WorldDynamic);
	TriggerVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerVolume->SetGenerateOverlapEvents(true);
	TriggerVolume->SetCanEverAffectNavigation(false);

	CameraRail = CreateDefaultSubobject<USplineComponent>(TEXT("CameraRail"));
	CameraRail->SetupAttachment(SceneRoot);
	CameraRail->SetClosedLoop(false);
	CameraRail->ClearSplinePoints(false);
	CameraRail->AddSplinePoint(FVector(-500.0f, -700.0f, 300.0f), ESplineCoordinateSpace::Local, false);
	CameraRail->AddSplinePoint(FVector(500.0f, -700.0f, 300.0f), ESplineCoordinateSpace::Local, false);
	CameraRail->SetSplinePointType(0, ESplinePointType::Linear, false);
	CameraRail->SetSplinePointType(1, ESplinePointType::Linear, false);
	CameraRail->UpdateSpline();

	TrackingCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TrackingCamera"));
	TrackingCamera->SetupAttachment(SceneRoot);
	TrackingCamera->SetMobility(EComponentMobility::Movable);
	TrackingCamera->bUsePawnControlRotation = false;
}

void ACodeVeronicaCameraZone::BeginPlay()
{
	Super::BeginPlay();

	TriggerVolume->OnComponentBeginOverlap.AddDynamic(
		this, &ACodeVeronicaCameraZone::HandleTriggerBeginOverlap);

	// Begin-overlap normally handles this. The explicit query also covers a
	// Player Start that was placed inside the volume before play begins.
	TArray<AActor*> OverlappingPawns;
	TriggerVolume->GetOverlappingActors(OverlappingPawns, APawn::StaticClass());
	for (AActor* OverlappingActor : OverlappingPawns)
	{
		if (APawn* PlayerPawn = Cast<APawn>(OverlappingActor))
		{
			TryActivateForPawn(PlayerPawn);
		}
	}
}

void ACodeVeronicaCameraZone::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	TrackingCamera->SetFieldOfView(CameraFieldOfView);

	if (CameraRail->GetNumberOfSplinePoints() > 0)
	{
		const FVector PreviewLocation = CameraRail->GetLocationAtSplinePoint(
			0, ESplineCoordinateSpace::World);
		const FVector PreviewTarget = GetActorLocation() + LookAtOffset;

		TrackingCamera->SetWorldLocation(PreviewLocation);
		TrackingCamera->SetWorldRotation((PreviewTarget - PreviewLocation).Rotation());
	}
}

void ACodeVeronicaCameraZone::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UpdateCameraTransform(DeltaSeconds, false);
}

void ACodeVeronicaCameraZone::CalcCamera(const float DeltaTime, FMinimalViewInfo& OutResult)
{
	TrackingCamera->GetCameraView(DeltaTime, OutResult);
}

void ACodeVeronicaCameraZone::StartTracking(APawn* PlayerPawn)
{
	if (!IsValid(PlayerPawn))
	{
		return;
	}

	TrackedPawn = PlayerPawn;
	TrackingCamera->SetFieldOfView(CameraFieldOfView);
	UpdateCameraTransform(0.0f, true);
	SetActorTickEnabled(true);
}

void ACodeVeronicaCameraZone::StopTracking()
{
	SetActorTickEnabled(false);
	TrackedPawn = nullptr;
}

void ACodeVeronicaCameraZone::HandleTriggerBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	const int32 OtherBodyIndex,
	const bool bFromSweep,
	const FHitResult& SweepResult)
{
	TryActivateForPawn(Cast<APawn>(OtherActor));
}

void ACodeVeronicaCameraZone::TryActivateForPawn(APawn* PlayerPawn)
{
	if (!IsValid(PlayerPawn) || !PlayerPawn->IsPlayerControlled())
	{
		return;
	}

	if (ASurvivorHorrorPlayerController* PlayerController =
		Cast<ASurvivorHorrorPlayerController>(PlayerPawn->GetController()))
	{
		PlayerController->ActivateCameraZone(this, PlayerPawn);
		return;
	}

	// Safe fallback if a test level overrides the project's PlayerController.
	if (APlayerController* PlayerController = Cast<APlayerController>(PlayerPawn->GetController()))
	{
		StartTracking(PlayerPawn);
		PlayerController->SetViewTargetWithBlend(
			this, CameraTransitionTime, VTBlend_Cubic, 2.0f, true);
	}
}

void ACodeVeronicaCameraZone::UpdateCameraTransform(
	const float DeltaSeconds, const bool bSnapImmediately)
{
	if (!IsValid(TrackedPawn) || CameraRail->GetNumberOfSplinePoints() == 0)
	{
		return;
	}

	const FVector PawnLocation = TrackedPawn->GetActorLocation();
	const FVector DesiredCameraLocation = CameraRail->FindLocationClosestToWorldLocation(
		PawnLocation, ESplineCoordinateSpace::World);

	const FVector NewCameraLocation = bSnapImmediately
		? DesiredCameraLocation
		: FMath::VInterpTo(
			TrackingCamera->GetComponentLocation(),
			DesiredCameraLocation,
			DeltaSeconds,
			PositionLagSpeed);

	const FVector LookAtLocation = PawnLocation + LookAtOffset;
	const FRotator DesiredCameraRotation = (LookAtLocation - NewCameraLocation).Rotation();
	const FRotator NewCameraRotation = bSnapImmediately
		? DesiredCameraRotation
		: FMath::RInterpTo(
			TrackingCamera->GetComponentRotation(),
			DesiredCameraRotation,
			DeltaSeconds,
			RotationLagSpeed);

	TrackingCamera->SetWorldLocationAndRotation(NewCameraLocation, NewCameraRotation);
}
