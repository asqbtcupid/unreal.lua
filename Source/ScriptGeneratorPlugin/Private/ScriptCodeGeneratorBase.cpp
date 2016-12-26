// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#include "ScriptGeneratorPluginPrivatePCH.h"
#include "ScriptCodeGeneratorBase.h"

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
	return FString::Printf(TEXT("%s%s"), Class->GetPrefixCPP(), *Class->GetName());
}

FString FScriptCodeGeneratorBase::GetPropertyTypeCPP(UProperty* Property, uint32 PortFlags /*= 0*/)
{
	static FString EnumDecl(TEXT("enum "));
	static FString StructDecl(TEXT("struct "));
	static FString ClassDecl(TEXT("class "));
	static FString TEnumAsByteDecl(TEXT("TEnumAsByte<enum "));
	static FString TSubclassOfDecl(TEXT("TSubclassOf<class "));

	FString PropertyType = Property->GetCPPType(NULL, PortFlags);
	// Strip any forward declaration keywords
	if (PropertyType.StartsWith(EnumDecl) || PropertyType.StartsWith(StructDecl) || PropertyType.StartsWith(ClassDecl))
	{
		int FirstSpaceIndex = PropertyType.Find(TEXT(" "));
		PropertyType = PropertyType.Mid(FirstSpaceIndex + 1);
	}
	else if (PropertyType.StartsWith(TEnumAsByteDecl))
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
	FString Params;
	FString paramList;
	FString returnType;
	//auto xx = Function->GetName() == "ReceivePossessed";
	const bool bHasParamsOrReturnValue = (Function->Children != NULL);
	if (bHasParamsOrReturnValue)
	{
		int32 ParamIndex = 0;
		if (bIsStaticFunc)
			ParamIndex = -1;
		for (TFieldIterator<UProperty> ParamIt(Function); ParamIt; ++ParamIt, ++ParamIndex)
		{
			UProperty* Param = *ParamIt;
			if ( Param->GetName() != "ReturnValue" )
			{
				FString initParam = InitializeFunctionDispatchParam(Function, Param, ParamIndex);
				Params += FString::Printf(TEXT("\t%s %s = %s;\r\n"), *GetPropertyTypeCPP(Param, CPPF_ArgumentOrReturnValue), *Param->GetName(), *initParam);
				paramList += Param->GetName() + ",";
			}
			else
			{
				returnType = GetPropertyTypeCPP(Param, CPPF_ArgumentOrReturnValue);
			}
		}
		
	}
	if (!returnType.IsEmpty())
		Params += FString::Printf(TEXT("\t%s result = "), *returnType);
	else
		Params += "\t";
	FString callobj = "Obj->";
	if (Function->FunctionFlags & FUNC_Static)
		callobj = FString::Printf(TEXT("%s::"), *ClassNameCPP);
	if (paramList.IsEmpty())
	{
		Params += FString::Printf(TEXT("%s%s();\r\n"), *callobj, *Function->GetName());
	}
	else
	{
		paramList.RemoveAt(paramList.Len()-1);
		Params += FString::Printf(TEXT("%s%s(%s);\r\n"), *callobj, *Function->GetName(), *paramList);
	}

	return Params;
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
	return GeneratedCodePath / (Class->GetName() + TEXT(".script.h"));
}

bool FScriptCodeGeneratorBase::CanExportClass(UClass* Class)
{
	bool bCanExport = (Class->ClassFlags & (CLASS_RequiredAPI | CLASS_MinimalAPI)) && // Don't export classes that don't export DLL symbols
		!ExportedClasses.Contains(Class->GetFName()); // Don't export classes that have already been exported
	bCanExport = bCanExport && Class->GetFName() != "DemoNetDriver";
	//bCanExport = bCanExport && Class->GetFName() == "Actor";
	return bCanExport;
}

bool FScriptCodeGeneratorBase::CanExportFunction(const FString& ClassNameCPP, UClass* Class, UFunction* Function)
{
	// We don't support delegates and non-public functions
	if ((Function->FunctionFlags & FUNC_Delegate) ||  !(Function->FunctionFlags & FUNC_Public))
	{
		return false;
	}
	

	// Reject if any of the parameter types is unsupported yet
	for (TFieldIterator<UProperty> ParamIt(Function); ParamIt; ++ParamIt)
	{
		UProperty* Param = *ParamIt;
		if (Param->IsA(UArrayProperty::StaticClass()) ||
			  Param->ArrayDim > 1 ||
			  Param->IsA(UDelegateProperty::StaticClass()) ||
				Param->IsA(UMulticastDelegateProperty::StaticClass()) ||
			  Param->IsA(UWeakObjectProperty::StaticClass()) ||
			  Param->IsA(UInterfaceProperty::StaticClass()))
		{
			return false;
		}
	}

	return true;
}

bool FScriptCodeGeneratorBase::CanExportProperty(const FString& ClassNameCPP, UClass* Class, UProperty* Property)
{
	// Property must be DLL exported
// 	if (!(Class->ClassFlags & CLASS_RequiredAPI))
// 	{
// 		return false;
// 	}

	// Only public, editable properties can be exported
// 	if (!Property->HasAnyFlags(RF_Public) || 
// 		  (Property->PropertyFlags & CPF_Protected) || 
// 			!(Property->PropertyFlags & CPF_Edit))
// 	{
// 		return false;
// 	}


	// Reject if it's one of the unsupported types (yet)
	if (Property->IsA(UArrayProperty::StaticClass()) ||
		Property->ArrayDim > 1 ||
		Property->IsA(UDelegateProperty::StaticClass()) ||
		Property->IsA(UMulticastDelegateProperty::StaticClass()) ||
		Property->IsA(UWeakObjectProperty::StaticClass()) ||
		Property->IsA(UInterfaceProperty::StaticClass()) ||
		Property->IsA(UStructProperty::StaticClass()))
	{
		return false;
	}

	return true;
}
