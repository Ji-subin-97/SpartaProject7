// Copyright Epic Games, Inc. All Rights Reserved.
#include "SpartaGameMode.h"
#include "CharacterPawn.h"
#include "SpartaPlayerController.h"

ASpartaGameMode::ASpartaGameMode()
{
	DefaultPawnClass = ACharacterPawn::StaticClass();
	PlayerControllerClass = ASpartaPlayerController::StaticClass();
}

