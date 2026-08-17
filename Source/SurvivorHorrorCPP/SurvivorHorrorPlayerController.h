// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SurvivorHorrorPlayerController.generated.h"

class ACodeVeronicaCameraZone;
class APawn;
class USurvivorInventoryComponent;

/** Owns the active room camera and guarantees that only one zone tracks at a time. */
UCLASS()
class SURVIVORHORRORCPP_API ASurvivorHorrorPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void ActivateCameraZone(ACodeVeronicaCameraZone* NewCameraZone, APawn* PlayerPawn);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool IsInventoryOpen() const { return bInventoryOpen; }

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetSelectedInventorySlot() const { return SelectedInventorySlot; }

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetMoveSourceSlot() const { return MoveSourceSlot; }

	UFUNCTION(BlueprintPure, Category = "Inventory")
	USurvivorInventoryComponent* GetPlayerInventory() const;

protected:
	virtual void SetupInputComponent() override;

private:
	void ToggleInventory();
	void CloseInventoryOrCancelMove();
	void NavigateInventory(int32 Horizontal, int32 Vertical);
	void NavigateInventoryUp();
	void NavigateInventoryDown();
	void NavigateInventoryLeft();
	void NavigateInventoryRight();
	void ConfirmInventoryMove();
	void InspectSelectedItem();
	void DiscardSelectedObsoleteItem();
	void SetInventoryOpen(bool bNewOpen);

	UPROPERTY(Transient)
	TObjectPtr<ACodeVeronicaCameraZone> ActiveCameraZone;

	bool bInventoryOpen = false;
	int32 SelectedInventorySlot = 0;
	int32 MoveSourceSlot = INDEX_NONE;
};
