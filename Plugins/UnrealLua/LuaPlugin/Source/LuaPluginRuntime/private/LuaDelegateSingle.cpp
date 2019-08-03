// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
#include "LuaDelegateSingle.h"
#include "TableUtil.h"
#include "BPAndLuaBridge.h"
#include "UserDefineClassMacro.h"

void ULuaDelegateSingle::NoUseAtAll()
{

}

void ULuaDelegateSingle::ProcessEvent(UFunction* Function, void* Parms)
{
	check(TheDelegatePtr && FunSig);
	static UFunction* NotAtAllFunction = ULuaDelegateSingle::StaticClass()->FindFunctionByName("NoUseAtAll");
	if (NotAtAllFunction == Function)
	{
		int OldTopCount = lua_gettop(TheBindLuaState);

		if(LuaCallBack > 0 && FunSig && TheDelegatePtr)
			UTableUtil::call(TheBindLuaState, LuaCallBack, FunSig, Parms);

		lua_settop(TheBindLuaState, OldTopCount);
	}
	else
		Super::ProcessEvent(Function, Parms);
}

ULuaDelegateSingle* ULuaDelegateSingle::Create(lua_State* inL)
{
	ULuaDelegateSingle* NewOne = NewObject<ULuaDelegateSingle>();
	NewOne->TheDelegatePtrInHeap = new FScriptDelegate;
	NewOne->TheDelegatePtr = NewOne->TheDelegatePtrInHeap;
	const FName FuncName("NoUseAtAll");
	NewOne->TheDelegatePtr->BindUFunction(NewOne, FuncName);
	NewOne->Bind(inL);
	return NewOne;
}

ULuaDelegateSingle* ULuaDelegateSingle::CreateInCppCopy(FScriptDelegate* InTheDelegatePtr, UFunction* InFuncSig)
{
	ULuaDelegateSingle* NewOne = NewObject<ULuaDelegateSingle>();
	NewOne->SetFuncSig(InFuncSig);
	NewOne->TheDelegatePtrInHeap = new FScriptDelegate;
	NewOne->TheDelegatePtr = NewOne->TheDelegatePtrInHeap;
	*(NewOne->TheDelegatePtr) = *InTheDelegatePtr;
	return NewOne;
}

ULuaDelegateSingle* ULuaDelegateSingle::CreateInCppRef(FScriptDelegate* InTheDelegatePtr, UFunction* InFuncSig)
{
	ULuaDelegateSingle* NewOne = NewObject<ULuaDelegateSingle>();
	NewOne->SetFuncSig(InFuncSig);
	NewOne->TheDelegatePtr = InTheDelegatePtr;
	return NewOne;
}

FScriptDelegate* ULuaDelegateSingle::GetCppDelegate()
{
	return TheDelegatePtr;
}

void ULuaDelegateSingle::SetFuncSig(UFunction* InFunSig)
{
	FunSig = InFunSig;
}

int ULuaDelegateSingle::Bind(lua_State* inL)
{
	lua_State* MainThread = UTableUtil::GetMainThread(inL);
	const FName FuncName("NoUseAtAll");
	TheDelegatePtr->BindUFunction(this, FuncName);
	if(LuaCallBack > 0)
		UTableUtil::unref(TheBindLuaState, LuaCallBack);
	LuaCallBack = UTableUtil::ref_luavalue(inL, 2);
	TheBindLuaState = MainThread;
	return LuaCallBack;
}

void ULuaDelegateSingle::Clear()
{
	if (TheDelegatePtr)
		TheDelegatePtr->Clear();
	if (LuaCallBack > 0)
	{
		LuaCallBack = -1;
		UTableUtil::unref(TheBindLuaState, LuaCallBack);
	}

}

void ULuaDelegateSingle::BindUFunction(UObject* InObject, const FName& InFunctionName)
{
	if (TheDelegatePtr)
		TheDelegatePtr->BindUFunction(InObject, InFunctionName);
}

bool ULuaDelegateSingle::IsBound()
{
	if (TheDelegatePtr)
		return TheDelegatePtr->IsBound();
	else
		return false;
}	

void ULuaDelegateSingle::Destroy()
{
	if (LuaCallBack > 0)
	{
		UTableUtil::unref(TheBindLuaState, LuaCallBack);
		LuaCallBack = -1;
	}
	if (TheDelegatePtrInHeap)
		delete TheDelegatePtrInHeap;
}

void ULuaDelegateSingle::Fire(lua_State* inL)
{
	check(TheDelegatePtr && FunSig);
	if (TheDelegatePtr->IsBound())
	{
		UFunction* Function = FunSig;
		uint8* Buffer = (uint8*)FMemory_Alloca(Function->ParmsSize);
		FScopedArguments scoped_arguments(Function, Buffer);

		int ArgIndex = 2;
		int ArgCount = lua_gettop(inL);

		// Iterate over input parameters
		for (TFieldIterator<UProperty> It(Function); It && (It->GetPropertyFlags() & (CPF_Parm)); ++It)
		{
			auto Prop = *It;
			if (ArgIndex <= ArgCount)
			{
				UTableUtil::popproperty(inL, ArgIndex, Prop, Buffer);
				++ArgIndex;
			}
		}
		TheDelegatePtr->ProcessDelegate<UObject>(Buffer);
	}
}

LUA_GLUE_BEGIN(ULuaDelegateSingle)
LUA_GLUE_FUNCTION(Create)
LUA_GLUE_FUNCTION(Bind)
LUA_GLUE_FUNCTION(Clear)
LUA_GLUE_FUNCTION(BindUFunction)
LUA_GLUE_FUNCTION(IsBound)
LUA_GLUE_FUNCTION(Destroy)
LUA_GLUE_FUNCTION(Fire)
LUA_GLUE_END()