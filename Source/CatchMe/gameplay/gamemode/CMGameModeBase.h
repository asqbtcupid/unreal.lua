// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "CMGameModeBase.generated.h"

/**
 * 
 */
UCLASS(meta = (Lua = 1))
class CATCHME_API ACMGameModeBase : public AGameMode
{
	GENERATED_BODY()
 
public:
	virtual void Tick(float DeltaSeconds);
	virtual void BeginPlay();
	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform);
	
};
