// Fill out your copyright notice in the Description page of Project Settings.

#include "BpCallLuaObj.h"
#include "TableUtil.h"

void UBpCallLuaObj::_LuaCtor(UObject* BPIns, const FString& LuaClassPath)
{
	if (BPIns == nullptr)
		ensureAlwaysMsgf(0, L"");
	else
	{
		UTableUtil::call("Ctor", LuaClassPath, BPIns);
	}
}

void UBpCallLuaObj::Call(UObject* Ins, const FString& FunctionName)
{
	if (Ins == nullptr)
	{
		UTableUtil::call(TCHAR_TO_UTF8(*FunctionName));
	}
	else
	{
		UTableUtil::inscall("Call", TCHAR_TO_UTF8(*FunctionName), Ins);
	}
}
