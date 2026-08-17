// Copyright Epic Games, Inc. All Rights Reserved.

#include "SurvivorHorrorPlayerController.h"

#include "CodeVeronicaCameraZone.h"
#include "Components/InputComponent.h"
#include "GameFramework/Pawn.h"
#include "SurvivorInventoryComponent.h"
#include "SurvivorItemDefinition.h"

void ASurvivorHorrorPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	check(InputComponent);

	FInputActionBinding& ToggleBinding = InputComponent->BindAction(
		TEXT("InventoryToggle"), IE_Pressed, this,
		&ASurvivorHorrorPlayerController::ToggleInventory);
	ToggleBinding.bExecuteWhenPaused = true;

	FInputActionBinding& CancelBinding = InputComponent->BindAction(
		TEXT("InventoryCancel"), IE_Pressed, this,
		&ASurvivorHorrorPlayerController::CloseInventoryOrCancelMove);
	CancelBinding.bExecuteWhenPaused = true;

	FInputActionBinding& UpBinding = InputComponent->BindAction(
		TEXT("InventoryUp"), IE_Pressed, this,
		&ASurvivorHorrorPlayerController::NavigateInventoryUp);
	UpBinding.bExecuteWhenPaused = true;

	FInputActionBinding& DownBinding = InputComponent->BindAction(
		TEXT("InventoryDown"), IE_Pressed, this,
		&ASurvivorHorrorPlayerController::NavigateInventoryDown);
	DownBinding.bExecuteWhenPaused = true;

	FInputActionBinding& LeftBinding = InputComponent->BindAction(
		TEXT("InventoryLeft"), IE_Pressed, this,
		&ASurvivorHorrorPlayerController::NavigateInventoryLeft);
	LeftBinding.bExecuteWhenPaused = true;

	FInputActionBinding& RightBinding = InputComponent->BindAction(
		TEXT("InventoryRight"), IE_Pressed, this,
		&ASurvivorHorrorPlayerController::NavigateInventoryRight);
	RightBinding.bExecuteWhenPaused = true;

	FInputActionBinding& ConfirmBinding = InputComponent->BindAction(
		TEXT("InventoryConfirm"), IE_Pressed, this,
		&ASurvivorHorrorPlayerController::ConfirmInventoryMove);
	ConfirmBinding.bExecuteWhenPaused = true;

	FInputActionBinding& InspectBinding = InputComponent->BindAction(
		TEXT("InventoryInspect"), IE_Pressed, this,
		&ASurvivorHorrorPlayerController::InspectSelectedItem);
	InspectBinding.bExecuteWhenPaused = true;
}

USurvivorInventoryComponent* ASurvivorHorrorPlayerController::GetPlayerInventory() const
{
	const APawn* PlayerPawn = GetPawn();
	return IsValid(PlayerPawn)
		? PlayerPawn->FindComponentByClass<USurvivorInventoryComponent>()
		: nullptr;
}

void ASurvivorHorrorPlayerController::ToggleInventory()
{
	SetInventoryOpen(!bInventoryOpen);
}

void ASurvivorHorrorPlayerController::SetInventoryOpen(const bool bNewOpen)
{
	if (bInventoryOpen == bNewOpen || (bNewOpen && !IsValid(GetPlayerInventory())))
	{
		return;
	}

	if (bNewOpen)
	{
		bInventoryOpen = true;
		SelectedInventorySlot = 0;
		MoveSourceSlot = INDEX_NONE;
		if (!SetPause(true))
		{
			bInventoryOpen = false;
		}
		return;
	}

	bInventoryOpen = false;
	MoveSourceSlot = INDEX_NONE;
	SetPause(false);
}

void ASurvivorHorrorPlayerController::CloseInventoryOrCancelMove()
{
	if (!bInventoryOpen)
	{
		return;
	}
	if (MoveSourceSlot != INDEX_NONE)
	{
		MoveSourceSlot = INDEX_NONE;
		return;
	}
	SetInventoryOpen(false);
}

void ASurvivorHorrorPlayerController::NavigateInventory(
	const int32 Horizontal,
	const int32 Vertical)
{
	USurvivorInventoryComponent* Inventory = GetPlayerInventory();
	if (!bInventoryOpen || !IsValid(Inventory))
	{
		return;
	}

	const int32 SlotCount = Inventory->GetSlotCount();
	const int32 Columns = FMath::Max(1, Inventory->GetColumnCount());
	const int32 Rows = FMath::Max(1, FMath::DivideAndRoundUp(SlotCount, Columns));
	int32 Column = SelectedInventorySlot % Columns;
	int32 Row = SelectedInventorySlot / Columns;
	Column = (Column + Horizontal + Columns) % Columns;
	Row = (Row + Vertical + Rows) % Rows;

	SelectedInventorySlot = FMath::Min(Row * Columns + Column, SlotCount - 1);
}

void ASurvivorHorrorPlayerController::NavigateInventoryUp()
{
	NavigateInventory(0, -1);
}

void ASurvivorHorrorPlayerController::NavigateInventoryDown()
{
	NavigateInventory(0, 1);
}

void ASurvivorHorrorPlayerController::NavigateInventoryLeft()
{
	NavigateInventory(-1, 0);
}

void ASurvivorHorrorPlayerController::NavigateInventoryRight()
{
	NavigateInventory(1, 0);
}

void ASurvivorHorrorPlayerController::ConfirmInventoryMove()
{
	USurvivorInventoryComponent* Inventory = GetPlayerInventory();
	if (!bInventoryOpen || !IsValid(Inventory))
	{
		return;
	}

	if (MoveSourceSlot == INDEX_NONE)
	{
		const FSurvivorInventoryEntry Entry = Inventory->GetEntryAt(SelectedInventorySlot);
		if (IsValid(Entry.ItemDefinition) && Entry.Quantity > 0)
		{
			MoveSourceSlot = SelectedInventorySlot;
		}
		return;
	}

	if (MoveSourceSlot == SelectedInventorySlot)
	{
		MoveSourceSlot = INDEX_NONE;
		return;
	}

	Inventory->MoveOrSwapSlots(MoveSourceSlot, SelectedInventorySlot);
	MoveSourceSlot = INDEX_NONE;
}

void ASurvivorHorrorPlayerController::InspectSelectedItem()
{
	USurvivorInventoryComponent* Inventory = GetPlayerInventory();
	if (!bInventoryOpen || !IsValid(Inventory))
	{
		return;
	}

	const FSurvivorInventoryEntry Entry = Inventory->GetEntryAt(SelectedInventorySlot);
	if (!IsValid(Entry.ItemDefinition))
	{
		return;
	}

	if (Inventory->InspectItemAtSlot(SelectedInventorySlot))
	{
		UE_LOG(
			LogTemp,
			Log,
			TEXT("Inspection memory unlocked for item '%s'."),
			*Entry.ItemDefinition->GetName());
	}
}

void ASurvivorHorrorPlayerController::ActivateCameraZone(
	ACodeVeronicaCameraZone* NewCameraZone, APawn* PlayerPawn)
{
	if (!IsValid(NewCameraZone) || !IsValid(PlayerPawn) || ActiveCameraZone == NewCameraZone)
	{
		return;
	}

	if (IsValid(ActiveCameraZone))
	{
		ActiveCameraZone->StopTracking();
	}

	ActiveCameraZone = NewCameraZone;
	ActiveCameraZone->StartTracking(PlayerPawn);

	UE_LOG(
		LogTemp,
		Log,
		TEXT("Activated room camera '%s' for pawn '%s'."),
		*GetNameSafe(ActiveCameraZone),
		*GetNameSafe(PlayerPawn));

	SetViewTargetWithBlend(
		ActiveCameraZone,
		ActiveCameraZone->GetCameraTransitionTime(),
		VTBlend_Cubic,
		2.0f,
		true);
}
