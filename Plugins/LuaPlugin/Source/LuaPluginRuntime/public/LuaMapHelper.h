// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UnrealType.h"
#include "LuaDelegate.h"
#include "LuaMapHelper.generated.h"

UCLASS()
class LUAPLUGINRUNTIME_API ULuaMapHelper : public UObject
{
	GENERATED_BODY()
public:
	ULuaMapHelper();
	void* Obj;

	UMapProperty* Property;
		
	void Init(void* _Obj, UMapProperty* _Property);

	template<class K, class V>
	TMap<K, V>* ValuePtr()
	{
		return Property->ContainerPtrToValuePtr<TMap<K, V>>(Obj);
	}

	UFUNCTION()
		static ULuaMapHelper* GetHelper(UObject* _Obj, const FName& PropertyName);

	static ULuaMapHelper* GetHelperCPP(void* _Obj, UMapProperty* Property);

	void Copy(FScriptMapHelper_InContainer& SrcMapHelper, FScriptMapHelper_InContainer& DestMapHelper, UMapProperty* p);
	void CopyTo(UMapProperty* p, void* ptr);
	void CopyFrom(UMapProperty* p, void* ptr);

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
