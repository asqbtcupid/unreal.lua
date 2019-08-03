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
LUA_GLUE_END()