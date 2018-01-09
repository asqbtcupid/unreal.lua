// Fill out your copyright notice in the Description page of Project Settings.

#include "CatchMeGameInstance.h"
#include "TableUtil.h"



UCatchMeGameInstance::UCatchMeGameInstance()
{
// 	UTableUtil::init(!HasAnyFlags(RF_ClassDefaultObject));
	LuaCtor("gameplay.catchmegameinstance", this)
}

void UCatchMeGameInstance::Init()
{
	Super::Init();
}

void UCatchMeGameInstance::Shutdown()
{
	Super::Shutdown();
// 	UTableUtil::shutdown();
}
