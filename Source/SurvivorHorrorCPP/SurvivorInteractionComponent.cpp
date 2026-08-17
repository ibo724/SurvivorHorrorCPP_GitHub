// Copyright Epic Games, Inc. All Rights Reserved.

#include "SurvivorInteractionComponent.h"

#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "SurvivorInteractable.h"

USurvivorInteractionComponent::USurvivorInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_PostPhysics;
	PrimaryComponentTick.TickInterval = 0.05f;
}

void USurvivorInteractionComponent::TickComponent(
	const float DeltaTime,
	const ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	RefreshFocusedActor();
}

void USurvivorInteractionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	SetFocusedActor(nullptr);
	Super::EndPlay(EndPlayReason);
}

void USurvivorInteractionComponent::TryInteract()
{
	// Refresh immediately so a key press between scheduled component ticks never
	// interacts with an object that the character has just turned away from.
	RefreshFocusedActor();

	APawn* OwnerPawn = GetOwnerPawn();
	if (!IsValid(OwnerPawn) || !IsValid(FocusedActor))
	{
		return;
	}

	if (FocusedActor->GetClass()->ImplementsInterface(USurvivorInteractable::StaticClass())
		&& ISurvivorInteractable::Execute_CanInteract(FocusedActor, OwnerPawn))
	{
		ISurvivorInteractable::Execute_Interact(FocusedActor, OwnerPawn);
	}
}

FText USurvivorInteractionComponent::GetCurrentInteractionPrompt() const
{
	APawn* OwnerPawn = GetOwnerPawn();
	if (!IsValid(OwnerPawn) || !IsValid(FocusedActor)
		|| !FocusedActor->GetClass()->ImplementsInterface(USurvivorInteractable::StaticClass()))
	{
		return FText::GetEmpty();
	}

	if (!ISurvivorInteractable::Execute_CanInteract(FocusedActor, OwnerPawn))
	{
		return FText::GetEmpty();
	}

	return ISurvivorInteractable::Execute_GetInteractionPrompt(FocusedActor, OwnerPawn);
}

void USurvivorInteractionComponent::RefreshFocusedActor()
{
	SetFocusedActor(FindBestInteractable());
}

AActor* USurvivorInteractionComponent::FindBestInteractable() const
{
	APawn* OwnerPawn = GetOwnerPawn();
	UWorld* World = GetWorld();
	if (!IsValid(OwnerPawn) || !IsValid(World))
	{
		return nullptr;
	}

	const FVector Start = OwnerPawn->GetActorLocation()
		+ OwnerPawn->GetActorUpVector() * InteractionHeightOffset;
	const FVector End = Start
		+ OwnerPawn->GetActorForwardVector() * InteractionDistance;

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(SurvivorInteraction), false, OwnerPawn);
	const FCollisionObjectQueryParams ObjectQueryParams(FCollisionObjectQueryParams::AllObjects);
	const FCollisionShape ScanShape = FCollisionShape::MakeCapsule(
		InteractionRadius,
		FMath::Max(InteractionHalfHeight, InteractionRadius));

	TArray<FHitResult> Hits;
	World->SweepMultiByObjectType(
		Hits,
		Start,
		End,
		FQuat::Identity,
		ObjectQueryParams,
		ScanShape,
		QueryParams);

	AActor* BestActor = nullptr;
	float BestHitDistance = TNumericLimits<float>::Max();
	TSet<AActor*> ExaminedActors;

	for (const FHitResult& Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();
		if (!IsValid(HitActor) || ExaminedActors.Contains(HitActor))
		{
			continue;
		}

		ExaminedActors.Add(HitActor);
		if (!HitActor->GetClass()->ImplementsInterface(USurvivorInteractable::StaticClass())
			|| !ISurvivorInteractable::Execute_CanInteract(HitActor, OwnerPawn))
		{
			continue;
		}

		if (Hit.Distance < BestHitDistance)
		{
			BestHitDistance = Hit.Distance;
			BestActor = HitActor;
		}
	}

	return BestActor;
}

void USurvivorInteractionComponent::SetFocusedActor(AActor* NewFocusedActor)
{
	if (FocusedActor == NewFocusedActor)
	{
		return;
	}

	APawn* OwnerPawn = GetOwnerPawn();
	if (IsValid(FocusedActor)
		&& FocusedActor->GetClass()->ImplementsInterface(USurvivorInteractable::StaticClass()))
	{
		ISurvivorInteractable::Execute_EndInteractionFocus(FocusedActor, OwnerPawn);
	}

	FocusedActor = NewFocusedActor;
	if (IsValid(FocusedActor)
		&& FocusedActor->GetClass()->ImplementsInterface(USurvivorInteractable::StaticClass()))
	{
		ISurvivorInteractable::Execute_BeginInteractionFocus(FocusedActor, OwnerPawn);
	}
}

APawn* USurvivorInteractionComponent::GetOwnerPawn() const
{
	return Cast<APawn>(GetOwner());
}
