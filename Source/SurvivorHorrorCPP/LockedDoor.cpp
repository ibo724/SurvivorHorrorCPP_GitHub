// Copyright Epic Games, Inc. All Rights Reserved.

#include "LockedDoor.h"

#include "Components/PointLightComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "SurvivorHorrorHUD.h"
#include "SurvivorInventoryComponent.h"
#include "SurvivorItemDefinition.h"
#include "UObject/ConstructorHelpers.h"

ALockedDoor::ALockedDoor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	DoorPivot = CreateDefaultSubobject<USceneComponent>(TEXT("DoorPivot"));
	DoorPivot->SetupAttachment(SceneRoot);
	DoorPivot->SetRelativeLocation(FVector(0.0f, -55.0f, 0.0f));

	DoorPanel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorPanel"));
	DoorPanel->SetupAttachment(DoorPivot);
	DoorPanel->SetRelativeLocation(FVector(0.0f, 55.0f, 105.0f));
	DoorPanel->SetRelativeScale3D(FVector(0.10f, 1.10f, 2.10f));
	DoorPanel->SetMobility(EComponentMobility::Movable);
	DoorPanel->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	DoorPanel->SetCollisionObjectType(ECC_WorldDynamic);
	DoorPanel->SetCollisionResponseToAllChannels(ECR_Block);

	LeftFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftFrame"));
	LeftFrame->SetupAttachment(SceneRoot);
	LeftFrame->SetRelativeLocation(FVector(0.0f, -65.0f, 110.0f));
	LeftFrame->SetRelativeScale3D(FVector(0.16f, 0.14f, 2.20f));
	LeftFrame->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	LeftFrame->SetCollisionObjectType(ECC_WorldStatic);
	LeftFrame->SetCollisionResponseToAllChannels(ECR_Block);

	RightFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightFrame"));
	RightFrame->SetupAttachment(SceneRoot);
	RightFrame->SetRelativeLocation(FVector(0.0f, 65.0f, 110.0f));
	RightFrame->SetRelativeScale3D(FVector(0.16f, 0.14f, 2.20f));
	RightFrame->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	RightFrame->SetCollisionObjectType(ECC_WorldStatic);
	RightFrame->SetCollisionResponseToAllChannels(ECR_Block);

	TopFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TopFrame"));
	TopFrame->SetupAttachment(SceneRoot);
	TopFrame->SetRelativeLocation(FVector(0.0f, 0.0f, 226.0f));
	TopFrame->SetRelativeScale3D(FVector(0.16f, 1.44f, 0.14f));
	TopFrame->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	TopFrame->SetCollisionObjectType(ECC_WorldStatic);
	TopFrame->SetCollisionResponseToAllChannels(ECR_Block);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(
		TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		DoorPanel->SetStaticMesh(CubeMesh.Object);
		LeftFrame->SetStaticMesh(CubeMesh.Object);
		RightFrame->SetStaticMesh(CubeMesh.Object);
		TopFrame->SetStaticMesh(CubeMesh.Object);
	}

	FocusLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("FocusLight"));
	FocusLight->SetupAttachment(SceneRoot);
	FocusLight->SetRelativeLocation(FVector(35.0f, 0.0f, 175.0f));
	FocusLight->SetAttenuationRadius(230.0f);
	FocusLight->SetLightColor(FLinearColor(0.22f, 0.46f, 1.0f));
	FocusLight->SetIntensity(0.0f);
}

void ALockedDoor::BeginPlay()
{
	Super::BeginPlay();
	bIsLocked = bStartsLocked;
}

void ALockedDoor::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (!bIsAnimating)
	{
		SetActorTickEnabled(false);
		return;
	}

	AnimationElapsed += DeltaSeconds;
	const float Alpha = FMath::Clamp(
		AnimationElapsed / FMath::Max(0.05f, DoorAnimationDuration),
		0.0f,
		1.0f);
	const float SmoothedAlpha = Alpha * Alpha * (3.0f - 2.0f * Alpha);
	const float NewYaw = FMath::Lerp(AnimationStartYaw, AnimationTargetYaw, SmoothedAlpha);
	DoorPivot->SetRelativeRotation(FRotator(0.0f, NewYaw, 0.0f));

	if (Alpha >= 1.0f)
	{
		bIsAnimating = false;
		bIsOpen = bTargetOpen;
		SetActorTickEnabled(false);
	}
}

bool ALockedDoor::CanInteract_Implementation(APawn* InteractingPawn) const
{
	return IsValid(InteractingPawn) && !bIsAnimating;
}

FText ALockedDoor::GetInteractionPrompt_Implementation(APawn* InteractingPawn) const
{
	if (bIsLocked)
	{
		return LockSymbolDisplayName.IsEmpty()
			? NSLOCTEXT("SurvivorDoor", "InspectLockedDoor", "Kilitli kapıyı incele")
			: FText::Format(
				NSLOCTEXT("SurvivorDoor", "InspectSymbolDoor", "{0} işaretli kapıyı incele"),
				LockSymbolDisplayName);
	}

	return bIsOpen
		? NSLOCTEXT("SurvivorDoor", "CloseDoor", "Kapıyı kapat")
		: NSLOCTEXT("SurvivorDoor", "OpenDoor", "Kapıyı aç");
}

void ALockedDoor::Interact_Implementation(APawn* InteractingPawn)
{
	if (!CanInteract_Implementation(InteractingPawn))
	{
		return;
	}

	USurvivorInventoryComponent* Inventory =
		InteractingPawn->FindComponentByClass<USurvivorInventoryComponent>();
	if (bIsLocked)
	{
		if (IsValid(Inventory))
		{
			Inventory->RememberLockSymbol(LockSymbol);
		}

		if (!IsValid(RequiredKey) || !IsValid(Inventory) || !Inventory->HasItem(RequiredKey))
		{
			const FText LockedMessage = LockSymbolDisplayName.IsEmpty()
				? NSLOCTEXT("SurvivorDoor", "DoorIsLocked", "Kapı kilitli.")
				: FText::Format(
					NSLOCTEXT("SurvivorDoor", "DoorSymbolMemory", "Kapı kilitli. Üzerinde {0} işareti var."),
					LockSymbolDisplayName);
			ShowPlayerMessage(InteractingPawn, LockedMessage);
			return;
		}

		bIsLocked = false;
		if (!HasAnotherLockedDoorForKey())
		{
			Inventory->MarkItemObsolete(RequiredKey);
			ShowPlayerMessage(
				InteractingPawn,
				NSLOCTEXT("SurvivorDoor", "KeyNoLongerNeeded", "Kilidi açtım. Bu anahtara artık ihtiyacım yok."));
		}
		else
		{
			ShowPlayerMessage(
				InteractingPawn,
				NSLOCTEXT("SurvivorDoor", "KeyUnlockedDoor", "Anahtar kilide uydu."));
		}

		UE_LOG(
			LogTemp,
			Log,
			TEXT("Door '%s' unlocked with key '%s'."),
			*GetName(),
			*GetNameSafe(RequiredKey));
		StartDoorAnimation(true, InteractingPawn);
		return;
	}

	StartDoorAnimation(!bIsOpen, InteractingPawn);
}

void ALockedDoor::BeginInteractionFocus_Implementation(APawn* InteractingPawn)
{
	SetFocused(true);
}

void ALockedDoor::EndInteractionFocus_Implementation(APawn* InteractingPawn)
{
	SetFocused(false);
}

void ALockedDoor::StartDoorAnimation(const bool bOpenDoor, const APawn* InteractingPawn)
{
	AnimationElapsed = 0.0f;
	AnimationStartYaw = DoorPivot->GetRelativeRotation().Yaw;
	bTargetOpen = bOpenDoor;

	if (bOpenDoor)
	{
		const FVector ToPawn = IsValid(InteractingPawn)
			? InteractingPawn->GetActorLocation() - GetActorLocation()
			: GetActorForwardVector();
		const float OpenDirection = FVector::DotProduct(GetActorForwardVector(), ToPawn) >= 0.0f
			? -1.0f
			: 1.0f;
		AnimationTargetYaw = OpenAngle * OpenDirection;
	}
	else
	{
		AnimationTargetYaw = 0.0f;
	}

	bIsAnimating = true;
	SetActorTickEnabled(true);
}

void ALockedDoor::SetFocused(const bool bNewFocused)
{
	bIsFocused = bNewFocused;
	FocusLight->SetIntensity(bIsFocused ? 1100.0f : 0.0f);
}

void ALockedDoor::ShowPlayerMessage(APawn* InteractingPawn, const FText& Message) const
{
	if (APlayerController* PlayerController = Cast<APlayerController>(InteractingPawn->GetController()))
	{
		if (ASurvivorHorrorHUD* HUD = Cast<ASurvivorHorrorHUD>(PlayerController->GetHUD()))
		{
			HUD->ShowNotification(Message, 3.0f);
		}
	}
}

bool ALockedDoor::HasAnotherLockedDoorForKey() const
{
	if (!IsValid(RequiredKey) || !GetWorld())
	{
		return false;
	}

	for (TActorIterator<ALockedDoor> DoorIterator(GetWorld()); DoorIterator; ++DoorIterator)
	{
		const ALockedDoor* OtherDoor = *DoorIterator;
		if (OtherDoor != this
			&& IsValid(OtherDoor)
			&& OtherDoor->bIsLocked
			&& OtherDoor->RequiredKey == RequiredKey)
		{
			return true;
		}
	}
	return false;
}
