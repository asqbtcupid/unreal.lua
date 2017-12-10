// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "CatchMeAIController.generated.h"

UCLASS()
class CATCHME_API ACatchMeAIController : public AAIController
{
	GENERATED_BODY()
public:
	ACatchMeAIController();
	virtual void BeginPlay() override;
	virtual void Possess(APawn* InPawn) override;
};
