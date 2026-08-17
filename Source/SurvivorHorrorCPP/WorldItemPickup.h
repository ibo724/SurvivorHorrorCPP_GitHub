// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SurvivorInteractable.h"
#include "WorldItemPickup.generated.h"

class UPointLightComponent;
class UStaticMeshComponent;
class USurvivorItemDefinition;

/** An item placed in a level and collected through the common interaction system. */
UCLASS()
class SURVIVORHORRORCPP_API AWorldItemPickup : public AActor, public ISurvivorInteractable
{
	GENERATED_BODY()

public:
	AWorldItemPickup();

	virtual bool CanInteract_Implementation(APawn* InteractingPawn) const override;
	virtual FText GetInteractionPrompt_Implementation(APawn* InteractingPawn) const override;
	virtual void Interact_Implementation(APawn* InteractingPawn) override;
	virtual void BeginInteractionFocus_Implementation(APawn* InteractingPawn) override;
	virtual void EndInteractionFocus_Implementation(APawn* InteractingPawn) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
	TObjectPtr<UStaticMeshComponent> PickupMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
	TObjectPtr<UPointLightComponent> FocusLight;

	/** Create a SurvivorItemDefinition Data Asset and assign it here. */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Pickup")
	TObjectPtr<USurvivorItemDefinition> ItemDefinition;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Pickup", meta = (ClampMin = "1", UIMin = "1"))
	int32 Quantity = 1;

private:
	void SetFocused(bool bNewFocused);

	bool bIsFocused = false;
};
