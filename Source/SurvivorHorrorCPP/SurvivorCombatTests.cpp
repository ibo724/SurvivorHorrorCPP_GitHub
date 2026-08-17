// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "SurvivorHealthComponent.h"
#include "SurvivorInventoryComponent.h"
#include "SurvivorItemDefinition.h"
#include "SurvivorItemUseComponent.h"
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

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FSurvivorHealthItemUseTest,
	"SurvivorHorror.Inventory.HealthItemUse",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FSurvivorHealthItemUseTest::RunTest(const FString& Parameters)
{
	USurvivorInventoryComponent* Inventory = NewObject<USurvivorInventoryComponent>();
	USurvivorHealthComponent* Health = NewObject<USurvivorHealthComponent>();
	USurvivorItemDefinition* HealthItem = NewObject<USurvivorItemDefinition>();
	HealthItem->ItemId = TEXT("automation_health_item");
	HealthItem->MaxStackSize = 2;
	HealthItem->UseEffect = ESurvivorItemUseEffect::RestoreHealth;
	HealthItem->HealthRestoreAmount = 50.0f;

	TestEqual(TEXT("Two health items fit in one test stack"), Inventory->AddItem(HealthItem, 2), 2);
	TestEqual(
		TEXT("A health item cannot be wasted at full health"),
		USurvivorItemUseComponent::TryUseItemForComponents(Inventory, Health, 0),
		ESurvivorItemUseResult::HealthFull);
	TestEqual(TEXT("Full-health rejection consumes nothing"), Inventory->GetItemQuantity(HealthItem), 2);

	Health->ApplyDamage(75.0f);
	TestEqual(TEXT("Test actor is reduced to 25 health"), Health->GetCurrentHealth(), 25.0f);
	TestEqual(
		TEXT("First health item succeeds"),
		USurvivorItemUseComponent::TryUseItemForComponents(Inventory, Health, 0),
		ESurvivorItemUseResult::Success);
	TestEqual(TEXT("First item restores 50 health"), Health->GetCurrentHealth(), 75.0f);
	TestEqual(TEXT("Exactly one item is consumed"), Inventory->GetItemQuantity(HealthItem), 1);

	TestEqual(
		TEXT("Second health item succeeds"),
		USurvivorItemUseComponent::TryUseItemForComponents(Inventory, Health, 0),
		ESurvivorItemUseResult::Success);
	TestEqual(TEXT("Healing clamps at maximum health"), Health->GetCurrentHealth(), 100.0f);
	TestEqual(TEXT("Second item empties the stack"), Inventory->GetItemQuantity(HealthItem), 0);

	Health->ApplyDamage(100.0f);
	Inventory->AddItem(HealthItem, 1);
	TestEqual(
		TEXT("A dead actor cannot use a health item"),
		USurvivorItemUseComponent::TryUseItemForComponents(Inventory, Health, 0),
		ESurvivorItemUseResult::UserDead);
	TestEqual(TEXT("Death rejection consumes nothing"), Inventory->GetItemQuantity(HealthItem), 1);

	return true;
}

#endif
