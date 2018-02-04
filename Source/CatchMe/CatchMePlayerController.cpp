// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "CatchMePlayerController.h"
#include "AI/Navigation/NavigationSystem.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "TableUtil.h"
#include "luautils.h"
#include "vector.lua.h"
#include "ReplifetimeCond.lua.h"


ACatchMePlayerController::ACatchMePlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
	LuaCtor("controller.cmplayercontroller", this);
}

void ACatchMePlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
}

void ACatchMePlayerController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	auto result = LuaStaticCallr(TArray<FReplifetimeCond>, "CMPlayerController_GetLifetimeReplicatedProps", this);
	for (auto &v : result)
	{
// 		UProperty* p = UTableUtil::GetPropertyByName(ACatchMePlayerController::StaticClass(), v.PropertyName);
		UProperty* p = ACatchMePlayerController::StaticClass()->FindPropertyByName(FName(*v.PropertyName));
		for (int32 i = 0; i < p->ArrayDim; i++)
		{
			OutLifetimeProps.AddUnique(FLifetimeProperty(p->RepIndex + i, v.Cond));
		}
	}
}

void ACatchMePlayerController::C_RemoveFoliage_Implementation(int32 FoliageComponentIndex, int32 Index)
{
	LuaCall("S_RemoveFoliage_Imp", this, FoliageComponentIndex, Index);
}

void ACatchMePlayerController::S_RemoveFoliage_Implementation(int32 FoliageComponentIndex, int32 Index)
{
	LuaCall("S_RemoveFoliage_Imp", this, FoliageComponentIndex, Index);
}

bool ACatchMePlayerController::S_RemoveFoliage_Validate(int32 FoliageComponentIndex, int32 Index)
{
	return true;
}


void ACatchMePlayerController::S_TapActor_Implementation(AActor* Target)
{
	LuaCall("S_TapActor_Imp", this, Target);
}

bool ACatchMePlayerController::S_TapActor_Validate(AActor* Target)
{
	return true;
}


void ACatchMePlayerController::S_TapFoliage_Implementation(UObject* Component, int32 Index)
{
	LuaCall("S_TapFoliage_Imp", this, Component, Index);
}

bool ACatchMePlayerController::S_TapFoliage_Validate(UObject* Component, int32 Index)
{
	return true;
}


void ACatchMePlayerController::S_TapFloor_Implementation(FVector Pos)
{
	TMap<FString, int32> haha;
	haha.Add("test", 2);
	LuaCall("S_TapFloor_Imp", this, Pos, haha);
}

bool ACatchMePlayerController::S_TapFloor_Validate(FVector Pos)
{
	return true;
}

void ACatchMePlayerController::S_PlaySkill_Implementation(int32 SkillId, AActor* Target)
{
	LuaCall("S_PlaySkill_Imp", this, SkillId, Target);
}

bool ACatchMePlayerController::S_PlaySkill_Validate(int32 SkillId, AActor* Target)
{
	return true;
}

void ACatchMePlayerController::S_MoveToLocation_Implementation(FVector Location)
{
	LuaCall("MoveToPos", this, Location);
}

bool ACatchMePlayerController::S_MoveToLocation_Validate(FVector Location)
{
	return true;
}