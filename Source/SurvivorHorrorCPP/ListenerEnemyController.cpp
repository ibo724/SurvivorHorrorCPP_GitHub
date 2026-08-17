// Copyright Epic Games, Inc. All Rights Reserved.

#include "ListenerEnemyController.h"

#include "GameFramework/Pawn.h"
#include "ListenerEnemy.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISense_Hearing.h"

AListenerEnemyController::AListenerEnemyController()
{
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("HearingPerception"));
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));

	// The placed enemy performs the final per-instance distance check. This broad
	// listener range only makes sure a tunable enemy is not clipped prematurely.
	HearingConfig->HearingRange = 5000.0f;
	HearingConfig->SetMaxAge(3.0f);
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
	PerceptionComponent->ConfigureSense(*HearingConfig);
	PerceptionComponent->SetDominantSense(UAISense_Hearing::StaticClass());
}

void AListenerEnemyController::BeginPlay()
{
	Super::BeginPlay();
	if (IsValid(PerceptionComponent))
	{
		PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(
			this,
			&AListenerEnemyController::HandleTargetPerceptionUpdated);
	}
}

void AListenerEnemyController::HandleTargetPerceptionUpdated(
	AActor* Actor,
	const FAIStimulus Stimulus)
{
	const APawn* SensedPawn = Cast<APawn>(Actor);
	AListenerEnemy* ListenerEnemy = Cast<AListenerEnemy>(GetPawn());
	if (!Stimulus.WasSuccessfullySensed()
		|| !IsValid(SensedPawn)
		|| !SensedPawn->IsPlayerControlled()
		|| !IsValid(ListenerEnemy))
	{
		return;
	}

	ListenerEnemy->HearNoise(Actor, Stimulus.StimulusLocation, Stimulus.Strength);
}
