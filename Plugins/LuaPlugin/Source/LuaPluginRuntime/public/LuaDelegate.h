// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ScriptDelegates.h"
#include "LuaDelegate.generated.h"

struct lua_State;
USTRUCT()
struct LUAPLUGINRUNTIME_API Flua_State
{
	GENERATED_BODY()
	lua_State* TheState;

	Flua_State():TheState(nullptr) {}
	Flua_State(lua_State* inL): TheState(inL) {}
	operator lua_State* ()
	{
		return TheState;
	}
};

USTRUCT()
struct LUAPLUGINRUNTIME_API Flua_Index
{
	GENERATED_BODY()
	int index;

	Flua_Index() :index(0) {}
	Flua_Index(int i) :index(i) {}
	operator int()
	{
		return index;
	}

};

LUAPLUGINRUNTIME_API Flua_State* GetFakeLuaState(lua_State* inL, int Index);
LUAPLUGINRUNTIME_API Flua_Index* GetLuaIndex(int Index);

UCLASS()
class LUAPLUGINRUNTIME_API ULuaDelegate : public UObject
{
	GENERATED_BODY()

	TMulticastScriptDelegate<FWeakObjectPtr>* TheDelegatePtr;
public:
	TMap<lua_State*, TSet<int>> LuaCallBacks;
	UFunction* FunSig;

	void Init(TMulticastScriptDelegate<FWeakObjectPtr>& Delegate, UFunction* _FunSig);
	void Init(void* Delegate, UFunction* _FunSig);

	UFUNCTION()
		void NoUseAtAll();

	virtual void ProcessEvent(UFunction* Function, void* Parms) override;

	UFUNCTION()
		int Add(Flua_State inL, Flua_Index StackIndex);
	UFUNCTION()
		void Remove(Flua_State inL, int32 r);

	UFUNCTION()
		void Destroy();

	UFUNCTION()
		void Fire(Flua_State inL);

};
