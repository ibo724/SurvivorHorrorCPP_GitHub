// Copyright Epic Games, Inc. All Rights Reserved.

#include "SurvivorItemUseComponent.h"

#include "SurvivorHealthComponent.h"
#include "SurvivorInventoryComponent.h"
#include "SurvivorItemDefinition.h"

USurvivorItemUseComponent::USurvivorItemUseComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

ESurvivorItemUseResult USurvivorItemUseComponent::TryUseItemAtSlot(const int32 SlotIndex)
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return ESurvivorItemUseResult::MissingComponent;
	}

	return TryUseItemForComponents(
		Owner->FindComponentByClass<USurvivorInventoryComponent>(),
		Owner->FindComponentByClass<USurvivorHealthComponent>(),
		SlotIndex);
}

ESurvivorItemUseResult USurvivorItemUseComponent::TryUseItemForComponents(
	USurvivorInventoryComponent* Inventory,
	USurvivorHealthComponent* Health,
	const int32 SlotIndex)
{
	if (!IsValid(Inventory) || !IsValid(Health))
	{
		return ESurvivorItemUseResult::MissingComponent;
	}

	const FSurvivorInventoryEntry Entry = Inventory->GetEntryAt(SlotIndex);
	if (!IsValid(Entry.ItemDefinition) || Entry.Quantity <= 0)
	{
		return ESurvivorItemUseResult::InvalidSlot;
	}
	if (Health->IsDead())
	{
		return ESurvivorItemUseResult::UserDead;
	}

	switch (Entry.ItemDefinition->UseEffect)
	{
	case ESurvivorItemUseEffect::RestoreHealth:
		if (Health->GetCurrentHealth() >= Health->GetMaxHealth())
		{
			return ESurvivorItemUseResult::HealthFull;
		}
		if (Health->Heal(Entry.ItemDefinition->HealthRestoreAmount) <= 0.0f)
		{
			return ESurvivorItemUseResult::NotUsable;
		}
		return Inventory->RemoveItemAtSlot(SlotIndex, 1) == 1
			? ESurvivorItemUseResult::Success
			: ESurvivorItemUseResult::InvalidSlot;

	default:
		return ESurvivorItemUseResult::NotUsable;
	}
}
