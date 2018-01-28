// Fill out your copyright notice in the Description page of Project Settings.

#include "DebuggerSetting.h"
#include "TableUtil.h"
#include "LuaDebugger.h"

UDebuggerSetting* UDebuggerSetting::Get()
{
	static UDebuggerSetting* Singleton = nullptr;
	if (Singleton == nullptr)
	{
		Singleton = NewObject<UDebuggerSetting>();
		Singleton->AddToRoot();
	}
	return Singleton;
}

FString UDebuggerSetting::GetLuaSourceDir()
{
	return FLuaDebuggerModule::Get()->GetLuaSourceDir();
}

void UDebuggerSetting::PullDataToLua()
{
	UpdateBreakPoint(BreakPoints);
	ToggleDebugStart(bIsStart);
	SetTabIsOpen(bTabIsOpen);
}

void UDebuggerSetting::UpdateBreakPoint(TMap<FString, TSet<int32>>& BreakPoint)
{
	BreakPoints = BreakPoint;
	LuaCall("UpdateBreakPoint", this, BreakPoint);
}

void UDebuggerSetting::ToggleDebugStart(bool IsStart)
{
	bIsStart = IsStart;
	LuaCall("ToggleDebugStart", this, IsStart);
}

void UDebuggerSetting::SetTabIsOpen(bool IsOpen)
{
	bTabIsOpen = IsOpen;
	LuaCall("SetTabIsOpen", this, IsOpen);
}

void UDebuggerSetting::EnterDebug(const FString& LuaFilePath, int32 Line)
{
	FLuaDebuggerModule::Get()->EnterDebug(LuaFilePath, Line);
}

void UDebuggerSetting::SetStackData(const TArray<FString>& Content, const TArray<int32>& Lines, const TArray<FString>& FilePaths, const TArray<int32>& StackIndex)
{
	FLuaDebuggerModule::Get()->SetStackData(Content, Lines, FilePaths, StackIndex);
}

// TArray<FDebuggerVarNode> UDebuggerSetting::GetStackVar(int32 StackIndex)
// {
// // 	return LuaCallr(TArray<FDebuggerVarNode>, "GetStackVar", this, StackIndex);
// 	return TArray<FDebuggerVarNode>();
// }
