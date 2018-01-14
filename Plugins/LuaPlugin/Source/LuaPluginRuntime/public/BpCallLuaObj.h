// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TableUtil.h"
#include "BpCallLuaObj.generated.h"


UCLASS()
class LUAPLUGINRUNTIME_API UBpCallLuaObj : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
		 
public:
	UFUNCTION(BlueprintCallable, Category = "LuaClass",meta=(DisplayName="LuaCtor"))
		static void _LuaCtor(UObject* BPIns, const FString& LuaClassPath);
	UFUNCTION(BlueprintCallable, Category = "LuaClass", meta = (DisplayName = "LuaCall_0_0"))
		static void Call(UObject* Ins, const FString& FunctionName);

	inline static void push_and_call(int32 ParamCount, int32 ReturnCount, FFrame& Stack)
	{
		P_GET_OBJECT(UObject, Z_Param_Ins);
		P_GET_PROPERTY(UStrProperty, Z_Param_FunctionName);

		P_NATIVE_BEGIN;
		lua_State* L = UTableUtil::GetTheOnlyLuaState();
		lua_pushcfunction(L, ErrHandleFunc);
		if (Z_Param_Ins == nullptr)
		{
			lua_getfield(L, LUA_GLOBALSINDEX, TCHAR_TO_UTF8(*Z_Param_FunctionName));
#ifdef LuaDebug
			ensureAlwaysMsgf(!lua_isnil(L, -1), L"Func doesn't exist");
#endif
			for (int i = 0; i < ParamCount; i++)
			{
				if (Stack.PeekCode() != 0x17)
				{
					Stack.MostRecentProperty = nullptr;
					Stack.StepCompiledIn<UProperty>(NULL);
					UProperty* ParamProperty = Stack.MostRecentProperty;
					UTableUtil::pushproperty_valueptr(L, ParamProperty, Stack.MostRecentPropertyAddress);
				}
				// self	
				else
				{
					Stack.MostRecentProperty = nullptr;
					UObject* self = nullptr;
					Stack.StepCompiledIn<UProperty>(&self);
					UTableUtil::push(L, self);
				}
			}
			if (lua_pcall(L, ParamCount, ReturnCount, -(ParamCount+2)))
			{
				UTableUtil::log(lua_tostring(L, -1));
			}
		}
		else
		{
#ifdef LuaDebug
			ensureAlwaysMsgf(UTableUtil::existluains(L, Z_Param_Ins), L"lua ins doesn't exist");
#endif
			lua_getfield(L, LUA_GLOBALSINDEX, "Call");
			UTableUtil::pushall(L, Z_Param_FunctionName, Z_Param_Ins);
			for (int i = 0; i < ParamCount; i++)
			{
				Stack.MostRecentProperty = nullptr;
				Stack.StepCompiledIn<UProperty>(NULL);

				UProperty* ParamProperty = Stack.MostRecentProperty;
				UTableUtil::pushproperty_valueptr(L, ParamProperty, Stack.MostRecentPropertyAddress);
			}
			if (lua_pcall(L, ParamCount+2, ReturnCount, -(ParamCount+4)))
			{
				UTableUtil::log(lua_tostring(L, -1));
			}
		}

#ifdef LuaDebug
		if (lua_gettop(L) < (ReturnCount+1))
			ensureAlwaysMsgf(0, L"some bug?May be lua didn't return enough value");
#endif
		for (int i = 0; i < ReturnCount; i++)
		{
			Stack.MostRecentProperty = nullptr;
			Stack.StepCompiledIn<UProperty>(NULL);

			UProperty* ReturnProperty = Stack.MostRecentProperty;
			UTableUtil::popproperty(L, i-ReturnCount, ReturnProperty, Stack.Locals);
		}
		lua_pop(L, (1 + ReturnCount));

		P_NATIVE_END
		P_FINISH;
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_0_1", CustomStructureParam = "Return1"))
		static void Call_0_1(UObject* Ins, const FString& FunctionName, FVector& Return1);
	DECLARE_FUNCTION(execCall_0_1)
	{
		push_and_call(0, 1, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_0_2", CustomStructureParam = "Return1,Return2"))
		static void Call_0_2(UObject* Ins, const FString& FunctionName, FVector& Return1, FVector& Return2);
	DECLARE_FUNCTION(execCall_0_2)
	{
		push_and_call(0, 2, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_0_3", CustomStructureParam = "Return1,Return2,Return3"))
		static void Call_0_3(UObject* Ins, const FString& FunctionName, FVector& Return1, FVector& Return2, FVector& Return3);
	DECLARE_FUNCTION(execCall_0_3)
	{
		push_and_call(0, 3, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_0_4", CustomStructureParam = "Return1,Return2,Return3,Return4"))
		static void Call_0_4(UObject* Ins, const FString& FunctionName, FVector& Return1, FVector& Return2, FVector& Return3, FVector& Return4);
	DECLARE_FUNCTION(execCall_0_4)
	{
		push_and_call(0, 4, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_0_5", CustomStructureParam = "Return1,Return2,Return3,Return4,Return5"))
		static void Call_0_5(UObject* Ins, const FString& FunctionName, FVector& Return1, FVector& Return2, FVector& Return3, FVector& Return4, FVector& Return5);
	DECLARE_FUNCTION(execCall_0_5)
	{
		push_and_call(0, 5, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_0_6", CustomStructureParam = "Return1,Return2,Return3,Return4,Return5,Return6"))
		static void Call_0_6(UObject* Ins, const FString& FunctionName, FVector& Return1, FVector& Return2, FVector& Return3, FVector& Return4, FVector& Return5, FVector& Return6);
	DECLARE_FUNCTION(execCall_0_6)
	{
		push_and_call(0, 6, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_0_7", CustomStructureParam = "Return1,Return2,Return3,Return4,Return5,Return6,Return7"))
		static void Call_0_7(UObject* Ins, const FString& FunctionName, FVector& Return1, FVector& Return2, FVector& Return3, FVector& Return4, FVector& Return5, FVector& Return6, FVector& Return7);
	DECLARE_FUNCTION(execCall_0_7)
	{
		push_and_call(0, 7, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_1_0", CustomStructureParam = "Param1"))
		static void Call_1_0(UObject* Ins, const FString& FunctionName, const FVector& Param1);
	DECLARE_FUNCTION(execCall_1_0)
	{
		push_and_call(1, 0, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_1_1", CustomStructureParam = "Param1,Return1"))
		static void Call_1_1(UObject* Ins, const FString& FunctionName, const FVector& Param1, FVector& Return1);
	DECLARE_FUNCTION(execCall_1_1)
	{
		push_and_call(1, 1, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_1_2", CustomStructureParam = "Param1,Return1,Return2"))
		static void Call_1_2(UObject* Ins, const FString& FunctionName, const FVector& Param1, FVector& Return1, FVector& Return2);
	DECLARE_FUNCTION(execCall_1_2)
	{
		push_and_call(1, 2, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_1_3", CustomStructureParam = "Param1,Return1,Return2,Return3"))
		static void Call_1_3(UObject* Ins, const FString& FunctionName, const FVector& Param1, FVector& Return1, FVector& Return2, FVector& Return3);
	DECLARE_FUNCTION(execCall_1_3)
	{
		push_and_call(1, 3, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_1_4", CustomStructureParam = "Param1,Return1,Return2,Return3,Return4"))
		static void Call_1_4(UObject* Ins, const FString& FunctionName, const FVector& Param1, FVector& Return1, FVector& Return2, FVector& Return3, FVector& Return4);
	DECLARE_FUNCTION(execCall_1_4)
	{
		push_and_call(1, 4, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_1_5", CustomStructureParam = "Param1,Return1,Return2,Return3,Return4,Return5"))
		static void Call_1_5(UObject* Ins, const FString& FunctionName, const FVector& Param1, FVector& Return1, FVector& Return2, FVector& Return3, FVector& Return4, FVector& Return5);
	DECLARE_FUNCTION(execCall_1_5)
	{
		push_and_call(1, 5, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_1_6", CustomStructureParam = "Param1,Return1,Return2,Return3,Return4,Return5,Return6"))
		static void Call_1_6(UObject* Ins, const FString& FunctionName, const FVector& Param1, FVector& Return1, FVector& Return2, FVector& Return3, FVector& Return4, FVector& Return5, FVector& Return6);
	DECLARE_FUNCTION(execCall_1_6)
	{
		push_and_call(1, 6, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_1_7", CustomStructureParam = "Param1,Return1,Return2,Return3,Return4,Return5,Return6,Return7"))
		static void Call_1_7(UObject* Ins, const FString& FunctionName, const FVector& Param1, FVector& Return1, FVector& Return2, FVector& Return3, FVector& Return4, FVector& Return5, FVector& Return6, FVector& Return7);
	DECLARE_FUNCTION(execCall_1_7)
	{
		push_and_call(1, 7, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_2_0", CustomStructureParam = "Param1,Param2"))
		static void Call_2_0(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2);
	DECLARE_FUNCTION(execCall_2_0)
	{
		push_and_call(2, 0, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_2_1", CustomStructureParam = "Param1,Param2,Return1"))
		static void Call_2_1(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2, FVector& Return1);
	DECLARE_FUNCTION(execCall_2_1)
	{
		push_and_call(2, 1, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_2_2", CustomStructureParam = "Param1,Param2,Return1,Return2"))
		static void Call_2_2(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2, FVector& Return1, FVector& Return2);
	DECLARE_FUNCTION(execCall_2_2)
	{
		push_and_call(2, 2, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_2_3", CustomStructureParam = "Param1,Param2,Return1,Return2,Return3"))
		static void Call_2_3(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2, FVector& Return1, FVector& Return2, FVector& Return3);
	DECLARE_FUNCTION(execCall_2_3)
	{
		push_and_call(2, 3, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_2_4", CustomStructureParam = "Param1,Param2,Return1,Return2,Return3,Return4"))
		static void Call_2_4(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2, FVector& Return1, FVector& Return2, FVector& Return3, FVector& Return4);
	DECLARE_FUNCTION(execCall_2_4)
	{
		push_and_call(2, 4, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_2_5", CustomStructureParam = "Param1,Param2,Return1,Return2,Return3,Return4,Return5"))
		static void Call_2_5(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2, FVector& Return1, FVector& Return2, FVector& Return3, FVector& Return4, FVector& Return5);
	DECLARE_FUNCTION(execCall_2_5)
	{
		push_and_call(2, 5, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_2_6", CustomStructureParam = "Param1,Param2,Return1,Return2,Return3,Return4,Return5,Return6"))
		static void Call_2_6(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2, FVector& Return1, FVector& Return2, FVector& Return3, FVector& Return4, FVector& Return5, FVector& Return6);
	DECLARE_FUNCTION(execCall_2_6)
	{
		push_and_call(2, 6, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_2_7", CustomStructureParam = "Param1,Param2,Return1,Return2,Return3,Return4,Return5,Return6,Return7"))
		static void Call_2_7(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2, FVector& Return1, FVector& Return2, FVector& Return3, FVector& Return4, FVector& Return5, FVector& Return6, FVector& Return7);
	DECLARE_FUNCTION(execCall_2_7)
	{
		push_and_call(2, 7, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_3_0", CustomStructureParam = "Param1,Param2,Param3"))
		static void Call_3_0(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2, const FVector& Param3);
	DECLARE_FUNCTION(execCall_3_0)
	{
		push_and_call(3, 0, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_3_1", CustomStructureParam = "Param1,Param2,Param3,Return1"))
		static void Call_3_1(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2, const FVector& Param3, FVector& Return1);
	DECLARE_FUNCTION(execCall_3_1)
	{
		push_and_call(3, 1, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_3_2", CustomStructureParam = "Param1,Param2,Param3,Return1,Return2"))
		static void Call_3_2(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2, const FVector& Param3, FVector& Return1, FVector& Return2);
	DECLARE_FUNCTION(execCall_3_2)
	{
		push_and_call(3, 2, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_3_3", CustomStructureParam = "Param1,Param2,Param3,Return1,Return2,Return3"))
		static void Call_3_3(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2, const FVector& Param3, FVector& Return1, FVector& Return2, FVector& Return3);
	DECLARE_FUNCTION(execCall_3_3)
	{
		push_and_call(3, 3, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_3_4", CustomStructureParam = "Param1,Param2,Param3,Return1,Return2,Return3,Return4"))
		static void Call_3_4(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2, const FVector& Param3, FVector& Return1, FVector& Return2, FVector& Return3, FVector& Return4);
	DECLARE_FUNCTION(execCall_3_4)
	{
		push_and_call(3, 4, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_3_5", CustomStructureParam = "Param1,Param2,Param3,Return1,Return2,Return3,Return4,Return5"))
		static void Call_3_5(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2, const FVector& Param3, FVector& Return1, FVector& Return2, FVector& Return3, FVector& Return4, FVector& Return5);
	DECLARE_FUNCTION(execCall_3_5)
	{
		push_and_call(3, 5, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_3_6", CustomStructureParam = "Param1,Param2,Param3,Return1,Return2,Return3,Return4,Return5,Return6"))
		static void Call_3_6(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2, const FVector& Param3, FVector& Return1, FVector& Return2, FVector& Return3, FVector& Return4, FVector& Return5, FVector& Return6);
	DECLARE_FUNCTION(execCall_3_6)
	{
		push_and_call(3, 6, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_3_7", CustomStructureParam = "Param1,Param2,Param3,Return1,Return2,Return3,Return4,Return5,Return6,Return7"))
		static void Call_3_7(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2, const FVector& Param3, FVector& Return1, FVector& Return2, FVector& Return3, FVector& Return4, FVector& Return5, FVector& Return6, FVector& Return7);
	DECLARE_FUNCTION(execCall_3_7)
	{
		push_and_call(3, 7, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_4_0", CustomStructureParam = "Param1,Param2,Param3,Param4"))
		static void Call_4_0(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2, const FVector& Param3, const FVector& Param4);
	DECLARE_FUNCTION(execCall_4_0)
	{
		push_and_call(4, 0, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_4_1", CustomStructureParam = "Param1,Param2,Param3,Param4,Return1"))
		static void Call_4_1(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2, const FVector& Param3, const FVector& Param4, FVector& Return1);
	DECLARE_FUNCTION(execCall_4_1)
	{
		push_and_call(4, 1, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_4_2", CustomStructureParam = "Param1,Param2,Param3,Param4,Return1,Return2"))
		static void Call_4_2(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2, const FVector& Param3, const FVector& Param4, FVector& Return1, FVector& Return2);
	DECLARE_FUNCTION(execCall_4_2)
	{
		push_and_call(4, 2, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_4_3", CustomStructureParam = "Param1,Param2,Param3,Param4,Return1,Return2,Return3"))
		static void Call_4_3(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2, const FVector& Param3, const FVector& Param4, FVector& Return1, FVector& Return2, FVector& Return3);
	DECLARE_FUNCTION(execCall_4_3)
	{
		push_and_call(4, 3, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_4_4", CustomStructureParam = "Param1,Param2,Param3,Param4,Return1,Return2,Return3,Return4"))
		static void Call_4_4(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2, const FVector& Param3, const FVector& Param4, FVector& Return1, FVector& Return2, FVector& Return3, FVector& Return4);
	DECLARE_FUNCTION(execCall_4_4)
	{
		push_and_call(4, 4, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_4_5", CustomStructureParam = "Param1,Param2,Param3,Param4,Return1,Return2,Return3,Return4,Return5"))
		static void Call_4_5(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2, const FVector& Param3, const FVector& Param4, FVector& Return1, FVector& Return2, FVector& Return3, FVector& Return4, FVector& Return5);
	DECLARE_FUNCTION(execCall_4_5)
	{
		push_and_call(4, 5, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_4_6", CustomStructureParam = "Param1,Param2,Param3,Param4,Return1,Return2,Return3,Return4,Return5,Return6"))
		static void Call_4_6(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2, const FVector& Param3, const FVector& Param4, FVector& Return1, FVector& Return2, FVector& Return3, FVector& Return4, FVector& Return5, FVector& Return6);
	DECLARE_FUNCTION(execCall_4_6)
	{
		push_and_call(4, 6, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_4_7", CustomStructureParam = "Param1,Param2,Param3,Param4,Return1,Return2,Return3,Return4,Return5,Return6,Return7"))
		static void Call_4_7(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2, const FVector& Param3, const FVector& Param4, FVector& Return1, FVector& Return2, FVector& Return3, FVector& Return4, FVector& Return5, FVector& Return6, FVector& Return7);
	DECLARE_FUNCTION(execCall_4_7)
	{
		push_and_call(4, 7, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_5_0", CustomStructureParam = "Param1,Param2,Param3,Param4,Param5"))
		static void Call_5_0(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2, const FVector& Param3, const FVector& Param4, const FVector& Param5);
	DECLARE_FUNCTION(execCall_5_0)
	{
		push_and_call(5, 0, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_5_1", CustomStructureParam = "Param1,Param2,Param3,Param4,Param5,Return1"))
		static void Call_5_1(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2, const FVector& Param3, const FVector& Param4, const FVector& Param5, FVector& Return1);
	DECLARE_FUNCTION(execCall_5_1)
	{
		push_and_call(5, 1, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_5_2", CustomStructureParam = "Param1,Param2,Param3,Param4,Param5,Return1,Return2"))
		static void Call_5_2(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2, const FVector& Param3, const FVector& Param4, const FVector& Param5, FVector& Return1, FVector& Return2);
	DECLARE_FUNCTION(execCall_5_2)
	{
		push_and_call(5, 2, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_5_3", CustomStructureParam = "Param1,Param2,Param3,Param4,Param5,Return1,Return2,Return3"))
		static void Call_5_3(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2, const FVector& Param3, const FVector& Param4, const FVector& Param5, FVector& Return1, FVector& Return2, FVector& Return3);
	DECLARE_FUNCTION(execCall_5_3)
	{
		push_and_call(5, 3, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_5_4", CustomStructureParam = "Param1,Param2,Param3,Param4,Param5,Return1,Return2,Return3,Return4"))
		static void Call_5_4(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2, const FVector& Param3, const FVector& Param4, const FVector& Param5, FVector& Return1, FVector& Return2, FVector& Return3, FVector& Return4);
	DECLARE_FUNCTION(execCall_5_4)
	{
		push_and_call(5, 4, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_5_5", CustomStructureParam = "Param1,Param2,Param3,Param4,Param5,Return1,Return2,Return3,Return4,Return5"))
		static void Call_5_5(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2, const FVector& Param3, const FVector& Param4, const FVector& Param5, FVector& Return1, FVector& Return2, FVector& Return3, FVector& Return4, FVector& Return5);
	DECLARE_FUNCTION(execCall_5_5)
	{
		push_and_call(5, 5, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_5_6", CustomStructureParam = "Param1,Param2,Param3,Param4,Param5,Return1,Return2,Return3,Return4,Return5,Return6"))
		static void Call_5_6(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2, const FVector& Param3, const FVector& Param4, const FVector& Param5, FVector& Return1, FVector& Return2, FVector& Return3, FVector& Return4, FVector& Return5, FVector& Return6);
	DECLARE_FUNCTION(execCall_5_6)
	{
		push_and_call(5, 6, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_5_7", CustomStructureParam = "Param1,Param2,Param3,Param4,Param5,Return1,Return2,Return3,Return4,Return5,Return6,Return7"))
		static void Call_5_7(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2, const FVector& Param3, const FVector& Param4, const FVector& Param5, FVector& Return1, FVector& Return2, FVector& Return3, FVector& Return4, FVector& Return5, FVector& Return6, FVector& Return7);
	DECLARE_FUNCTION(execCall_5_7)
	{
		push_and_call(5, 7, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_6_0", CustomStructureParam = "Param1,Param2,Param3,Param4,Param5,Param6"))
		static void Call_6_0(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2, const FVector& Param3, const FVector& Param4, const FVector& Param5, const FVector& Param6);
	DECLARE_FUNCTION(execCall_6_0)
	{
		push_and_call(6, 0, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_6_1", CustomStructureParam = "Param1,Param2,Param3,Param4,Param5,Param6,Return1"))
		static void Call_6_1(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2, const FVector& Param3, const FVector& Param4, const FVector& Param5, const FVector& Param6, FVector& Return1);
	DECLARE_FUNCTION(execCall_6_1)
	{
		push_and_call(6, 1, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_6_2", CustomStructureParam = "Param1,Param2,Param3,Param4,Param5,Param6,Return1,Return2"))
		static void Call_6_2(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2, const FVector& Param3, const FVector& Param4, const FVector& Param5, const FVector& Param6, FVector& Return1, FVector& Return2);
	DECLARE_FUNCTION(execCall_6_2)
	{
		push_and_call(6, 2, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_6_3", CustomStructureParam = "Param1,Param2,Param3,Param4,Param5,Param6,Return1,Return2,Return3"))
		static void Call_6_3(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2, const FVector& Param3, const FVector& Param4, const FVector& Param5, const FVector& Param6, FVector& Return1, FVector& Return2, FVector& Return3);
	DECLARE_FUNCTION(execCall_6_3)
	{
		push_and_call(6, 3, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_6_4", CustomStructureParam = "Param1,Param2,Param3,Param4,Param5,Param6,Return1,Return2,Return3,Return4"))
		static void Call_6_4(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2, const FVector& Param3, const FVector& Param4, const FVector& Param5, const FVector& Param6, FVector& Return1, FVector& Return2, FVector& Return3, FVector& Return4);
	DECLARE_FUNCTION(execCall_6_4)
	{
		push_and_call(6, 4, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_6_5", CustomStructureParam = "Param1,Param2,Param3,Param4,Param5,Param6,Return1,Return2,Return3,Return4,Return5"))
		static void Call_6_5(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2, const FVector& Param3, const FVector& Param4, const FVector& Param5, const FVector& Param6, FVector& Return1, FVector& Return2, FVector& Return3, FVector& Return4, FVector& Return5);
	DECLARE_FUNCTION(execCall_6_5)
	{
		push_and_call(6, 5, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_6_6", CustomStructureParam = "Param1,Param2,Param3,Param4,Param5,Param6,Return1,Return2,Return3,Return4,Return5,Return6"))
		static void Call_6_6(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2, const FVector& Param3, const FVector& Param4, const FVector& Param5, const FVector& Param6, FVector& Return1, FVector& Return2, FVector& Return3, FVector& Return4, FVector& Return5, FVector& Return6);
	DECLARE_FUNCTION(execCall_6_6)
	{
		push_and_call(6, 6, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_6_7", CustomStructureParam = "Param1,Param2,Param3,Param4,Param5,Param6,Return1,Return2,Return3,Return4,Return5,Return6,Return7"))
		static void Call_6_7(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2, const FVector& Param3, const FVector& Param4, const FVector& Param5, const FVector& Param6, FVector& Return1, FVector& Return2, FVector& Return3, FVector& Return4, FVector& Return5, FVector& Return6, FVector& Return7);
	DECLARE_FUNCTION(execCall_6_7)
	{
		push_and_call(6, 7, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_7_0", CustomStructureParam = "Param1,Param2,Param3,Param4,Param5,Param6,Param7"))
		static void Call_7_0(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2, const FVector& Param3, const FVector& Param4, const FVector& Param5, const FVector& Param6, const FVector& Param7);
	DECLARE_FUNCTION(execCall_7_0)
	{
		push_and_call(7, 0, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_7_1", CustomStructureParam = "Param1,Param2,Param3,Param4,Param5,Param6,Param7,Return1"))
		static void Call_7_1(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2, const FVector& Param3, const FVector& Param4, const FVector& Param5, const FVector& Param6, const FVector& Param7, FVector& Return1);
	DECLARE_FUNCTION(execCall_7_1)
	{
		push_and_call(7, 1, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_7_2", CustomStructureParam = "Param1,Param2,Param3,Param4,Param5,Param6,Param7,Return1,Return2"))
		static void Call_7_2(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2, const FVector& Param3, const FVector& Param4, const FVector& Param5, const FVector& Param6, const FVector& Param7, FVector& Return1, FVector& Return2);
	DECLARE_FUNCTION(execCall_7_2)
	{
		push_and_call(7, 2, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_7_3", CustomStructureParam = "Param1,Param2,Param3,Param4,Param5,Param6,Param7,Return1,Return2,Return3"))
		static void Call_7_3(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2, const FVector& Param3, const FVector& Param4, const FVector& Param5, const FVector& Param6, const FVector& Param7, FVector& Return1, FVector& Return2, FVector& Return3);
	DECLARE_FUNCTION(execCall_7_3)
	{
		push_and_call(7, 3, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_7_4", CustomStructureParam = "Param1,Param2,Param3,Param4,Param5,Param6,Param7,Return1,Return2,Return3,Return4"))
		static void Call_7_4(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2, const FVector& Param3, const FVector& Param4, const FVector& Param5, const FVector& Param6, const FVector& Param7, FVector& Return1, FVector& Return2, FVector& Return3, FVector& Return4);
	DECLARE_FUNCTION(execCall_7_4)
	{
		push_and_call(7, 4, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_7_5", CustomStructureParam = "Param1,Param2,Param3,Param4,Param5,Param6,Param7,Return1,Return2,Return3,Return4,Return5"))
		static void Call_7_5(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2, const FVector& Param3, const FVector& Param4, const FVector& Param5, const FVector& Param6, const FVector& Param7, FVector& Return1, FVector& Return2, FVector& Return3, FVector& Return4, FVector& Return5);
	DECLARE_FUNCTION(execCall_7_5)
	{
		push_and_call(7, 5, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_7_6", CustomStructureParam = "Param1,Param2,Param3,Param4,Param5,Param6,Param7,Return1,Return2,Return3,Return4,Return5,Return6"))
		static void Call_7_6(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2, const FVector& Param3, const FVector& Param4, const FVector& Param5, const FVector& Param6, const FVector& Param7, FVector& Return1, FVector& Return2, FVector& Return3, FVector& Return4, FVector& Return5, FVector& Return6);
	DECLARE_FUNCTION(execCall_7_6)
	{
		push_and_call(7, 6, Stack);
	}

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "LuaClass", meta = (DisplayName = "LuaCall_7_7", CustomStructureParam = "Param1,Param2,Param3,Param4,Param5,Param6,Param7,Return1,Return2,Return3,Return4,Return5,Return6,Return7"))
		static void Call_7_7(UObject* Ins, const FString& FunctionName, const FVector& Param1, const FVector& Param2, const FVector& Param3, const FVector& Param4, const FVector& Param5, const FVector& Param6, const FVector& Param7, FVector& Return1, FVector& Return2, FVector& Return3, FVector& Return4, FVector& Return5, FVector& Return6, FVector& Return7);
	DECLARE_FUNCTION(execCall_7_7)
	{
		push_and_call(7, 7, Stack);
	}
};
