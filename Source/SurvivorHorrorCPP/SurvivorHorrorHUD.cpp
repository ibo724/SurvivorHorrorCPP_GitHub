// Copyright Epic Games, Inc. All Rights Reserved.

#include "SurvivorHorrorHUD.h"

#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/Font.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "SurvivorInteractionComponent.h"

void ASurvivorHorrorHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!Canvas || !PlayerOwner)
	{
		return;
	}

	UFont* PromptFont = GEngine ? GEngine->GetMediumFont() : nullptr;
	if (!PromptFont)
	{
		return;
	}

	if (!NotificationText.IsEmpty() && GetWorld()
		&& GetWorld()->GetTimeSeconds() < NotificationEndTime)
	{
		const FString NotificationString = NotificationText.ToString();
		float NotificationWidth = 0.0f;
		float NotificationHeight = 0.0f;
		Canvas->StrLen(PromptFont, NotificationString, NotificationWidth, NotificationHeight);
		DrawText(
			NotificationString,
			FLinearColor(1.0f, 0.88f, 0.58f),
			(Canvas->ClipX - NotificationWidth) * 0.5f,
			Canvas->ClipY * 0.70f,
			PromptFont,
			1.0f,
			false);
	}

	APawn* PlayerPawn = PlayerOwner->GetPawn();
	USurvivorInteractionComponent* InteractionComponent = IsValid(PlayerPawn)
		? PlayerPawn->FindComponentByClass<USurvivorInteractionComponent>()
		: nullptr;
	const FText ObjectPrompt = IsValid(InteractionComponent)
		? InteractionComponent->GetCurrentInteractionPrompt()
		: FText::GetEmpty();
	if (ObjectPrompt.IsEmpty())
	{
		return;
	}

	const FText Prompt = FText::Format(
		NSLOCTEXT("SurvivorHUD", "InteractionPrompt", "[E] {0}"),
		ObjectPrompt);
	const FString PromptString = Prompt.ToString();
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

void ASurvivorHorrorHUD::ShowNotification(const FText& Message, const float Duration)
{
	NotificationText = Message;
	NotificationEndTime = GetWorld()
		? GetWorld()->GetTimeSeconds() + FMath::Max(0.0f, Duration)
		: 0.0;
}
