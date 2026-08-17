// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SurvivorHorrorHUD.generated.h"

/** Minimal interaction prompt HUD; inventory and health UI will be added later. */
UCLASS()
class SURVIVORHORRORCPP_API ASurvivorHorrorHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;
};
