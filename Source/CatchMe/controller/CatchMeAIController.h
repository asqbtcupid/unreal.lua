// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "Launch/Resources/Version.h"
#include "CatchMeAIController.generated.h"

UCLASS()
class CATCHME_API ACatchMeAIController : public AAIController
{
	GENERATED_BODY()
public:
	ACatchMeAIController();
	virtual void BeginPlay() override;
#if ENGINE_MINOR_VERSION < 22
	virtual void Possess(APawn* InPawn) override;
#else
	virtual void OnPossess(APawn* InPawn) override;
#endif
};
