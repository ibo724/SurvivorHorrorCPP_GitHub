// Copyright Epic Games, Inc. All Rights Reserved.

#include "SurvivorInventoryComponent.h"

#include "SurvivorItemDefinition.h"

namespace SurvivorInventory
{
	int32 GetBaseNoise(const ESurvivorItemNoiseClass NoiseClass)
	{
		switch (NoiseClass)
		{
		case ESurvivorItemNoiseClass::Rigid:
			return 1;
		case ESurvivorItemNoiseClass::Metallic:
			return 2;
		default:
			return 0;
		}
	}

	int32 GetAdjacencyNoise(
		const ESurvivorItemNoiseClass First,
		const ESurvivorItemNoiseClass Second)
	{
		if (First == ESurvivorItemNoiseClass::Metallic
			&& Second == ESurvivorItemNoiseClass::Metallic)
		{
			return 3;
		}

		const bool bHasMetal = First == ESurvivorItemNoiseClass::Metallic
			|| Second == ESurvivorItemNoiseClass::Metallic;
		const bool bHasRigid = First == ESurvivorItemNoiseClass::Rigid
			|| Second == ESurvivorItemNoiseClass::Rigid;
		if (bHasMetal && bHasRigid)
		{
			return 2;
		}

		if (First == ESurvivorItemNoiseClass::Rigid
			&& Second == ESurvivorItemNoiseClass::Rigid)
		{
			return 1;
		}

		return 0;
	}
}

USurvivorInventoryComponent::USurvivorInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	Entries.SetNum(MaxSlots);
}

void USurvivorInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	EnsureSlotsInitialized();
}

void USurvivorInventoryComponent::EnsureSlotsInitialized()
{
	MaxSlots = FMath::Max(1, MaxSlots);
	InventoryColumns = FMath::Clamp(InventoryColumns, 1, MaxSlots);
	Entries.SetNum(MaxSlots);
}

int32 USurvivorInventoryComponent::AddItem(
	USurvivorItemDefinition* ItemDefinition,
	const int32 Quantity)
{
	if (!IsValid(ItemDefinition) || Quantity <= 0)
	{
		return 0;
	}

	EnsureSlotsInitialized();
	const int32 StackSize = FMath::Max(1, ItemDefinition->MaxStackSize);
	int32 Remaining = Quantity;

	// Fill existing stacks before occupying another slot.
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

	// Then use the first empty slots, preserving all empty positions in Entries.
	for (FSurvivorInventoryEntry& Entry : Entries)
	{
		if (Remaining <= 0)
		{
			break;
		}
		if (IsValid(Entry.ItemDefinition) && Entry.Quantity > 0)
		{
			continue;
		}

		Entry.ItemDefinition = ItemDefinition;
		Entry.Quantity = FMath::Min(StackSize, Remaining);
		Remaining -= Entry.Quantity;
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
			Entry = FSurvivorInventoryEntry();
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

FSurvivorInventoryEntry USurvivorInventoryComponent::GetEntryAt(const int32 SlotIndex) const
{
	return Entries.IsValidIndex(SlotIndex)
		? Entries[SlotIndex]
		: FSurvivorInventoryEntry();
}

bool USurvivorInventoryComponent::MoveOrSwapSlots(const int32 FromSlot, const int32 ToSlot)
{
	if (!Entries.IsValidIndex(FromSlot)
		|| !Entries.IsValidIndex(ToSlot)
		|| FromSlot == ToSlot
		|| !IsValid(Entries[FromSlot].ItemDefinition))
	{
		return false;
	}

	Entries.Swap(FromSlot, ToSlot);
	OnInventoryChanged.Broadcast();
	return true;
}

FName USurvivorInventoryComponent::GetInspectionMemoryKey(
	const USurvivorItemDefinition* ItemDefinition)
{
	if (!IsValid(ItemDefinition))
	{
		return NAME_None;
	}
	return ItemDefinition->ItemId.IsNone()
		? ItemDefinition->GetFName()
		: ItemDefinition->ItemId;
}

bool USurvivorInventoryComponent::InspectItemAtSlot(const int32 SlotIndex)
{
	const FSurvivorInventoryEntry Entry = GetEntryAt(SlotIndex);
	const FName MemoryKey = GetInspectionMemoryKey(Entry.ItemDefinition);
	if (MemoryKey.IsNone() || InspectedItemIds.Contains(MemoryKey))
	{
		return false;
	}

	InspectedItemIds.Add(MemoryKey);
	OnInventoryChanged.Broadcast();
	return true;
}

bool USurvivorInventoryComponent::IsItemInspected(
	const USurvivorItemDefinition* ItemDefinition) const
{
	const FName MemoryKey = GetInspectionMemoryKey(ItemDefinition);
	return !MemoryKey.IsNone() && InspectedItemIds.Contains(MemoryKey);
}

int32 USurvivorInventoryComponent::GetBagNoiseScore() const
{
	int32 Score = 0;
	for (const FSurvivorInventoryEntry& Entry : Entries)
	{
		if (IsValid(Entry.ItemDefinition) && Entry.Quantity > 0)
		{
			Score += SurvivorInventory::GetBaseNoise(Entry.ItemDefinition->NoiseClass);
		}
	}

	const int32 ColumnCount = FMath::Max(1, InventoryColumns);
	for (int32 Index = 0; Index < Entries.Num(); ++Index)
	{
		const USurvivorItemDefinition* CurrentItem = Entries[Index].ItemDefinition;
		if (!IsValid(CurrentItem) || Entries[Index].Quantity <= 0)
		{
			continue;
		}

		const int32 Column = Index % ColumnCount;
		const int32 RightIndex = Index + 1;
		if (Column + 1 < ColumnCount && Entries.IsValidIndex(RightIndex)
			&& IsValid(Entries[RightIndex].ItemDefinition))
		{
			Score += SurvivorInventory::GetAdjacencyNoise(
				CurrentItem->NoiseClass,
				Entries[RightIndex].ItemDefinition->NoiseClass);
		}

		const int32 DownIndex = Index + ColumnCount;
		if (Entries.IsValidIndex(DownIndex) && IsValid(Entries[DownIndex].ItemDefinition))
		{
			Score += SurvivorInventory::GetAdjacencyNoise(
				CurrentItem->NoiseClass,
				Entries[DownIndex].ItemDefinition->NoiseClass);
		}
	}

	return Score;
}

ESurvivorBagNoiseLevel USurvivorInventoryComponent::GetBagNoiseLevel() const
{
	const int32 Score = GetBagNoiseScore();
	if (Score <= 0)
	{
		return ESurvivorBagNoiseLevel::Silent;
	}
	if (Score <= 4)
	{
		return ESurvivorBagNoiseLevel::Low;
	}
	if (Score <= 11)
	{
		return ESurvivorBagNoiseLevel::Medium;
	}
	return ESurvivorBagNoiseLevel::High;
}
