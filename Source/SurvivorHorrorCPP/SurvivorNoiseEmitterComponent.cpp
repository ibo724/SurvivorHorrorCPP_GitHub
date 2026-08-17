// Copyright Epic Games, Inc. All Rights Reserved.

#include "SurvivorNoiseEmitterComponent.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AISense_Hearing.h"
#include "SurvivorInventoryComponent.h"

USurvivorNoiseEmitterComponent::USurvivorNoiseEmitterComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USurvivorNoiseEmitterComponent::BeginPlay()
{
	Super::BeginPlay();
	InventoryComponent = GetOwner()
		? GetOwner()->FindComponentByClass<USurvivorInventoryComponent>()
		: nullptr;
}

float USurvivorNoiseEmitterComponent::CalculateMovementLoudness(
	const float MovementSpeed,
	const int32 BagNoiseScore,
	const float InMinimumMovementSpeed,
	const float InRunSpeedThreshold,
	const float InWalkLoudness,
	const float InRunLoudness,
	const float InBagScoreMultiplier,
	const float InMaximumLoudness)
{
	if (MovementSpeed < FMath::Max(0.0f, InMinimumMovementSpeed))
	{
		return 0.0f;
	}

	const float MovementLoudness = MovementSpeed >= InRunSpeedThreshold
		? InRunLoudness
		: InWalkLoudness;
	const float BagLoudness = FMath::Max(0, BagNoiseScore) * InBagScoreMultiplier;
	return FMath::Clamp(
		MovementLoudness + BagLoudness,
		0.0f,
		FMath::Max(0.0f, InMaximumLoudness));
}

int32 USurvivorNoiseEmitterComponent::GetCurrentBagNoiseScore() const
{
	return IsValid(InventoryComponent) ? InventoryComponent->GetBagNoiseScore() : 0;
}

void USurvivorNoiseEmitterComponent::TickComponent(
	const float DeltaTime,
	const ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AActor* Owner = GetOwner();
	const ACharacter* CharacterOwner = Cast<ACharacter>(Owner);
	if (!bNoiseSystemEnabled || !IsValid(Owner)
		|| (IsValid(CharacterOwner) && CharacterOwner->GetCharacterMovement()->IsFalling()))
	{
		CurrentLoudness = 0.0f;
		return;
	}

	const float MovementSpeed = Owner->GetVelocity().Size2D();
	CurrentLoudness = CalculateMovementLoudness(
		MovementSpeed,
		GetCurrentBagNoiseScore(),
		MinimumMovementSpeed,
		RunSpeedThreshold,
		WalkLoudness,
		RunLoudness,
		BagScoreMultiplier,
		MaximumLoudness);

	if (CurrentLoudness <= 0.0f)
	{
		TimeUntilNextNoise = 0.0f;
		return;
	}

	TimeUntilNextNoise -= DeltaTime;
	if (TimeUntilNextNoise > 0.0f)
	{
		return;
	}

	const bool bRunning = MovementSpeed >= RunSpeedThreshold;
	TimeUntilNextNoise = bRunning ? RunNoiseInterval : WalkNoiseInterval;
	UAISense_Hearing::ReportNoiseEvent(
		this,
		Owner->GetActorLocation(),
		CurrentLoudness,
		Owner,
		BaseNoiseRange,
		TEXT("PlayerMovement"));
}
