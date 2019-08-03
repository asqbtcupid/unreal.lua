// Fill out your copyright notice in the Description page of Project Settings.

#include "DebuggerSetting.h"
#include "UnrealLua.h"
#include "LuaDebugger.h"
#include "RemoteDebuggerSetting.h"
// #include "DebuggerVarNode.lua.h"

UDebuggerSetting* UDebuggerSetting::Get(bool IsRemoteDebugger)
{
	if (IsRemoteDebugger == false)
	{
		static UDebuggerSetting* Singleton = nullptr;
		if (Singleton == nullptr)
		{
			Singleton = NewObject<UDebuggerSetting>();
			Singleton->AddToRoot();
		}
		return Singleton;
	}
	else
	{
		static URemoteDebuggerSetting* Singleton = nullptr;
		if (Singleton == nullptr)
		{
			Singleton = NewObject<URemoteDebuggerSetting>();
			Singleton->AddToRoot();
		}
		return Singleton;
	}
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
	LuaCall_AllState("UpdateBreakPoint", this, BreakPoint);
}

void UDebuggerSetting::ToggleDebugStart(bool IsStart)
{
	bIsStart = IsStart;
	LuaCall_AllState("ToggleDebugStart", this, IsStart);
}

void UDebuggerSetting::SetTabIsOpen(bool IsOpen)
{
	bTabIsOpen = IsOpen;
	LuaCall_AllState("SetTabIsOpen", this, IsOpen);
}

void UDebuggerSetting::CombineNodeArr(TArray<FDebuggerVarNode_Ref>& PreVars, TArray<FDebuggerVarNode>& NowVars)
{
	TMap<FString, FDebuggerVarNode> NameToNowVarNode;
	TMap<FString, FDebuggerVarNode_Ref> NameToPreVarNode;

	for (auto& Node : NowVars)
	{
		NameToNowVarNode.Add(Node.Name.ToString(), Node);
	}

	for (auto& Node : PreVars)
	{
		FString NodeName(Node.Get().Name.ToString());
		NameToPreVarNode.Add(NodeName, Node);
		FDebuggerVarNode* NodePtr = NameToNowVarNode.Find(NodeName);
		if (NodePtr == nullptr)
		{
			Node.Get().Name = FText::FromString("nil");
			Node.Get().Value = FText::FromString("nil");
			Node.Get().ValueWeakIndex = -1;
			Node.Get().ValueChildren.Reset();
		}
		else
		{
			Node.Get().Name = NodePtr->Name;
			Node.Get().Value = NodePtr->Value;
			if (NodePtr->Value.ToString() == "nil")
			{
				Node.Get().ValueChildren.Reset();
				Node.Get().ValueWeakIndex = -1;
			}
			else
				Node.Get().ValueWeakIndex = NodePtr->ValueWeakIndex;
		}
	}

	for (auto& Node : NowVars)
	{
		FDebuggerVarNode_Ref* PreNodePtr = NameToPreVarNode.Find(Node.Name.ToString());
		if (PreNodePtr == nullptr)
		{
			Node.DebuggerSetting = this;
			PreVars.Add(MakeShareable(new FDebuggerVarNode(Node)));
		}
	}
}

void UDebuggerSetting::GetStackVars(int32 StackIndex, TArray<FDebuggerVarNode_Ref>& Vars)
{
 	TArray<FDebuggerVarNode> Result = LuaCallr_State(HookingLuaState, TArray<FDebuggerVarNode>, "GetStackVars", this, StackIndex);
	CombineNodeArr(Vars, Result);
}

void UDebuggerSetting::GetVarsChildren(FDebuggerVarNode InNode, TArray<FDebuggerVarNode_Ref>& OutChildren)
{
	TArray<FDebuggerVarNode> Result = LuaCallr_State(HookingLuaState, TArray<FDebuggerVarNode>, "GetVarNodeChildren", this, InNode);
	Result.StableSort([&](const FDebuggerVarNode& a, const FDebuggerVarNode& b) 
	{
		bool aIsFunc = a.Value.ToString().Contains("function:");
		bool bIsFunc = b.Value.ToString().Contains("function:");
		if (aIsFunc != bIsFunc)
			return bIsFunc;
		FString a_KeyString = a.Name.ToString();
		FString b_KeyString = b.Name.ToString();
		if (a_KeyString.IsNumeric() != b_KeyString.IsNumeric())
		{
			return b_KeyString.IsNumeric();
		}
		else if (a_KeyString.IsNumeric())
		{
			return FCString::Atoi(*a_KeyString) < FCString::Atoi(*b_KeyString);
		}
		else
			return a.Name.CompareTo(b.Name)<0; 
	});

	CombineNodeArr(OutChildren, Result);
}

void UDebuggerSetting::EnterDebug(lua_State* inL)
{
	HookingLuaState = inL;
	FString LuaFilePath = UTableUtil::pop<FString>(inL, 2);
	int32 Line = UTableUtil::pop<int32>(inL, 3);
	FLuaDebuggerModule::Get()->EnterDebug(LuaFilePath, Line);
}

void UDebuggerSetting::SetStackData(const TArray<FString>& Content, const TArray<int32>& Lines, const TArray<FString>& FilePaths, const TArray<int32>& StackIndex, const TArray<FString>& FuncInfos)
{
	FLuaDebuggerModule::Get()->SetStackData(Content, Lines, FilePaths, StackIndex, FuncInfos);
}

FText UDebuggerSetting::GetBreakPointHitConditionText(FString& FilePath, int32 CodeLine)
{
	return FLuaDebuggerModule::Get()->GetBreakPointHitConditionText(FilePath, CodeLine);
}

void UDebuggerSetting::StepOver()
{
	if (HookingLuaState)
	{
		LuaCall_State(HookingLuaState, "StepOver", this);
	}
}

void UDebuggerSetting::StepIn()
{
	if (HookingLuaState)
	{
		LuaCall_State(HookingLuaState, "StepIn", this);
	}
}

void UDebuggerSetting::StepOut()
{
	if (HookingLuaState)
	{
		LuaCall_State(HookingLuaState, "StepOut", this);
	}
}

void UDebuggerSetting::BreakConditionChange()
{
	LuaCall_AllState("BreakConditionChange", this);
}

void FDebuggerVarNode::GetChildren(TArray<TSharedRef<FDebuggerVarNode>>& OutChildren)
{
	if (DebuggerSetting && ValueWeakIndex > 0)
		DebuggerSetting->GetVarsChildren(*this, ValueChildren);
	OutChildren = ValueChildren;
}
LUA_GLUE_EXPAND_BEGIN(UDebuggerSetting)
LUA_GLUE_FUNCTION(EnterDebug)
LUA_GLUE_EXPAND_END(UDebuggerSetting)