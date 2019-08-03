#include "RemoteDebuggerSetting.h"
#include "UnrealLuaInterface.h"

// void URemoteDebuggerSetting::UpdateBreakPoint(TMap<FString, TSet<int32>>& BreakPoint)
// {
// 	
// }

// void URemoteDebuggerSetting::GetStackVars(int32 StackIndex, TArray<FDebuggerVarNode_Ref>& Vars)
// {
// 
// }

// void URemoteDebuggerSetting::GetVarsChildren(FDebuggerVarNode Node, TArray<TSharedRef<FDebuggerVarNode>>& OutChildren)
// {
// 
// }

void URemoteDebuggerSetting::DebugContinue()
{
	LuaCall_AllState("DebugContinue", this);
}
