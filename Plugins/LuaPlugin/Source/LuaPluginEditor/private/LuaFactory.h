// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "LuaFactory.generated.h"

UCLASS()
class  ULuaFactory : public UFactory
{
	GENERATED_BODY()
	
public:
	ULuaFactory();
	bool FactoryCanImport(const FString& Filename);
	UObject* FactoryCreateText(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, const TCHAR* Type, const TCHAR*& Buffer, const TCHAR* BufferEnd, FFeedbackContext* Warn);
	
};
