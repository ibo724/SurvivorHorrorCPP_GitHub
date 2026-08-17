// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ClassicTankCharacter.generated.h"

class UStaticMeshComponent;
class USurvivorInventoryComponent;
class USurvivorInteractionComponent;

/**
 * Old-school survival-horror character movement:
 * - Forward/backward input moves along the character's own forward direction.
 * - Left/right input rotates the character instead of strafing.
 *
 * The values below can later be tuned from a Blueprint child class without
 * changing C++ code.
 */
UCLASS()
class SURVIVORHORRORCPP_API AClassicTankCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AClassicTankCharacter();

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	void MoveForward(float AxisValue);
	void Turn(float AxisValue);
	void Interact();

	/** Character-facing interaction detection; independent of the room camera. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USurvivorInteractionComponent> InteractionComponent;

	/** Runtime item storage; its final capacity and UI will be decided later. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USurvivorInventoryComponent> InventoryComponent;

	/** Temporary visible body so the blank project is testable before character art is imported. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank Controls|Preview", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> PreviewBody;

	/** Maximum forward movement speed in centimetres per second. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tank Controls|Movement", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", Units = "cm/s"))
	float ForwardSpeed = 180.0f;

	/** Reverse speed as a fraction of ForwardSpeed. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tank Controls|Movement", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", ClampMax = "1.0"))
	float BackwardSpeedMultiplier = 0.65f;

	/** Rotation speed while holding left or right. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tank Controls|Movement", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", Units = "deg/s"))
	float TurnRate = 95.0f;
};
