// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SurvivorNoiseEmitterComponent.generated.h"

class USurvivorInventoryComponent;

/** Emits AI-hearing events from movement speed plus the inventory's bag-noise score. */
UCLASS(ClassGroup = (Survivor), meta = (BlueprintSpawnableComponent))
class SURVIVORHORRORCPP_API USurvivorNoiseEmitterComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USurvivorNoiseEmitterComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

public:
	/** Pure calculation kept public so balancing can be tested without spawning an actor. */
	static float CalculateMovementLoudness(
		float MovementSpeed,
		int32 BagNoiseScore,
		float MinimumMovementSpeed = 10.0f,
		float RunSpeedThreshold = 240.0f,
		float WalkLoudness = 0.30f,
		float RunLoudness = 0.90f,
		float BagScoreMultiplier = 0.045f,
		float MaximumLoudness = 1.50f);

	UFUNCTION(BlueprintPure, Category = "Noise")
	float GetCurrentLoudness() const { return CurrentLoudness; }

	UFUNCTION(BlueprintPure, Category = "Noise")
	int32 GetCurrentBagNoiseScore() const;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise", meta = (AllowPrivateAccess = "true"))
	bool bNoiseSystemEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise|Movement", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", Units = "cm/s"))
	float MinimumMovementSpeed = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise|Movement", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", Units = "cm/s"))
	float RunSpeedThreshold = 240.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise|Movement", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float WalkLoudness = 0.30f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise|Movement", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float RunLoudness = 0.90f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise|Bag", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float BagScoreMultiplier = 0.045f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise", meta = (AllowPrivateAccess = "true", ClampMin = "0.01"))
	float MaximumLoudness = 1.50f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise|Cadence", meta = (AllowPrivateAccess = "true", ClampMin = "0.05", Units = "s"))
	float WalkNoiseInterval = 0.55f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise|Cadence", meta = (AllowPrivateAccess = "true", ClampMin = "0.05", Units = "s"))
	float RunNoiseInterval = 0.32f;

	/** At loudness 1.0 this is the maximum hearing range. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", Units = "cm"))
	float BaseNoiseRange = 1200.0f;

	UPROPERTY(Transient)
	TObjectPtr<USurvivorInventoryComponent> InventoryComponent;

	UPROPERTY(VisibleInstanceOnly, Category = "Noise")
	float CurrentLoudness = 0.0f;

	float TimeUntilNextNoise = 0.0f;
};
