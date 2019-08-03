// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "UnrealLua.h"
#include "Luautils.generated.h"
  
USTRUCT()
struct LUAPLUGINRUNTIME_API FReplifetimeCond {
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FString PropertyName;

	UPROPERTY()
	TEnumAsByte<ELifetimeCondition> Cond;
};

UCLASS()
class LUAPLUGINRUNTIME_API ULuautils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	static UWorld* GetWorld1(AActor *obj);

	UFUNCTION()
		static UWorld* GetActorWorld(AActor *obj);

	UFUNCTION()
	static void SetupAttachment(USceneComponent* Child, USceneComponent* InParent, FName InSocketName = NAME_None);

	UFUNCTION(BlueprintCallable)
	static void GetPlayerViewPoint(APlayerController* controler, FVector& out_Location, FRotator& out_Rotation);

	UFUNCTION()
	static FRotator GetViewRotation(APawn* pawn);

	UFUNCTION()
	static FVector VRandCone(FRandomStream stream, FVector const& Dir, float HorizontalConeHalfAngleRad, float VerticalConeHalfAngleRad);

	UFUNCTION()
	static FVector2D FVector2D_New(float InX =0, float InY =0);

	UFUNCTION(BlueprintCallable, Category = "luautils")
	static FColor FColor_New(uint8 InR = 0, uint8 InG = 0, uint8 InB = 0, uint8 InA = 255);

	UFUNCTION()
	static void GetAllWidgets(UUserWidget* UserWidget, UPARAM(ref)TArray<FName>& Names, UPARAM(ref)TArray<UWidget*>& Widgets, UPARAM(ref)TArray<FName>&TypesOfWidget);

	UFUNCTION()
	static void log(FString content);

	UFUNCTION()
	static FString GetName(UObject* p);

	UFUNCTION()
		static void UpdateNav(UActorComponent *Component);

	UFUNCTION()
		static void AddGCRef(UObject*p);

	UFUNCTION()
		static UObject* GetPrivateClass(UObject *p);

	UFUNCTION()
		static void CompareSpeed1(UObject*p);

	
	static void CompareSpeed11() {};

	UFUNCTION()
		static void CompareSpeed2(int i) {}

	UFUNCTION()
		static void CompareSpeed3(int i, UObject* p) {}

	static void call() {};

	static void call1(int i) {};

	static void call2(FVector& i) {};

	static void call3(int i, float j, FString k) {};

	static FVector call4() { static FVector i; return i; }

	UFUNCTION()
		static void ucall(UObject*p) { delete p; };

	UFUNCTION()
		static void ucall1(int i) {};

	UFUNCTION()
		static void ucall2(FVector& i) {};

	UFUNCTION()
		static void ucall3(int i, float j, FString k) {};

	UFUNCTION()
		static FVector ucall4() { static FVector i; return i; }

	UFUNCTION()
		static FString TestAlloc();
};
   
USTRUCT()
struct LUAPLUGINRUNTIME_API Flalala
{
	GENERATED_BODY()

		UPROPERTY()
		int32 i;
};

struct CmpSpeed
{
	static int32 TestTable(lua_State*inL)
	{
		lua_pushvalue(inL, lua_upvalueindex(1));
		lua_pushvalue(inL, 2);
		lua_rawget(inL, -2);
		return 1;
	}

	static int32 TestTMap(lua_State*inL)
	{
		void* p = (void*)lua_touserdata(inL, lua_upvalueindex(1));
		TMap<FName, void*>* Class = (TMap<FName, void*>*)(p);
		FName Key = lua_tostring(inL, 2);
		lua_pushlightuserdata(inL, *Class->Find(Key));
		return 1;
	}

	static int32 GetTwoFunc(lua_State*inL)
	{
		lua_newtable(inL);
		void*p = reinterpret_cast<void*>(123);
		lua_pushlightuserdata(inL, p);
		lua_setfield(inL, -2, "longlongnamelala");
		lua_pushcclosure(inL, TestTable, 1);

		static TMap<FName, void*> Map;
		Map.Add(FName("longlongnamelala"), p);
		lua_pushlightuserdata(inL, &Map);
		lua_pushcclosure(inL, TestTMap, 1);

		return 2;
	}
};

LUA_GLUE_BEGIN(CmpSpeed)
LUA_GLUE_FUNCTION(GetTwoFunc)
LUA_GLUE_END()