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
	if (Property->IsA(UArrayProperty::StaticClass()))
	{
		auto PropertyArr = Cast<UArrayProperty>(Property);
		FString inerTypeCpp = GetPropertyTypeCPP(PropertyArr->Inner, CPPF_ArgumentOrReturnValue);
		if (inerTypeCpp == "EObjectTypeQuery")
			inerTypeCpp = "TEnumAsByte<EObjectTypeQuery> ";
		PropertyType = FString::Printf(TEXT("TArray<%s>"), *inerTypeCpp);
	}
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

FString CallCode(UFunction* Function, bool bIsStaticFunc, bool hasresult, int num, FString paramlist, FString ClassNameCPP, bool isfinal = false)
{
	if (!paramlist.IsEmpty())
		paramlist.RemoveAt(paramlist.Len() - 1);
	if (!bIsStaticFunc)
		num = num + 1;
	FString code;
	if (!isfinal)
		code += FString::Printf(TEXT("\tif (num_params == %d )\r\n\t{\r\n"), num);
	if (bIsStaticFunc)
		if (hasresult)
			code += FString::Printf(TEXT("\t\tresult = %s::%s(%s);\r\n"), *ClassNameCPP, *Function->GetName(), *paramlist);
		else
			code += FString::Printf(TEXT("\t\t%s::%s(%s);\r\n"), *ClassNameCPP, *Function->GetName(), *paramlist);
	else
		if (hasresult)
			code += FString::Printf(TEXT("\t\tresult = Obj->%s(%s);\r\n"), *Function->GetName(), *paramlist);
		else
			code += FString::Printf(TEXT("\t\tObj->%s(%s);\r\n"), *Function->GetName(), *paramlist);
	if(!isfinal)
		code += "\t\tgoto end;\r\n\t}\r\n";
	return code;
}

FString FScriptCodeGeneratorBase::GenerateFunctionDispatch(UFunction* Function, const FString &ClassNameCPP, bool bIsStaticFunc)
{
	FString Params;
	FString paramList;
	FString returnType;
	int count_all_param = 0;
	int count_default_param = 0;
	int index_first_default = 0;
	bool hasDefaultAugmentParam = false;
	const bool bHasParamsOrReturnValue = (Function->Children != NULL);
	if (bHasParamsOrReturnValue)
	{
		int32 ParamIndex = 0;
		if (bIsStaticFunc)
			ParamIndex = -1;
		auto x = Function->GetName() == "PlaySoundAtLocation";
		for (TFieldIterator<UProperty> ParamIt(Function); ParamIt; ++ParamIt)
		{
			UProperty* Param = *ParamIt;
			if (Param->GetName() == "ReturnValue")
			{
				returnType = GetPropertyTypeCPP(Param, CPPF_ArgumentOrReturnValue);
			}
			else
			{
				++count_all_param;
				if (index_first_default == 0)
				{
					FString defaultvalue = Function->GetMetaData(*FString::Printf(TEXT("CPP_Default_%s"), *Param->GetName()));
					if (!defaultvalue.IsEmpty())
						index_first_default = count_all_param;
				}
			}
		}
		if (index_first_default != 0 && Function->GetName() != "SpawnSoundAttached")
			count_default_param = count_all_param - index_first_default + 1;
		if (count_default_param > 0)
		{
			Params += TEXT("\tint num_params = lua_gettop(L);\r\n");
			for (TFieldIterator<UProperty> ParamIt(Function); ParamIt; ++ParamIt)
			{
				UProperty* Param = *ParamIt;
				if (! (Param->GetName() == "ReturnValue"))
				{
					Params += FString::Printf(TEXT("\t%s %s;\r\n"), *GetPropertyTypeCPP(Param, CPPF_ArgumentOrReturnValue), *Param->GetName());
				}
			}
			if (!returnType.IsEmpty())
				Params += FString::Printf(TEXT("\t%s result;\r\n"), *returnType);
		}
		int count_init_param = 0;
		if (count_default_param > 0 && count_default_param + count_init_param >= count_all_param)
		{
			Params += CallCode(Function, bIsStaticFunc, !returnType.IsEmpty(), count_init_param, paramList, ClassNameCPP, count_init_param == count_all_param);
		}
		for (TFieldIterator<UProperty> ParamIt(Function); ParamIt; ++ParamIt, ++ParamIndex)
		{
			UProperty* Param = *ParamIt;
			if (Param->IsA(UArrayProperty::StaticClass()))
			{ 
				FString nameCpp = GetPropertyTypeCPP(Param, CPPF_ArgumentOrReturnValue);
				if (Param->GetName() != "ReturnValue")
				{
					paramList += Param->GetName() + ",";
					count_init_param++;
					if (count_default_param > 0 )
					{
						if (count_default_param + count_init_param >= count_all_param)
							Params += CallCode(Function, bIsStaticFunc, !returnType.IsEmpty(), count_init_param, paramList, ClassNameCPP, count_init_param == count_all_param);
					}
					else
					{
						Params += FString::Printf(TEXT("\t%s %s;\r\n"), *nameCpp, *Param->GetName());
					}
				}
			}
			else
			{
				if (Param->GetName() != "ReturnValue")
				{
					FString initParam = InitializeFunctionDispatchParam(Function, Param, ParamIndex);
					FString nameCpp = GetPropertyTypeCPP(Param, CPPF_ArgumentOrReturnValue);

					paramList += Param->GetName() + ",";
					count_init_param++;
					if (count_default_param > 0)
					{
						if (!nameCpp.Contains("*") && Param->IsA(UStructProperty::StaticClass()))
							Params += FString::Printf(TEXT("\t%s = %s;\r\n"), *Param->GetName(), *initParam);
						else
							Params += FString::Printf(TEXT("\t%s = %s;\r\n"), *Param->GetName(), *initParam);
						if (count_default_param + count_init_param >= count_all_param)
							Params += CallCode(Function, bIsStaticFunc, !returnType.IsEmpty(), count_init_param, paramList, ClassNameCPP, count_init_param == count_all_param);
					}
					else
					{
						if (!nameCpp.Contains("*") && Param->IsA(UStructProperty::StaticClass()))
							Params += FString::Printf(TEXT("\t%s %s = %s;\r\n"), *nameCpp, *Param->GetName(), *initParam);
						else
							Params += FString::Printf(TEXT("\t%s %s = %s;\r\n"), *nameCpp, *Param->GetName(), *initParam);
					}
				}
			}
		}
		
	}
	if (count_default_param == 0)
	{
		if (!returnType.IsEmpty())
			Params += FString::Printf(TEXT("\t%s result = "), *returnType);
		else
			Params += "\t";
		FString callobj = "Obj->";
		if (bIsStaticFunc)
			callobj = FString::Printf(TEXT("%s::"), *ClassNameCPP);
		if (paramList.IsEmpty())
		{
			Params += FString::Printf(TEXT("%s%s();\r\n"), *callobj, *Function->GetName());
		}
		else
		{
			paramList.RemoveAt(paramList.Len() - 1);
			Params += FString::Printf(TEXT("%s%s(%s);\r\n"), *callobj, *Function->GetName(), *paramList);
		}
	}
	else
	{
		Params += "\tend:\r\n";
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
	bCanExport = bCanExport && Class->GetFName() != "KismetArrayLibrary";
	//bCanExport = bCanExport && Class->GetFName() == "Actor";
	return bCanExport;
}

bool FScriptCodeGeneratorBase::CanExportFunction(const FString& ClassNameCPP, UClass* Class, UFunction* Function)
{
	// We don't support delegates and non-public functions
	auto x = Function->GetName() == "Array_AddUnique";
	if (Function->FunctionFlags & FUNC_Delegate)
	{
		return false;
	}
	

	// Reject if any of the parameter types is unsupported yet
	for (TFieldIterator<UProperty> ParamIt(Function); ParamIt; ++ParamIt)
	{
		UProperty* Param = *ParamIt;
		if (//Param->IsA(UArrayProperty::StaticClass()) ||
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
	if (//Property->IsA(UArrayProperty::StaticClass()) ||
 		Property->ArrayDim > 1 ||
		Property->IsA(UDelegateProperty::StaticClass()) ||
		Property->IsA(UMulticastDelegateProperty::StaticClass()) ||
		Property->IsA(UWeakObjectProperty::StaticClass()) ||
		Property->IsA(UInterfaceProperty::StaticClass())
		//Property->IsA(UStructProperty::StaticClass())
		)
	{
		return false;
	}

	return true;
}
