// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SurvivorHorrorHUD.generated.h"

/** Minimal interaction prompt and temporary gameplay notification HUD. */
UCLASS()
class SURVIVORHORRORCPP_API ASurvivorHorrorHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

	/** Shows a short message without committing to the final inventory UI style. */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ShowNotification(const FText& Message, float Duration = 2.5f);

private:
	FText NotificationText;
	double NotificationEndTime = 0.0;
};
