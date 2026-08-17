// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "SurvivorInventoryComponent.h"
#include "SurvivorItemDefinition.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FSurvivorInventoryCapacityTest,
	"SurvivorHorror.Inventory.CapacityAndArrangement",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FSurvivorInventoryCapacityTest::RunTest(const FString& Parameters)
{
	USurvivorItemDefinition* SingleItem = NewObject<USurvivorItemDefinition>();
	SingleItem->ItemId = TEXT("automation_single_item");
	SingleItem->MaxStackSize = 1;
	SingleItem->NoiseClass = ESurvivorItemNoiseClass::Silent;

	USurvivorInventoryComponent* CapacityInventory =
		NewObject<USurvivorInventoryComponent>();
	TestEqual(TEXT("Inventory exposes eight physical slots"), CapacityInventory->GetSlotCount(), 8);
	TestEqual(TEXT("Eight single items fit"), CapacityInventory->AddItem(SingleItem, 8), 8);
	TestEqual(TEXT("Ninth single item is rejected"), CapacityInventory->AddItem(SingleItem, 1), 0);

	USurvivorItemDefinition* MetalItem = NewObject<USurvivorItemDefinition>();
	MetalItem->ItemId = TEXT("automation_metal_item");
	MetalItem->MaxStackSize = 1;
	MetalItem->NoiseClass = ESurvivorItemNoiseClass::Metallic;

	USurvivorInventoryComponent* NoiseInventory =
		NewObject<USurvivorInventoryComponent>();
	TestEqual(TEXT("Two metal items are added"), NoiseInventory->AddItem(MetalItem, 2), 2);
	TestEqual(TEXT("Adjacent metal items include the adjacency penalty"), NoiseInventory->GetBagNoiseScore(), 7);
	TestTrue(TEXT("Item can move to a distant empty slot"), NoiseInventory->MoveOrSwapSlots(1, 7));
	TestEqual(TEXT("Separating metal items reduces bag noise"), NoiseInventory->GetBagNoiseScore(), 4);

	TestFalse(TEXT("Item starts uninspected"), NoiseInventory->IsItemInspected(MetalItem));
	TestTrue(TEXT("First inspection creates a memory"), NoiseInventory->InspectItemAtSlot(0));
	TestTrue(TEXT("Inspection memory is retained"), NoiseInventory->IsItemInspected(MetalItem));
	TestFalse(TEXT("Repeated inspection does not create a second memory"), NoiseInventory->InspectItemAtSlot(0));

	return true;
}

#endif
