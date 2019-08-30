#include "BenchmarkActor.h"
#include "UnrealLua.h"

UBenchmarkActor::UBenchmarkActor()
{}

void UBenchmarkActor::Call_BenchDelegate_2Param(int Count)
{
	for (int i = 0; i < Count; i++)
	{
		BenchDelegate_2Param.Broadcast(i, FVector(1, 2, 3));
	}
}

int UBenchmarkActor::Glue_StaticReadWriteInt;
FVector UBenchmarkActor::Glue_StaticReadWriteVector;

int TestGlueEx::Glue_StaticReadWriteInt;
FVector TestGlueEx::Glue_StaticReadWriteVector;

int FTestGlueExBp::Glue_StaticReadWriteInt;
FVector FTestGlueExBp::Glue_StaticReadWriteVector;
