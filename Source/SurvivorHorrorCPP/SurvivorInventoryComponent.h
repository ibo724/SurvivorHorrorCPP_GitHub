// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SurvivorInventoryComponent.generated.h"

class USurvivorItemDefinition;

/** One stack inside the inventory. */
USTRUCT(BlueprintType)
struct SURVIVORHORRORCPP_API FSurvivorInventoryEntry
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<USurvivorItemDefinition> ItemDefinition = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	int32 Quantity = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSurvivorInventoryChangedSignature);

/** Player-facing summary of the current bag noise score. */
UENUM(BlueprintType)
enum class ESurvivorBagNoiseLevel : uint8
{
	Silent,
	Low,
	Medium,
	High
};

/**
 * Reusable item storage for the player character.
 *
 * Entries always contains eight real slots, including empty slots. Keeping
 * empty positions lets arrangement affect bag noise and supports classic
 * inventory navigation.
 */
UCLASS(ClassGroup = (Survivor), meta = (BlueprintSpawnableComponent))
class SURVIVORHORRORCPP_API USurvivorInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USurvivorInventoryComponent();

protected:
	virtual void BeginPlay() override;

public:

	/** Adds as many items as possible and returns the amount actually added. */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 AddItem(USurvivorItemDefinition* ItemDefinition, int32 Quantity = 1);

	/** Removes up to Quantity items and returns the amount actually removed. */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 RemoveItem(USurvivorItemDefinition* ItemDefinition, int32 Quantity = 1);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetItemQuantity(const USurvivorItemDefinition* ItemDefinition) const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool HasItem(const USurvivorItemDefinition* ItemDefinition, int32 Quantity = 1) const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	TArray<FSurvivorInventoryEntry> GetEntries() const { return Entries; }

	UFUNCTION(BlueprintPure, Category = "Inventory")
	FSurvivorInventoryEntry GetEntryAt(int32 SlotIndex) const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetSlotCount() const { return MaxSlots; }

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetColumnCount() const { return InventoryColumns; }

	/** Swaps an occupied slot with an empty or occupied target slot. */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool MoveOrSwapSlots(int32 FromSlot, int32 ToSlot);

	/** Returns true only when this call discovers and remembers the item type. */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Inspection")
	bool InspectItemAtSlot(int32 SlotIndex);

	UFUNCTION(BlueprintPure, Category = "Inventory|Inspection")
	bool IsItemInspected(const USurvivorItemDefinition* ItemDefinition) const;

	/** Records a symbol found on a locked door. */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Inspection")
	bool RememberLockSymbol(FName LockSymbol);

	UFUNCTION(BlueprintPure, Category = "Inventory|Inspection")
	bool HasObservedLockSymbol(FName LockSymbol) const;

	/** Marks an item type as no longer needed without discarding it automatically. */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool MarkItemObsolete(const USurvivorItemDefinition* ItemDefinition);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool IsItemObsolete(const USurvivorItemDefinition* ItemDefinition) const;

	/** Discards the selected stack only if its item type has become obsolete. */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 DiscardObsoleteItemAtSlot(int32 SlotIndex);

	UFUNCTION(BlueprintPure, Category = "Inventory|Bag Noise")
	int32 GetBagNoiseScore() const;

	UFUNCTION(BlueprintPure, Category = "Inventory|Bag Noise")
	ESurvivorBagNoiseLevel GetBagNoiseLevel() const;

	/** Fired after the stored entries change; the future UI can listen to this. */
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FSurvivorInventoryChangedSignature OnInventoryChanged;

private:
	void EnsureSlotsInitialized();
	static FName GetInspectionMemoryKey(const USurvivorItemDefinition* ItemDefinition);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true", ClampMin = "1", UIMin = "1"))
	int32 MaxSlots = 8;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true", ClampMin = "1", UIMin = "1"))
	int32 InventoryColumns = 4;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TArray<FSurvivorInventoryEntry> Entries;

	/** Session memory; this will be serialized when the save-game system is added. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Inventory|Inspection", meta = (AllowPrivateAccess = "true"))
	TSet<FName> InspectedItemIds;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Inventory|Inspection", meta = (AllowPrivateAccess = "true"))
	TSet<FName> ObservedLockSymbols;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TSet<FName> ObsoleteItemIds;
};
