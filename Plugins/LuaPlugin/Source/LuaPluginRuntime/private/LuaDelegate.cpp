// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
#include "LuaDelegate.h"
#include "tableutil.h"
#include "BPAndLuaBridge.h"

void ULuaDelegate::Init(TMulticastScriptDelegate<FWeakObjectPtr>& Delegate, UFunction* _FunSig)
{
	const FName FireFunctionName("NoUseAtAll");
	TScriptDelegate<FWeakObjectPtr> Handle;
	Handle.BindUFunction(this, FireFunctionName);
	Delegate.Add(Handle);
	FunSig = _FunSig;
	TheDelegatePtr = &Delegate;
}

void ULuaDelegate::Init(void* Delegate, UFunction* _FunSig)
{
	Init(*(TMulticastScriptDelegate<FWeakObjectPtr>*)Delegate, _FunSig);
}

void ULuaDelegate::NoUseAtAll()
{

}

void ULuaDelegate::ProcessEvent(UFunction* Function, void* Parms)
{
	for (auto& v : LuaCallBacks) 
	{
		for(int ref:v.Value)
			UTableUtil::call(UTableUtil::GetTheOnlyLuaState(), ref, FunSig, Parms);
	}
}

int ULuaDelegate::Add(Flua_State inL, Flua_Index StackIndex)
{
	luavalue_ref r = UTableUtil::ref_luavalue(inL.TheState, StackIndex);
	TSet<int>& CallBacksOfState = LuaCallBacks.FindOrAdd(inL.TheState);
	CallBacksOfState.Add(r);
	return r; 
}

void ULuaDelegate::Remove(Flua_State inL, int32 r)
{
	TSet<int>& CallBacksOfState = LuaCallBacks.FindOrAdd(inL.TheState);
	if (CallBacksOfState.Contains(r))
	{
		UTableUtil::unref(inL.TheState, r);
		CallBacksOfState.Remove(r);
 	}
}

void ULuaDelegate::Destroy()
{
	for (auto& v : LuaCallBacks)
	{
		for (int ref : v.Value)
		{
			UTableUtil::unref(UTableUtil::GetTheOnlyLuaState(), ref);
		}
	}
	LuaCallBacks.Reset();
	UTableUtil::rmgcref(this);
}

void ULuaDelegate::Fire(Flua_State inL)
{
	if (TheDelegatePtr && FunSig)
	{
		UFunction* Function = FunSig;
		uint8* Buffer = (uint8*)FMemory_Alloca(Function->ParmsSize);
		FScopedArguments scoped_arguments(Function, Buffer);

		int ArgIndex = 3;
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
		TheDelegatePtr->ProcessMulticastDelegate<UObject>(Buffer);
	}
}

Flua_State* GetFakeLuaState(lua_State* inL, int Index)
{
	static Flua_State fake_State;
	fake_State.TheState = inL;
	lua_pushnil(inL);
	lua_insert(inL, Index);
	return &fake_State;
}

LUAPLUGINRUNTIME_API Flua_Index* GetLuaIndex(int Index)
{
	static Flua_Index Indexs[] = { 0,1,2,3,4,5,6,7,8,9 };
	return &(Indexs[Index]);
}
