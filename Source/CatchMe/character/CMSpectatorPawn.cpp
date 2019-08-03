// Fill out your copyright notice in the Description page of Project Settings.

#include "CMSpectatorPawn.h"
#include "UnrealLua.h"



ACMSpectatorPawn::ACMSpectatorPawn()
{
	LuaCtor("character.cmspectatorpawn");
}

void ACMSpectatorPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	LuaCall("EndPlay", this, int(EndPlayReason));

}
