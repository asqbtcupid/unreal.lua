// Fill out your copyright notice in the Description page of Project Settings.

#include "BpCallLuaObj.h"
#include "UnrealLua.h"

void UBpCallLuaObj::_LuaCtor(UObject* BPIns, const FString& LuaClassPath)
{
	if (BPIns == nullptr)
		ensureAlwaysMsgf(0, TEXT(""));
	else
	{
		UUnrealLuaInterface::Call("Ctor", TCHAR_TO_UTF8(*LuaClassPath), BPIns);
	}
}

void UBpCallLuaObj::_LuaInject(UObject* BPIns, const FString& LuaClassPath)
{
	if (BPIns == nullptr)
		ensureAlwaysMsgf(0, TEXT(""));
	else
	{
		UUnrealLuaInterface::ReplaceUe4Function(BPIns, TCHAR_TO_UTF8(*LuaClassPath));
	}
}

void UBpCallLuaObj::_LuaCtorInject(UObject* BPIns, const FString& LuaClassPath)
{
	_LuaCtor(BPIns, LuaClassPath);
	_LuaInject(BPIns, LuaClassPath);
}

// void UBpCallLuaObj::Call(UObject* Ins, const FString& FunctionName)
// {
// 	if (Ins == nullptr)
// 	{
// 		ensureAlwaysMsgf(0, TEXT(""));
// // 		UTableUtil::call(TCHAR_TO_UTF8(*FunctionName));
// 	}
// 	else
// 	{
// 		ensureAlwaysMsgf(0, TEXT(""));
// // 		UTableUtil::inscall("Call", TCHAR_TO_UTF8(*FunctionName), Ins);
// 	}
// }
