#pragma once

#include "CoreMinimal.h"
#include "DebuggerSetting.h"
#include "RemoteDebuggerSetting.generated.h"

UCLASS()
class LUADEBUGGER_API URemoteDebuggerSetting : public UDebuggerSetting
{
	GENERATED_BODY()
	
// 	virtual void UpdateBreakPoint(TMap<FString, TSet<int32>>& BreakPoint);
// 	virtual void ToggleDebugStart(bool IsStart);
// 	virtual void SetTabIsOpen(bool IsOpen);
// 	virtual void GetStackVars(int32 StackIndex, TArray<FDebuggerVarNode_Ref>& Vars);
// 	virtual void GetVarsChildren(FDebuggerVarNode Node, TArray<TSharedRef<FDebuggerVarNode>>& OutChildren);
	virtual void DebugContinue();
	// 
// 	UFUNCTION()
// 		virtual void EnterDebug(Flua_State State, const FString& LuaFilePath, int32 Line);
// 
// 	UFUNCTION()
// 		virtual void SetStackData(const TArray<FString>& Content, const TArray<int32>& Lines, const TArray<FString>& FilePaths, const TArray<int32>& StackIndex);
// 
// 	UFUNCTION()
// 		virtual FText GetBreakPointHitConditionText(FString& FilePath, int32 CodeLine);
// 
// 	virtual void StepOver();
// 	virtual void StepIn();
// 	virtual void StepOut();
};
