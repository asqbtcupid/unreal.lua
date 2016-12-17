// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "KismetCompiler.h"

/**
* Script Blueprint Compiler
*/
class FScriptBlueprintCompiler : public FKismetCompilerContext
{
protected:

	typedef FKismetCompilerContext Super;

public:
	FScriptBlueprintCompiler(UScriptBlueprint* SourceSketch, FCompilerResultsLog& InMessageLog, const FKismetCompilerOptions& InCompilerOptions, TArray<UObject*>* InObjLoaded);
	virtual ~FScriptBlueprintCompiler();

	// FKismetCompilerContext
	virtual void Compile() override;
	// End FKismetCompilerContext

protected:
	UScriptBlueprint* ScriptBlueprint() const { return Cast<UScriptBlueprint>(Blueprint); }

	// FKismetCompilerContext
	virtual void SpawnNewClass(const FString& NewClassName) override;
	virtual void CleanAndSanitizeClass(UBlueprintGeneratedClass* ClassToClean, UObject*& OldCDO) override;
	virtual void EnsureProperGeneratedClass(UClass*& TargetClass) override;
	virtual void CreateClassVariablesFromBlueprint() override;
	virtual void CreateFunctionList() override;
	virtual void FinishCompilingClass(UClass* Class) override;
	virtual bool ValidateGeneratedClass(UBlueprintGeneratedClass* Class) override;
	// End FKismetCompilerContext

protected:

	/**
	 * Creates a script context property for this class (if needed)
	 */
	void CreateScriptContextProperty();

	/**
	 * Creates a Blueprint Graph function definition for script defined function
	 *
	 * @param Field Function exported by script
	 */
	void CreateScriptDefinedFunction(FScriptField& Field);

	/** New script class */
	UScriptBlueprintGeneratedClass* NewScriptBlueprintClass;
	/** Script context */
	TAutoPtr<FScriptContextBase> ScriptContext;
	/** Script-defined properties and functions */
	TArray<FScriptField> ScriptDefinedFields;
	/** Script context property generated for the compiled class */
	UObjectProperty* ContextProperty;
};

