// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SurvivorInteractable.generated.h"

class APawn;

/** Reflection wrapper for the common survival-horror interaction contract. */
UINTERFACE(BlueprintType, Blueprintable)
class SURVIVORHORRORCPP_API USurvivorInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * Implement this interface on doors, pickups, inspection points and puzzle
 * objects so the player can interact with all of them through one system.
 */
class SURVIVORHORRORCPP_API ISurvivorInteractable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	bool CanInteract(APawn* InteractingPawn) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	FText GetInteractionPrompt(APawn* InteractingPawn) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void Interact(APawn* InteractingPawn);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void BeginInteractionFocus(APawn* InteractingPawn);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void EndInteractionFocus(APawn* InteractingPawn);
};
