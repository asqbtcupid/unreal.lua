// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "VarWatcherCommands.h"

#define LOCTEXT_NAMESPACE "FVarWatcherModule"

void FVarWatcherCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "VarWatcher", "Bring up VarWatcher window", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
