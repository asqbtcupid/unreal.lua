// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UnrealType.h"
#include "LuaDelegate.h"
#include "LuaArrayHelper.generated.h"

UCLASS()
class LUAPLUGINRUNTIME_API ULuaArrayHelper : public UObject
{
	GENERATED_BODY()
public:
	void* Obj;

	UArrayProperty* Property;
		
	void Init(void* _Obj, UArrayProperty* _Property);

	template<class T>
	TArray<T>* ValuePtr()
	{
		return Property->ContainerPtrToValuePtr<TArray<T>>(Obj);
	}

	UFUNCTION()
		static ULuaArrayHelper* GetHelper(UObject* _Obj, const FName& PropertyName);

	static ULuaArrayHelper* GetHelperCPP(void* _Obj, UArrayProperty* Property);
	
	UFUNCTION()
		int32 Num();

	UFUNCTION(meta = (LuaCustomReturn))
		int32 Get(Flua_State inL, int32 Index);

	UFUNCTION()
		void Set(Flua_State inL, int32 Index, Flua_Index Value);

	UFUNCTION()
		void Add(Flua_State inL, Flua_Index Value);

	UFUNCTION(meta = (LuaCustomReturn))
		int32 Remove(Flua_State inL, int32 Index);

	UFUNCTION(meta = (LuaCustomReturn))
		int32 Pop(Flua_State inL);

	UFUNCTION()
		void Insert(Flua_State inL, int32 Index, Flua_Index Value);

	UFUNCTION(BlueprintCallable, Category = JustForDefaultValue)
		void Empty(int32 Slack = 0);

	UFUNCTION(meta = (LuaCustomReturn))
		int32 Table(Flua_State inL);
		
};
