// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "STreeView.h"
#include "LuaDelegateMulti.h"
#include "DebuggerSetting.generated.h"

USTRUCT()
struct LUADEBUGGER_API FDebuggerVarNode
{
	GENERATED_BODY()

	FDebuggerVarNode():
	NameWeakIndex(-1), ValueWeakIndex(-1)
	{}

	UPROPERTY()
	FText Name;

	UPROPERTY()
	FText Value;

	UPROPERTY()
	int32 NameWeakIndex;

	UPROPERTY()
	int32 ValueWeakIndex;

	class UDebuggerSetting* DebuggerSetting;

	void GetChildren(TArray<TSharedRef<FDebuggerVarNode>>& OutChildren);

	TArray<TSharedRef<FDebuggerVarNode>> KeyChildren;
	TArray<TSharedRef<FDebuggerVarNode>> ValueChildren;
};

using FDebuggerVarNode_Ref = TSharedRef<FDebuggerVarNode>;
using SDebuggerVarTree = STreeView<FDebuggerVarNode_Ref>;

USTRUCT()
struct LUADEBUGGER_API FBreakPointNode
{
	GENERATED_BODY()
	FBreakPointNode() {};
	FBreakPointNode(int32 _Line, FString _FilePath)
		:FilePath(_FilePath), Line(_Line)
	{}
	UPROPERTY()
	FString FilePath;

	UPROPERTY()
	int32 Line;

	UPROPERTY()
	FText HitCondition;
};

UCLASS(config=Editor)
class LUADEBUGGER_API UDebuggerSetting : public UObject
{
	GENERATED_BODY()
	TMap<FString, TSet<int32>> BreakPoints;
	bool bIsStart;
	bool bTabIsOpen;
	struct lua_State *HookingLuaState;
public:
	UDebuggerSetting():HookingLuaState(nullptr){}
	UFUNCTION()
		static UDebuggerSetting* Get(bool IsRemoteDebugger = false);
	UFUNCTION()
		virtual FString GetLuaSourceDir();

	UFUNCTION()
		virtual void PullDataToLua();
	virtual void UpdateBreakPoint(TMap<FString, TSet<int32>>& BreakPoint);
	virtual void ToggleDebugStart(bool IsStart);
	virtual void SetTabIsOpen(bool IsOpen);
	virtual void GetStackVars(int32 StackIndex, TArray<FDebuggerVarNode_Ref>& Vars);
	virtual void GetVarsChildren(FDebuggerVarNode Node, TArray<FDebuggerVarNode_Ref>& OutChildren);

	
		virtual void EnterDebug(lua_State* State);

	UFUNCTION()
		virtual void SetStackData(const TArray<FString>& Content, const TArray<int32>& Lines, const TArray<FString>& FilePaths, const TArray<int32>& StackIndex, const TArray<FString>& FuncInfos);

	UFUNCTION()
		virtual FText GetBreakPointHitConditionText(FString& FilePath, int32 CodeLine);

	void CombineNodeArr(TArray<FDebuggerVarNode_Ref>& PreVars, TArray<FDebuggerVarNode>& NowVars);
	virtual void DebugContinue() {};
	virtual void StepOver();
	virtual void StepIn();
	virtual void StepOut();
	virtual void BreakConditionChange();
	UPROPERTY(config)
	TMap<FString, float> LastTimeFileOffset;

	UPROPERTY(config)
		FString RecentFilePath;
	
	UPROPERTY(config)
		TArray<FBreakPointNode> RecentBreakPoint;
};
