// Fill out your copyright notice in the Description page of Project Settings.

#include "CatchMe.h"
#include "CMGameModeBase.h"
#include "TableUtil.h"



void ACMGameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	LuaStaticCall("Tick", DeltaSeconds);
}

void ACMGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	LuaCall("BeginPlay", this);

}

APawn* ACMGameModeBase::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
	APawn* DefaultPawn = Super::SpawnDefaultPawnAtTransform_Implementation(NewPlayer, SpawnTransform);
	LuaCall("SetDefaultPawn", this, DefaultPawn);
	return DefaultPawn;
}
