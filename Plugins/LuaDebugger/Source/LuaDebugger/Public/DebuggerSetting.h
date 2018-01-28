// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DebuggerSetting.generated.h"

USTRUCT()
struct LUADEBUGGER_API FDebuggerVarNode
{
	GENERATED_BODY()

	UPROPERTY()
	FString Content;

	UPROPERTY()
	int32 LuaIndex;

	TArray<TSharedRef<FDebuggerVarNode>> KeyChildren;
	TArray<TSharedRef<FDebuggerVarNode>> ValueChildren;
};

UCLASS()
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

	UFUNCTION()
		void EnterDebug(const FString& LuaFilePath, int32 Line);
	UFUNCTION()
	void SetStackData(const TArray<FString>& Content, const TArray<int32>& Lines, const TArray<FString>& FilePaths, const TArray<int32>& StackIndex);

	TArray<FDebuggerVarNode> GetStackVar(int32 StackIndex);
};
