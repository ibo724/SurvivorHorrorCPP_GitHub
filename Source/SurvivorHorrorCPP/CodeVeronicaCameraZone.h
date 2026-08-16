// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CodeVeronicaCameraZone.generated.h"

class APawn;
class UBoxComponent;
class UCameraComponent;
class USceneComponent;
class USplineComponent;
class UPrimitiveComponent;

/**
 * A room-sized camera zone inspired by Resident Evil Code: Veronica.
 *
 * When the player enters the trigger, this actor becomes the active camera.
 * The camera moves along an editable spline to stay near the player and keeps
 * looking toward the player with configurable cinematic lag.
 */
UCLASS(Blueprintable)
class SURVIVORHORRORCPP_API ACodeVeronicaCameraZone : public AActor
{
	GENERATED_BODY()

public:
	ACodeVeronicaCameraZone();

	virtual void Tick(float DeltaSeconds) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult) override;

	void StartTracking(APawn* PlayerPawn);
	void StopTracking();

	float GetCameraTransitionTime() const { return CameraTransitionTime; }

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void HandleTriggerBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	void TryActivateForPawn(APawn* PlayerPawn);
	void UpdateCameraTransform(float DeltaSeconds, bool bSnapImmediately);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera Zone|Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> SceneRoot;

	/** The room area in which this camera becomes active. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera Zone|Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> TriggerVolume;

	/** Editable path along which the camera is allowed to travel. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera Zone|Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USplineComponent> CameraRail;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera Zone|Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> TrackingCamera;

	/** Point above the pawn origin toward which the camera looks. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Zone|Tracking", meta = (AllowPrivateAccess = "true"))
	FVector LookAtOffset = FVector(0.0f, 0.0f, 60.0f);

	/** Higher values make the camera catch up to the player more quickly. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Zone|Tracking", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float PositionLagSpeed = 3.0f;

	/** Higher values make the camera turn toward the player more quickly. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Zone|Tracking", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float RotationLagSpeed = 6.0f;

	/** Blend duration when entering this camera zone. Set to zero for a hard cut. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Zone|Transition", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", Units = "s"))
	float CameraTransitionTime = 0.25f;

	/** Horizontal field of view for this room camera. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Zone|Lens", meta = (AllowPrivateAccess = "true", ClampMin = "5.0", ClampMax = "170.0", Units = "deg"))
	float CameraFieldOfView = 50.0f;

	UPROPERTY(Transient)
	TObjectPtr<APawn> TrackedPawn;
};
