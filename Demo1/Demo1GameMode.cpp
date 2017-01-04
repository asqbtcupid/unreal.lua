// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "Demo1.h"
#include "Demo1GameMode.h"
#include "Demo1HUD.h"
#include "Demo1Character.h"

ADemo1GameMode::ADemo1GameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ADemo1HUD::StaticClass();
}
