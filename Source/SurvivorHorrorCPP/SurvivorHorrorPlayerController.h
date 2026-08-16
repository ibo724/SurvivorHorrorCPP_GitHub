// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SurvivorHorrorPlayerController.generated.h"

class ACodeVeronicaCameraZone;
class APawn;

/** Owns the active room camera and guarantees that only one zone tracks at a time. */
UCLASS()
class SURVIVORHORRORCPP_API ASurvivorHorrorPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void ActivateCameraZone(ACodeVeronicaCameraZone* NewCameraZone, APawn* PlayerPawn);

private:
	UPROPERTY(Transient)
	TObjectPtr<ACodeVeronicaCameraZone> ActiveCameraZone;
};
