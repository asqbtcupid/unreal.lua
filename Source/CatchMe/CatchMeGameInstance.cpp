// Fill out your copyright notice in the Description page of Project Settings.

#include "CatchMe.h"
#include "CatchMeGameInstance.h"
#include "TableUtil.h"



void UCatchMeGameInstance::Init()
{
	Super::Init();
	UTableUtil::init(true);
}

void UCatchMeGameInstance::Shutdown()
{
	Super::Shutdown();
	UTableUtil::shutdown();
}
