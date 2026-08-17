// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SurvivorHorrorHUD.generated.h"

class ASurvivorHorrorPlayerController;
class UFont;
class USurvivorInventoryComponent;

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
	void DrawInventoryScreen(
		const ASurvivorHorrorPlayerController* SurvivorController,
		const USurvivorInventoryComponent* Inventory,
		UFont* Font);
	void DrawBorder(float X, float Y, float Width, float Height, float Thickness, const FLinearColor& Color);
	void DrawCenteredTextInArea(
		const FString& Text,
		float X,
		float Y,
		float Width,
		UFont* Font,
		const FLinearColor& Color,
		float Scale = 1.0f);
	void DrawWrappedText(
		const FString& Text,
		float X,
		float Y,
		float MaxWidth,
		UFont* Font,
		const FLinearColor& Color,
		float Scale,
		int32 MaxLines);

	FText NotificationText;
	double NotificationEndTime = 0.0;
};
