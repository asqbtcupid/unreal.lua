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
