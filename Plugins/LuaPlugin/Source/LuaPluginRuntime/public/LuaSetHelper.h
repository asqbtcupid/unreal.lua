// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UnrealType.h"
#include "LuaDelegate.h"
#include "LuaSetHelper.generated.h"

/**
 * 
 */
UCLASS()
class LUAPLUGINRUNTIME_API ULuaSetHelper : public UObject
{
	GENERATED_BODY()
	
public:
	ULuaSetHelper();
	void* Obj;

	USetProperty* Property;

	void Init(void* _Obj, USetProperty* _Property);
	void Init_ValuePtr(void* _Obj, USetProperty* _Property);

	template<class T>
	TSet<T>* ValuePtr()
	{
		return (TSet<T>*)(Obj);
	}

	UFUNCTION()
		static ULuaSetHelper* GetHelper(UObject* _Obj, const FName& PropertyName);

	static ULuaSetHelper* GetHelperCPP(void* _Obj, USetProperty* Property);
	static ULuaSetHelper* GetHelperCPP_ValuePtr(void* _Obj, USetProperty* Property);

	void Copy(FScriptSetHelper& SrcSetHelper, FScriptSetHelper& DestSetHelper, USetProperty* p);
	void CopyTo(USetProperty* p, void* ptr);
	void CopyFrom(USetProperty* p, void* ptr);

	UFUNCTION()
		int32 Num();

	UFUNCTION(meta = (LuaCustomReturn))
		int32 Get(Flua_State inL);

	UFUNCTION()
		void Set(Flua_State inL);

	UFUNCTION()
		void Add(Flua_State inL);

	UFUNCTION(BlueprintCallable, Category = JustForDefaultValue)
		void Reset();

	UFUNCTION(meta = (LuaCustomReturn))
		int32 Table(Flua_State inL);
	
	
};
