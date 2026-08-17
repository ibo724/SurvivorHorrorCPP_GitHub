// Copyright Epic Games, Inc. All Rights Reserved.

#include "SurvivorHorrorHUD.h"

#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/Font.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "SurvivorHorrorPlayerController.h"
#include "SurvivorHealthComponent.h"
#include "SurvivorInteractionComponent.h"
#include "SurvivorInventoryComponent.h"
#include "SurvivorItemDefinition.h"

namespace SurvivorHUD
{
	FString GetBagNoiseLabel(const ESurvivorBagNoiseLevel Level)
	{
		switch (Level)
		{
		case ESurvivorBagNoiseLevel::Low:
			return TEXT("DÜŞÜK");
		case ESurvivorBagNoiseLevel::Medium:
			return TEXT("ORTA");
		case ESurvivorBagNoiseLevel::High:
			return TEXT("YÜKSEK");
		default:
			return TEXT("SESSİZ");
		}
	}

	FLinearColor GetBagNoiseColor(const ESurvivorBagNoiseLevel Level)
	{
		switch (Level)
		{
		case ESurvivorBagNoiseLevel::Low:
			return FLinearColor(0.38f, 0.78f, 0.42f);
		case ESurvivorBagNoiseLevel::Medium:
			return FLinearColor(1.0f, 0.70f, 0.20f);
		case ESurvivorBagNoiseLevel::High:
			return FLinearColor(0.92f, 0.16f, 0.12f);
		default:
			return FLinearColor(0.48f, 0.58f, 0.62f);
		}
	}

	int32 GetActiveNoiseBars(const ESurvivorBagNoiseLevel Level)
	{
		switch (Level)
		{
		case ESurvivorBagNoiseLevel::Low:
			return 1;
		case ESurvivorBagNoiseLevel::Medium:
			return 2;
		case ESurvivorBagNoiseLevel::High:
			return 4;
		default:
			return 0;
		}
	}

	FString GetItemNoiseLabel(const ESurvivorItemNoiseClass NoiseClass)
	{
		switch (NoiseClass)
		{
		case ESurvivorItemNoiseClass::Soft:
			return TEXT("Yumuşak");
		case ESurvivorItemNoiseClass::Rigid:
			return TEXT("Sert");
		case ESurvivorItemNoiseClass::Metallic:
			return TEXT("Metal");
		default:
			return TEXT("Sessiz");
		}
	}
}

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

	const ASurvivorHorrorPlayerController* SurvivorController =
		Cast<ASurvivorHorrorPlayerController>(PlayerOwner);
	APawn* PlayerPawn = PlayerOwner->GetPawn();
	const USurvivorHealthComponent* Health = IsValid(PlayerPawn)
		? PlayerPawn->FindComponentByClass<USurvivorHealthComponent>()
		: nullptr;
	if (IsValid(Health) && Health->IsDead())
	{
		DrawDeathScreen(PromptFont);
		return;
	}

	const USurvivorInventoryComponent* Inventory = IsValid(SurvivorController)
		? SurvivorController->GetPlayerInventory()
		: nullptr;
	if (IsValid(SurvivorController) && SurvivorController->IsInventoryOpen()
		&& IsValid(Inventory))
	{
		DrawInventoryScreen(SurvivorController, Inventory, PromptFont);
		if (IsValid(Health))
		{
			DrawHealthStatus(Health, PromptFont);
		}
		DrawActiveNotification(PromptFont);
		return;
	}

	if (IsValid(Health))
	{
		DrawHealthStatus(Health, PromptFont);
	}

	DrawActiveNotification(PromptFont);

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

void ASurvivorHorrorHUD::DrawHealthStatus(
	const USurvivorHealthComponent* Health,
	UFont* Font)
{
	FString StatusText = TEXT("DURUM: İYİ");
	FLinearColor StatusColor(0.26f, 0.80f, 0.36f);
	switch (Health->GetHealthState())
	{
	case ESurvivorHealthState::Warning:
		StatusText = TEXT("DURUM: DİKKAT");
		StatusColor = FLinearColor(1.0f, 0.68f, 0.16f);
		break;
	case ESurvivorHealthState::Danger:
		StatusText = TEXT("DURUM: TEHLİKE");
		StatusColor = FLinearColor(0.95f, 0.10f, 0.06f);
		break;
	default:
		break;
	}

	constexpr float X = 24.0f;
	constexpr float Y = 24.0f;
	constexpr float Width = 190.0f;
	constexpr float Height = 42.0f;
	DrawRect(FLinearColor(0.0f, 0.0f, 0.0f, 0.68f), X, Y, Width, Height);
	DrawBorder(X, Y, Width, Height, 2.0f, StatusColor);
	DrawText(StatusText, StatusColor, X + 13.0f, Y + 9.0f, Font, 0.82f, false);
}

void ASurvivorHorrorHUD::DrawDeathScreen(UFont* Font)
{
	DrawRect(FLinearColor(0.015f, 0.0f, 0.0f, 0.90f), 0.0f, 0.0f, Canvas->ClipX, Canvas->ClipY);
	DrawCenteredTextInArea(
		TEXT("ÖLDÜN"),
		0.0f,
		Canvas->ClipY * 0.38f,
		Canvas->ClipX,
		Font,
		FLinearColor(0.72f, 0.04f, 0.03f),
		2.0f);
	DrawCenteredTextInArea(
		TEXT("[R / ENTER] Tekrar dene"),
		0.0f,
		Canvas->ClipY * 0.54f,
		Canvas->ClipX,
		Font,
		FLinearColor(0.72f, 0.70f, 0.66f),
		0.90f);
}

void ASurvivorHorrorHUD::DrawActiveNotification(UFont* Font)
{
	if (NotificationText.IsEmpty() || !GetWorld()
		|| GetWorld()->GetTimeSeconds() >= NotificationEndTime)
	{
		return;
	}

	const FString NotificationString = NotificationText.ToString();
	float NotificationWidth = 0.0f;
	float NotificationHeight = 0.0f;
	Canvas->StrLen(Font, NotificationString, NotificationWidth, NotificationHeight);
	const float Padding = 12.0f;
	const float X = (Canvas->ClipX - NotificationWidth) * 0.5f;
	const float Y = Canvas->ClipY * 0.70f;
	DrawRect(
		FLinearColor(0.0f, 0.0f, 0.0f, 0.82f),
		X - Padding,
		Y - Padding * 0.5f,
		NotificationWidth + Padding * 2.0f,
		NotificationHeight + Padding);
	DrawText(
		NotificationString,
		FLinearColor(1.0f, 0.88f, 0.58f),
		X,
		Y,
		Font,
		1.0f,
		false);
}

void ASurvivorHorrorHUD::DrawInventoryScreen(
	const ASurvivorHorrorPlayerController* SurvivorController,
	const USurvivorInventoryComponent* Inventory,
	UFont* Font)
{
	const float UIScale = FMath::Clamp(
		FMath::Min(Canvas->ClipX / 1280.0f, Canvas->ClipY / 720.0f),
		0.65f,
		1.50f);
	DrawRect(FLinearColor(0.015f, 0.022f, 0.025f, 0.96f), 0.0f, 0.0f, Canvas->ClipX, Canvas->ClipY);

	DrawCenteredTextInArea(
		TEXT("ENVANTER"),
		0.0f,
		34.0f * UIScale,
		Canvas->ClipX,
		Font,
		FLinearColor(0.82f, 0.78f, 0.64f),
		1.20f * UIScale);

	const int32 SlotCount = Inventory->GetSlotCount();
	const int32 Columns = FMath::Max(1, Inventory->GetColumnCount());
	const int32 Rows = FMath::Max(1, FMath::DivideAndRoundUp(SlotCount, Columns));
	const float Gap = 12.0f * UIScale;
	const float SlotWidth = FMath::Min(
		210.0f * UIScale,
		(Canvas->ClipX * 0.88f - Gap * (Columns - 1)) / Columns);
	const float SlotHeight = 96.0f * UIScale;
	const float GridWidth = SlotWidth * Columns + Gap * (Columns - 1);
	const float GridHeight = SlotHeight * Rows + Gap * (Rows - 1);
	const float GridX = (Canvas->ClipX - GridWidth) * 0.5f;
	const float GridY = 90.0f * UIScale;

	for (int32 SlotIndex = 0; SlotIndex < SlotCount; ++SlotIndex)
	{
		const int32 Row = SlotIndex / Columns;
		const int32 Column = SlotIndex % Columns;
		const float X = GridX + Column * (SlotWidth + Gap);
		const float Y = GridY + Row * (SlotHeight + Gap);
		const bool bSelected = SurvivorController->GetSelectedInventorySlot() == SlotIndex;
		const bool bMoveSource = SurvivorController->GetMoveSourceSlot() == SlotIndex;

		DrawRect(
			bSelected
				? FLinearColor(0.18f, 0.17f, 0.12f, 0.98f)
				: FLinearColor(0.06f, 0.075f, 0.08f, 0.98f),
			X,
			Y,
			SlotWidth,
			SlotHeight);
		DrawBorder(
			X,
			Y,
			SlotWidth,
			SlotHeight,
			bSelected || bMoveSource ? 3.0f * UIScale : 1.0f * UIScale,
			bMoveSource
				? FLinearColor(0.30f, 0.68f, 0.95f)
				: (bSelected ? FLinearColor(0.95f, 0.76f, 0.28f) : FLinearColor(0.22f, 0.27f, 0.28f)));

		DrawText(
			FString::Printf(TEXT("%d"), SlotIndex + 1),
			FLinearColor(0.36f, 0.42f, 0.43f),
			X + 7.0f * UIScale,
			Y + 5.0f * UIScale,
			Font,
			0.65f * UIScale,
			false);

		const FSurvivorInventoryEntry Entry = Inventory->GetEntryAt(SlotIndex);
		if (!IsValid(Entry.ItemDefinition) || Entry.Quantity <= 0)
		{
			DrawCenteredTextInArea(
				TEXT("— boş —"),
				X,
				Y + SlotHeight * 0.40f,
				SlotWidth,
				Font,
				FLinearColor(0.32f, 0.38f, 0.39f),
				0.72f * UIScale);
			continue;
		}

		const FString ItemName = Entry.ItemDefinition->DisplayName.IsEmpty()
			? Entry.ItemDefinition->ItemId.ToString()
			: Entry.ItemDefinition->DisplayName.ToString();
		DrawCenteredTextInArea(
			ItemName,
			X + 10.0f * UIScale,
			Y + SlotHeight * 0.34f,
			SlotWidth - 20.0f * UIScale,
			Font,
			FLinearColor(0.90f, 0.90f, 0.84f),
			0.78f * UIScale);
		if (Entry.Quantity > 1)
		{
			DrawText(
				FString::Printf(TEXT("x%d"), Entry.Quantity),
				FLinearColor(0.95f, 0.76f, 0.28f),
				X + SlotWidth - 42.0f * UIScale,
				Y + SlotHeight - 27.0f * UIScale,
				Font,
				0.72f * UIScale,
				false);
		}
	}

	const ESurvivorBagNoiseLevel NoiseLevel = Inventory->GetBagNoiseLevel();
	const int32 NoiseScore = Inventory->GetBagNoiseScore();
	const FLinearColor NoiseColor = SurvivorHUD::GetBagNoiseColor(NoiseLevel);
	const float NoiseY = GridY + GridHeight + 24.0f * UIScale;
	const FString NoiseText = FString::Printf(
		TEXT("ÇANTA SESİ: %s  (puan %d)"),
		*SurvivorHUD::GetBagNoiseLabel(NoiseLevel),
		NoiseScore);
	DrawCenteredTextInArea(
		NoiseText,
		0.0f,
		NoiseY,
		Canvas->ClipX,
		Font,
		NoiseColor,
		0.88f * UIScale);

	const int32 ActiveBars = SurvivorHUD::GetActiveNoiseBars(NoiseLevel);
	const float BarWidth = 34.0f * UIScale;
	const float BarGap = 7.0f * UIScale;
	const float BarX = (Canvas->ClipX - (BarWidth * 4.0f + BarGap * 3.0f)) * 0.5f;
	for (int32 BarIndex = 0; BarIndex < 4; ++BarIndex)
	{
		DrawRect(
			BarIndex < ActiveBars ? NoiseColor : FLinearColor(0.12f, 0.15f, 0.15f),
			BarX + BarIndex * (BarWidth + BarGap),
			NoiseY + 27.0f * UIScale,
			BarWidth,
			5.0f * UIScale);
	}

	const float DetailX = GridX;
	const float DetailY = NoiseY + 50.0f * UIScale;
	const float DetailHeight = 190.0f * UIScale;
	DrawRect(FLinearColor(0.035f, 0.045f, 0.047f, 0.98f), DetailX, DetailY, GridWidth, DetailHeight);
	DrawBorder(DetailX, DetailY, GridWidth, DetailHeight, UIScale, FLinearColor(0.20f, 0.25f, 0.25f));

	const FSurvivorInventoryEntry SelectedEntry =
		Inventory->GetEntryAt(SurvivorController->GetSelectedInventorySlot());
	if (!IsValid(SelectedEntry.ItemDefinition))
	{
		DrawCenteredTextInArea(
			TEXT("Boş yuva"),
			DetailX,
			DetailY + DetailHeight * 0.42f,
			GridWidth,
			Font,
			FLinearColor(0.40f, 0.46f, 0.46f),
			0.82f * UIScale);
	}
	else
	{
		const USurvivorItemDefinition* Item = SelectedEntry.ItemDefinition;
		const FString ItemName = Item->DisplayName.IsEmpty()
			? Item->ItemId.ToString()
			: Item->DisplayName.ToString();
		DrawText(
			ItemName,
			FLinearColor(0.95f, 0.86f, 0.56f),
			DetailX + 18.0f * UIScale,
			DetailY + 13.0f * UIScale,
			Font,
			0.92f * UIScale,
			false);
		DrawText(
			FString::Printf(TEXT("Malzeme: %s"), *SurvivorHUD::GetItemNoiseLabel(Item->NoiseClass)),
			FLinearColor(0.48f, 0.58f, 0.58f),
			DetailX + GridWidth - 170.0f * UIScale,
			DetailY + 15.0f * UIScale,
			Font,
			0.68f * UIScale,
			false);

		DrawWrappedText(
			Item->Description.ToString(),
			DetailX + 18.0f * UIScale,
			DetailY + 48.0f * UIScale,
			GridWidth - 36.0f * UIScale,
			Font,
			FLinearColor(0.78f, 0.80f, 0.76f),
			0.72f * UIScale,
			3);

		const bool bInspected = Inventory->IsItemInspected(Item);
		FString InspectionText;
		FLinearColor InspectionColor;
		if (bInspected)
		{
			InspectionText = Item->InspectionDiscovery.IsEmpty()
				? TEXT("HAFIZAYA KAYDEDİLDİ: Yeni bir ayrıntı bulunamadı.")
				: FString::Printf(TEXT("HAFIZAYA KAYDEDİLDİ: %s"), *Item->InspectionDiscovery.ToString());
			InspectionColor = FLinearColor(0.52f, 0.76f, 0.68f);
		}
		else
		{
			InspectionText = Item->InspectionDiscovery.IsEmpty()
				? TEXT("[F] Eşyayı incele")
				: TEXT("[F] Eşyayı incele — gözden kaçan bir ayrıntı olabilir.");
			InspectionColor = FLinearColor(0.62f, 0.68f, 0.66f);
		}

		if (bInspected
			&& !Item->MatchingLockSymbol.IsNone()
			&& Inventory->HasObservedLockSymbol(Item->MatchingLockSymbol))
		{
			const FString SymbolName = Item->LockSymbolDisplayName.IsEmpty()
				? Item->MatchingLockSymbol.ToString()
				: Item->LockSymbolDisplayName.ToString();
			InspectionText += FString::Printf(
				TEXT(" HAFIZA BAĞLANTISI: %s işareti kilitli bir kapıda görülmüştü."),
				*SymbolName);
			InspectionColor = FLinearColor(0.42f, 0.78f, 0.88f);
		}

		if (Inventory->IsItemObsolete(Item))
		{
			InspectionText = TEXT("[R] ARTIK GEREKMİYOR — çantadan çıkar. ") + InspectionText;
			InspectionColor = FLinearColor(0.96f, 0.65f, 0.24f);
		}
		if (Item->UseEffect == ESurvivorItemUseEffect::RestoreHealth)
		{
			InspectionText = FString::Printf(
				TEXT("[BOŞLUK] KULLAN — %d can yeniler. "),
				FMath::RoundToInt(Item->HealthRestoreAmount)) + InspectionText;
			InspectionColor = FLinearColor(0.36f, 0.86f, 0.46f);
		}
		DrawWrappedText(
			InspectionText,
			DetailX + 18.0f * UIScale,
			DetailY + 124.0f * UIScale,
			GridWidth - 36.0f * UIScale,
			Font,
			InspectionColor,
			0.67f * UIScale,
			3);
	}

	if (SurvivorController->GetMoveSourceSlot() != INDEX_NONE)
	{
		DrawCenteredTextInArea(
			TEXT("Eşya seçildi — hedef yuvaya gidip E'ye bas."),
			0.0f,
			DetailY + DetailHeight + 9.0f * UIScale,
			Canvas->ClipX,
			Font,
			FLinearColor(0.32f, 0.72f, 1.0f),
			0.72f * UIScale);
	}

	DrawCenteredTextInArea(
		TEXT("WASD / Oklar: Gezin   E: Taşı   BOŞLUK: Kullan   F: İncele   R: Gereksiz anahtarı at   I / Tab: Kapat"),
		0.0f,
		Canvas->ClipY - 38.0f * UIScale,
		Canvas->ClipX,
		Font,
		FLinearColor(0.48f, 0.54f, 0.54f),
		0.62f * UIScale);
}

void ASurvivorHorrorHUD::DrawBorder(
	const float X,
	const float Y,
	const float Width,
	const float Height,
	const float Thickness,
	const FLinearColor& Color)
{
	DrawRect(Color, X, Y, Width, Thickness);
	DrawRect(Color, X, Y + Height - Thickness, Width, Thickness);
	DrawRect(Color, X, Y, Thickness, Height);
	DrawRect(Color, X + Width - Thickness, Y, Thickness, Height);
}

void ASurvivorHorrorHUD::DrawCenteredTextInArea(
	const FString& Text,
	const float X,
	const float Y,
	const float Width,
	UFont* Font,
	const FLinearColor& Color,
	const float Scale)
{
	float TextWidth = 0.0f;
	float TextHeight = 0.0f;
	Canvas->StrLen(Font, Text, TextWidth, TextHeight);
	DrawText(Text, Color, X + (Width - TextWidth * Scale) * 0.5f, Y, Font, Scale, false);
}

void ASurvivorHorrorHUD::DrawWrappedText(
	const FString& Text,
	const float X,
	const float Y,
	const float MaxWidth,
	UFont* Font,
	const FLinearColor& Color,
	const float Scale,
	const int32 MaxLines)
{
	TArray<FString> Words;
	Text.ParseIntoArrayWS(Words);
	if (Words.IsEmpty())
	{
		return;
	}

	TArray<FString> Lines;
	FString CurrentLine;
	for (const FString& Word : Words)
	{
		const FString Candidate = CurrentLine.IsEmpty() ? Word : CurrentLine + TEXT(" ") + Word;
		float CandidateWidth = 0.0f;
		float CandidateHeight = 0.0f;
		Canvas->StrLen(Font, Candidate, CandidateWidth, CandidateHeight);
		if (!CurrentLine.IsEmpty() && CandidateWidth * Scale > MaxWidth)
		{
			Lines.Add(CurrentLine);
			CurrentLine = Word;
			if (Lines.Num() >= MaxLines)
			{
				break;
			}
		}
		else
		{
			CurrentLine = Candidate;
		}
	}
	if (!CurrentLine.IsEmpty() && Lines.Num() < MaxLines)
	{
		Lines.Add(CurrentLine);
	}

	const float LineHeight = 24.0f * Scale;
	for (int32 LineIndex = 0; LineIndex < Lines.Num(); ++LineIndex)
	{
		DrawText(
			Lines[LineIndex],
			Color,
			X,
			Y + LineIndex * LineHeight,
			Font,
			Scale,
			false);
	}
}

void ASurvivorHorrorHUD::ShowNotification(const FText& Message, const float Duration)
{
	NotificationText = Message;
	NotificationEndTime = GetWorld()
		? GetWorld()->GetTimeSeconds() + FMath::Max(0.0f, Duration)
		: 0.0;
}
