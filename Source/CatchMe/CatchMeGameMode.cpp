// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "CatchMeGameMode.h"
#include "UnrealLua.h"

ACatchMeGameMode::ACatchMeGameMode()
{
	LuaCtor("gameplay.gamemode.cmgamemode", this);
}

void ACatchMeGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	LuaCall("EndPlay", this, int(EndPlayReason));
	Super::EndPlay(EndPlayReason);
}
