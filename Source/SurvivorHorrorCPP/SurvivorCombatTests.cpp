// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "SurvivorHealthComponent.h"
#include "SurvivorNoiseEmitterComponent.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FSurvivorHealthThresholdsTest,
	"SurvivorHorror.Combat.HealthThresholdsAndDeath",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FSurvivorHealthThresholdsTest::RunTest(const FString& Parameters)
{
	USurvivorHealthComponent* Health = NewObject<USurvivorHealthComponent>();
	TestEqual(TEXT("Health starts full"), Health->GetCurrentHealth(), 100.0f);
	TestEqual(TEXT("Full health is healthy"), Health->GetHealthState(), ESurvivorHealthState::Healthy);

	TestEqual(TEXT("First attack applies 25 damage"), Health->ApplyDamage(25.0f), 25.0f);
	TestEqual(TEXT("75 health remains healthy"), Health->GetHealthState(), ESurvivorHealthState::Healthy);
	TestEqual(TEXT("Second attack applies 25 damage"), Health->ApplyDamage(25.0f), 25.0f);
	TestEqual(TEXT("50 health is warning"), Health->GetHealthState(), ESurvivorHealthState::Warning);
	TestEqual(TEXT("Third attack applies 25 damage"), Health->ApplyDamage(25.0f), 25.0f);
	TestEqual(TEXT("25 health is danger"), Health->GetHealthState(), ESurvivorHealthState::Danger);
	TestEqual(TEXT("Fourth attack applies 25 damage"), Health->ApplyDamage(25.0f), 25.0f);
	TestTrue(TEXT("Four 25-damage attacks kill a full-health actor"), Health->IsDead());
	TestEqual(TEXT("Dead health state is exposed"), Health->GetHealthState(), ESurvivorHealthState::Dead);
	TestEqual(TEXT("Damage after death is ignored"), Health->ApplyDamage(25.0f), 0.0f);
	TestEqual(TEXT("Healing after death is ignored"), Health->Heal(25.0f), 0.0f);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FSurvivorMovementNoiseTest,
	"SurvivorHorror.Combat.MovementAndBagNoise",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FSurvivorMovementNoiseTest::RunTest(const FString& Parameters)
{
	const float Silent = USurvivorNoiseEmitterComponent::CalculateMovementLoudness(0.0f, 20);
	const float QuietWalk = USurvivorNoiseEmitterComponent::CalculateMovementLoudness(180.0f, 0);
	const float NoisyWalk = USurvivorNoiseEmitterComponent::CalculateMovementLoudness(180.0f, 7);
	const float Run = USurvivorNoiseEmitterComponent::CalculateMovementLoudness(320.0f, 0);

	TestEqual(TEXT("Standing still emits no movement noise even with a noisy bag"), Silent, 0.0f);
	TestTrue(TEXT("Walking emits noise"), QuietWalk > 0.0f);
	TestTrue(TEXT("Bag arrangement makes the same walk louder"), NoisyWalk > QuietWalk);
	TestTrue(TEXT("Running is louder than an empty-bag walk"), Run > QuietWalk);
	TestTrue(TEXT("Calculated loudness respects its maximum"),
		USurvivorNoiseEmitterComponent::CalculateMovementLoudness(320.0f, 100) <= 1.50f);

	return true;
}

#endif
