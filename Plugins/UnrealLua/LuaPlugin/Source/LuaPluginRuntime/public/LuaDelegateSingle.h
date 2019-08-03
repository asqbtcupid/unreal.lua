// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ScriptDelegates.h"
#include "LuaDelegateMulti.h"
#include "LuaDelegateSingle.generated.h"

UCLASS()
class LUAPLUGINRUNTIME_API ULuaDelegateSingle : public UObject
{
	GENERATED_BODY()

	FScriptDelegate* TheDelegatePtr;
	FScriptDelegate* TheDelegatePtrInHeap;
	UFunction* FunSig;
	ULuaDelegateSingle():TheDelegatePtr(nullptr), TheDelegatePtrInHeap(nullptr), FunSig(nullptr), LuaCallBack(-1){}
public:
	int LuaCallBack;
	lua_State* TheBindLuaState;
	void Init(FScriptDelegate* InTheDelegatePtr, UFunction* InFunSig);

	UFUNCTION()
		void NoUseAtAll();

	virtual void ProcessEvent(UFunction* Function, void* Parms) override;

	static ULuaDelegateSingle* CreateInCppCopy(FScriptDelegate* InTheDelegatePtr, UFunction* InFuncSig);
	static ULuaDelegateSingle* CreateInCppRef(FScriptDelegate* InTheDelegatePtr, UFunction* InFuncSig);

	FScriptDelegate* GetCppDelegate();

		static ULuaDelegateSingle* Create(lua_State* inL);
		void SetFuncSig(UFunction* InFunSig);
		int Bind(lua_State* inL);
		void Clear();
		void BindUFunction(UObject* InObject, const FName& InFunctionName);
		bool IsBound();
		void Destroy();
		void Fire(lua_State* inL);

};
