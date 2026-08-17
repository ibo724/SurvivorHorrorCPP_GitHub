// Copyright Epic Games, Inc. All Rights Reserved.

#include "SurvivorHorrorGameMode.h"

#include "ClassicTankCharacter.h"
#include "SurvivorHorrorHUD.h"
#include "SurvivorHorrorPlayerController.h"

ASurvivorHorrorGameMode::ASurvivorHorrorGameMode()
{
	DefaultPawnClass = AClassicTankCharacter::StaticClass();
	PlayerControllerClass = ASurvivorHorrorPlayerController::StaticClass();
	HUDClass = ASurvivorHorrorHUD::StaticClass();
}
