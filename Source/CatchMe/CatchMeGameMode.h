// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "gameplay/gamemode/cmgamemodebase.h"
#include "CatchMeGameMode.generated.h"

UCLASS(meta = (Lua = 1))
class ACatchMeGameMode : public ACMGameModeBase
{
	GENERATED_BODY()

public:
	ACatchMeGameMode();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

};



