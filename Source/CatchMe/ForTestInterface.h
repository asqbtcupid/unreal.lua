// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interface.h"
#include "ForTestInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UForTestInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CATCHME_API IForTestInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Test")
	bool Interface_BpImplement(int32 v);

	UFUNCTION(BlueprintNativeEvent, Category = "Test")
	int32 Interface_NativeEvent(int32 v);
	
};
