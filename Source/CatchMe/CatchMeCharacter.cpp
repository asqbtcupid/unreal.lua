// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "CatchMe.h"
#include "CatchMeCharacter.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "TableUtil.h"
#include "luautils.h"

ACatchMeCharacter::ACatchMeCharacter()
{
	LuaCtor("character.catchmecharacter", this);
}

void ACatchMeCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}

bool ACatchMeCharacter::IsReplicationPausedForConnection(const FNetViewer& ConnectionOwnerNetViewer)
{
	return LuaCallr(bool, "IsReplicationPausedForConnection", this, ConnectionOwnerNetViewer);
}

void ACatchMeCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	auto result = LuaStaticCallr(TArray<FReplifetimeCond>, "CatchMeCharacter_GetLifetimeReplicatedProps", this);
	for (auto &v : result)
	{
// 		UProperty* p = UTableUtil::GetPropertyByName(ACatchMeCharacter::StaticClass(), v.PropertyName);
		UProperty* p = ACatchMeCharacter::StaticClass()->FindPropertyByName(FName(*v.PropertyName));
		for (int32 i = 0; i < p->ArrayDim; i++)
		{
			OutLifetimeProps.AddUnique(FLifetimeProperty(p->RepIndex + i, v.Cond));
		}
	}
}


void ACatchMeCharacter::OnRep_SkillInfo()
{
	LuaCall("OnRep_SkillInfo", this);
}


void ACatchMeCharacter::OnRep_EXP()
{
	LuaCall("OnRep_EXP", this);
}


void ACatchMeCharacter::OnRep_Level()
{
	LuaCall("OnRep_Level", this);

}


void ACatchMeCharacter::OnRep_CharacterId()
{
	LuaCall("OnRep_CharacterId", this);
}

void ACatchMeCharacter::OnRep_HP()
{
	LuaCall("OnRep_HP", this);
}


void ACatchMeCharacter::OnRep_HPPercent()
{
	LuaCall("OnRep_HPPercent", this);
}

void ACatchMeCharacter::OnRep_MP()
{
	LuaCall("OnRep_MP", this);
}

void ACatchMeCharacter::M_PlaySkillMontage_Implementation(int32 SKillId)
{
	LuaCall("M_PlaySkillMontage_Imp", this, SKillId);
}
