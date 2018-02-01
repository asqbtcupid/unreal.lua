// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "LuaDebuggerCommands.h"

#define LOCTEXT_NAMESPACE "FLuaDebuggerModule"

void FLuaDebuggerCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "LuaDebugger", "Bring up LuaDebugger window", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
