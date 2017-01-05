// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Uluautils.generated.h"

/**
 * 
 */
UCLASS()
class DEMO1_API UUluautils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	static UWorld* GetWorld(AActor *obj);
	
	
};
