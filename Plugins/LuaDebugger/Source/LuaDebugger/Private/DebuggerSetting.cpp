// Fill out your copyright notice in the Description page of Project Settings.

#include "DebuggerSetting.h"
#include "TableUtil.h"
#include "LuaDebugger.h"
#include "DebuggerVarNode.lua.h"

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

void UDebuggerSetting::GetStackVars(int32 StackIndex, TArray<FDebuggerVarNode_Ref>& Vars)
{
	Vars.Reset();
 	TArray<FDebuggerVarNode> Result = LuaCallr(TArray<FDebuggerVarNode>, "GetStackVars", this, StackIndex);
	for(auto& Node: Result)
	{
		Vars.Add(MakeShareable(new FDebuggerVarNode(Node)));
	}
}

void UDebuggerSetting::GetVarsChildren(FDebuggerVarNode InNode, TArray<TSharedRef<FDebuggerVarNode>>& OutChildren)
{
	TArray<FDebuggerVarNode> Result = LuaCallr(TArray<FDebuggerVarNode>, "GetVarNodeChildren", this, InNode);
	Result.StableSort([&](const FDebuggerVarNode& a, const FDebuggerVarNode& b) {return a.Name.CompareTo(b.Name) < 0; });
	for (auto& Node : Result)
	{
		OutChildren.Add(MakeShareable(new FDebuggerVarNode(Node)));
	}
}

void UDebuggerSetting::EnterDebug(const FString& LuaFilePath, int32 Line)
{
	FLuaDebuggerModule::Get()->EnterDebug(LuaFilePath, Line);
}

void UDebuggerSetting::SetStackData(const TArray<FString>& Content, const TArray<int32>& Lines, const TArray<FString>& FilePaths, const TArray<int32>& StackIndex)
{
	FLuaDebuggerModule::Get()->SetStackData(Content, Lines, FilePaths, StackIndex);
}

void UDebuggerSetting::StepOver()
{
	LuaCall("StepOver", this);
}

void UDebuggerSetting::StepIn()
{
	LuaCall("StepIn", this);
}

void UDebuggerSetting::StepOut()
{
	LuaCall("StepOut", this);
}

void FDebuggerVarNode::GetChildren(TArray<TSharedRef<FDebuggerVarNode>>& OutChildren)
{
	if (ValueChildren.Num() == 0)
		UDebuggerSetting::Get()->GetVarsChildren(*this, ValueChildren);
	OutChildren = ValueChildren;
}
