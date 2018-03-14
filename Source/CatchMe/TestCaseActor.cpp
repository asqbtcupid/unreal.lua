// Fill out your copyright notice in the Description page of Project Settings.

#include "TestCaseActor.h"
#include "TableUtil.h"
#include "Vector.lua.h"
#include "TestFirstMemStruct_Public.lua.h"
#include "TestFirstMemStruct_Nest.lua.h"
#include "TestFirstMemStruct_Private.lua.h"

// Sets default values
ATestCaseActor::ATestCaseActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATestCaseActor::BeginPlay()
{
	test_bool_public = true;
	test_int_public = 3423;
	test_int64_public = 82374;
	test_byte_public = 22;
	test_float_public = 34.23;
	test_double_public = 283814.3;
	test_string_public = "nativestring";
	test_text_public = FText::FromString("nativetext");
	test_name_public = FName("nativename");
	test_vector_public = FVector(12, 34, 56);
	test_object_public = this;
	test_class_public = ATestCaseActor::StaticClass();
	test_weakptr_public = this;

	test_arr_public.Add(2);
	test_arr_public.Add(3);
	test_arr_public.Add(7);

	test_arr_struct_public.Add(FVector(55, 66, 77));
	test_arr_struct_public.Add(FVector(123, 456, 789));

	test_set_public.Add(9);
	test_set_public.Add(99);
	test_set_public.Add(199);

	test_map_public.Add("test1", 100);
	test_map_public.Add("test2", 200);

	test_bool_private = true;
	test_int_private = 3423;
	test_int64_private = 82374;
	test_byte_private = 22;
	test_float_private = 34.23;
	test_double_private = 283814.3;
	test_string_private = "nativestring";
	test_text_private = FText::FromString("nativetext");
	test_name_private = FName("nativename");
	test_vector_private = FVector(12, 34, 56);
	test_object_private = this;
	test_class_private = ATestCaseActor::StaticClass();
	test_weakptr_private = this;

	test_arr_private.Add(2);
	test_arr_private.Add(3);
	test_arr_private.Add(7);

	test_arr_struct_private.Add(FVector(55, 66, 77));
	test_arr_struct_private.Add(FVector(123, 456, 789));

	test_set_private.Add(9);
	test_set_private.Add(99);
	test_set_private.Add(199);

	test_map_copy_public.Add("haha", FVector(1, 2, 3));
	test_map_copy_public.Add("jiji", FVector(2, 3, 4));

	test_map_copy_private.Add("haha", FVector(1, 2, 3));
	test_map_copy_private.Add("jiji", FVector(2, 3, 4));

	test_set_copy_private.Add(FVector(7, 8, 9));
	test_set_copy_public.Add(FVector(7, 8, 9));
	Super::BeginPlay();
	TestCallLua();

}

void ATestCaseActor::TestCallLua()
{
	test_bool_public = true;
	test_int_public = 3423;
	test_int64_public = 82374;
	test_byte_public = 22;
	test_float_public = 34.23;
	test_double_public = 283814.3;
	test_string_public = "nativestring";
	test_text_public = FText::FromString("nativetext");
	test_name_public = FName("nativename");
	test_vector_public = FVector(12, 34, 56);
	test_object_public = this;
	test_class_public = ATestCaseActor::StaticClass();
	test_weakptr_public = this;

	test_arr_public.Reset();
	test_arr_public.Add(2);
	test_arr_public.Add(3);
	test_arr_public.Add(7);

	test_arr_struct_public.Reset();
	test_arr_struct_public.Add(FVector(55, 66, 77));
	test_arr_struct_public.Add(FVector(123, 456, 789));

	test_set_public.Reset();
	test_set_public.Add(9);
	test_set_public.Add(99);
	test_set_public.Add(199);

	test_map_public.Reset();
	test_map_public.Add("test1", 100);
	test_map_public.Add("test2", 200);
	LuaCall("TestCallLua", this, test_bool_public, test_int_public, test_int64_public, test_byte_public, test_float_public, 
		test_double_public, test_string_public, test_text_public, test_name_public, test_vector_public, test_object_public,
		test_class_public, test_weakptr_public, test_arr_public, test_arr_struct_public, test_set_public, test_map_public
		);
}

// Called every frame
void ATestCaseActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATestCaseActor::TestLoad1()
{
}

void ATestCaseActor::TestLoad2()
{
}

void ATestCaseActor::RunCppCallLuaTest()
{

}

int32 ATestCaseActor::TestNativeFunction(bool test_bool, int32 test_int, int64 test_int64, uint8 test_byte, float test_float, 
	double test_double, FString test_string, FText test_text, FName test_name, FVector test_struct, 
	UObject* test_obj, TSubclassOf<AActor> test_class, TArray<int32> test_arr, TSet<int32> test_set, TMap<FString, int32> test_map)
{
	check(test_bool == true);
	check(test_int == 1234);
	check(test_int64 == 567895678);
	check(test_byte == 31);
	check(FMath::IsNearlyEqual(test_float, 81723.87f));
	check(FMath::IsNearlyEqual(test_double, 981723879412.89));
	check(test_string == "str");
	check(test_text.ToString() == "txt");
	check(test_name == FName("name"));
	check(test_struct.X == 3.0f);
	check(test_struct.Y == 2.0f);
	check(test_struct.Z == 1.0f);
	check(test_obj == this);
	check(test_class == AActor::StaticClass());
	check(test_arr.Num() == 3);
	check(test_arr[0] == 2);
	check(test_arr[1] == 8);
	check(test_arr[2] == 8);
	check(test_set.Num() == 3);
	check(test_set.Contains(9));
	check(test_set.Contains(8));
	check(test_set.Contains(1));
	check(test_map.Num() == 3);
	check(test_map["test1"] == 99);
	check(test_map["test2"] == 999);
	check(test_map["test3"] == 9999);

	return 812738174;
}

int32 ATestCaseActor::TestNativeFunction_Ref(bool& test_bool, int32& test_int, int64& test_int64, uint8& test_byte, float& test_float, double& test_double, FString& test_string, FText& test_text, FName& test_name, FVector& test_struct, UObject*& test_obj, TSubclassOf<AActor>& test_class)
{
	check(test_bool == true);
	test_bool = false;
	check(test_int == 1234);
	test_int = 4321;
	check(test_int64 == 567895678);
	test_int64 = 8765431;
	check(test_byte == 31);
	test_byte = 32;
	check(FMath::IsNearlyEqual(test_float, 81723.87f));
	test_float = 85555.87f;
	check(FMath::IsNearlyEqual(test_double, 981723879412.89));
	test_double = 8981723879412.89;
	check(test_string == "str");
	test_string = "strstr";
	check(test_text.ToString() == "txt");
	test_text = FText::FromString("txttxt");
	check(test_name == FName("name"));
	test_name = FName("namename");
	check(test_struct.X == 3.0f);
	check(test_struct.Y == 2.0f);
	check(test_struct.Z == 1.0f);
	test_struct = FVector(44, 55, 66);
	check(test_obj == this);
	test_obj = nullptr;
	check(test_class == AActor::StaticClass());
	test_class = ATestCaseActor::StaticClass();
	return 712738174;
}

void ATestCaseActor::FuncReturn_ParamArr(TArray<int32>& Arr)
{
	Arr.Reset();
	Arr.Add(100);
	Arr.Add(200);
}

void ATestCaseActor::FuncReturn_ParamMap(TMap<FString, int32>& Map)
{
	Map.Reset();
	Map.Add("hehe", 100);
}

void ATestCaseActor::FuncReturn_ParamMap_struct(TMap<FString, FVector>& Map)
{
	Map.Reset();
	Map.Add("test1", FVector(1, 2, 3));
	Map.Add("test2", FVector(4, 5, 6));
}

FVector ATestCaseActor::FuncReturnStruct()
{
	return FVector(11, 22, 12);
}

TMap<FString, FVector> ATestCaseActor::FuncReturnMap()
{
	TMap<FString, FVector> result;
	result.Add("lala1", FVector(100, 200, 300));
	result.Add("lala2", FVector(222, 333, 444));
	return result;
}

TSet<FVector> ATestCaseActor::FuncReturnSet_public(TSet<FVector>& Set)
{
	Set.Reset();
	Set.Add(FVector(99, 88, 77));
	TSet<FVector> result;
	result.Add(FVector(9, 8, 7));
	return result;
}

void ATestCaseActor::TestSpeed_Native(int32 i)
{
	for (int j = 0; j < i; j++)
	{
		FuncReturn_ParamMap_struct(test_map_copy_public);
	}
}

void ATestCaseActor::Func_Test_Enum(TestEnum enum_Param)
{

}

int32 ATestCaseActor::Interface_NativeEvent_Implementation(int32 v)
{
	checkf(v == 200, L"");
	return 1999;
}

void ATestCaseActor::Test_Param_Interface_public(TScriptInterface<IForTestInterface> I)
{
	checkf(I.GetObject() == this, L"");
}

void ATestCaseActor::Test_FirstMem_Struct()
{
	LuaCall("TestFistMemOfStruct_Internal", this, FTestFirstMemStruct_Public(), FTestFirstMemStruct_Nest(), FTestFirstMemStruct_Private());
}

void ATestCaseActor::Test_PushStack_Struct()
{
	FVector test(1,2,3);
	LuaCall("PushStackStruct", this, test, test);
}

void ATestCaseActor::Test_Param_Interface_private(TScriptInterface<IForTestInterface> I)
{
	checkf(I.GetObject() == this, L"");
}

void ATestCaseActor::FuncReturn_ParamMap_struct_private(TMap<FString, FVector>& Map)
{
	Map.Reset();
	Map.Add("test1", FVector(1, 2, 3));
	Map.Add("test2", FVector(4, 5, 6));
}

TSet<FVector> ATestCaseActor::FuncReturnSet_private(TSet<FVector>& Set)
{
	Set.Reset();
	Set.Add(FVector(99, 88, 77));
	TSet<FVector> result;
	result.Add(FVector(9, 8, 7));
	return result;
}

TMap<FString, FVector> ATestCaseActor::FuncReturnMap_private()
{
	TMap<FString, FVector> result;
	result.Add("lala1", FVector(100, 200, 300));
	result.Add("lala2", FVector(222, 333, 444));
	return result;
}

void ATestCaseActor::FuncReturn_ParamArr_private(TArray<int32>& Arr)
{
	Arr.Reset();
	Arr.Add(300);
	Arr.Add(400);
	Arr.Add(500);
}

void ATestCaseActor::FuncReturn_ParamMap_private(TMap<FString, int32>& Map)
{
	Map.Reset();
	Map.Add("hehe", 100);
}

int32 ATestCaseActor::TestNativeFunction_private(bool test_bool, int32 test_int, int64 test_int64, uint8 test_byte, float test_float, double test_double, FString test_string, FText test_text, FName test_name, FVector test_struct, UObject* test_obj, TSubclassOf<AActor> test_class, TArray<int32> test_arr, TSet<int32> test_set, TMap<FString, int32> test_map)
{
	return 0;
}

int32 ATestCaseActor::TestNativeFunction_Ref_private(bool& test_bool, int32& test_int, int64& test_int64, uint8& test_byte, float& test_float, double& test_double, FString& test_string, FText& test_text, FName& test_name, FVector& test_struct, UObject*& test_obj, TSubclassOf<AActor>& test_class)
{
	check(test_bool == true);
	test_bool = false;
	check(test_int == 1234);
	test_int = 4321;
	check(test_int64 == 567895678);
	test_int64 = 8765431;
	check(test_byte == 31);
	test_byte = 32;
	check(FMath::IsNearlyEqual(test_float, 81723.87f));
	test_float = 85555.87f;
	check(FMath::IsNearlyEqual(test_double, 981723879412.89));
	test_double = 8981723879412.89;
	check(test_string == "str");
	test_string = "strstr";
	check(test_text.ToString() == "txt");
	test_text = FText::FromString("txttxt");
	check(test_name == FName("name"));
	test_name = FName("namename");
	check(test_struct.X == 3.0f);
	check(test_struct.Y == 2.0f);
	check(test_struct.Z == 1.0f);
	test_struct = FVector(44, 55, 66);
	check(test_obj == this);
	test_obj = nullptr;
	check(test_class == AActor::StaticClass());
	test_class = ATestCaseActor::StaticClass();
	return 712738174;
}

FVector ATestCaseActor::FuncReturnStruct_private()
{
	return FVector(77, 88, 99);
}

