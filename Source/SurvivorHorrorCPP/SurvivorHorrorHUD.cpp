// Copyright Epic Games, Inc. All Rights Reserved.

#include "SurvivorHorrorHUD.h"

#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/Font.h"
#include "GameFramework/PlayerController.h"
#include "SurvivorInteractionComponent.h"

void ASurvivorHorrorHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!Canvas || !PlayerOwner)
	{
		return;
	}

	APawn* PlayerPawn = PlayerOwner->GetPawn();
	USurvivorInteractionComponent* InteractionComponent = IsValid(PlayerPawn)
		? PlayerPawn->FindComponentByClass<USurvivorInteractionComponent>()
		: nullptr;
	if (!IsValid(InteractionComponent))
	{
		return;
	}

	const FText ObjectPrompt = InteractionComponent->GetCurrentInteractionPrompt();
	if (ObjectPrompt.IsEmpty())
	{
		return;
	}

	const FText Prompt = FText::Format(
		NSLOCTEXT("SurvivorHUD", "InteractionPrompt", "[E] {0}"),
		ObjectPrompt);
	const FString PromptString = Prompt.ToString();
	UFont* PromptFont = GEngine ? GEngine->GetMediumFont() : nullptr;
	if (!PromptFont)
	{
		return;
	}

	float TextWidth = 0.0f;
	float TextHeight = 0.0f;
	Canvas->StrLen(PromptFont, PromptString, TextWidth, TextHeight);

	constexpr float Padding = 12.0f;
	const float TextX = (Canvas->ClipX - TextWidth) * 0.5f;
	const float TextY = Canvas->ClipY * 0.82f;

	DrawRect(
		FLinearColor(0.0f, 0.0f, 0.0f, 0.65f),
		TextX - Padding,
		TextY - Padding * 0.5f,
		TextWidth + Padding * 2.0f,
		TextHeight + Padding);
	DrawText(PromptString, FLinearColor::White, TextX, TextY, PromptFont, 1.0f, false);
}
