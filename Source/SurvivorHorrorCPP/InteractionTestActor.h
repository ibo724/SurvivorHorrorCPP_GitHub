// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SurvivorInteractable.h"
#include "InteractionTestActor.generated.h"

class APawn;
class UPointLightComponent;
class UStaticMeshComponent;

/** Temporary actor used only to verify the reusable interaction system. */
UCLASS(Blueprintable)
class SURVIVORHORRORCPP_API AInteractionTestActor
	: public AActor
	, public ISurvivorInteractable
{
	GENERATED_BODY()

public:
	AInteractionTestActor();

	virtual bool CanInteract_Implementation(APawn* InteractingPawn) const override;
	virtual FText GetInteractionPrompt_Implementation(APawn* InteractingPawn) const override;
	virtual void Interact_Implementation(APawn* InteractingPawn) override;
	virtual void BeginInteractionFocus_Implementation(APawn* InteractingPawn) override;
	virtual void EndInteractionFocus_Implementation(APawn* InteractingPawn) override;

private:
	void RefreshLight();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction Test", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> TestMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction Test", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPointLightComponent> FocusLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction Test", meta = (AllowPrivateAccess = "true"))
	FText PromptText;

	bool bIsFocused = false;
	bool bWasActivated = false;
};
