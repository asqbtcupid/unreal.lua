// Fill out your copyright notice in the Description page of Project Settings.

#include "CatchMe.h"
#include "LuaAnimInstance.h"
#include "tableutil.h"



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
