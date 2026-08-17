// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SurvivorInteractable.h"
#include "LockedDoor.generated.h"

class UPointLightComponent;
class USceneComponent;
class UStaticMeshComponent;
class USurvivorInventoryComponent;
class USurvivorItemDefinition;

/**
 * Reusable physical door with an optional key requirement.
 *
 * Doors sharing RequiredKey are counted at runtime. After the final matching
 * lock opens, the key becomes optional-to-discard in the inventory.
 */
UCLASS()
class SURVIVORHORRORCPP_API ALockedDoor : public AActor, public ISurvivorInteractable
{
	GENERATED_BODY()

public:
	ALockedDoor();

	virtual void Tick(float DeltaSeconds) override;
	virtual bool CanInteract_Implementation(APawn* InteractingPawn) const override;
	virtual FText GetInteractionPrompt_Implementation(APawn* InteractingPawn) const override;
	virtual void Interact_Implementation(APawn* InteractingPawn) override;
	virtual void BeginInteractionFocus_Implementation(APawn* InteractingPawn) override;
	virtual void EndInteractionFocus_Implementation(APawn* InteractingPawn) override;

protected:
	virtual void BeginPlay() override;

private:
	void StartDoorAnimation(bool bOpenDoor, const APawn* InteractingPawn);
	void SetFocused(bool bNewFocused);
	void ShowPlayerMessage(APawn* InteractingPawn, const FText& Message) const;
	bool HasAnotherLockedDoorForKey() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> DoorPivot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> DoorPanel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> LeftFrame;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> RightFrame;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> TopFrame;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPointLightComponent> FocusLight;

	/** Assign the same item definition to every door opened by this key. */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Door|Lock", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USurvivorItemDefinition> RequiredKey;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Door|Lock", meta = (AllowPrivateAccess = "true"))
	bool bStartsLocked = true;

	/** Internal symbol ID shared with the key Data Asset. Example: moon. */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Door|Lock", meta = (AllowPrivateAccess = "true"))
	FName LockSymbol = NAME_None;

	/** Player-facing name of the symbol. Example: Ay. */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Door|Lock", meta = (AllowPrivateAccess = "true"))
	FText LockSymbolDisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door|Animation", meta = (AllowPrivateAccess = "true", ClampMin = "1.0", ClampMax = "170.0", Units = "deg"))
	float OpenAngle = 95.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door|Animation", meta = (AllowPrivateAccess = "true", ClampMin = "0.05", Units = "s"))
	float DoorAnimationDuration = 0.75f;

	bool bIsLocked = false;
	bool bIsOpen = false;
	bool bIsAnimating = false;
	bool bTargetOpen = false;
	bool bIsFocused = false;
	float AnimationElapsed = 0.0f;
	float AnimationStartYaw = 0.0f;
	float AnimationTargetYaw = 0.0f;
};
