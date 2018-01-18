// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#include "ScriptCodeGeneratorBase.h"
#include "LuaglueGeneratorPrivatePCH.h"

FScriptCodeGeneratorBase::FScriptCodeGeneratorBase(const FString& InRootLocalPath, const FString& InRootBuildPath, const FString& InOutputDirectory, const FString& InIncludeBase)
{
	GeneratedCodePath = InOutputDirectory;
	RootLocalPath = InRootLocalPath;
	RootBuildPath = InRootBuildPath;
	IncludeBase = InIncludeBase;
}

bool FScriptCodeGeneratorBase::SaveHeaderIfChanged(const FString& HeaderPath, const FString& NewHeaderContents)
{
	FString OriginalHeaderLocal;
	FFileHelper::LoadFileToString(OriginalHeaderLocal, *HeaderPath);

	const bool bHasChanged = OriginalHeaderLocal.Len() == 0 || FCString::Strcmp(*OriginalHeaderLocal, *NewHeaderContents);
	if (bHasChanged)
	{
		// save the updated version to a tmp file so that the user can see what will be changing
		const FString TmpHeaderFilename = HeaderPath + TEXT(".tmp");

		// delete any existing temp file
		IFileManager::Get().Delete(*TmpHeaderFilename, false, true);
		if (!FFileHelper::SaveStringToFile(NewHeaderContents, *TmpHeaderFilename))
		{
			UE_LOG(LogScriptGenerator, Warning, TEXT("Failed to save header export: '%s'"), *TmpHeaderFilename);
		}
		else
		{
			TempHeaders.Add(TmpHeaderFilename);
		}
	}

	return bHasChanged;
}

void FScriptCodeGeneratorBase::RenameTempFiles()
{
	// Rename temp headers
	for (auto& TempFilename : TempHeaders)
	{
		FString Filename = TempFilename.Replace(TEXT(".tmp"), TEXT(""));
		if (!IFileManager::Get().Move(*Filename, *TempFilename, true, true))
		{
			UE_LOG(LogScriptGenerator, Error, TEXT("%s"), *FString::Printf(TEXT("Couldn't write file '%s'"), *Filename));
		}
		else
		{
			UE_LOG(LogScriptGenerator, Log, TEXT("Exported updated script header: %s"), *Filename);
		}
	}
}

FString FScriptCodeGeneratorBase::RebaseToBuildPath(const FString& FileName) const
{
	FString NewFilename(FileName);
	FPaths::MakePathRelativeTo(NewFilename, *IncludeBase);
	return NewFilename;
}

FString FScriptCodeGeneratorBase::GetClassNameCPP(UClass* Class)
{
	if (Class->ClassFlags & CLASS_Interface)
		return FString::Printf(TEXT("I%s"), *Class->GetName());
	else
		return FString::Printf(TEXT("%s%s"), Class->GetPrefixCPP(), *Class->GetName());
}

FString FScriptCodeGeneratorBase::GetPropertyTypeCPP(UProperty* Property, uint32 PortFlags /*= 0*/, bool bIsKeepTEnumAsByte)
{
	static FString EnumDecl(TEXT("enum "));
	static FString StructDecl(TEXT("struct "));
	static FString ClassDecl(TEXT("class "));
	static FString TEnumAsByteDecl(TEXT("TEnumAsByte<enum "));
	static FString TSubclassOfDecl(TEXT("TSubclassOf<class "));

	FString PropertyType = Property->GetCPPType(NULL, PortFlags);
	if (Property->IsA(UArrayProperty::StaticClass()))
	{
		auto PropertyArr = Cast<UArrayProperty>(Property);
		FString inerTypeCpp = GetPropertyTypeCPP(PropertyArr->Inner, CPPF_ArgumentOrReturnValue);
		if (inerTypeCpp == "EObjectTypeQuery")
			inerTypeCpp = "TEnumAsByte<EObjectTypeQuery> ";
		PropertyType = FString::Printf(TEXT("TArray<%s>"), *inerTypeCpp);
	}
	else if (auto p = Cast<UMapProperty>(Property))
	{
		FString keytype = GetPropertyTypeCPP(p->KeyProp, CPPF_ArgumentOrReturnValue);
		FString valuetype = GetPropertyTypeCPP(p->ValueProp, CPPF_ArgumentOrReturnValue);
		PropertyType = FString::Printf(L"TMap<%s, %s>", *keytype, *valuetype);
	}
	else if (auto p = Cast<USetProperty>(Property))
	{
		FString keytype = GetPropertyTypeCPP(p->ElementProp, CPPF_ArgumentOrReturnValue);
		PropertyType = FString::Printf(L"TSet<%s>", *keytype);
	}
	else if (UInterfaceProperty* p = Cast<UInterfaceProperty>(Property))
	{
		FString TypeStr;
		FString TypePrefix = p->GetCPPType(&TypeStr, 0);
		PropertyType = TypePrefix + TypeStr;
	}
	// Strip any forward declaration keywords
	if (PropertyType.StartsWith(EnumDecl) || PropertyType.StartsWith(StructDecl) || PropertyType.StartsWith(ClassDecl))
	{
		int FirstSpaceIndex = PropertyType.Find(TEXT(" "));
		PropertyType = PropertyType.Mid(FirstSpaceIndex + 1);
	}
	else if (!bIsKeepTEnumAsByte && PropertyType.StartsWith(TEnumAsByteDecl))
	{
		int FirstSpaceIndex = PropertyType.Find(TEXT(" "));
		PropertyType = TEXT("TEnumAsByte<") + PropertyType.Mid(FirstSpaceIndex + 1);
	}
	else if (PropertyType.StartsWith(TSubclassOfDecl))
	{
		int FirstSpaceIndex = PropertyType.Find(TEXT(" "));
		PropertyType = TEXT("TSubclassOf<") + PropertyType.Mid(FirstSpaceIndex + 1);
	}
	return PropertyType;
}



FString FScriptCodeGeneratorBase::GenerateFunctionDispatch(UFunction* Function, const FString &ClassNameCPP, bool bIsStaticFunc)
{
	return "";
}

FString FScriptCodeGeneratorBase::InitializeFunctionDispatchParam(UFunction* Function, UProperty* Param, int32 ParamIndex)
{
	if (Param->IsA(UObjectPropertyBase::StaticClass()) || Param->IsA(UClassProperty::StaticClass()))
	{
		return TEXT("NULL");
	}
	else
	{
		return FString::Printf(TEXT("%s()"), *GetPropertyTypeCPP(Param, CPPF_ArgumentOrReturnValue));
	}	
}

FString FScriptCodeGeneratorBase::GetScriptHeaderForClass(UClass* Class)
{
	return GeneratedCodePath / (Class->GetName() + TEXT(".lua.h"));
}

bool FScriptCodeGeneratorBase::CanExportClass(UClass* Class)
{
	bool bCanExport = ((Class->ClassFlags & (CLASS_RequiredAPI | CLASS_MinimalAPI)) || !Class->GetMetaData(TEXT("Lua")).IsEmpty()) && // Don't export classes that don't export DLL symbols
		!ExportedClasses.Contains(Class->GetFName()); // Don't export classes that have already been exported
	return bCanExport;
}

bool FScriptCodeGeneratorBase::CanExportFunction(const FString& ClassNameCPP, UClass* Class, UFunction* Function)
{
	// We don't support delegates and non-public functions
	if (Function->FunctionFlags & FUNC_Delegate)
	{
		return false;
	}

	// Reject if any of the parameter types is unsupported yet
	for (TFieldIterator<UProperty> ParamIt(Function); ParamIt; ++ParamIt)
	{
		UProperty* Param = *ParamIt;
		if (!FScriptCodeGeneratorBase::CanExportProperty(ClassNameCPP, Class, Param))
		{
			return false;
		}
	}

	return true;
}

bool FScriptCodeGeneratorBase::CanExportProperty(const FString& ClassNameCPP, UClass* Class, UProperty* Property)
{
	if (
		Property->IsA(UDelegateProperty::StaticClass()) 
//		Property->IsA(UMulticastDelegateProperty::StaticClass()) ||
// 		Property->IsA(UWeakObjectProperty::StaticClass()) ||
// 		Property->IsA(UInterfaceProperty::StaticClass())
		)
	{
		return false;
	}

	return true;
}
