// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ForTestInterface.h"
#include "TextProperty.h"
#include "LuaDelegateMulti.h"
#include "UnrealLua.h"
#include "BaseEngineLuaGlue.h"
#include "WidgetBlueprintLibrary.h"
#include "TestCaseActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_NineParams(FDelegateTest1,  bool,    test_bool,   int32,    test_int, int64, test_int64, uint8, test_byte, float, test_float, double, test_double, FString, test_string, FText, test_text, FName, test_name);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FDelegateTest2, FVector, test_struct, UObject*, test_obj, TSubclassOf<AActor>, test_class, TArray<int32>, test_arr, TArray<FVector>, test_arrstruct, TSet<int32>, test_set);

DECLARE_DYNAMIC_DELEGATE_TwoParams(FDelegateTest3, int, p1, FVector, p2);
DECLARE_DELEGATE_TwoParams(FDelegateTest4, int, FVector)
DECLARE_DELEGATE_RetVal_TwoParams(double, FDelegateTest5, int, FVector)

DECLARE_DELEGATE(FDelegateRename1)
DECLARE_DELEGATE(FDelegateRename2)

UENUM(BlueprintType)
enum class TestEnum : uint8
{
	one,
	two,
};


USTRUCT()
struct FSomeStruct4
{
	GENERATED_BODY()

		UPROPERTY()
		int i;
};

static bool operator==(const FSomeStruct4& A, const FSomeStruct4& B)
{
	return A.i == B.i;
}

static uint32 GetTypeHash(const FSomeStruct4& A)
{
	return A.i;
}

USTRUCT()
struct FTestFirstMemStruct_Public
{
	GENERATED_BODY()

	UPROPERTY()
	FVector value;
};

USTRUCT()
struct FTestFirstMemStruct_Private
{
	GENERATED_BODY()
private:
	UPROPERTY()
	FVector value;
};

USTRUCT()
struct FTestFirstMemStruct_Nest
{
	GENERATED_BODY()

	UPROPERTY()
	FTestFirstMemStruct_Public value;

	UPROPERTY()
	int32 TestPassRefNum;
};

struct lua_State;
struct SomeBasic
{
	int32 BasicField1;
	void BasicFunc1() {}
	static int32 RawFunc(lua_State*inL) { return 0; };
	virtual int32 VirtualFunc() { return 22; }
	virtual ~SomeBasic(){}
};

struct SomeStruct :public SomeBasic
{
	SomeStruct() :
		Field1(8197238),
		Field2(885.4),
		Field3(FVector(99, 88, 77)),
		Field4(nullptr),
		Field5(12838148213898),
		Field6(22838148213898),
		Field7("aksdjiasdo"),
		Field8(FText::FromString(Field7))
	{
		Field10.Add(2);
		Field10.Add(4);
		Field11.Add(FVector(1, 2, 3));
		Field11.Add(FVector(2, 4, 6));
		Field12.Add("uu");
		Field12.Add("bb");
		Field13.Add("keykey1", 2389);
		Field13.Add("keykey2", 9887);
		Field14.Add("k", Field10);
	}
	int32 Field1;
	float Field2;
	FVector Field3;
	UObject* Field4;
	int64 Field5;
	uint64 Field6;
	FString Field7;
	FText Field8;
	FName Field9;
	TArray<int> Field10;
	TArray<FVector> Field11;
	TSet<FString> Field12;
	TMap<FString, int32> Field13;
	TMap<FString, TArray<int>> Field14;
	uint32 Field15;
	uint32 Field16 : 1;

	static int32 StaticFunc(int32& i) { int j = i; i = 1000;return j; }
	static void StaticFunc1(double i) { i = 9999999; }
	void InsFunc(int32& h, TArray<FVector>& arr, TSet<FString>& set, TMap<int32, int32>& m)
	{	
		ensureAlwaysMsgf(h == 4772, TEXT(""));
		h = 100;
		ensureAlwaysMsgf(arr.Num() == 1, TEXT(""));
		ensureAlwaysMsgf(arr[0] == FVector(823,743,8839), TEXT(""));
		arr.Add(FVector(1, 2, 3));
		ensureAlwaysMsgf(set.Contains("quwoie") && set.Contains("qilk"), TEXT(""));
		set.Add("bbbb");
		ensureAlwaysMsgf(m.Find(81238) && m.Find(99887), TEXT(""));
		m.Remove(81238);
	}
	int32 GetField1() { return Field1; }
	static TArray<int> TestReturnArrForGlueStruct(TArray<int>& Ref) 
	{ 
		Ref = { 1,2,3,4 };
		return{ 5,6,7,8 }; 
	}
	int32 VirtualFunc() { return 33; }

	static TMap<FString, int> FuncForTestGlueTMap(TMap<FString, int> Value, TMap<FString, int>& Ref)
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

	static TSet<FString> FuncForTestGlueTSet(TSet<FString> Value, TSet<FString>& Ref)
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
	FVector& TestReturnRef() { return Field3; };

	void TestPassRef(FVector& P1, SomeBasic& P2, FTestFirstMemStruct_Nest& P3)
	{
		P1 = FVector(22, 33, 44);
		P2.BasicField1 = 2000;
		P3.TestPassRefNum = 3999;
	}

	void TestPassRef1(FVector P1, SomeBasic P3, FTestFirstMemStruct_Nest BpStructVar)
	{
		P1 = FVector(22, 33, 44);
		P3.BasicField1 = 2000;
	}

	void TestPassRef2(const FVector& P1, const SomeBasic& P2, const FTestFirstMemStruct_Nest& P3)
	{
	}

	void TestPassRef3(FVector* P1, SomeBasic* P2, FTestFirstMemStruct_Nest* P3)
	{
	}

	void TestPassRef4(FText& text)
	{
		check(text.ToString() == "TestText");
	}

	void TestArrRef1(TArray<int> arr)
	{
		ensureAlwaysMsgf(arr.Num() == 3, TEXT(""));
		ensureAlwaysMsgf(arr[0] == 3, TEXT(""));
		ensureAlwaysMsgf(arr[1] == 2, TEXT(""));
		ensureAlwaysMsgf(arr[2] == 1, TEXT(""));
	}

	void TestArrRef2(TArray<int>& arr, TArray<int>& arr1)
	{
		ensureAlwaysMsgf(arr.Num() == 3, TEXT(""));
		ensureAlwaysMsgf(arr[0] == 3, TEXT(""));
		ensureAlwaysMsgf(arr[1] == 2, TEXT(""));
		ensureAlwaysMsgf(arr[2] == 1, TEXT(""));

		arr1.Reset();
		arr1.Add(9);
		arr1.Add(8);
		arr1.Add(7);
	}
	
	void TestArrRef3(const TArray<int>& arr)
	{
		ensureAlwaysMsgf(arr.Num() == 3, TEXT(""));
		ensureAlwaysMsgf(arr[0] == 3, TEXT(""));
		ensureAlwaysMsgf(arr[1] == 2, TEXT(""));
		ensureAlwaysMsgf(arr[2] == 1, TEXT(""));
	}

	void TestArrRef4(TArray<int>* p_arr)
	{
		TArray<int>& arr = *p_arr;
		ensureAlwaysMsgf(arr.Num() == 3, TEXT(""));
		ensureAlwaysMsgf(arr[0] == 3, TEXT(""));
		ensureAlwaysMsgf(arr[1] == 2, TEXT(""));
		ensureAlwaysMsgf(arr[2] == 1, TEXT(""));
	}

};

UCLASS()
class CATCHME_API ATestCaseActor : public AActor, public IForTestInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATestCaseActor();
	struct classinerstruct
	{
		int32 lala;
	};


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void TestCallLua();
public:	
	int t(int) { return 1; }
	int t(const float) { return 2; }
	int t(const FVector) { return 3; }
	int t(UObject*) { return 4; }
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
// 	virtual void BeginDestroy();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UClass* testload;
	UFUNCTION()
		void TestLoad1();
	UFUNCTION()
		void TestLoad2();

	UFUNCTION()
		void RunCppCallLuaTest();
	
	UPROPERTY()
	bool test_bool_public;

	UPROPERTY()
	int32 test_int_public;
	
	UPROPERTY()
	int64 test_int64_public;

	UPROPERTY()
	uint64 test_uint64_public;

	UPROPERTY()
	uint8 test_byte_public;

	UPROPERTY()
	float test_float_public;

	UPROPERTY()
	double test_double_public;

	UPROPERTY()
	FString test_string_public;

	UPROPERTY()
	FText test_text_public;

	UPROPERTY()
	FName test_name_public;
	
	UPROPERTY()
	FVector test_vector_public;

	UPROPERTY()
	UObject* test_object_public;

	UPROPERTY()
	TSubclassOf<AActor> test_class_public;

	UPROPERTY()
	TWeakObjectPtr<AActor> test_weakptr_public;

	UPROPERTY()
	AActor* test_actor_public;

	UPROPERTY()
		TArray<int32> test_arr_public;

	UPROPERTY()
		TArray<FVector> test_arr_struct_public;

	UPROPERTY()
		TSet<int32> test_set_public;

	UPROPERTY()
		TSet<FVector> test_set_struct_public;

	UPROPERTY()
		TMap<FString, int32> test_map_public;

	UPROPERTY()
		TMap<FString, FVector> test_map_struct_public;

	UFUNCTION()
		int32 TestNativeFunction(bool test_bool, int32 test_int, int64 test_int64, uint8 test_byte, float test_float, 
			double test_double, FString test_string, FText test_text, FName test_name,
			FVector test_struct, UObject* test_obj, TSubclassOf<AActor> test_class,
			TArray<int32> test_arr, TSet<int32> test_set,  TMap<FString, int32> test_map
		);

	UFUNCTION()
		int32 TestNativeFunction_Ref(UPARAM(ref) bool& test_bool,UPARAM(ref) int32& test_int,UPARAM(ref) int64& test_int64,UPARAM(ref) uint8& test_byte,UPARAM(ref) float& test_float,UPARAM(ref)
			double& test_double,UPARAM(ref) FString& test_string,UPARAM(ref) FText& test_text,UPARAM(ref) FName& test_name,UPARAM(ref)
			FVector& test_struct,UPARAM(ref) UObject*& test_obj,UPARAM(ref) TSubclassOf<AActor>& test_class);

	UFUNCTION(BlueprintCallable)
		void TestRefStruct(FHitResult& out, UPARAM(ref)FHitResult& out1, const FHitResult& out2, FHitResult out3);

	UFUNCTION()
		TArray<int32> FuncReturnArr() { return TArray<int32>(); };

	UPROPERTY()
		TArray<int32> Test_ParamArr;

	UFUNCTION()
		void FuncReturn_ParamArr(UPARAM(ref)TArray<int32>& Arr);

	UFUNCTION()
		void FuncReturn_ParamMap(UPARAM(ref)TMap<FString,int32>& Map);

	UFUNCTION(BlueprintCallable, Category = "TestCase")
		void FuncReturn_ParamMap_struct(UPARAM(ref)TMap<FString, FVector>& Map);

	UFUNCTION()
		FVector FuncReturnStruct();
	UPROPERTY()
		FDelegateTest1 Delegate1;
	UPROPERTY()
		FDelegateTest2 Delegate2;

	UFUNCTION()
		TMap<FString, FVector> FuncReturnMap();
	UPROPERTY()
		TMap<FString, FVector> test_map_copy_public;

	UPROPERTY()
		TSet<FVector> test_set_copy_public;
	UFUNCTION()
		TSet<FVector> FuncReturnSet_public(UPARAM(ref)TSet<FVector>& Set);
	UFUNCTION()
		void TestSpeed_Native(int32 i);

	UPROPERTY()
		TMap<FString, TestEnum> test_map_enum_public;
	UFUNCTION(BlueprintCallable, Category = "TestCase")
		void Func_Test_Enum(TestEnum enum_Param);

	virtual int32 Interface_NativeEvent_Implementation(int32 v) override;

	UPROPERTY()
		TScriptInterface<IForTestInterface> test_interface_public;

	UFUNCTION()
		void Test_Param_Interface_public(TScriptInterface<IForTestInterface> I);

	UFUNCTION()
		void Test_FirstMem_Struct();

	UFUNCTION()
		void Test_PushStack_Struct();

	UFUNCTION()
		static bool TestStaticFunction_public(int something);

	UFUNCTION()
		void TestContainerReturnFromLua();

	UFUNCTION()
		TArray<int32> TestFuncReturnArr();

	UFUNCTION()
		void TestPushSomeStruct();

	UPROPERTY()
		TMap<FString, int> UsedForTestGlueTMap;

	UPROPERTY()
		TSet<FString> UsedForTestGlueTSet;

	UFUNCTION()
		TMap<FString, int> FuncForTestGlueTMap(TMap<FString, int> Value, TMap<FString, int>& Ref);

	UFUNCTION()
		TSet<FString> FuncForTestGlueTSet(TSet<FString> Value, TSet<FString>& Ref);

	UFUNCTION()
		void FuncForTestPushVoidPtr();
	UFUNCTION(BlueprintCallable)
		FVector& TestReturnRef() { return test_vector_public; }
	void TestExpand();
	int32 int_test_expand;

	void TestFunctor2(lua_State* inL);

	UPROPERTY()
		FDelegateTest3 SingleDelegateTest1;

		FDelegateTest4 CppDelegate;

	UFUNCTION()
		void TestCppDelegate();

		void TestPassCppDelegate(FDelegateTest4 DelegatePassed);

	UFUNCTION()
		void TestRetDelegate();

	void TestPassRetCppDelegate(FDelegateTest5 DelegatePassed);

	UFUNCTION()
		void TestSingleDelegate(FDelegateTest3 LuaFunction1);

	static int CallDeadFunctor(lua_State* inL);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		int32 TestNativeEvent();

	static FDelegateTest4 StaticDelegateTest;

	FDelegateTest5 HasRetValueDelegate;

	UPROPERTY()
		TArray<int32> UseForTestArrRef;

	static int StaticInt;

	int seestatic();
	static int staticseestatic();
private:
	UFUNCTION()
		void Test_Param_Interface_private(TScriptInterface<IForTestInterface> I);

	UPROPERTY()
		TScriptInterface<IForTestInterface> test_interface_private;
	UPROPERTY()
		TMap<FString, TestEnum> test_map_enum_private;
	UFUNCTION()
		void FuncReturn_ParamMap_struct_private(UPARAM(ref)TMap<FString, FVector>& Map);
	UFUNCTION()
		TSet<FVector> FuncReturnSet_private(UPARAM(ref)TSet<FVector>& Set);
	UPROPERTY()
		TSet<FVector> test_set_copy_private;
	UPROPERTY()
		TMap<FString, FVector> test_map_copy_private;
	UPROPERTY()
		FDelegateTest1 Delegate1_private;
	UPROPERTY()
		FDelegateTest2 Delegate2_private;
	UPROPERTY()
		TArray<int32> Test_ParamArr_private;

	UFUNCTION()
		TMap<FString, FVector> FuncReturnMap_private();

	UFUNCTION()
		void FuncReturn_ParamArr_private(UPARAM(ref)TArray<int32>& Arr);
	UFUNCTION()
		void FuncReturn_ParamMap_private(UPARAM(ref)TMap<FString, int32>& Map);
	UFUNCTION()
		TArray<int32> FuncReturnArr_private() { return TArray<int32>(); };
	UFUNCTION()
		int32 TestNativeFunction_private(bool test_bool, int32 test_int, int64 test_int64, uint8 test_byte, float test_float,
			double test_double, FString test_string, FText test_text, FName test_name,
			FVector test_struct, UObject* test_obj, TSubclassOf<AActor> test_class,
			TArray<int32> test_arr, TSet<int32> test_set, TMap<FString, int32> test_map
		);
	UFUNCTION()
		int32 TestNativeFunction_Ref_private(UPARAM(ref) bool& test_bool,UPARAM(ref) int32& test_int,UPARAM(ref) int64& test_int64,UPARAM(ref) uint8& test_byte,UPARAM(ref) float& test_float,UPARAM(ref)
			double& test_double,UPARAM(ref) FString& test_string,UPARAM(ref) FText& test_text,UPARAM(ref) FName& test_name,UPARAM(ref)
			FVector& test_struct,UPARAM(ref) UObject*& test_obj,UPARAM(ref) TSubclassOf<AActor>& test_class);
	UFUNCTION()
		FVector FuncReturnStruct_private();

	UPROPERTY()
	bool test_bool_private;

	UPROPERTY()
	int32 test_int_private;

	UPROPERTY()
	int64 test_int64_private;

	UPROPERTY()
	uint64 test_uint64_private;

	UPROPERTY()
	uint8 test_byte_private;

	UPROPERTY()
	float test_float_private;

	UPROPERTY()
	double test_double_private;

	UPROPERTY()
	FString test_string_private;

	UPROPERTY()
	FText test_text_private;

	UPROPERTY()
	FName test_name_private;
	
	UPROPERTY()
	FVector test_vector_private;

	UPROPERTY()
	UObject* test_object_private;

	UPROPERTY()
	TSubclassOf<AActor> test_class_private;

	UPROPERTY()
	TWeakObjectPtr<AActor> test_weakptr_private;

	UPROPERTY()
		TArray<int32> test_arr_private;

	UPROPERTY()
		TArray<FVector> test_arr_struct_private;

	UPROPERTY()
		TSet<int32> test_set_private;

	UPROPERTY()
		TSet<FVector> test_set_struct_private;

	UPROPERTY()
		TMap<FString, int32> test_map_private;

	UPROPERTY()
		TMap<FString, FVector> test_map_struct_private;
	UFUNCTION()
	static bool TestStaticFunction_private(int something);

	UFUNCTION(BlueprintCallable)
		int32 TestReturnValue(int32 Param = 100);

	UPROPERTY()
		TArray<FSomeStruct4> test_pass_to_glue;

	UPROPERTY()
		TMap<int32, FSomeStruct4> test_pass_to_glue_map;

	UPROPERTY()
		TSet<FSomeStruct4> test_pass_to_glue_set;
};


LUA_GLUE_BEGIN(SomeBasic)
LUA_GLUE_PROPERTY(BasicField1)
LUA_GLUE_FUNCTION(BasicFunc1)
LUA_GLUE_FUNCTION_VIRTUAL(VirtualFunc)
LUA_GLUE_CUSTOM_FUNCTION(RawFunc)
LUA_GLUE_END()

LUA_GLUE_BEGIN(SomeStruct)
LUA_GLUE_PROPERTY(Field1)
LUA_GLUE_PROPERTY(Field2)
LUA_GLUE_PROPERTY(Field3)
LUA_GLUE_PROPERTY(Field4)
LUA_GLUE_PROPERTY(Field5)
LUA_GLUE_PROPERTY(Field6)
LUA_GLUE_PROPERTY(Field7)
LUA_GLUE_PROPERTY(Field8)
LUA_GLUE_PROPERTY(Field9)
LUA_GLUE_PROPERTY(Field10)
LUA_GLUE_PROPERTY(Field11)
LUA_GLUE_PROPERTY(Field12)
LUA_GLUE_PROPERTY(Field13)
LUA_GLUE_PROPERTY(Field14)
LUA_GLUE_PROPERTY(Field15)
// todo
LUA_GLUE_PROPERTY(Field16)
LUA_GLUE_FUNCTION(GetField1)
LUA_GLUE_FUNCTION(InsFunc)
LUA_GLUE_FUNCTION(StaticFunc)
LUA_GLUE_FUNCTION(StaticFunc1)
LUA_GLUE_FUNCTION(TestReturnArrForGlueStruct)
LUA_GLUE_FUNCTION(VirtualFunc)
LUA_GLUE_FUNCTION(FuncForTestGlueTMap)
LUA_GLUE_FUNCTION(FuncForTestGlueTSet)
LUA_GLUE_FUNCTION(TestReturnRef)
LUA_GLUE_FUNCTION(TestPassRef)
LUA_GLUE_FUNCTION(TestPassRef1)
LUA_GLUE_FUNCTION(TestPassRef2)
LUA_GLUE_FUNCTION(TestPassRef3)
LUA_GLUE_FUNCTION(TestPassRef4)
LUA_GLUE_FUNCTION(TestArrRef1)
LUA_GLUE_FUNCTION(TestArrRef2)
LUA_GLUE_FUNCTION(TestArrRef3)
LUA_GLUE_FUNCTION(TestArrRef4)
LUA_GLUE_END(SomeBasic)

LUA_GLUE_SHARED(SomeBasic, SomeBasic)
LUA_GLUE_SHARED(SomeStruct, SomeStruct)
LUA_GLUE_ARR(SomeStruct, SomeStruct)

LUA_GLUE_DELEGATE(FDelegateTest4, FDelegateTest4)
LUA_GLUE_DELEGATE(FDelegateTest5, FDelegateTest5)

LUA_GLUE_BEGIN(ATestCaseActor)
LUA_GLUE_FUNCTION(TestExpand)
LUA_GLUE_FUNCTION(TestFunctor2)
LUA_GLUE_FUNCTION(TestPassCppDelegate)
LUA_GLUE_FUNCTION(TestPassRetCppDelegate)
LUA_GLUE_CUSTOM_FUNCTION(CallDeadFunctor)
LUA_GLUE_PROPERTY(int_test_expand)
LUA_GLUE_PROPERTY(CppDelegate)
LUA_GLUE_PROPERTY(StaticDelegateTest)
LUA_GLUE_PROPERTY(HasRetValueDelegate)
LUA_GLUE_PROPERTY(StaticInt)
LUA_GLUE_FUNCTION(seestatic)
LUA_GLUE_FUNCTION(staticseestatic)
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
LUA_GLUE_END()

UCLASS()
class CATCHME_API UTestCallSuperVirtual_Base : public UObject
{
	GENERATED_BODY()
public:
		UFUNCTION()
		int TheTestVirtualFunc() { return 100; }
};
// template <class T, class C, int(C::*method_ptr)(A...)>
// static int method_stub(T * object_ptr, A && ... args)
// {
// 	return (static_cast<T *>(object_ptr)->*method_ptr)(::std::forward<A>(args)...);
// }
UCLASS()
class CATCHME_API UTestCallSuperVirtual_Derived : public UTestCallSuperVirtual_Base
{
	GENERATED_BODY()
public:
// 	UFUNCTION()
// 	int TheTestVirtualFunc()  { return 200; }

	UFUNCTION()
	int Test()
	{
// 		typedef int(UTestCallSuperVirtual_Base::*functype)();
// 		auto c = [](const UTestCallSuperVirtual_Derived* p, auto&&...arg) {return p->UTestCallSuperVirtual_Base::TheTestVirtualFunc(arg...);};
//  		return c(this);
// 		UTestCallSuperVirtual_Derived* p = this;
// 		return method_stub<UTestCallSuperVirtual_Derived, UTestCallSuperVirtual_Base, &UTestCallSuperVirtual_Base::TheTestVirtualFunc>(p);
		return 0;
	}
	
	static SomeStruct* testPopStructPointer(SomeStruct* i) { return i; };
};

LUA_GLUE_EXPAND_BEGIN(UTestCallSuperVirtual_Derived)
LUA_GLUE_FUNCTION(testPopStructPointer)
LUA_GLUE_EXPAND_END(UTestCallSuperVirtual_Derived)

struct StructTwoParts
{
	void Test1() {}
	void Test2() {}
	int data1;
	int data2;
};

struct StructTwoParts1
{
	void Test1() {}
	void Test2() {}
	int data1;
	int data2;
};


template <typename Indices, class... Args>
struct pop1;

template <uint32... Indices, class... Args>
struct pop1< TIntegerSequence<uint32, Indices...>, Args... >
{
	template<int N>
	inline static NthType<N, Args...> Data(lua_State* inL)
	{
		return popiml<NthType<N, Args...>>::pop(inL, N+1);
	}

	template<class FuncType>
	inline static void Call(FuncType&& func, lua_State*inL)
	{
// 		func(Data<Indices>(inL)...);
		func(popiml<NthType<Indices, Args...>>::pop(inL, Indices + 1)...);
	}
};
template<class... Args>
struct Pop1 : pop1<TMakeIntegerSequence<uint32, sizeof...(Args)>, Args...>
{};

struct SomeStruct1
{
	int i;
	int j;
	TArray<uint8> arr1;
};
static void TestArrPassByRefAndValue(TArray<SomeStruct1>& Arr)
{
	Arr.Reset();
	Arr.Add({ 3,4 });
	Arr.Add({ 5,6 });
};

LUA_GLUE_BEGIN(SomeStruct1)
LUA_GLUE_PROPERTY(i)
LUA_GLUE_PROPERTY(j)
LUA_GLUE_FUNCTION_OUT(TestArrPassByRefAndValue, TestArrPassByRefAndValue)
LUA_GLUE_END()

static TArray<uint8>& TestReturnRef_arr_uint8(SomeStruct1& p)
{
	return p.arr1;
}

LUA_GLUE_ARR(SomeStruct1, SomeStruct1)

struct SomeStruct2
{
	int i;
};
LUA_GLUE_BEGIN(SomeStruct2)
LUA_GLUE_FUNCTION_OUT(TestArrPassByRefAndValue, TestArrPassByRefAndValue)
LUA_GLUE_FUNCTION_OUT(TestReturnRef_arr_uint8, TestReturnRef_arr_uint8)
LUA_GLUE_PROPERTY(i)
LUA_GLUE_END()


struct SomeStruct3
{
	static TArray<SomeStruct2> NoGlueArr(TArray<SomeStruct2> p){
		p.Reset();
		p.Add({ 2 });
		p.Add({ 4 });
		return p;
	}
	static TArray<int32> GlueArr() {
		return { 1,2,3,5 };
	}
	static TArray<FSomeStruct4> NoGlueArrTestPassFromUProperty(TArray<FSomeStruct4> p) 
	{
		return p;
	}

	static void NoGlueArrTestPassFromUProperty1(TArray<FSomeStruct4>& p)
	{
		p.Reset();
	}
	static void NoGlueArrTestPassFromUProperty2(TMap<int32, FSomeStruct4>& p)
	{
		p.Reset();
	}
	static TSet<FSomeStruct4> NoGlueArrTestPassFromUProperty3(TSet<FSomeStruct4> p)
	{
		p.Reset();
		p.Add({ 7 });
		p.Add({ 8 });
		return p;
	}

	static void NoGlueArrTestPassFromUProperty4(TSet<FSomeStruct4>& p)
	{
		p.Reset();
		p.Add({ 9 });
		p.Add({ 10 });
	}
	static TArray<SomeStruct2> NoGlueArrRefParam(TArray<SomeStruct2>& p) {
		p.Reset();
		p.Add({ 2 });
		p.Add({ 4 });
		return p;
	}
	static TMap<int32, SomeStruct2> NoGlueMap(TMap<int32, SomeStruct2> p )
	{
		return p;
	}
};

LUA_GLUE_BEGIN(SomeStruct3)
LUA_GLUE_FUNCTION(NoGlueArr)
LUA_GLUE_FUNCTION(GlueArr)
LUA_GLUE_FUNCTION(NoGlueArrTestPassFromUProperty)
LUA_GLUE_FUNCTION(NoGlueArrTestPassFromUProperty1)
LUA_GLUE_FUNCTION(NoGlueArrTestPassFromUProperty2)
LUA_GLUE_FUNCTION(NoGlueArrTestPassFromUProperty3)
LUA_GLUE_FUNCTION(NoGlueArrTestPassFromUProperty4)
LUA_GLUE_FUNCTION(NoGlueArrRefParam)
LUA_GLUE_FUNCTION(NoGlueMap)
LUA_GLUE_END()

struct SomeStruct7
{};

struct SomeStruct5
{
	SomeStruct7 InnerStruct;
};

struct SomeStruct6
{
	SomeStruct5 InnerStruct;
};

LUA_GLUE_BEGIN(SomeStruct7)
LUA_GLUE_END()

LUA_GLUE_BEGIN(SomeStruct5)
LUA_GLUE_PROPERTY(InnerStruct)
LUA_GLUE_END()

LUA_GLUE_BEGIN(SomeStruct6)
LUA_GLUE_PROPERTY(InnerStruct)
LUA_GLUE_END()

LUA_GLUE_SHARED_PTR(SomeStruct2, SomeStruct2)
LUA_GLUE_SHARED_REF(SomeStruct2, SomeStruct2)

struct TestNoCtorSharedType
{
private:
	TestNoCtorSharedType() {}
	TestNoCtorSharedType(const TestNoCtorSharedType&) {}
};
LUA_GLUE_BEGIN(TestNoCtorSharedType)
LUA_GLUE_END()
LUA_GLUE_SHARED_PTR(TestNoCtorSharedType, TestNoCtorSharedType)
LUA_GLUE_SHARED_REF(TestNoCtorSharedType, TestNoCtorSharedType)

struct SomeStruct8
{
	FDelegateTest4 Delegate;
	FDelegateTest4 Test_DelegateRet() { return Delegate; }

	void Test_DelegateRef(FDelegateTest4& p) { p.Unbind(); }
	TSharedPtr<SomeStruct2> GetSomeStruct2Shared1(TSharedPtr<SomeStruct2> p)
	{
		ensureAlwaysMsgf(p.Get()->i == 100, TEXT(""));
		return p;
	}
	void GetSomeStruct2Shared2(TSharedPtr<SomeStruct2>& p)
	{
		ensureAlwaysMsgf(p.Get()->i == 100, TEXT(""));
		p.Get()->i = 200;
	}
	TSharedRef<SomeStruct2> GetSomeStruct2Shared3(TSharedRef<SomeStruct2> p)
	{
		ensureAlwaysMsgf(p.Get().i == 100, TEXT(""));
		return p;
	}
	void GetSomeStruct2Shared4(TSharedRef<SomeStruct2>& p)
	{
		ensureAlwaysMsgf(p.Get().i == 100, TEXT(""));
		p.Get().i = 200;
	}
};

LUA_GLUE_BEGIN(SomeStruct8)
LUA_GLUE_PROPERTY(Delegate)
LUA_GLUE_FUNCTION(Test_DelegateRet)
LUA_GLUE_FUNCTION(Test_DelegateRef)
LUA_GLUE_FUNCTION(GetSomeStruct2Shared1)
LUA_GLUE_FUNCTION(GetSomeStruct2Shared2)
LUA_GLUE_FUNCTION(GetSomeStruct2Shared3)
LUA_GLUE_FUNCTION(GetSomeStruct2Shared4)
LUA_GLUE_END()

struct StructTestOperator
{
	StructTestOperator() { i = 10; }
	int operator+(const StructTestOperator& b)
	{
		return i + b.i;
	}
	int operator-(const StructTestOperator& b)
	{
		return i - b.i;
	}
	bool operator==(const StructTestOperator& b)
	{
		return i == b.i;
	}
	int operator()() { return i; }
	int i;
};

LUA_GLUE_BEGIN(StructTestOperator)
LUA_GLUE_PROPERTY(i)
LUA_GLUE_END()

struct TestNoCtorSharedType1
{
	TestNoCtorSharedType1(int) {}
private:
	TestNoCtorSharedType1(const TestNoCtorSharedType&) {}
};

struct TestWriteNoConstructProperty
{
	TestWriteNoConstructProperty():Data(1){}
	TestNoCtorSharedType1 Data;
};

LUA_GLUE_BEGIN(TestNoCtorSharedType1)
LUA_GLUE_END()

LUA_GLUE_BEGIN(TestWriteNoConstructProperty)
LUA_GLUE_PROPERTY(Data)
LUA_GLUE_END()

struct TestDelegateRename
{
	FDelegateRename1 Delegate1;
	FDelegateRename1 Delegate2;
};

LUA_GLUE_ALIAS(FDelegateRename1, FSimpleDelegate)
LUA_GLUE_ALIAS(FDelegateRename2, FSimpleDelegate)

LUA_GLUE_BEGIN(TestDelegateRename)
LUA_GLUE_PROPERTY(Delegate1)
LUA_GLUE_PROPERTY(Delegate2)
LUA_GLUE_END()


template<class T, class T1>
void TestTmpFunc_Test(T i, T1 j) {}

struct TestTmpFunc
{
	template<class T, class T1>
	void Test(T i, T1 j){}
};

LUA_GLUE_BEGIN(TestTmpFunc)
LUA_GLUE_FUNCTION_NAME(Test, Test<int, float>)
LUA_GLUE_FUNCTION_NAME(Test2, Test<TestDelegateRename, float>)
LUA_GLUE_FUNCTION_OUT(Test3, TestTmpFunc_Test<int, float>)
LUA_GLUE_FUNCTION_OUT(Test4, TestTmpFunc_Test<TestDelegateRename, float>)
LUA_GLUE_END()

template<class T>
struct TestTplStruct
{
	T i;
	T f(T j) { return j; }
};

LUA_GLUE_BEGIN_NAME(TestTplStruct_int, TestTplStruct<int>)
LUA_GLUE_PROPERTY(i)
LUA_GLUE_FUNCTION(f)
LUA_GLUE_END()

LUA_GLUE_BEGIN_NAME(TestTplStruct_FVector, TestTplStruct<FVector>)
LUA_GLUE_PROPERTY(i)
LUA_GLUE_FUNCTION(f)
LUA_GLUE_END()

struct TestPushStructOpt
{
	static FVector Test() { return FVector(1,2,3); }
};

LUA_GLUE_BEGIN(TestPushStructOpt)
LUA_GLUE_FUNCTION(Test)
LUA_GLUE_END()


struct base1 {
	int base1_i;
};

struct base2 :public base1
{
	int base2_i;
};

struct base3 :public base2
{
	int base3_i;
};
struct anotherbase
{
	int another_i;
};
struct base4 :public base3, public anotherbase
{};

LUA_GLUE_BEGIN(base1)
LUA_GLUE_PROPERTY(base1_i)
LUA_GLUE_END()

LUA_GLUE_BEGIN(base2)
LUA_GLUE_PROPERTY(base2_i)
LUA_GLUE_END(base1)

LUA_GLUE_BEGIN(base3)
LUA_GLUE_PROPERTY(base3_i)
LUA_GLUE_END(base2)

LUA_GLUE_BEGIN(anotherbase)
LUA_GLUE_PROPERTY(another_i)
LUA_GLUE_END()

LUA_GLUE_BEGIN(base4)
LUA_GLUE_END(base3, anotherbase)

struct TestStaticProperty
{
	static int i;
	int see()
	{
		return i;
	}
	static int staticsee()
	{
		return i;
	}
};
LUA_GLUE_BEGIN(TestStaticProperty)
LUA_GLUE_FUNCTION(see)
LUA_GLUE_FUNCTION(staticsee)
LUA_GLUE_PROPERTY(i)
LUA_GLUE_END()

UCLASS()
class UTestUobjectGlue :public UObject
{
	GENERATED_BODY()
};

LUA_GLUE_BEGIN(UTestUobjectGlue)
LUA_GLUE_END()