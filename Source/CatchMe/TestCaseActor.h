// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ForTestInterface.h"
#include "TextProperty.h"
#include "TestCaseActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_NineParams(FDelegateTest1,  bool,    test_bool,   int32,    test_int, int64, test_int64, uint8, test_byte, float, test_float, double, test_double, FString, test_string, FText, test_text, FName, test_name);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FDelegateTest2, FVector, test_struct, UObject*, test_obj, TSubclassOf<AActor>, test_class, TArray<int32>, test_arr, TArray<FVector>, test_arrstruct, TSet<int32>, test_set);

UENUM(BlueprintType)
enum class TestEnum : uint8
{
	one,
	two,
};


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
};

UCLASS()
class CATCHME_API ATestCaseActor : public AActor, public IForTestInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATestCaseActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void TestCallLua();
public:	
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
		int32 TestNativeFunction_Ref(bool& test_bool, int32& test_int, int64& test_int64, uint8& test_byte, float& test_float,
			double& test_double, FString& test_string, FText& test_text, FName& test_name,
			FVector& test_struct, UObject*& test_obj, TSubclassOf<AActor>& test_class);

	UFUNCTION()
		TArray<int32> FuncReturnArr() { return TArray<int32>(); };

	UPROPERTY()
		TArray<int32> Test_ParamArr;

	UFUNCTION()
		void FuncReturn_ParamArr(TArray<int32>& Arr);

	UFUNCTION()
		void FuncReturn_ParamMap(TMap<FString,int32>& Map);

	UFUNCTION(BlueprintCallable, Category = "TestCase")
		void FuncReturn_ParamMap_struct(TMap<FString, FVector>& Map);

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
		TSet<FVector> FuncReturnSet_public(TSet<FVector>& Set);
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
private:
	UFUNCTION()
		void Test_Param_Interface_private(TScriptInterface<IForTestInterface> I);

	UPROPERTY()
		TScriptInterface<IForTestInterface> test_interface_private;
	UPROPERTY()
		TMap<FString, TestEnum> test_map_enum_private;
	UFUNCTION()
		void FuncReturn_ParamMap_struct_private(TMap<FString, FVector>& Map);
	UFUNCTION()
		TSet<FVector> FuncReturnSet_private(TSet<FVector>& Set);
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
		void FuncReturn_ParamArr_private(TArray<int32>& Arr);
	UFUNCTION()
		void FuncReturn_ParamMap_private(TMap<FString, int32>& Map);
	UFUNCTION()
		TArray<int32> FuncReturnArr_private() { return TArray<int32>(); };
	UFUNCTION()
		int32 TestNativeFunction_private(bool test_bool, int32 test_int, int64 test_int64, uint8 test_byte, float test_float,
			double test_double, FString test_string, FText test_text, FName test_name,
			FVector test_struct, UObject* test_obj, TSubclassOf<AActor> test_class,
			TArray<int32> test_arr, TSet<int32> test_set, TMap<FString, int32> test_map
		);
	UFUNCTION()
		int32 TestNativeFunction_Ref_private(bool& test_bool, int32& test_int, int64& test_int64, uint8& test_byte, float& test_float,
			double& test_double, FString& test_string, FText& test_text, FName& test_name,
			FVector& test_struct, UObject*& test_obj, TSubclassOf<AActor>& test_class);
	UFUNCTION()
		FVector FuncReturnStruct_private();

	UPROPERTY()
	bool test_bool_private;

	UPROPERTY()
	int32 test_int_private;

	UPROPERTY()
	int64 test_int64_private;

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
};
