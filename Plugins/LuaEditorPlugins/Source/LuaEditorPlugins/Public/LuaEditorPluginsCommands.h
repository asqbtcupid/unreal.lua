// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "LuaEditorPluginsStyle.h"

class FLuaEditorPluginsCommands : public TCommands<FLuaEditorPluginsCommands>
{
public:

	FLuaEditorPluginsCommands()
		: TCommands<FLuaEditorPluginsCommands>(TEXT("LuaEditorPlugins"), NSLOCTEXT("Contexts", "LuaEditorPlugins", "LuaEditorPlugins Plugin"), NAME_None, FLuaEditorPluginsStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};