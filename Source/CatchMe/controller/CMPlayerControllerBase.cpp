// Fill out your copyright notice in the Description page of Project Settings.

#include "CMPlayerControllerBase.h"
#include "TableUtil.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerInput.h"

void ACMPlayerControllerBase::BeginPlay()
{
	Super::BeginPlay();
	LuaCall("BeginPlay", this);
}

void ACMPlayerControllerBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	LuaCall("EndPlay", this, int(EndPlayReason));
	Super::EndPlay(EndPlayReason);
}

void ACMPlayerControllerBase::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	LuaCall("PlayerTick", this, DeltaTime);
}

void ACMPlayerControllerBase::PreProcessInput(const float DeltaTime, const bool bGamePaused)
{
	Super::PreProcessInput(DeltaTime, bGamePaused);
	LuaCall("PreProcessInput", this, DeltaTime, bGamePaused);
}

void ACMPlayerControllerBase::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	Super::PostProcessInput(DeltaTime, bGamePaused);
	LuaCall("PostProcessInput", this, DeltaTime, bGamePaused);
}

void ACMPlayerControllerBase::ProcessPlayerInput(const float DeltaTime, const bool bGamePaused)
{
	LuaCall("ProcessPlayerInput", this, DeltaTime, bGamePaused);
	Super::ProcessPlayerInput(DeltaTime, bGamePaused);
}

void ACMPlayerControllerBase::SetPawn(APawn* aPawn)
{
	Super::SetPawn(aPawn);
	LuaCall("SetPawn", this, aPawn);
}

TArray<float> ACMPlayerControllerBase::GetInputState()
{
	TArray<float> result;
	for (int32 i = 0; i < ARRAY_COUNT(PlayerInput->Touches); i++)
	{
		result.Add(PlayerInput->Touches[i].X);
		result.Add(PlayerInput->Touches[i].Y);
		result.Add(PlayerInput->Touches[i].Z);
	}
	return result;
}

bool ACMPlayerControllerBase::GetHitResult(float x, float y, FHitResult& HitResult, int TraceChannel, bool bTraceComplex /*= true*/)
{
	FVector2D Pos(x, y);
	if (TraceChannel == -1)
		TraceChannel = int(CurrentClickTraceChannel);

	return GetHitResultAtScreenPosition(Pos, ECollisionChannel(TraceChannel), bTraceComplex, HitResult);
}
