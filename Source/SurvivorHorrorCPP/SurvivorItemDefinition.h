// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SurvivorItemDefinition.generated.h"

class UTexture2D;

/** How much an item contributes to the player's bag noise. */
UENUM(BlueprintType)
enum class ESurvivorItemNoiseClass : uint8
{
	Silent UMETA(DisplayName = "Sessiz"),
	Soft UMETA(DisplayName = "Yumuşak"),
	Rigid UMETA(DisplayName = "Sert"),
	Metallic UMETA(DisplayName = "Metal")
};

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

	/**
	 * Hidden detail revealed the first time this item type is inspected. Once
	 * found, the detail is remembered for every copy of the same item.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Inspection", meta = (MultiLine = "true"))
	FText InspectionDiscovery;

	/** Optional icon reserved for the future inventory screen. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	TSoftObjectPtr<UTexture2D> Icon;

	/** Used by the Bag Noise system. Soft or empty slots break noisy adjacency. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Bag Noise")
	ESurvivorItemNoiseClass NoiseClass = ESurvivorItemNoiseClass::Silent;

	/** Internal symbol shared with matching locked doors. Leave empty for non-keys. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Key")
	FName MatchingLockSymbol = NAME_None;

	/** Player-facing symbol name, such as Moon, Serpent or Tower. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Key")
	FText LockSymbolDisplayName;

	/** How many copies fit in one stack. Use 1 for unique items and keys. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (ClampMin = "1", UIMin = "1"))
	int32 MaxStackSize = 1;
};
