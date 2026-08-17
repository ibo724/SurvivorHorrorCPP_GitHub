// Copyright Epic Games, Inc. All Rights Reserved.

#include "SurvivorInventoryComponent.h"

#include "SurvivorItemDefinition.h"

USurvivorInventoryComponent::USurvivorInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

int32 USurvivorInventoryComponent::AddItem(
	USurvivorItemDefinition* ItemDefinition,
	const int32 Quantity)
{
	if (!IsValid(ItemDefinition) || Quantity <= 0)
	{
		return 0;
	}

	const int32 StackSize = FMath::Max(1, ItemDefinition->MaxStackSize);
	int32 Remaining = Quantity;

	// Fill existing stacks before occupying another future inventory slot.
	for (FSurvivorInventoryEntry& Entry : Entries)
	{
		if (Entry.ItemDefinition != ItemDefinition || Entry.Quantity >= StackSize)
		{
			continue;
		}

		const int32 AmountToAdd = FMath::Min(StackSize - Entry.Quantity, Remaining);
		Entry.Quantity += AmountToAdd;
		Remaining -= AmountToAdd;

		if (Remaining <= 0)
		{
			break;
		}
	}

	while (Remaining > 0 && (MaxSlots <= 0 || Entries.Num() < MaxSlots))
	{
		FSurvivorInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
		NewEntry.ItemDefinition = ItemDefinition;
		NewEntry.Quantity = FMath::Min(StackSize, Remaining);
		Remaining -= NewEntry.Quantity;
	}

	const int32 AddedQuantity = Quantity - Remaining;
	if (AddedQuantity > 0)
	{
		OnInventoryChanged.Broadcast();
	}

	return AddedQuantity;
}

int32 USurvivorInventoryComponent::RemoveItem(
	USurvivorItemDefinition* ItemDefinition,
	const int32 Quantity)
{
	if (!IsValid(ItemDefinition) || Quantity <= 0)
	{
		return 0;
	}

	int32 Remaining = Quantity;
	for (int32 Index = Entries.Num() - 1; Index >= 0 && Remaining > 0; --Index)
	{
		FSurvivorInventoryEntry& Entry = Entries[Index];
		if (Entry.ItemDefinition != ItemDefinition)
		{
			continue;
		}

		const int32 AmountToRemove = FMath::Min(Entry.Quantity, Remaining);
		Entry.Quantity -= AmountToRemove;
		Remaining -= AmountToRemove;

		if (Entry.Quantity <= 0)
		{
			Entries.RemoveAt(Index);
		}
	}

	const int32 RemovedQuantity = Quantity - Remaining;
	if (RemovedQuantity > 0)
	{
		OnInventoryChanged.Broadcast();
	}

	return RemovedQuantity;
}

int32 USurvivorInventoryComponent::GetItemQuantity(
	const USurvivorItemDefinition* ItemDefinition) const
{
	if (!IsValid(ItemDefinition))
	{
		return 0;
	}

	int32 TotalQuantity = 0;
	for (const FSurvivorInventoryEntry& Entry : Entries)
	{
		if (Entry.ItemDefinition == ItemDefinition)
		{
			TotalQuantity += Entry.Quantity;
		}
	}

	return TotalQuantity;
}

bool USurvivorInventoryComponent::HasItem(
	const USurvivorItemDefinition* ItemDefinition,
	const int32 Quantity) const
{
	return Quantity <= 0 || GetItemQuantity(ItemDefinition) >= Quantity;
}
