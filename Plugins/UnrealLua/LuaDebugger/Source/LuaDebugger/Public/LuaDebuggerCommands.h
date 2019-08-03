// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "LuaDebuggerStyle.h"

class FLuaDebuggerCommands : public TCommands<FLuaDebuggerCommands>
{
public:

	FLuaDebuggerCommands()
		: TCommands<FLuaDebuggerCommands>(TEXT("LuaDebugger"), NSLOCTEXT("Contexts", "LuaDebugger", "LuaDebugger Plugin"), NAME_None, FLuaDebuggerStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};