// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "LuaEditorPluginsCommands.h"

#define LOCTEXT_NAMESPACE "FLuaEditorPluginsModule"

void FLuaEditorPluginsCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "LuaEditorPlugins", "Bring up LuaEditorPlugins window", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
