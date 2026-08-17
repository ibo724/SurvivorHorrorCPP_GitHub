// Copyright Epic Games, Inc. All Rights Reserved.

#include "SurvivorHealthComponent.h"

USurvivorHealthComponent::USurvivorHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USurvivorHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	ResetHealth();
}

float USurvivorHealthComponent::ApplyDamage(const float DamageAmount)
{
	if (DamageAmount <= 0.0f || IsDead())
	{
		return 0.0f;
	}

	const float OldHealth = CurrentHealth;
	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, MaxHealth);
	const float AppliedDamage = OldHealth - CurrentHealth;
	if (AppliedDamage > 0.0f)
	{
		OnHealthChanged.Broadcast(CurrentHealth, MaxHealth, -AppliedDamage);
	}

	if (IsDead() && !bDeathBroadcast)
	{
		bDeathBroadcast = true;
		OnDeath.Broadcast();
	}

	return AppliedDamage;
}

float USurvivorHealthComponent::Heal(const float HealAmount)
{
	if (HealAmount <= 0.0f || IsDead())
	{
		return 0.0f;
	}

	const float OldHealth = CurrentHealth;
	CurrentHealth = FMath::Clamp(CurrentHealth + HealAmount, 0.0f, MaxHealth);
	const float AppliedHealing = CurrentHealth - OldHealth;
	if (AppliedHealing > 0.0f)
	{
		OnHealthChanged.Broadcast(CurrentHealth, MaxHealth, AppliedHealing);
	}
	return AppliedHealing;
}

void USurvivorHealthComponent::ResetHealth()
{
	MaxHealth = FMath::Max(1.0f, MaxHealth);
	CurrentHealth = MaxHealth;
	bDeathBroadcast = false;
}

float USurvivorHealthComponent::GetHealthRatio() const
{
	return MaxHealth > 0.0f ? CurrentHealth / MaxHealth : 0.0f;
}

ESurvivorHealthState USurvivorHealthComponent::GetHealthState() const
{
	if (IsDead())
	{
		return ESurvivorHealthState::Dead;
	}

	const float Ratio = GetHealthRatio();
	if (Ratio <= 0.25f)
	{
		return ESurvivorHealthState::Danger;
	}
	if (Ratio <= 0.50f)
	{
		return ESurvivorHealthState::Warning;
	}
	return ESurvivorHealthState::Healthy;
}
