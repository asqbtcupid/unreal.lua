// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "gameplay/gamemode/CMGameModeBase.h"
#include "CatchMeGameMode.generated.h"

UCLASS()
class CATCHME_API ACatchMeGameMode : public ACMGameModeBase
{
	GENERATED_BODY()
   
public:
	ACatchMeGameMode();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

};

