// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "ScriptCodeGeneratorBase.h"

/**
 * Lua glue generator. 
 */
class FLuaScriptCodeGenerator : public FScriptCodeGeneratorBase
{
protected:
	FString IncludeBase;
	FString ExportingClassSourcefile;
	FString LuaGeneratedCodeDir;
	FString SublimeCompletionDir;
	TArray<FString> DelegateSourceFiles;
	struct FPropertyAccessor
	{
		FString FunctionName;
		FString AccessorName;
	};
	struct FDelegateExported
	{
		FString ClassNameCPP;
		FString DelegateName;
		UFunction* SignatureFunction;
		bool bIsGameDelegate;
	};
	/** All generated script header filenames */
	TArray<FString> AllScriptHeaders;
	/** Source header filenames for all exported classes */
	TArray<FString> AllSourceClassHeaders;
	TArray<FString> GameSourceClassHeaders;
	TArray<FString> GameScriptHeaders;
	TMap<UClass*, FString> Class2ScriptName;
	/** All exported classes */
	TArray<UClass*> LuaExportedClasses;
	TMap<FString, TArray<UClass*>> ModulesClass;
	TMap<FString, TArray<UScriptStruct*>> ModuleStruct;
	TArray<FString> ChangeableModules;
	TArray<UClass*> GameExportedClasses;
	TArray<FString> LuaExportedTMPClasses;
	TArray<FString> StructNamesInGame;
	TArray<FString> StructNamesInEngine;
	TArray<FString> EnumtNames;
	/** Functions exported for a class */
	TMap<UClass*, TArray<FName> > ClassExportedFunctions;
	TMap<FName, FString> FuncSuperClassName;
	/** Proprties exported for a class */
	TMap<UClass*, TArray<FPropertyAccessor> > ClassExportedProperties;
	TArray<FString> SupportedStruct;
	TArray<FString> NoSupportedStruct;
	TArray<FString> PrivatePropertyStruct;
	TArray<FString> NoCopyStruct;
	TArray<FString> NoNewStruct;
	TArray<FString> NotSupportedClassFunction;
	TArray<FString> NotSupportedClassProperty;
	TArray<FString> NotSupportedClass;
	TArray<FString> CircularModules;
	bool IsExportingCircularModule;
	TSet<FString> WeakPtrClass;
	TMap<UClass*, FString> Class2Sourcefile;
	TMap<UClass*, TArray<UClass*>> ExtraIncludeClass;
// 	TMap<UClass*, FString> ClassIncludeStrToFinish;
	TArray<FString> ExtraIncludeHeader;

	TArray<FDelegateExported> delegates;
	inline bool bExportSbCompletion() { return !SublimeCompletionDir.IsEmpty(); };
	TSet<FString> SublimeCompletions;
	void AddFunctionToCompletions(UFunction* Function);
	void AddPropertyToCompletions(UProperty* Property);
	bool IsGameClass(UClass * Class);
	bool IsChangebleClass(UStruct * Class);
	bool bExportDelegateProxy;
	/** Creats a 'glue' file that merges all generated script files */
	void GenerateSBCompletion();
	void GlueAllGeneratedFiles();
	void GlueAllGeneratedFiles_CircularModule();
	/** Exports a wrapper function */
	FString ExportFunction(const FString& ClassNameCPP, UClass* Class, UFunction* Function);
	/** Exports a wrapper functions for properties */
	FString ExportProperty(const FString& ClassNameCPP, UClass* Class, UProperty* Property, int32 PropertyIndex);
	/** Exports additional class glue code (like ctor/dtot) */
	FString ExportAdditionalClassGlue(const FString& ClassNameCPP, UClass* Class);
	/** Generates wrapper function declaration */
	FString GenerateWrapperFunctionDeclaration(const FString& ClassNameCPP, FString classname, UFunction* Function);
	/** Generates wrapper function declaration */
	FString GenerateWrapperFunctionDeclaration(const FString& ClassNameCPP, FString classname, const FString& FunctionName);
	/** Generates code responsible for getting the object pointer from script context */
	FString GenerateObjectDeclarationFromContext(const FString& ClassNameCPP);
	/** Handles the wrapped function's return value */
	FString GenerateReturnValueHandler(const FString& ClassNameCPP, UFunction* Function, UProperty* ReturnValue);
	/** Check if a property type is supported */
	bool IsPropertyTypeSupported(UProperty* Property);
	FString GetPropertyType(UProperty* Property) const;
	FString GetPropertyGetFunc(UProperty* Property) const;
	FString GetPropertySetFunc(UProperty* Property) const;
	FString GetPropertyCastType(UProperty* Property);
	FString GetPropertySetCastType(UProperty* Property);
	bool isNotCopyStruct(UScriptStruct* thestruct);
	bool CanCopyProperty(UProperty* Property);
	bool isStructSupported(UScriptStruct* thestruct) const;
	FString Push(const FString& ClassNameCPP, UFunction* Function, UProperty* ReturnValue, FString name, UClass* Class, bool bConsiderArrayDim = true);
	FString GetterCode(FString  ClassNameCPP, FString classname, FString FuncName, UProperty* Property, UClass* Class = nullptr, UClass* PropertySuper = nullptr);
	FString SetterBody(UProperty* Property);
	FString SetterCode(FString  ClassNameCPP, FString classname, FString FuncName, UProperty* Property, UClass* Class = nullptr, UClass* PropertySuper = nullptr);
	FString FuncCode(FString  ClassNameCPP, FString classname, UFunction* Function, UClass* FuncSuper = nullptr, UClass* Class = nullptr, UClass* InterfaceClass = nullptr);
	void GenerateWeakClass();
	void GenerateDelegateClass();
	
	FString GenerateFunctionDispatch(UFunction* Function, const FString &ClassNameCPP, bool bIsStaticFunc = false, UClass* InterfaceClass = nullptr);
	FString GenerateFunctionDispatch_private(UFunction* Function, const FString &ClassNameCPP, UClass* Class, bool bIsStaticFunc = false, UClass* InterfaceClass = nullptr);
	FString InitializeParam(UProperty* Param, int32 ParamIndex, bool isnotpublicproperty = false, FString arrayName = "");
	void ExportStruct();
	void ExportEnum();
	// FScriptCodeGeneratorBase interface
	bool HasExportedClass(UClass* Class);
	virtual bool CanExportClass(UClass* Class) override;
	virtual bool CanExportFunction(const FString& ClassNameCPP, UClass* Class, UFunction* Function) override;
	virtual bool CanExportFunctionForInterface(const FString& ClassNameCPP, UClass* Class, UFunction* Function);
	virtual bool CanExportProperty(const FString& ClassNameCPP, UClass* Class, UProperty* Property) override;
	FString ExportInterfaceFunc(UClass* Class);
	virtual FString InitializeFunctionDispatchParam(UFunction* Function, UProperty* Param, int32 ParamIndex) override;

public:
	FLuaScriptCodeGenerator(const FString& RootLocalPath, const FString& RootBuildPath, const FString& OutputDirectory, const FString& InIncludeBase, const FString& LuaConfigPath);
	TArray<FString> SupportModules;
	// FScriptCodeGeneratorBase interface
	virtual void ExportClass(UClass* Class, const FString& SourceHeaderFilename, const FString& GeneratedHeaderFilename, bool bHasChanged) override;
	virtual void FinishExport() override;
};
