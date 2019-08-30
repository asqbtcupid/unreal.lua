#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UnrealLua.h"
#include "BaseEngineLuaGlue.h"
#include "BenchmarkActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBenchDelegate_2Param, int, i, FVector, v);
UCLASS()
class CATCHME_API UBenchmarkActor : public UObject
{
	GENERATED_BODY()
	
public:	
	UBenchmarkActor();

	UFUNCTION()
		void TestCallFunc_0param() {}

	UFUNCTION()
		void TestCallFunc_1param_int(int i) {}

	UFUNCTION()
		void TestCallFunc_1param_int_ref(int& i) {}

	UFUNCTION()
		void TestCallFunc_1param_FVector(FVector i) {}

	UFUNCTION()
		void TestCallFunc_1param_FVector_ref(FVector& i) {}

	UFUNCTION()
		void TestCallFunc_3param(int i, UObject* p, FVector j) {}

	UFUNCTION()
		int TestCallFunc_Ret_int() { return 1; }

	UFUNCTION()
		FVector TestCallFunc_Ret_FVector() { return FVector(); }

	UPROPERTY()
		int ReadWriteInt;

	UPROPERTY()
		FVector ReadWriteVector;

	UPROPERTY()
		FBenchDelegate_2Param BenchDelegate_2Param;

	UFUNCTION()
		void Call_BenchDelegate_2Param(int Count);

	void Glue_TestCallFunc_0param() {}
	void Glue_TestCallFunc_1param_int(int i) {}
	void Glue_TestCallFunc_1param_int_ref(int& i) {}
	void Glue_TestCallFunc_1param_FVector(FVector i) {}
	void Glue_TestCallFunc_1param_FVector_ref(FVector& i) {}
	void Glue_TestCallFunc_3param(int i, UObject* p, FVector j) {}
	int Glue_TestCallFunc_Ret_int() { return 1; }
	FVector Glue_TestCallFunc_Ret_FVector() { return FVector(); }
	int Glue_ReadWriteInt;
	FVector Glue_ReadWriteVector;

	static int Glue_StaticReadWriteInt;
	static FVector Glue_StaticReadWriteVector;
};

LUA_GLUE_BEGIN(UBenchmarkActor)
LUA_GLUE_FUNCTION(Glue_TestCallFunc_0param)
LUA_GLUE_FUNCTION(Glue_TestCallFunc_1param_int)
LUA_GLUE_FUNCTION(Glue_TestCallFunc_1param_int_ref)
LUA_GLUE_FUNCTION(Glue_TestCallFunc_1param_FVector)
LUA_GLUE_FUNCTION(Glue_TestCallFunc_1param_FVector_ref)
LUA_GLUE_FUNCTION(Glue_TestCallFunc_3param)
LUA_GLUE_FUNCTION(Glue_TestCallFunc_Ret_int)
LUA_GLUE_FUNCTION(Glue_TestCallFunc_Ret_FVector)
LUA_GLUE_PROPERTY(Glue_ReadWriteInt)
LUA_GLUE_PROPERTY(Glue_ReadWriteVector)
LUA_GLUE_PROPERTY(Glue_StaticReadWriteInt)
LUA_GLUE_PROPERTY(Glue_StaticReadWriteVector)
LUA_GLUE_END()

struct TestGlueEx
{
	int TwoParam(int i, const FVector& j) { return i; } 
	FVector Vec;
	FVector& ReturnStruct() { return Vec; }
	FVector ReturnStructCopy() { return Vec; }

	static int TwoParamEx(lua_State*L)
	{
		TestGlueEx* obj = (TestGlueEx*)(*(void**)(lua_touserdata(L, 1)));
		int i = lua_tointeger(L, 2);
		FVector* j = (FVector*)(*(void**)(lua_touserdata(L, 3)));
		int result = obj->TwoParam(i, *j);
		lua_pushinteger(L, result);
		return 1;
	}

	void Glue_TestCallFunc_0param() {}
	void Glue_TestCallFunc_1param_int(int i) {}
	void Glue_TestCallFunc_1param_int_ref(int& i) {}
	void Glue_TestCallFunc_1param_FVector(FVector i) {}
	void Glue_TestCallFunc_1param_FVector_ref(FVector& i) {}
	void Glue_TestCallFunc_3param(int i, UObject* p, FVector j) {}
	int Glue_TestCallFunc_Ret_int() { return 1; }
	FVector Glue_TestCallFunc_Ret_FVector() { return FVector(); }
	int Glue_ReadWriteInt;
	FVector Glue_ReadWriteVector;

	static int Glue_StaticReadWriteInt;
	static FVector Glue_StaticReadWriteVector;
};


LUA_GLUE_BEGIN(TestGlueEx)
LUA_GLUE_FUNCTION(TwoParam)
LUA_GLUE_FUNCTION(TwoParamEx)
LUA_GLUE_FUNCTION(ReturnStruct)
LUA_GLUE_FUNCTION(ReturnStructCopy)
LUA_GLUE_PROPERTY(Vec)

LUA_GLUE_FUNCTION(Glue_TestCallFunc_0param)
LUA_GLUE_FUNCTION(Glue_TestCallFunc_1param_int)
LUA_GLUE_FUNCTION(Glue_TestCallFunc_1param_int_ref)
LUA_GLUE_FUNCTION(Glue_TestCallFunc_1param_FVector)
LUA_GLUE_FUNCTION(Glue_TestCallFunc_1param_FVector_ref)
LUA_GLUE_FUNCTION(Glue_TestCallFunc_3param)
LUA_GLUE_FUNCTION(Glue_TestCallFunc_Ret_int)
LUA_GLUE_FUNCTION(Glue_TestCallFunc_Ret_FVector)
LUA_GLUE_PROPERTY(Glue_ReadWriteInt)
LUA_GLUE_PROPERTY(Glue_ReadWriteVector)
LUA_GLUE_PROPERTY(Glue_StaticReadWriteInt)
LUA_GLUE_PROPERTY(Glue_StaticReadWriteVector)
LUA_GLUE_END()


USTRUCT()
struct FTestGlueExBp
{
	GENERATED_BODY()

	UPROPERTY()
	int ReadWriteInt;

	UPROPERTY()
	FVector ReadWriteVector;

	void Glue_TestCallFunc_0param() {}
	void Glue_TestCallFunc_1param_int(int i) {}
	void Glue_TestCallFunc_1param_int_ref(int& i) {}
	void Glue_TestCallFunc_1param_FVector(FVector i) {}
	void Glue_TestCallFunc_1param_FVector_ref(FVector& i) {}
	void Glue_TestCallFunc_3param(int i, UObject* p, FVector j) {}
	int Glue_TestCallFunc_Ret_int() { return 1; }
	FVector Glue_TestCallFunc_Ret_FVector() { return FVector(); }
	int Glue_ReadWriteInt;
	FVector Glue_ReadWriteVector;

	static int Glue_StaticReadWriteInt;
	static FVector Glue_StaticReadWriteVector;
};

LUA_GLUE_BEGIN(FTestGlueExBp)

LUA_GLUE_FUNCTION(Glue_TestCallFunc_0param)
LUA_GLUE_FUNCTION(Glue_TestCallFunc_1param_int)
LUA_GLUE_FUNCTION(Glue_TestCallFunc_1param_int_ref)
LUA_GLUE_FUNCTION(Glue_TestCallFunc_1param_FVector)
LUA_GLUE_FUNCTION(Glue_TestCallFunc_1param_FVector_ref)
LUA_GLUE_FUNCTION(Glue_TestCallFunc_3param)
LUA_GLUE_FUNCTION(Glue_TestCallFunc_Ret_int)
LUA_GLUE_FUNCTION(Glue_TestCallFunc_Ret_FVector)
LUA_GLUE_PROPERTY(Glue_ReadWriteInt)
LUA_GLUE_PROPERTY(Glue_ReadWriteVector)
LUA_GLUE_PROPERTY(Glue_StaticReadWriteInt)
LUA_GLUE_PROPERTY(Glue_StaticReadWriteVector)
LUA_GLUE_END()