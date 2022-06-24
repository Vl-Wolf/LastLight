// Copyright Epic Games, Inc. All Rights Reserved.

#include "LastLightGameMode.h"
#include "LastLightHUD.h"
#include "LastLightCharacter.h"
#include "UObject/ConstructorHelpers.h"

ALastLightGameMode::ALastLightGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ALastLightHUD::StaticClass();
}
