// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SurvivorInteractionComponent.generated.h"

class AActor;
class APawn;

/** Detects and activates interactable objects in front of the owning pawn. */
UCLASS(ClassGroup = (SurvivorHorror), meta = (BlueprintSpawnableComponent))
class SURVIVORHORRORCPP_API USurvivorInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USurvivorInteractionComponent();

	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void TryInteract();

	UFUNCTION(BlueprintPure, Category = "Interaction")
	FText GetCurrentInteractionPrompt() const;

	UFUNCTION(BlueprintPure, Category = "Interaction")
	AActor* GetFocusedActor() const { return FocusedActor; }

private:
	void RefreshFocusedActor();
	AActor* FindBestInteractable() const;
	void SetFocusedActor(AActor* NewFocusedActor);
	APawn* GetOwnerPawn() const;

	/** How far in front of the character an interaction can be found. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Detection", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", Units = "cm"))
	float InteractionDistance = 160.0f;

	/** Radius of the forgiving interaction capsule. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Detection", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", Units = "cm"))
	float InteractionRadius = 40.0f;

	/** Vertical half-height lets the same scan find doors and small floor items. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Detection", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", Units = "cm"))
	float InteractionHalfHeight = 70.0f;

	/** Raises the scan from the pawn origin toward chest height. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Detection", meta = (AllowPrivateAccess = "true", Units = "cm"))
	float InteractionHeightOffset = 35.0f;

	UPROPERTY(Transient)
	TObjectPtr<AActor> FocusedActor;
};
