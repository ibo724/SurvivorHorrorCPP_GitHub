// Copyright Epic Games, Inc. All Rights Reserved.

#include "SurvivorHorrorGameMode.h"

#include "ClassicTankCharacter.h"

ASurvivorHorrorGameMode::ASurvivorHorrorGameMode()
{
	DefaultPawnClass = AClassicTankCharacter::StaticClass();
}
