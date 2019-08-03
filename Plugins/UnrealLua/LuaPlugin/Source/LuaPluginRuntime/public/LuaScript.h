// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LuaScript.generated.h"

UCLASS()
class LUAPLUGINRUNTIME_API ULuaScript : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = LuaScript)
	FString Code;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = LuaScript)
	FString ImportPath;
};
