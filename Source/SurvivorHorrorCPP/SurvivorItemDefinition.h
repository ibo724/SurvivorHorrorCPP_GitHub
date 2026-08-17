// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SurvivorItemDefinition.generated.h"

class UTexture2D;

/**
 * Designer-authored data shared by every copy of an item.
 *
 * New items can be created in the Content Browser as Data Assets, so adding a
 * key, ammunition type or healing item does not require another C++ class.
 */
UCLASS(BlueprintType)
class SURVIVORHORRORCPP_API USurvivorItemDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** Stable internal identifier. Example: mansion_key or handgun_ammo. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FName ItemId = NAME_None;

	/** Name shown to the player. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FText DisplayName;

	/** Longer text reserved for the future inventory/inspection screen. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (MultiLine = "true"))
	FText Description;

	/** Optional icon reserved for the future inventory screen. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	TSoftObjectPtr<UTexture2D> Icon;

	/** How many copies fit in one stack. Use 1 for unique items and keys. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (ClampMin = "1", UIMin = "1"))
	int32 MaxStackSize = 1;
};
