// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SurvivorItemUseComponent.generated.h"

class USurvivorHealthComponent;
class USurvivorInventoryComponent;

UENUM(BlueprintType)
enum class ESurvivorItemUseResult : uint8
{
	Success,
	InvalidSlot,
	NotUsable,
	HealthFull,
	UserDead,
	MissingComponent
};

/**
 * Executes inventory-item effects without coupling the inventory container to
 * the player, health system, weapons or presentation.
 */
UCLASS(ClassGroup = (Survivor), meta = (BlueprintSpawnableComponent))
class SURVIVORHORRORCPP_API USurvivorItemUseComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USurvivorItemUseComponent();

	UFUNCTION(BlueprintCallable, Category = "Item Use")
	ESurvivorItemUseResult TryUseItemAtSlot(int32 SlotIndex);

	/** Component-independent entry point used by automation and future AI companions. */
	static ESurvivorItemUseResult TryUseItemForComponents(
		USurvivorInventoryComponent* Inventory,
		USurvivorHealthComponent* Health,
		int32 SlotIndex);
};
