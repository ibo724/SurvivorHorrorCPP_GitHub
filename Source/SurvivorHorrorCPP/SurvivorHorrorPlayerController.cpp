// Copyright Epic Games, Inc. All Rights Reserved.

#include "SurvivorHorrorPlayerController.h"

#include "CodeVeronicaCameraZone.h"

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
