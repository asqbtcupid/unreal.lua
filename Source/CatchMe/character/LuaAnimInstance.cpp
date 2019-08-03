// Fill out your copyright notice in the Description page of Project Settings.

#include "LuaAnimInstance.h"
#include "UnrealLua.h"



ULuaAnimInstance::ULuaAnimInstance()
{
	LuaCtor("character.animinstance", this);
}

void ULuaAnimInstance::NativeInitializeAnimation()
{
	LuaCall("NativeInitializeAnimation", this);

}

void ULuaAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	LuaCall("NativeUpdateAnimation", this, DeltaSeconds);
}
