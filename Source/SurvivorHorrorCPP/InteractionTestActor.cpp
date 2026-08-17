// Copyright Epic Games, Inc. All Rights Reserved.

#include "InteractionTestActor.h"

#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

AInteractionTestActor::AInteractionTestActor()
{
	PrimaryActorTick.bCanEverTick = false;

	TestMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TestMesh"));
	SetRootComponent(TestMesh);
	TestMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	TestMesh->SetCollisionObjectType(ECC_WorldDynamic);
	TestMesh->SetCollisionResponseToAllChannels(ECR_Block);
	TestMesh->SetRelativeScale3D(FVector(0.6f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(
		TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		TestMesh->SetStaticMesh(CubeMesh.Object);
	}

	FocusLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("FocusLight"));
	FocusLight->SetupAttachment(TestMesh);
	FocusLight->SetRelativeLocation(FVector(0.0f, 0.0f, 110.0f));
	FocusLight->SetAttenuationRadius(300.0f);
	FocusLight->SetLightColor(FLinearColor(0.15f, 0.35f, 1.0f));
	FocusLight->SetIntensity(0.0f);

	PromptText = NSLOCTEXT("SurvivorInteraction", "TestActorPrompt", "Test nesnesini incele");
}

bool AInteractionTestActor::CanInteract_Implementation(APawn* InteractingPawn) const
{
	return IsValid(InteractingPawn);
}

FText AInteractionTestActor::GetInteractionPrompt_Implementation(APawn* InteractingPawn) const
{
	return PromptText;
}

void AInteractionTestActor::Interact_Implementation(APawn* InteractingPawn)
{
	bWasActivated = !bWasActivated;
	RefreshLight();

	const FString Message = bWasActivated
		? TEXT("Etkileşim başarılı: test nesnesi etkinleştirildi.")
		: TEXT("Etkileşim başarılı: test nesnesi kapatıldı.");

	UE_LOG(LogTemp, Log, TEXT("%s"), *Message);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor::Green, Message);
	}
}

void AInteractionTestActor::BeginInteractionFocus_Implementation(APawn* InteractingPawn)
{
	bIsFocused = true;
	RefreshLight();
}

void AInteractionTestActor::EndInteractionFocus_Implementation(APawn* InteractingPawn)
{
	bIsFocused = false;
	RefreshLight();
}

void AInteractionTestActor::RefreshLight()
{
	if (bWasActivated)
	{
		FocusLight->SetLightColor(FLinearColor(0.1f, 1.0f, 0.2f));
		FocusLight->SetIntensity(2500.0f);
		return;
	}

	FocusLight->SetLightColor(FLinearColor(0.15f, 0.35f, 1.0f));
	FocusLight->SetIntensity(bIsFocused ? 1500.0f : 0.0f);
}
