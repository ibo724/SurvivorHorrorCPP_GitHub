// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SurvivorHealthComponent.generated.h"

UENUM(BlueprintType)
enum class ESurvivorHealthState : uint8
{
	Healthy,
	Warning,
	Danger,
	Dead
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FSurvivorHealthChangedSignature,
	float, CurrentHealth,
	float, MaxHealth,
	float, HealthDelta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSurvivorDeathSignature);

/**
 * Reusable health container shared by the player, enemies and future actors.
 * It deliberately knows nothing about weapons, AI, animations or save rules.
 */
UCLASS(ClassGroup = (Survivor), meta = (BlueprintSpawnableComponent))
class SURVIVORHORRORCPP_API USurvivorHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USurvivorHealthComponent();

protected:
	virtual void BeginPlay() override;

public:
	/** Returns the amount of damage that was actually applied. */
	UFUNCTION(BlueprintCallable, Category = "Health")
	float ApplyDamage(float DamageAmount);

	/** Returns the amount of health that was actually restored. Dead actors cannot be healed. */
	UFUNCTION(BlueprintCallable, Category = "Health")
	float Heal(float HealAmount);

	/** Restores the component for a fresh level/session. */
	UFUNCTION(BlueprintCallable, Category = "Health")
	void ResetHealth();

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetCurrentHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealthRatio() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	ESurvivorHealthState GetHealthState() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsDead() const { return CurrentHealth <= 0.0f; }

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FSurvivorHealthChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FSurvivorDeathSignature OnDeath;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true", ClampMin = "1.0"))
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
	float CurrentHealth = 100.0f;

	bool bDeathBroadcast = false;
};
