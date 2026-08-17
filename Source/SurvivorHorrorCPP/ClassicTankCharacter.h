// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ClassicTankCharacter.generated.h"

class UStaticMeshComponent;
class USurvivorHealthComponent;
class USurvivorInventoryComponent;
class USurvivorInteractionComponent;
class USurvivorItemUseComponent;
class USurvivorNoiseEmitterComponent;

/**
 * Old-school survival-horror character movement:
 * - Forward/backward input moves along the character's own forward direction.
 * - Left/right input rotates the character instead of strafing.
 * - Holding Run increases forward speed but never accelerates reverse movement.
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
	virtual float TakeDamage(
		float DamageAmount,
		const FDamageEvent& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser) override;

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	void MoveForward(float AxisValue);
	void Turn(float AxisValue);
	void Interact();
	void StartRunning();
	void StopRunning();
	void PerformQuickTurn();
	bool IsDead() const;

	UFUNCTION()
	void HandleDeath();

	/** Character-facing interaction detection; independent of the room camera. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USurvivorInteractionComponent> InteractionComponent;

	/** Runtime item storage; its final capacity and UI will be decided later. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USurvivorInventoryComponent> InventoryComponent;

	/** Reusable health; future healing items and weapons will talk only to this component. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USurvivorHealthComponent> HealthComponent;

	/** Reports walking/running and bag noise to hearing-based AI. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Noise", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USurvivorNoiseEmitterComponent> NoiseEmitterComponent;

	/** Executes tunable consumable effects selected from the inventory. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USurvivorItemUseComponent> ItemUseComponent;

	/** Temporary visible body so the blank project is testable before character art is imported. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank Controls|Preview", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> PreviewBody;

	/** Maximum forward movement speed in centimetres per second. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tank Controls|Movement", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", Units = "cm/s"))
	float ForwardSpeed = 180.0f;

	/** Forward speed while the run button is held. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tank Controls|Movement", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", Units = "cm/s"))
	float RunSpeed = 320.0f;

	/** Reverse speed as a fraction of ForwardSpeed. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tank Controls|Movement", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", ClampMax = "1.0"))
	float BackwardSpeedMultiplier = 0.65f;

	/** Rotation speed while holding left or right. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tank Controls|Movement", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", Units = "deg/s"))
	float TurnRate = 95.0f;

	/** Snap rotation used by Run + Backward. Animation will be added with the real character. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tank Controls|Movement", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", ClampMax = "360.0", Units = "deg"))
	float QuickTurnAngle = 180.0f;

	bool bRunInputHeld = false;
	bool bQuickTurnLatched = false;
	float LastMoveAxisValue = 0.0f;
};
