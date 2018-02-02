// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "STreeView.h"
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
		:Line(_Line), FilePath(_FilePath)
	{}
	UPROPERTY()
	FString FilePath;

	UPROPERTY()
	int32 Line;
};

UCLASS(config=Editor)
class LUADEBUGGER_API UDebuggerSetting : public UObject
{
	GENERATED_BODY()
	TMap<FString, TSet<int32>> BreakPoints;
	bool bIsStart;
	bool bTabIsOpen;
public:
	UFUNCTION()
		static UDebuggerSetting* Get();
	UFUNCTION()
		FString GetLuaSourceDir();

	UFUNCTION()
		void PullDataToLua();
	void UpdateBreakPoint(TMap<FString, TSet<int32>>& BreakPoint);
	void ToggleDebugStart(bool IsStart);
	void SetTabIsOpen(bool IsOpen);
	void GetStackVars(int32 StackIndex, TArray<FDebuggerVarNode_Ref>& Vars);
	void GetVarsChildren(FDebuggerVarNode Node, TArray<TSharedRef<FDebuggerVarNode>>& OutChildren);

	UFUNCTION()
		void EnterDebug(const FString& LuaFilePath, int32 Line);
	UFUNCTION()
	void SetStackData(const TArray<FString>& Content, const TArray<int32>& Lines, const TArray<FString>& FilePaths, const TArray<int32>& StackIndex);

	void StepOver();
	void StepIn();
	void StepOut();

	UPROPERTY(config)
	TMap<FString, float> LastTimeFileOffset;

	UPROPERTY(config)
		FString RecentFilePath;
	
	UPROPERTY(config)
		TArray<FBreakPointNode> RecentBreakPoint;
};
