// Fill out your copyright notice in the Description page of Project Settings.

#include "TestCaseActor.h"
#include "UnrealLua.h"
#include "MetaInterface.h"
#include "CatchMeGameMode.h"

int TestStaticProperty::i;

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
	test_text_public = NSLOCTEXT("lala", "ohoh", "testlocal");
	test_text_public = FText::FromString("nativetext");
//	test_text_public = TEXT("nativetext");
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

// void ATestCaseActor::BeginDestroy()
// {
// 	LuaCall("BeginDestroy", this);
// 	Super::BeginDestroy();
// }

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

void ATestCaseActor::TestRefStruct(FHitResult& out, FHitResult& out1, const FHitResult& out2, FHitResult out3)
{
	out.Actor = this;
	out1.Actor = this;
// 	out2.Actor = this;
	out3.Actor = this;
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
	checkf(v == 200, TEXT(""));
	return 1999;
}

void ATestCaseActor::Test_Param_Interface_public(TScriptInterface<IForTestInterface> I)
{
	checkf(I.GetObject() == this, TEXT(""));
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
	checkf(I.GetObject() == this, TEXT(""));
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

bool ATestCaseActor::TestStaticFunction_public(int something)
{
	check(something == 2831978);
	return true;
}

void ATestCaseActor::TestContainerReturnFromLua()
{
	TArray<int> arr = LuaCallr(TArray<int>, "LuaTestContainerReturnFromLua", this);
	ensureAlwaysMsgf(arr.Num() == 2, TEXT(""));
	ensureAlwaysMsgf(arr[0] == 1, TEXT(""));
	ensureAlwaysMsgf(arr[1] == 3, TEXT(""));
}

TArray<int32> ATestCaseActor::TestFuncReturnArr()
{
	return{ 634,123,1293,12398 };
}

void ATestCaseActor::TestPushSomeStruct()
{
	SomeStruct var;
	TArray<SomeStruct> arr_var;
	arr_var.Add(SomeStruct());
	arr_var.Add(SomeStruct());
	LuaCall("LuaTestPushSomeStruct", this, var, arr_var);
}

TMap<FString, int> ATestCaseActor::FuncForTestGlueTMap(TMap<FString, int> Value, TMap<FString, int>& Ref)
{
	ensureAlwaysMsgf(Value["key1"] == 11, TEXT(""));
	ensureAlwaysMsgf(Value["key2"] == 100, TEXT(""));
	ensureAlwaysMsgf(Ref["key3"] == 22, TEXT(""));
	ensureAlwaysMsgf(Ref["key4"] == 99, TEXT(""));
	Ref.Reset();
	Ref.Add("ohoh", 88);
	Ref.Add("jiujiu", 66);
	TMap<FString, int> Result;
	Result.Add("lala", 2);
	Result.Add("didi", 99);
	return Result;
}

TSet<FString> ATestCaseActor::FuncForTestGlueTSet(TSet<FString> Value, TSet<FString>& Ref)
{
	ensureAlwaysMsgf(Value.Contains("key1"), TEXT(""));
	ensureAlwaysMsgf(Value.Contains("key2"), TEXT(""));
	ensureAlwaysMsgf(Ref.Contains("key3"), TEXT(""));
	ensureAlwaysMsgf(Ref.Contains("key4"), TEXT(""));
	Ref.Reset();
	Ref.Add("ohoh");
	Ref.Add("jiujiu");
	TSet<FString> Result;
	Result.Add("lala");
	Result.Add("didi");
	return Result;
}

void ATestCaseActor::FuncForTestPushVoidPtr()
{
	int int_i =9887;
	float float_i = 81723.4;
	double double_i = 812739.8;
	bool bool_i = true;
	FString FString_i = "lalala";
	int64 int64_i = 2371984298172;
	FVector vec_i(1,2,3);
	TArray<int> arr_i{ 4,5,6 };
	
	LuaCall("LuaFuncForTestPushVoidPtr", this, (void*)(&int_i), (void*)(&float_i), (void*)(&double_i),
		(void*)(&bool_i), (void*)(&FString_i), (void*)(&int64_i), (void*)(&vec_i), (void*)(&arr_i), (void*)this);
}

void ATestCaseActor::TestExpand()
{
	int_test_expand = 88234;
	LuaCall("LuaTestExpand", this);
}

void ATestCaseActor::TestFunctor2(lua_State* inL)
{
	UnrealLuaFunctor functor(inL, 2);
	UnrealLuaFunctor functor1(inL, 3);
	functor.Call(1, 2, 3, this, FVector(1,2,3));
	check(functor1.Callr<int32>() == 332);
}

void ATestCaseActor::TestCppDelegate()
{
	CppDelegate.ExecuteIfBound(100, FVector(1, 2, 3));
}

void ATestCaseActor::TestPassCppDelegate(FDelegateTest4 DelegatePassed)
{
	DelegatePassed.ExecuteIfBound(100, FVector(2, 3, 4));
}

void ATestCaseActor::TestRetDelegate()
{
	double result = HasRetValueDelegate.Execute(222, FVector(88, 99, 77));
	check(FMath::IsNearlyEqual(result, 10000.0));
}

void ATestCaseActor::TestPassRetCppDelegate(FDelegateTest5 DelegatePassed)
{
	double result = DelegatePassed.Execute(222, FVector(88, 99, 77));
	check(FMath::IsNearlyEqual(result, 10000.0));
}

void ATestCaseActor::TestSingleDelegate(FDelegateTest3 LuaFunction1)
{
	LuaFunction1.ExecuteIfBound(100, FVector(1, 2, 3));
}

int ATestCaseActor::CallDeadFunctor(lua_State* inL)
{
	static UnrealLuaFunctor functor(inL, 1);
	functor.Call();
	return 0;
}

bool ATestCaseActor::TestStaticFunction_private(int something)
{
	check(something == 192831982);
	return true;
}

int32 ATestCaseActor::TestReturnValue(int32 Param)
{
	return Param;
}

void ATestCaseActor::TestReEnterUFunction(bool Reinter, FVector& TestResult)
{
	if (Reinter)
	{
		TestResult = FVector(1, 2, 3);
		LuaCall("ReenterAgain", this);
	}
	else
	{
		TestResult = FVector(2, 3, 4);
	}
}

FDelegateTest4 ATestCaseActor::StaticDelegateTest;

int ATestCaseActor::StaticInt = 0;

int ATestCaseActor::seestatic()
{
	return StaticInt;
}

int ATestCaseActor::staticseestatic()
{
	return StaticInt;
}

int32 ATestCaseActor::TestNativeEvent_Implementation()
{
	return 100;
}

struct TestnewStruct
{
	TestnewStruct() {};
	TestnewStruct(int _Data, float Float = 1000.0f):IntData(_Data),FloatData(Float)
	{};
	TestnewStruct(float _Data):FloatData(_Data) {};
	TestnewStruct(FVector _Data):StructData(_Data) {};
	TestnewStruct(int Data1, float Data2, FVector& _Data)
		:IntData(Data1), FloatData(Data2), StructData(_Data)
	{};

	int IntData;
	float FloatData;
	FVector StructData;
};

LUA_GLUE_BEGIN(TestnewStruct)
LUA_GLUE_NEW(new1, (int, float),1000)
LUA_GLUE_NEW(new2, (float))
LUA_GLUE_NEW(new3, (FVector))
LUA_GLUE_NEW(new4, (int, float, FVector&))
LUA_GLUE_TEMP(Temp1, (int, float), 1000)
LUA_GLUE_TEMP(Temp2, (float))
LUA_GLUE_TEMP(Temp3, (FVector))
LUA_GLUE_TEMP(Temp4, (int, float, FVector&), FVector(4,5,6))
LUA_GLUE_PROPERTY(IntData)
LUA_GLUE_PROPERTY(FloatData)
LUA_GLUE_PROPERTY(StructData)
LUA_GLUE_END()

struct TestDefaultAug
{
	virtual ~TestDefaultAug(){}
	void test(int p1, double p2 = 3.0, ATestCaseActor* p = nullptr)
	{
		ensureAlwaysMsgf(FMath::IsNearlyEqual(p2, 3.0), TEXT(""));
		ensureAlwaysMsgf(p == nullptr, TEXT(""));
	}
	void test1(FVector v = FVector(1,2,3))
	{
		ensureAlwaysMsgf(v == FVector(1,2,3), TEXT(""));
	}

	void test1(FRotator v = FRotator(10.0f, 20.0f, 30.0f))
	{
		ensureAlwaysMsgf(v == FRotator(10.0f, 20.0f, 30.0f), TEXT(""));
	}

	void test3(TArray<int> lala)
	{}

	void test4(FDelegateTest4 p = nullptr)
	{}


	virtual void test_virtual(int p)
	{
		ensureAlwaysMsgf(p == 2, TEXT(""));
	}
	static void TestStaticClassFunc(int a = 1, float b=2.0f, int c=3)
	{
		ensureAlwaysMsgf(a == 1, TEXT(""));
		ensureAlwaysMsgf(FMath::IsNearlyEqual(b, 2.0f), TEXT(""));
		ensureAlwaysMsgf(c == 3, TEXT(""));
	}
	static int TestStaticClassFunc_Ret(int p = 3)
	{
		return p;
	}
};

static void TestStaticDefault(int a = 1, float b = 2.0f, int c = 3)
{
	ensureAlwaysMsgf(a == 1, TEXT(""));
	ensureAlwaysMsgf(FMath::IsNearlyEqual(b, 2.0f), TEXT(""));
	ensureAlwaysMsgf(c == 3, TEXT(""));
}

LUA_GLUE_BEGIN(TestDefaultAug)
LUA_GLUE_FUNCTION(test, 3.0, nullptr)
LUA_GLUE_FUNCTION_NAME(test_name, test, 3.0, nullptr)
LUA_GLUE_FUNCTION_OVERLOAD(test1_1, test1, void (TestDefaultAug::*)(FVector v), FVector(1, 2, 3))
LUA_GLUE_FUNCTION_OVERLOAD(test1_2, test1, void (TestDefaultAug::*)(FRotator v), FRotator(10.0f, 20.0f, 30.0f))
LUA_GLUE_FUNCTION(test3, TArray<int>())
LUA_GLUE_FUNCTION(TestStaticClassFunc, 1, 2.0, 3)
LUA_GLUE_FUNCTION(TestStaticClassFunc_Ret, 3)
LUA_GLUE_FUNCTION_OUT(TestStaticDefault, TestStaticDefault, 1, 2.0, 3)
LUA_GLUE_FUNCTION(test4, FDelegateTest4())
LUA_GLUE_FUNCTION_NAME(test5, test4, nullptr)
LUA_GLUE_FUNCTION_VIRTUAL(test_virtual, 2)
LUA_GLUE_END()


struct TestNoDefaultCtor
{
	TestNoDefaultCtor() = delete;
	~TestNoDefaultCtor() = delete;
};

LUA_GLUE_BEGIN(TestNoDefaultCtor)
LUA_GLUE_END()

struct TestNoCustom
{
	int32 Test1(lua_State*inL)
	{
		ensureAlwaysMsgf(Data == 5, TEXT(""));
		int p;
		UTableUtil::read(p, inL, 2);
		ensureAlwaysMsgf(p == 99, TEXT(""));
		UTableUtil::push(inL, 100);
		UTableUtil::push(inL, 200);
		return 2;
	}

	static int32 Test2(lua_State*inL)
	{
		int p;
		UTableUtil::read(p, inL, 1);
		ensureAlwaysMsgf(p == 99, TEXT(""));
		UTableUtil::push(inL, 100);
		UTableUtil::push(inL, 200);
		return 2;
	}
	int Data;
};

static int32 TestNoCustom_Test3(lua_State*inL)
{
	return TestNoCustom::Test2(inL);
}

LUA_GLUE_BEGIN(TestNoCustom)
LUA_GLUE_FUNCTION(Test1)
LUA_GLUE_FUNCTION(Test2)
LUA_GLUE_FUNCTION_OUT(Test3, TestNoCustom_Test3)
LUA_GLUE_PROPERTY(Data)
LUA_GLUE_END()

LUA_GLUE_BEGIN(StructTwoParts)
LUA_GLUE_FUNCTION(Test2)
LUA_GLUE_PROPERTY(data2)
LUA_GLUE_END()

LUA_GLUE_EXPAND_BEGIN(StructTwoParts1)
LUA_GLUE_FUNCTION(Test2)
LUA_GLUE_PROPERTY(data2)
LUA_GLUE_EXPAND_END(StructTwoParts1)

struct TestBeginNameBug
{
	int i;
};

LUA_GLUE_BEGIN_NAME(TestBeginNameBug1, TestBeginNameBug)
LUA_GLUE_PROPERTY(i)
LUA_GLUE_END()

struct ParamType1
{
};

struct ParamType2
{
	ParamType2(const ParamType1& p)
	{}
};

static void TestCopyConstructForDefault(ParamType2 p2 = ParamType1())
{
}

LUA_GLUE_BEGIN(ParamType2)
LUA_GLUE_FUNCTION_OUT(f, TestCopyConstructForDefault, ParamType1())
LUA_GLUE_END()

template<class T>
struct TestTemplateStruct
{
	T Get() { return Data; }
	T Data;
};
#define TEST_TEM_GLUE(Name, TheType) \
LUA_GLUE_BEGIN_NAME(TestTemplateStruct_##Name, TestTemplateStruct<TheType>)\
LUA_GLUE_PROPERTY(Data)\
LUA_GLUE_FUNCTION(Get)\
LUA_GLUE_END()

TEST_TEM_GLUE(int, int)
TEST_TEM_GLUE(float, float)
TEST_TEM_GLUE(Arr_int, TArray<int>)

// LUA_GLUE_BEGIN_NAME(TArray_int, TArray<int>)
// LUA_GLUE_NEW(CopyCtor, (const TArray<int>&))
// LUA_GLUE_FUNCTION(Num)
// LUA_GLUE_FUNCTION(Max)
// LUA_GLUE_FUNCTION(Pop, true)
// LUA_GLUE_FUNCTION_OVERLOAD(Add, Add, int (TArray<int>::*)(const int& Item))
// LUA_GLUE_FUNCTION(AddUninitialized,1)
// LUA_GLUE_END(TArray_int)



template<class... Args>
int32 LuaPopAndCallAndReturn1(lua_State*inL, void(*func)(Args...))
{
	const int FirstIndexExcludeSelf = 1;
	Pop1<Args...>::Call(func, inL);
	return 0;
}

template<class... Args>
int32 LuaPopAndCallAndReturn2(lua_State*inL, void(*func)(Args...))
{
	const int FirstIndexExcludeSelf = 1;
	TTuple<typename TraitTupleInerType<Args>::Type...> tuple;
	TupleOperationHelper<Args...>::PopAllValueFromStack(inL, tuple, FirstIndexExcludeSelf);
	auto lambda = [func](typename TraitTupleInerType<Args>::Type&... args) { (*func)(args...); };
	TupleOperationHelper<Args...>::CallCppFunc(tuple, lambda);
	return 0;
}

int32 LuaPopAndCallAndReturn3(lua_State*inL, void(*func)(int, int, int, int))
{
	const int FirstIndexExcludeSelf = 1;
	int p1 = popiml<int>::pop(inL, 1);
	int p2 = popiml<int>::pop(inL, 2);
	int p3 = popiml<int>::pop(inL, 3);
	int p4 = popiml<int>::pop(inL, 4);
	(*func)(p1, p2, p3, p4);
	return 0;
}

#define LUA_GLUE_FUNCTION_OUT1(FUNC_NAME,FUNC_DECL, ...) \
	{#FUNC_NAME, [](lua_State*inL)->int32 { return LuaPopAndCallAndReturn1(inL, &FUNC_DECL,##__VA_ARGS__);}},

#define LUA_GLUE_FUNCTION_OUT2(FUNC_NAME,FUNC_DECL, ...) \
	{#FUNC_NAME, [](lua_State*inL)->int32 { return LuaPopAndCallAndReturn2(inL, &FUNC_DECL,##__VA_ARGS__);}},

#define LUA_GLUE_FUNCTION_OUT3(FUNC_NAME,FUNC_DECL, ...) \
	{#FUNC_NAME, [](lua_State*inL)->int32 { return LuaPopAndCallAndReturn3(inL, &FUNC_DECL,##__VA_ARGS__);}},

static void TestTheSpeed(int p1, int p2, int p3, int p4)
{

}

struct TestInitilizerStruct
{
	int i;
	bool IsAdd = true;
};

struct TestSpeedStruct
{
	void Test()
	{
		TArray<TestInitilizerStruct> a = { {1}, {2, false}, {3} };
	}
};

LUA_GLUE_BEGIN(TestSpeedStruct)
LUA_GLUE_FUNCTION_OUT1(Test1, TestTheSpeed)
LUA_GLUE_FUNCTION_OUT2(Test2, TestTheSpeed)
LUA_GLUE_FUNCTION_OUT3(Test3, TestTheSpeed)
LUA_GLUE_FUNCTION_OUT(Test4, TestTheSpeed)
LUA_GLUE_END()

static int O_t(int) { return 1; }
static int O_t(float) { return 2; }
static int O_t(FVector) { return 3; }
static int O_t(UObject*) { return 4; }

struct TestOverloadFunction
{
	int Data;
	TestOverloadFunction(){ Data = 0; }
	TestOverloadFunction(int) { Data = 1; }
	TestOverloadFunction(float) { Data = 2; }
	TestOverloadFunction(FVector) { Data = 3; }
	TestOverloadFunction(UObject*) { Data = 4; }
	int t(int) { return 1; }
	int t(const float){ return 2; }
	int t(const FVector){ return 3; }
	int t(UObject*){ return 4; }
	int t(FVector, int i) { return 5; }
	int t(FVector, float) { return 6; }
	int t(FVector, FVector) { return 7; }
	int t(FVector, UObject*) { return 8; }
	int t(TArray<FVector>) { return 9; }
	int t(TArray<int32>) { return 10; }
	int t(TArray<float>) { return 11; }
	int t(FText) { return 12; }
	int t(TMap<int32, FString>) { return 13; }
	int t(TSet<int32>) { return 14; }
	int t(TSharedPtr<SomeBasic>) { return 15; }
	int t(TSharedRef<SomeBasic>) { return 16; }
	int t(SomeBasic) { return 17; }

	static int32 test() {
		return GetStructNewType<FText>::GetType();
	}

	static bool test1()
	{
		return std::is_convertible < FText*, UObject*>::value;
	}
};

LUA_GLUE_BEGIN(TestOverloadFunction)
LUA_GLUE_PROPERTY(Data)
LUA_GLUE_CTOR((int))
LUA_GLUE_CTOR((float))
LUA_GLUE_CTOR((FVector))
LUA_GLUE_CTOR((UObject*))
LUA_GLUE_OVERLOAD(t, int(TheClassType::*)(int), 0)
LUA_GLUE_OVERLOAD(t, int(TheClassType::*)(const float))
LUA_GLUE_OVERLOAD(t, int(TheClassType::*)(const FVector))
LUA_GLUE_OVERLOAD(t, int(TheClassType::*)(UObject*))
LUA_GLUE_OVERLOAD(t, int(TheClassType::*)(FVector, int))
LUA_GLUE_OVERLOAD(t, int(TheClassType::*)(FVector, float))
LUA_GLUE_OVERLOAD(t, int(TheClassType::*)(FVector, FVector))
LUA_GLUE_OVERLOAD(t, int(TheClassType::*)(FVector, UObject*))

LUA_GLUE_OVERLOAD(t, int(TheClassType::*)(TArray<FVector>))
LUA_GLUE_OVERLOAD(t, int(TheClassType::*)(TArray<int32>))
LUA_GLUE_OVERLOAD(t, int(TheClassType::*)(TArray<float>))
LUA_GLUE_OVERLOAD(t, int(TheClassType::*)(FText))
LUA_GLUE_OVERLOAD(t, int(TheClassType::*)(TMap<int32, FString>))
LUA_GLUE_OVERLOAD(t, int(TheClassType::*)(TSet<int32>))
LUA_GLUE_OVERLOAD(t, int(TheClassType::*)(TSharedPtr<SomeBasic>))
LUA_GLUE_OVERLOAD(t, int(TheClassType::*)(TSharedRef<SomeBasic>))
LUA_GLUE_OVERLOAD(t, int(TheClassType::*)(SomeBasic))
// LUA_GLUE_FUNCTION_OVERLOAD(t, t, int(TheClassType::*)(double))


LUA_GLUE_FUNCTION_OUT_OVERLOAD(O_t, O_t, int(*)(int), 0)
LUA_GLUE_FUNCTION_OUT_OVERLOAD(O_t, O_t, int(*)(float))
LUA_GLUE_FUNCTION_OUT_OVERLOAD(O_t, O_t, int(*)(FVector))
LUA_GLUE_FUNCTION_OUT_OVERLOAD(O_t, O_t, int(*)(UObject*))

LUA_GLUE_FUNCTION(test)
LUA_GLUE_FUNCTION(test1)
LUA_GLUE_END()


	