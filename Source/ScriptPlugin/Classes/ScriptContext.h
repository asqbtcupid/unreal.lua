// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "Engine/BlueprintGeneratedClass.h"
#include "ScriptContext.generated.h"

/**
* Script context subobject
*/
UCLASS(DefaultToInstanced, EarlyAccessPreview)
class SCRIPTPLUGIN_API UScriptContext : public UObject
{
	GENERATED_UCLASS_BODY()

	virtual void PostInitProperties() override;
	virtual void BeginDestroy() override;

	UFUNCTION(BlueprintCallable, Category="ScriptContext")
	void CallScriptFunction(FString FunctionName);

protected:

	/** Script context (code) */
	FScriptContextBase* Context;
};