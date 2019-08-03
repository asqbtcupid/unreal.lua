// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
#include "LuaDelegateMulti.h"
#include "TableUtil.h"
#include "BPAndLuaBridge.h"
#include "UserDefineClassMacro.h"

void ULuaDelegateMulti::Init(TMulticastScriptDelegate<FWeakObjectPtr>& Delegate, UFunction* _FunSig)
{
	const FName FireFunctionName("NoUseAtAll");
	TScriptDelegate<FWeakObjectPtr> Handle;
	Handle.BindUFunction(this, FireFunctionName);
	Delegate.Add(Handle);
	FunSig = _FunSig;
	TheDelegatePtr = &Delegate;
}

void ULuaDelegateMulti::Init(void* Delegate, UFunction* _FunSig)
{
	Init(*(TMulticastScriptDelegate<FWeakObjectPtr>*)Delegate, _FunSig);
}

void ULuaDelegateMulti::NoUseAtAll(){}

void ULuaDelegateMulti::ProcessEvent(UFunction* Function, void* Parms)
{
	auto LuaFuncToCall = LuaCallBacks;
	for (auto& v: LuaFuncToCall)
	{
		for (int ref : v.Value)
			UTableUtil::call(v.Key, ref, FunSig, Parms);
	}
}

int ULuaDelegateMulti::Add(lua_State* inL)
{
	lua_State* MainThread = UTableUtil::GetMainThread(inL);
	luavalue_ref r = UTableUtil::ref_luavalue(inL, 2);
	TSet<int>& CallBacksOfState = LuaCallBacks.FindOrAdd(MainThread);
	CallBacksOfState.Add(r);
	UTableUtil::push(inL, r);
	return 1; 
}

void ULuaDelegateMulti::Remove(lua_State* inL)
{
	int32 r = UTableUtil::pop<int32>(inL, 2);
	lua_State* MainThread = UTableUtil::GetMainThread(inL);
	TSet<int>& CallBacksOfState = LuaCallBacks.FindOrAdd(MainThread);
	if (CallBacksOfState.Contains(r))
	{
		UTableUtil::unref(MainThread, r);
		CallBacksOfState.Remove(r);
 	}
}


void ULuaDelegateMulti::RemoveAll()
{
	for (auto& v : LuaCallBacks)
	{
		for (int ref : v.Value)
		{
			UTableUtil::unref(v.Key, ref);
		}
	}
	LuaCallBacks.Reset();
}

void ULuaDelegateMulti::Destroy()
{
	RemoveAll();
// 	UTableUtil::rmgcref(this);
}

void ULuaDelegateMulti::Fire(lua_State* inL)
{
	if (TheDelegatePtr && FunSig)
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
		TheDelegatePtr->ProcessMulticastDelegate<UObject>(Buffer);
	}
}

void ULuaDelegateMulti::Clear()
{
	RemoveAll();
	TheDelegatePtr->Clear();
	const FName FireFunctionName("NoUseAtAll");
	TScriptDelegate<FWeakObjectPtr> Handle;
	Handle.BindUFunction(this, FireFunctionName);
	TheDelegatePtr->Add(Handle);
}


void ULuaDelegateMulti::RemoveUObject(UObject* Ptr)
{
	TheDelegatePtr->RemoveAll(Ptr);
}


void ULuaDelegateMulti::RemoveUFunction(UObject* Ptr, FName InFunctionName)
{
	TheDelegatePtr->Remove(Ptr, InFunctionName);
}

LUA_GLUE_BEGIN(ULuaDelegateMulti)
LUA_GLUE_FUNCTION(Add)
LUA_GLUE_FUNCTION(Remove)
LUA_GLUE_FUNCTION(RemoveAll)
LUA_GLUE_FUNCTION(Destroy)
LUA_GLUE_FUNCTION(Fire)
LUA_GLUE_FUNCTION(Clear)
LUA_GLUE_FUNCTION(RemoveUObject)
LUA_GLUE_FUNCTION(RemoveUFunction)
LUA_GLUE_END()
