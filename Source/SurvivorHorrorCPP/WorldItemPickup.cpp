// Copyright Epic Games, Inc. All Rights Reserved.

#include "WorldItemPickup.h"

#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/PlayerController.h"
#include "SurvivorHorrorHUD.h"
#include "SurvivorInventoryComponent.h"
#include "SurvivorItemDefinition.h"
#include "UObject/ConstructorHelpers.h"

AWorldItemPickup::AWorldItemPickup()
{
	PrimaryActorTick.bCanEverTick = false;

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	SetRootComponent(PickupMesh);
	PickupMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	PickupMesh->SetCollisionObjectType(ECC_WorldDynamic);
	PickupMesh->SetCollisionResponseToAllChannels(ECR_Block);
	PickupMesh->SetRelativeScale3D(FVector(0.30f, 0.30f, 0.12f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(
		TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		PickupMesh->SetStaticMesh(CubeMesh.Object);
	}

	FocusLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("FocusLight"));
	FocusLight->SetupAttachment(PickupMesh);
	FocusLight->SetRelativeLocation(FVector(0.0f, 0.0f, 55.0f));
	FocusLight->SetAttenuationRadius(180.0f);
	FocusLight->SetLightColor(FLinearColor(1.0f, 0.72f, 0.18f));
	FocusLight->SetIntensity(0.0f);
}

bool AWorldItemPickup::CanInteract_Implementation(APawn* InteractingPawn) const
{
	return IsValid(InteractingPawn)
		&& IsValid(ItemDefinition)
		&& Quantity > 0
		&& IsValid(InteractingPawn->FindComponentByClass<USurvivorInventoryComponent>());
}

FText AWorldItemPickup::GetInteractionPrompt_Implementation(APawn* InteractingPawn) const
{
	if (!IsValid(ItemDefinition))
	{
		return FText::GetEmpty();
	}

	const FText ItemName = ItemDefinition->DisplayName.IsEmpty()
		? FText::FromName(ItemDefinition->ItemId)
		: ItemDefinition->DisplayName;
	return FText::Format(
		NSLOCTEXT("SurvivorPickup", "PickupPrompt", "{0} al"),
		ItemName);
}

void AWorldItemPickup::Interact_Implementation(APawn* InteractingPawn)
{
	if (!CanInteract_Implementation(InteractingPawn))
	{
		return;
	}

	USurvivorInventoryComponent* Inventory =
		InteractingPawn->FindComponentByClass<USurvivorInventoryComponent>();
	const int32 AddedQuantity = Inventory->AddItem(ItemDefinition, Quantity);
	if (AddedQuantity <= 0)
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(InteractingPawn->GetController()))
		{
			if (ASurvivorHorrorHUD* HUD = Cast<ASurvivorHorrorHUD>(PlayerController->GetHUD()))
			{
				HUD->ShowNotification(
					NSLOCTEXT("SurvivorPickup", "InventoryFull", "Daha fazla taşıyamam."));
			}
		}
		return;
	}

	const FText ItemName = ItemDefinition->DisplayName.IsEmpty()
		? FText::FromName(ItemDefinition->ItemId)
		: ItemDefinition->DisplayName;
	const FText Message = AddedQuantity == 1
		? FText::Format(NSLOCTEXT("SurvivorPickup", "PickedUpOne", "{0} alındı."), ItemName)
		: FText::Format(
			NSLOCTEXT("SurvivorPickup", "PickedUpMany", "{0} alındı. Adet: {1}"),
			ItemName,
			FText::AsNumber(AddedQuantity));

	UE_LOG(LogTemp, Log, TEXT("Pickup collected: %s x%d"), *ItemName.ToString(), AddedQuantity);

	if (APlayerController* PlayerController = Cast<APlayerController>(InteractingPawn->GetController()))
	{
		if (ASurvivorHorrorHUD* HUD = Cast<ASurvivorHorrorHUD>(PlayerController->GetHUD()))
		{
			HUD->ShowNotification(Message);
		}
	}

	Quantity -= AddedQuantity;
	if (Quantity <= 0)
	{
		Destroy();
	}
}

void AWorldItemPickup::BeginInteractionFocus_Implementation(APawn* InteractingPawn)
{
	SetFocused(true);
}

void AWorldItemPickup::EndInteractionFocus_Implementation(APawn* InteractingPawn)
{
	SetFocused(false);
}

void AWorldItemPickup::SetFocused(const bool bNewFocused)
{
	bIsFocused = bNewFocused;
	FocusLight->SetIntensity(bIsFocused ? 1200.0f : 0.0f);
}
