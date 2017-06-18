// Fill out your copyright notice in the Description page of Project Settings.

#include "CatchMe.h"
#include "CatchMeAIController.h"
#include "tableutil.h"



ACatchMeAIController::ACatchMeAIController()
{
	LuaCtor("controller.catchmeaicontroller", this);
}

void ACatchMeAIController::BeginPlay()
{
	Super::BeginPlay();
	LuaCall("BeginPlay", this);
}

void ACatchMeAIController::Possess(APawn* InPawn)
{
	Super::Possess(InPawn);
	LuaCall("Possess", this, InPawn);
}
