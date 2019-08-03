// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LuaDelegateMulti.h"
#include "BPAndLuaBridge.generated.h"

//copy from unreal.js
struct FScopedArguments
{
	FScopedArguments(UFunction* InFunction, uint8* InBuffer)
		: Function(InFunction), Buffer(InBuffer)
	{
		for (TFieldIterator<UProperty> It(Function); It && (It->PropertyFlags & CPF_Parm) == CPF_Parm; ++It)
		{
			auto Prop = *It;
			Prop->InitializeValue_InContainer(Buffer);
		}
	}

	~FScopedArguments()
	{
		for (TFieldIterator<UProperty> It(Function); It && (It->PropertyFlags & CPF_Parm) == CPF_Parm; ++It)
		{
			auto Prop = *It;
			Prop->DestroyValue_InContainer(Buffer);
		}
	}

	UFunction* Function;
	uint8* Buffer;
};

UCLASS()
class LUAPLUGINRUNTIME_API UBPAndLuaBridge : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
// 	UFUNCTION()
// 		static bool IsBpIns(UObject *p);
// 
// 	UFUNCTION()
// 		static TMap<FString, UObject*> GetClassBPFunctions(UObject *p);
// 
// 	UFUNCTION()
// 		static TMap<FString, UObject*> GetClassBPPropertys(UObject *p);
// 
// 	UFUNCTION(meta = (LuaCustomReturn))
// 		static int32 CallBlueprintFunction(Flua_State inL, UObject* funcsig, UObject* Obj);
// 
// 	UFUNCTION(meta = (LuaCustomReturn))
// 		static int32 GetBlueprintProperty(Flua_State inL, UObject *Property, UObject* Obj);
// 
// 	UFUNCTION()
// 		static void SetBlueprintProperty(Flua_State inL, UObject *Property, UObject* Obj, Flua_Index Index);
};
