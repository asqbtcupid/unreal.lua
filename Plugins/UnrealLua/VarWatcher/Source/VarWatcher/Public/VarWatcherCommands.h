// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "VarWatcherStyle.h"

class FVarWatcherCommands : public TCommands<FVarWatcherCommands>
{
public:

	FVarWatcherCommands()
		: TCommands<FVarWatcherCommands>(TEXT("VarWatcher"), NSLOCTEXT("Contexts", "VarWatcher", "VarWatcher Plugin"), NAME_None, FVarWatcherStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};