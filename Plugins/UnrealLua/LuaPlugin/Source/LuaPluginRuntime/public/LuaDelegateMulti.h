// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ScriptDelegates.h"
#include "LuaDelegateMulti.generated.h"
 
struct lua_State;
UCLASS()
class LUAPLUGINRUNTIME_API ULuaDelegateMulti : public UObject
{
	GENERATED_BODY()

	TMulticastScriptDelegate<FWeakObjectPtr>* TheDelegatePtr;
public:
	TMap<lua_State*, TSet<int>> LuaCallBacks;
	TMap<lua_State*, TSet<int>> LuaCallBacksToRemove;
	bool bIsProcessingEvent;
	UFunction* FunSig;

	void Init(TMulticastScriptDelegate<FWeakObjectPtr>& Delegate, UFunction* _FunSig);
	void Init(void* Delegate, UFunction* _FunSig);

	UFUNCTION()
		void NoUseAtAll();

	virtual void ProcessEvent(UFunction* Function, void* Parms) override;

		int Add(lua_State* inL);
		void Remove(lua_State* inL);
		void RemoveAll();
		void Destroy();
		void Clear();
		void Fire(lua_State* inL);
		void RemoveUObject(UObject* Ptr);
		void RemoveUFunction(UObject* Ptr, FName InFunctionName);
};
