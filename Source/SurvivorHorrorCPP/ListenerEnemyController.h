// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AIController.h"
#include "CoreMinimal.h"
#include "Perception/AIPerceptionTypes.h"
#include "ListenerEnemyController.generated.h"

class UAISenseConfig_Hearing;

/** Hearing adapter kept separate from the enemy's decision/state code. */
UCLASS()
class SURVIVORHORRORCPP_API AListenerEnemyController : public AAIController
{
	GENERATED_BODY()

public:
	AListenerEnemyController();

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	UPROPERTY()
	TObjectPtr<UAISenseConfig_Hearing> HearingConfig;
};
