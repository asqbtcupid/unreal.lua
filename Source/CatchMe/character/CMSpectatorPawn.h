// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/SpectatorPawn.h"
#include "CMSpectatorPawn.generated.h"

/**
 * 
 */
UCLASS(meta=(lua=1))
class CATCHME_API ACMSpectatorPawn : public ASpectatorPawn
{
	GENERATED_BODY()
public:
	ACMSpectatorPawn();
	
};
