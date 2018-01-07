// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "LuaDelegate.h"
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

	UFUNCTION()
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
	static void GetAllWidgets(UUserWidget* UserWidget, TArray<FName>& Names, TArray<UWidget*>& Widgets, TArray<FName>&TypesOfWidget);	

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

	UFUNCTION(BlueprintCallable, Category = "luautils", meta = (DefaultToSelf = "Ins"))
		static void Ctor(UObject* Ins, const FString& LuaClassName);
	UFUNCTION(BlueprintCallable, Category = "luautils", meta = (DefaultToSelf = "Ins"))
		static void Call(UObject* Ins, const FString& FunctionName);
};
