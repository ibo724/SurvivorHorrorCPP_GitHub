// Copyright Epic Games, Inc. All Rights Reserved.

#include "ClassicTankCharacter.h"

#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "SurvivorInventoryComponent.h"
#include "SurvivorInteractionComponent.h"
#include "UObject/ConstructorHelpers.h"

AClassicTankCharacter::AClassicTankCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->InitCapsuleSize(34.0f, 88.0f);

	// A tank-controlled character owns its facing direction. Movement input must
	// never rotate it automatically toward the direction of travel.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	UCharacterMovementComponent* Movement = GetCharacterMovement();
	Movement->bOrientRotationToMovement = false;
	Movement->bUseControllerDesiredRotation = false;
	Movement->MaxWalkSpeed = ForwardSpeed;
	Movement->MaxAcceleration = 650.0f;
	Movement->BrakingDecelerationWalking = 900.0f;
	Movement->GroundFriction = 8.0f;
	Movement->bCanWalkOffLedgesWhenCrouching = false;

	InteractionComponent = CreateDefaultSubobject<USurvivorInteractionComponent>(
		TEXT("InteractionComponent"));
	InventoryComponent = CreateDefaultSubobject<USurvivorInventoryComponent>(
		TEXT("InventoryComponent"));

	// This engine capsule is only a temporary stand-in. It makes turning and
	// movement visible in a completely blank project and will be removed when a
	// real skeletal character and animations are added.
	PreviewBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PreviewBody"));
	PreviewBody->SetupAttachment(GetCapsuleComponent());
	PreviewBody->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PreviewBody->SetGenerateOverlapEvents(false);
	PreviewBody->SetRelativeScale3D(FVector(0.55f, 0.38f, 1.70f));
	PreviewBody->SetOwnerNoSee(true);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> PreviewMesh(
		TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (PreviewMesh.Succeeded())
	{
		PreviewBody->SetStaticMesh(PreviewMesh.Object);
	}
}

void AClassicTankCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AClassicTankCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AClassicTankCharacter::Turn);
	PlayerInputComponent->BindAction(
		TEXT("Interact"), IE_Pressed, this, &AClassicTankCharacter::Interact);
}

void AClassicTankCharacter::MoveForward(const float AxisValue)
{
	if (FMath::IsNearlyZero(AxisValue))
	{
		return;
	}

	GetCharacterMovement()->MaxWalkSpeed = ForwardSpeed;

	const float DirectionMultiplier = AxisValue < 0.0f ? BackwardSpeedMultiplier : 1.0f;
	AddMovementInput(GetActorForwardVector(), AxisValue * DirectionMultiplier);
}

void AClassicTankCharacter::Turn(const float AxisValue)
{
	if (FMath::IsNearlyZero(AxisValue) || !GetWorld())
	{
		return;
	}

	const float DeltaYaw = AxisValue * TurnRate * GetWorld()->GetDeltaSeconds();
	AddActorLocalRotation(FRotator(0.0f, DeltaYaw, 0.0f));

	// Until the fixed-camera system is added, keep Unreal's temporary default
	// view facing the same direction as the character.
	if (Controller)
	{
		Controller->SetControlRotation(GetActorRotation());
	}
}

void AClassicTankCharacter::Interact()
{
	if (InteractionComponent)
	{
		InteractionComponent->TryInteract();
	}
}
