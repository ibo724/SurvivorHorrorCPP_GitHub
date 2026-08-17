// Copyright Epic Games, Inc. All Rights Reserved.

#include "ListenerEnemy.h"

#include "AIController.h"
#include "Components/CapsuleComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ListenerEnemyController.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "SurvivorHealthComponent.h"
#include "UObject/ConstructorHelpers.h"

AListenerEnemy::AListenerEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.10f;

	AIControllerClass = AListenerEnemyController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	Movement->bOrientRotationToMovement = true;
	Movement->bUseControllerDesiredRotation = false;
	Movement->RotationRate = FRotator(0.0f, 220.0f, 0.0f);
	Movement->MaxWalkSpeed = InvestigateSpeed;
	Movement->MaxAcceleration = 500.0f;
	Movement->BrakingDecelerationWalking = 700.0f;

	HealthComponent = CreateDefaultSubobject<USurvivorHealthComponent>(TEXT("HealthComponent"));

	PreviewBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PreviewBody"));
	PreviewBody->SetupAttachment(GetCapsuleComponent());
	PreviewBody->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PreviewBody->SetGenerateOverlapEvents(false);
	PreviewBody->SetRelativeScale3D(FVector(0.72f, 0.54f, 1.75f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> PreviewMesh(
		TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (PreviewMesh.Succeeded())
	{
		PreviewBody->SetStaticMesh(PreviewMesh.Object);
	}

	StateLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("StateLight"));
	StateLight->SetupAttachment(GetCapsuleComponent());
	StateLight->SetRelativeLocation(FVector(0.0f, 0.0f, 105.0f));
	StateLight->SetIntensity(850.0f);
	StateLight->SetAttenuationRadius(250.0f);
	StateLight->SetCastShadows(false);
}

void AListenerEnemy::BeginPlay()
{
	Super::BeginPlay();
	if (IsValid(HealthComponent))
	{
		HealthComponent->OnDeath.AddDynamic(this, &AListenerEnemy::HandleDeath);
	}
	if (IsValid(PreviewBody))
	{
		PreviewMaterial = PreviewBody->CreateAndSetMaterialInstanceDynamic(0);
	}
	UpdatePreviewForState();
}

void AListenerEnemy::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UpdateState(DeltaSeconds);

	if (bShowDebugState && GetWorld())
	{
		DrawDebugString(
			GetWorld(),
			GetActorLocation() + FVector(0.0f, 0.0f, 135.0f),
			GetDebugStateText(),
			nullptr,
			GetDebugStateColor().ToFColor(true),
			0.0f,
			false,
			1.0f);
	}
}

float AListenerEnemy::TakeDamage(
	const float DamageAmount,
	const FDamageEvent& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser)
{
	const float ParentDamage = Super::TakeDamage(
		DamageAmount,
		DamageEvent,
		EventInstigator,
		DamageCauser);
	return IsValid(HealthComponent)
		? HealthComponent->ApplyDamage(ParentDamage)
		: 0.0f;
}

void AListenerEnemy::HearNoise(
	AActor* NoiseActor,
	const FVector& NoiseLocation,
	const float Loudness)
{
	if (CurrentState == EListenerEnemyState::Dead || !IsValid(NoiseActor))
	{
		return;
	}

	const float EffectiveRange = HearingDistance * FMath::Max(0.0f, Loudness);
	if (FVector::DistSquared(GetActorLocation(), NoiseLocation) > FMath::Square(EffectiveRange))
	{
		return;
	}

	HeardActor = NoiseActor;
	LastHeardLocation = NoiseLocation;
	LastHeardTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
	if (CurrentState != EListenerEnemyState::Chasing
		&& CurrentState != EListenerEnemyState::Attacking)
	{
		StartInvestigating(NoiseLocation);
	}
}

void AListenerEnemy::UpdateState(const float DeltaSeconds)
{
	if (CurrentState == EListenerEnemyState::Dead || !GetWorld())
	{
		return;
	}

	const float Now = GetWorld()->GetTimeSeconds();
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	const bool bClosePlayerDetected =
		IsLivingPlayer(PlayerPawn) && CanDetectPlayerAtCloseRange(PlayerPawn);
	if (bClosePlayerDetected)
	{
		HeardActor = PlayerPawn;
		LastHeardLocation = PlayerPawn->GetActorLocation();
		LastCloseDetectionTime = Now;
		if (CurrentState != EListenerEnemyState::Attacking)
		{
			SetState(EListenerEnemyState::Chasing);
		}
	}

	AAIController* EnemyController = Cast<AAIController>(GetController());
	switch (CurrentState)
	{
	case EListenerEnemyState::Idle:
		break;

	case EListenerEnemyState::Investigating:
		if (FVector::DistSquared2D(GetActorLocation(), LastHeardLocation)
			<= FMath::Square(SearchAcceptanceRadius))
		{
			if (IsValid(EnemyController))
			{
				EnemyController->StopMovement();
			}
			SearchEndTime = Now + SearchDuration;
			SetState(EListenerEnemyState::Searching);
		}
		else if (IsValid(EnemyController))
		{
			EnemyController->MoveToLocation(
				LastHeardLocation,
				SearchAcceptanceRadius,
				true,
				true,
				true,
				false);
		}
		break;

	case EListenerEnemyState::Searching:
		AddActorLocalRotation(FRotator(0.0f, SearchTurnRate * DeltaSeconds, 0.0f));
		if (Now >= SearchEndTime)
		{
			SetState(EListenerEnemyState::Idle);
		}
		break;

	case EListenerEnemyState::Chasing:
		if (!IsLivingPlayer(PlayerPawn))
		{
			StartInvestigating(LastHeardLocation);
			break;
		}

		if (bClosePlayerDetected
			&& FVector::DistSquared2D(GetActorLocation(), PlayerPawn->GetActorLocation())
			<= FMath::Square(AttackDistance))
		{
			TryAttack(PlayerPawn);
		}
		else if (FVector::DistSquared2D(GetActorLocation(), PlayerPawn->GetActorLocation())
			> FMath::Square(LoseChaseDistance)
			|| Now - FMath::Max(LastHeardTime, LastCloseDetectionTime) > ChaseMemoryDuration)
		{
			StartInvestigating(LastHeardLocation);
		}
		else if (IsValid(EnemyController))
		{
			// Follow only information the creature actually received. If the player
			// becomes silent behind a corner, it goes to the last sound instead of
			// tracking the pawn through walls.
			EnemyController->MoveToLocation(
				LastHeardLocation,
				bClosePlayerDetected ? AttackDistance * 0.75f : SearchAcceptanceRadius,
				true,
				true,
				true,
				false);
		}
		break;

	case EListenerEnemyState::Attacking:
		if (Now >= AttackStateEndTime)
		{
			SetState(IsLivingPlayer(PlayerPawn)
				? EListenerEnemyState::Chasing
				: EListenerEnemyState::Idle);
		}
		break;

	default:
		break;
	}
}

void AListenerEnemy::StartInvestigating(const FVector& Location)
{
	LastHeardLocation = Location;
	SetState(EListenerEnemyState::Investigating);
}

void AListenerEnemy::TryAttack(APawn* PlayerPawn)
{
	if (!IsLivingPlayer(PlayerPawn) || !GetWorld())
	{
		return;
	}

	const float Now = GetWorld()->GetTimeSeconds();
	if (Now - LastAttackTime < AttackCooldown)
	{
		return;
	}

	LastAttackTime = Now;
	AttackStateEndTime = Now + 0.30f;
	if (AAIController* EnemyController = Cast<AAIController>(GetController()))
	{
		EnemyController->StopMovement();
	}
	SetState(EListenerEnemyState::Attacking);
	UGameplayStatics::ApplyDamage(
		PlayerPawn,
		AttackDamage,
		GetController(),
		this,
		UDamageType::StaticClass());
}

bool AListenerEnemy::CanDetectPlayerAtCloseRange(APawn* PlayerPawn) const
{
	if (!IsValid(PlayerPawn)
		|| FVector::DistSquared2D(GetActorLocation(), PlayerPawn->GetActorLocation())
			> FMath::Square(CloseDetectionDistance))
	{
		return false;
	}

	const AAIController* EnemyController = Cast<AAIController>(GetController());
	return IsValid(EnemyController) && EnemyController->LineOfSightTo(PlayerPawn);
}

bool AListenerEnemy::IsLivingPlayer(const APawn* PlayerPawn) const
{
	if (!IsValid(PlayerPawn) || !PlayerPawn->IsPlayerControlled())
	{
		return false;
	}

	const USurvivorHealthComponent* PlayerHealth =
		PlayerPawn->FindComponentByClass<USurvivorHealthComponent>();
	return !IsValid(PlayerHealth) || !PlayerHealth->IsDead();
}

void AListenerEnemy::SetState(const EListenerEnemyState NewState)
{
	if (CurrentState == NewState)
	{
		return;
	}

	CurrentState = NewState;
	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->MaxWalkSpeed = NewState == EListenerEnemyState::Chasing
			? ChaseSpeed
			: InvestigateSpeed;
	}
	UpdatePreviewForState();
}

void AListenerEnemy::HandleDeath()
{
	if (AAIController* EnemyController = Cast<AAIController>(GetController()))
	{
		EnemyController->StopMovement();
	}
	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetState(EListenerEnemyState::Dead);
}

void AListenerEnemy::UpdatePreviewForState()
{
	const FLinearColor StateColor = GetDebugStateColor();
	if (IsValid(PreviewMaterial))
	{
		PreviewMaterial->SetVectorParameterValue(TEXT("Color"), StateColor);
	}
	if (IsValid(StateLight))
	{
		StateLight->SetLightColor(StateColor);
		StateLight->SetIntensity(CurrentState == EListenerEnemyState::Dead ? 120.0f : 850.0f);
	}
}

FString AListenerEnemy::GetDebugStateText() const
{
	switch (CurrentState)
	{
	case EListenerEnemyState::Investigating:
		return TEXT("DINLIYOR / ARASTIRIYOR");
	case EListenerEnemyState::Searching:
		return TEXT("ARIYOR");
	case EListenerEnemyState::Chasing:
		return TEXT("KOVALIYOR");
	case EListenerEnemyState::Attacking:
		return TEXT("SALDIRIYOR");
	case EListenerEnemyState::Dead:
		return TEXT("ETKISIZ");
	default:
		return TEXT("SESSIZ");
	}
}

FLinearColor AListenerEnemy::GetDebugStateColor() const
{
	switch (CurrentState)
	{
	case EListenerEnemyState::Investigating:
		return FLinearColor(1.0f, 0.72f, 0.10f);
	case EListenerEnemyState::Searching:
		return FLinearColor(0.35f, 0.65f, 1.0f);
	case EListenerEnemyState::Chasing:
		return FLinearColor(1.0f, 0.08f, 0.03f);
	case EListenerEnemyState::Attacking:
		return FLinearColor(1.0f, 1.0f, 1.0f);
	case EListenerEnemyState::Dead:
		return FLinearColor(0.08f, 0.08f, 0.08f);
	default:
		return FLinearColor(0.18f, 0.85f, 0.42f);
	}
}
