// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ListenerEnemy.generated.h"

class UMaterialInstanceDynamic;
class UPointLightComponent;
class UStaticMeshComponent;
class USurvivorHealthComponent;

UENUM(BlueprintType)
enum class EListenerEnemyState : uint8
{
	Idle,
	Investigating,
	Searching,
	Chasing,
	Attacking,
	Dead
};

/**
 * Removable hearing-focused enemy prototype.
 * Model, presentation and final creature fiction are intentionally not embedded here.
 */
UCLASS()
class SURVIVORHORRORCPP_API AListenerEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	AListenerEnemy();

	virtual void Tick(float DeltaSeconds) override;
	virtual float TakeDamage(
		float DamageAmount,
		const FDamageEvent& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser) override;

	/** Called by the hearing controller after it receives a valid player noise. */
	void HearNoise(AActor* NoiseActor, const FVector& NoiseLocation, float Loudness);

	UFUNCTION(BlueprintPure, Category = "Listener Enemy")
	EListenerEnemyState GetCurrentState() const { return CurrentState; }

protected:
	virtual void BeginPlay() override;

private:
	void SetState(EListenerEnemyState NewState);
	void UpdateState(float DeltaSeconds);
	void StartInvestigating(const FVector& Location);
	void TryAttack(APawn* PlayerPawn);
	bool CanDetectPlayerAtCloseRange(APawn* PlayerPawn) const;
	bool IsLivingPlayer(const APawn* PlayerPawn) const;
	void UpdatePreviewForState();
	FString GetDebugStateText() const;
	FLinearColor GetDebugStateColor() const;

	UFUNCTION()
	void HandleDeath();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Listener Enemy|Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USurvivorHealthComponent> HealthComponent;

	/** Temporary blockout body; replace it in a Blueprint child without changing AI code. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Listener Enemy|Prototype", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> PreviewBody;

	/** Temporary state light makes hearing/chasing transitions readable during testing. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Listener Enemy|Prototype", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPointLightComponent> StateLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Listener Enemy|Hearing", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", Units = "cm"))
	float HearingDistance = 1200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Listener Enemy|Detection", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", Units = "cm"))
	float CloseDetectionDistance = 210.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Listener Enemy|Detection", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", Units = "cm"))
	float LoseChaseDistance = 700.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Listener Enemy|Detection", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", Units = "s"))
	float ChaseMemoryDuration = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Listener Enemy|Movement", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", Units = "cm/s"))
	float InvestigateSpeed = 125.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Listener Enemy|Movement", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", Units = "cm/s"))
	float ChaseSpeed = 190.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Listener Enemy|Investigation", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", Units = "s"))
	float SearchDuration = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Listener Enemy|Investigation", meta = (AllowPrivateAccess = "true", ClampMin = "1.0", Units = "cm"))
	float SearchAcceptanceRadius = 65.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Listener Enemy|Investigation", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", Units = "deg/s"))
	float SearchTurnRate = 80.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Listener Enemy|Attack", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", Units = "cm"))
	float AttackDistance = 115.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Listener Enemy|Attack", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float AttackDamage = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Listener Enemy|Attack", meta = (AllowPrivateAccess = "true", ClampMin = "0.05", Units = "s"))
	float AttackCooldown = 1.6f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Listener Enemy|Prototype", meta = (AllowPrivateAccess = "true"))
	bool bShowDebugState = true;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Listener Enemy", meta = (AllowPrivateAccess = "true"))
	EListenerEnemyState CurrentState = EListenerEnemyState::Idle;

	UPROPERTY(Transient)
	TObjectPtr<AActor> HeardActor;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> PreviewMaterial;

	FVector LastHeardLocation = FVector::ZeroVector;
	float LastHeardTime = -BIG_NUMBER;
	float LastCloseDetectionTime = -BIG_NUMBER;
	float LastAttackTime = -BIG_NUMBER;
	float SearchEndTime = 0.0f;
	float AttackStateEndTime = 0.0f;
};
