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

/**
 * Reusable item storage for the player character.
 *
 * MaxSlots is intentionally unlimited for now. Setting it later makes the
 * component enforce a classic slot limit without replacing this system.
 */
UCLASS(ClassGroup = (Survivor), meta = (BlueprintSpawnableComponent))
class SURVIVORHORRORCPP_API USurvivorInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USurvivorInventoryComponent();

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

	/** Fired after the stored entries change; the future UI can listen to this. */
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FSurvivorInventoryChangedSignature OnInventoryChanged;

private:
	/** Zero means unlimited. We will choose the real capacity together later. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true", ClampMin = "0", UIMin = "0"))
	int32 MaxSlots = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TArray<FSurvivorInventoryEntry> Entries;
};
