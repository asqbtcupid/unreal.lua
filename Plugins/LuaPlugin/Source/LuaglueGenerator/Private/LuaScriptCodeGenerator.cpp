// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#include "LuaScriptCodeGenerator.h"
#include "LuaglueGeneratorPrivatePCH.h"
#include "ModulePath.h"
#include "Paths.h"
#include "FileManager.h"
#include "../Launch/Resources/Version.h"
#define GetWeakObjType(varname, returnname) 		FString typeName = GetPropertyTypeCPP(varname, CPPF_ArgumentOrReturnValue); \
		int FirstSpaceIndex = typeName.Find(TEXT("<"));\
		typeName = typeName.Mid(FirstSpaceIndex + 1);\
		typeName.RemoveAt(typeName.Len() - 1);\
		FString returnname = FString::Printf(TEXT("TWeakObjectPtr_%s"), *typeName);

#define GetStructName(p) ("F"+ p->GetName())

bool IsApiClass(UClass* Class)
{
	return !!(Class->ClassFlags & (CLASS_MinimalAPI | CLASS_RequiredAPI));
}

bool IsApiFunc(UFunction* Function)
{
	UClass* Class = (UClass*)Function->GetOuter();
	if (Class->ClassFlags & CLASS_RequiredAPI)
		return true;

	if (Class->ClassFlags & CLASS_MinimalAPI)
		if (Function->FunctionFlags & FUNC_RequiredAPI)
			return true;

	return false;
}

bool IsApiProp(UProperty* Prop)
{
	UClass* Class = Cast<UClass>(Prop->GetOuter());
	if (Class)
	{
		if (Class->ClassFlags & CLASS_RequiredAPI)
			return true;
	}
	else
	{
		UScriptStruct* Struct = (UScriptStruct*)(Prop->GetOuter());
		if (Struct->StructFlags & STRUCT_RequiredAPI)
			return true;
	}

	return false;
}

FString GetUClassGlue(UClass* Class)
{
	FString CPPName = FString::Printf(TEXT("%s%s"), Class->GetPrefixCPP(), *Class->GetName());
	if (IsApiClass(Class))
		return CPPName + "::StaticClass()";
	else
		return "FindObject<UClass>(ANY_PACKAGE, TEXT(\"" + Class->GetName() + "\"))";
}

// #define GetUFunction(classname, propertyname) classname::StaticClass()->FindPropertyByName(FName(#propertyname))
const FString GetterPrefixStr = "LuaGet";
const FString SetterPrefixStr = "LuaSet";
FLuaScriptCodeGenerator::FLuaScriptCodeGenerator(const FString& RootLocalPath, const FString& RootBuildPath, const FString& OutputDirectory, const FString& InIncludeBase, const FString& LuaConfigPath)
	: FScriptCodeGeneratorBase(RootLocalPath, RootBuildPath, OutputDirectory, InIncludeBase)
{
	WeakPtrClass.Add("UObject");
	IncludeBase = InIncludeBase;
	GConfig->GetArray(TEXT("Lua"), TEXT("SupportedStruct"), SupportedStruct, LuaConfigPath);
	GConfig->GetArray(TEXT("Lua"), TEXT("NoSupportedStruct"), NoSupportedStruct, LuaConfigPath);
	GConfig->GetArray(TEXT("Lua"), TEXT("PrivatePropertyStruct"), PrivatePropertyStruct, LuaConfigPath);

	GConfig->GetArray(TEXT("Lua"), TEXT("NotSupportedClassFunction"), NotSupportedClassFunction, LuaConfigPath);
	GConfig->GetArray(TEXT("Lua"), TEXT("NotSupportedClassProperty"), NotSupportedClassProperty, LuaConfigPath);
	GConfig->GetArray(TEXT("Lua"), TEXT("NotSupportedClass"), NotSupportedClass, LuaConfigPath);
	GConfig->GetArray(TEXT("Lua"), TEXT("ChangeableModules"), ChangeableModules, LuaConfigPath);
	GConfig->GetArray(TEXT("Lua"), TEXT("NoCopyStruct"), NoCopyStruct, LuaConfigPath);
	GConfig->GetArray(TEXT("Lua"), TEXT("NoNewStruct"), NoNewStruct, LuaConfigPath);
	GConfig->GetArray(TEXT("Lua"), TEXT("CircularModules"), CircularModules, LuaConfigPath);

	GConfig->GetString(TEXT("Lua"), TEXT("GeneratedCodeDir"), LuaGeneratedCodeDir, LuaConfigPath);
	GConfig->GetString(TEXT("Lua"), TEXT("SublimeCompletionDir"), SublimeCompletionDir, LuaConfigPath);
}

FString FLuaScriptCodeGenerator::GenerateWrapperFunctionDeclaration(const FString& ClassNameCPP, FString classname, UFunction* Function)
{
	return GenerateWrapperFunctionDeclaration(ClassNameCPP, classname, Function->GetName());
}

FString FLuaScriptCodeGenerator::GenerateWrapperFunctionDeclaration(const FString& ClassNameCPP, FString classname, const FString& FunctionName)
{
	int i = 0;
	FString funcname = FString::Printf(TEXT("static int32 %s_%s(lua_State* L)"), *ClassNameCPP, *FunctionName, i);
	while (true)
	{
		if (ExportedFunc.Contains(funcname) == false)
			break;
		funcname = FString::Printf(TEXT("static int32 %s_%s%d(lua_State* L)"), *ClassNameCPP, *FunctionName, i);
	}
	ExportedFunc.Add(funcname);
	return funcname;
}

FString FLuaScriptCodeGenerator::InitializeParam(UProperty* Param, int32 ParamIndex, bool isnotpublicproperty, FString arrayName)
{
	FString Initializer;
	FString CircularModulesLuaPrefix = "";
	if (IsExportingCircularModule)
		CircularModulesLuaPrefix = "ue_";
	if (!(Param->GetPropertyFlags() & CPF_ReturnParm))
	{
		// In Lua, the first param index on the stack is 1 and it's the object we're invoking the function on
		ParamIndex += 2;

		if (Param->IsA(UInt64Property::StaticClass()) || Param->IsA(UUInt64Property::StaticClass()))
		{
			Initializer = TEXT("(" + CircularModulesLuaPrefix + "lua_tonumber");
		}
		else if (Param->IsA(UIntProperty::StaticClass()) ||
			Param->IsA(UUInt16Property::StaticClass())||
			Param->IsA(UUInt32Property::StaticClass()))
		{
			Initializer = TEXT("("+ CircularModulesLuaPrefix+ "lua_tointeger");
		}
		else if (Param->IsA(UFloatProperty::StaticClass()))
		{
			Initializer = TEXT("(float)(" + CircularModulesLuaPrefix + "lua_tonumber");
		}
		else if (Param->IsA(UDoubleProperty::StaticClass()))
		{
			Initializer = TEXT("(double)(" + CircularModulesLuaPrefix + "lua_tonumber");
		}
		else if (Param->IsA(UStrProperty::StaticClass()))
		{
			Initializer = TEXT("UTF8_TO_TCHAR(" + CircularModulesLuaPrefix + "lua_tostring");
		}
		else if (Param->IsA(UNameProperty::StaticClass()))
		{
			Initializer = TEXT("FName(" + CircularModulesLuaPrefix + "lua_tostring");
		}
		else if (Param->IsA(UTextProperty::StaticClass()))
		{
			Initializer = TEXT("FText::FromString(UTF8_TO_TCHAR(" + CircularModulesLuaPrefix + "lua_tostring");
		}
		else if (Param->IsA(UBoolProperty::StaticClass()))
		{
			Initializer = TEXT("!!(" + CircularModulesLuaPrefix + "lua_toboolean");
		}
		else if (Param->IsA(UClassProperty::StaticClass()))
		{
			FString typeName = GetPropertyTypeCPP(Param, CPPF_Implementation);

			if (typeName.StartsWith("TSubclass"))
				Initializer = TEXT("(UClass*)(touobject(L,");
			else
			{
				(Cast<UObjectProperty>(Param))->UObjectProperty::GetCPPMacroType(typeName);
				if (typeName.Contains("AnimBlueprintGeneratedClass"))
					Initializer = FString::Printf(TEXT("(%s*)(touobject(L,"), *typeName);
				else
					Initializer = TEXT("(UClass*)(touobject(L,");
			}

			return FString::Printf(TEXT("%s %d))"), *Initializer, ParamIndex);
		}
		else if (auto p = Cast<UArrayProperty>(Param))
		{
			Initializer += FString::Printf(TEXT("\tUTableUtil::read(%s, L, %d);\r\n"), *arrayName, ParamIndex);
			return Initializer;
		}
		else if (auto p = Cast<UMapProperty>(Param))
		{
			Initializer += FString::Printf(TEXT("\tUTableUtil::read(%s, L, %d);\r\n"), *arrayName, ParamIndex);
			return Initializer;
		}
		else if (auto p = Cast<USetProperty>(Param))
		{
			Initializer += FString::Printf(TEXT("\tUTableUtil::read(%s, L, %d);\r\n"), *arrayName, ParamIndex);
			return Initializer;
		}
		else if (Param->IsA(UWeakObjectProperty::StaticClass()))
		{
			GetWeakObjType(Param, nomeaning)
				Initializer = FString::Printf(TEXT("(%s*)(touobject(L,"), *typeName);
			return FString::Printf(TEXT("%s %d))"), *Initializer, ParamIndex);
		}
		else if (Param->IsA(UInterfaceProperty::StaticClass()))
		{		
			Initializer = "(UObject*)(touobject";
		}
		else if (Param->IsA(UObjectPropertyBase::StaticClass()))
		{
			FString typeName = GetPropertyTypeCPP(Param, CPPF_ArgumentOrReturnValue);
			if (typeName.Contains("*"))
			{
				FString luatypename = GetPropertyTypeCPP(Param, CPPF_ArgumentOrReturnValue);
				luatypename.RemoveAt(luatypename.Len() - 1);
				Initializer = FString::Printf(TEXT("(%s)(touobject(L,"), *typeName);
			}
			else if (isnotpublicproperty)
			{
				Initializer = FString::Printf(TEXT("(%s*)(touobject(L,"), *typeName);
			}
			else
			{
				Initializer = FString::Printf(TEXT("(%s*)(touobject(L,"), *typeName);
			}
			return FString::Printf(TEXT("%s %d))"), *Initializer, ParamIndex);
		}
		else if (Param->IsA(UStructProperty::StaticClass()))
		{
			FString typeName = GetPropertyTypeCPP(Param, CPPF_ArgumentOrReturnValue);
			if (typeName.Contains("lua_State"))
			{
				return FString::Printf(TEXT("GetFakeLuaState(L, %d)"), ParamIndex);
			}
			else if (typeName.Contains("lua_Index"))
			{
				return FString::Printf(TEXT("GetLuaIndex(%d)"), ParamIndex);
			}
			else if (typeName.Contains("*"))
			{
				FString luatypename = GetPropertyTypeCPP(Param, CPPF_ArgumentOrReturnValue);
				luatypename.RemoveAt(luatypename.Len() - 1);
				Initializer = FString::Printf(TEXT("(%s)(tostruct(L,"), *typeName);
			}
			else if (isnotpublicproperty)
			{
				Initializer = FString::Printf(TEXT("(%s*)(tostruct(L,"), *typeName);
			}
			else
			{
				Initializer = FString::Printf(TEXT("(%s*)(tostruct(L,"), *typeName);
			}
			return FString::Printf(TEXT("%s %d))"), *Initializer, ParamIndex);
		}
		else if (Param->IsA(UByteProperty::StaticClass()) || Param->IsA(UEnumProperty::StaticClass()))
		{
			FString typeName = GetPropertyTypeCPP(Param, CPPF_ArgumentOrReturnValue);
			Initializer = FString::Printf(TEXT("(%s)((int)%slua_tointeger"), *typeName, *CircularModulesLuaPrefix);
		}
		else if (Param->IsA(UMulticastDelegateProperty::StaticClass()))
		{
			FString typeName = GetPropertyTypeCPP(Param, CPPF_ArgumentOrReturnValue);
			Initializer = FString::Printf(TEXT("(%s)(%slua_tointeger"), *typeName, *CircularModulesLuaPrefix);
		}
		else if (Param->IsA(UInterfaceProperty::StaticClass()))
		{
			FString typeName = GetPropertyTypeCPP(Param, CPPF_ArgumentOrReturnValue);
			Initializer = FString::Printf(TEXT("(UObject*)(touobject"), ParamIndex);
		}
		else
		{
			Initializer = TEXT("(" + CircularModulesLuaPrefix + "lua_tointeger");
		}
	}
	if (Param->IsA(UTextProperty::StaticClass()))
		return FString::Printf(TEXT("%s(L, %d)))"), *Initializer, ParamIndex);
	else
		return FString::Printf(TEXT("%s(L, %d))"), *Initializer, ParamIndex);
}

FString FLuaScriptCodeGenerator::InitializeFunctionDispatchParam(UFunction* Function, UProperty* Param, int32 ParamIndex)
{
	if (!(Param->GetPropertyFlags() & CPF_ReturnParm))
	{
		//return FString::Printf(TEXT("%s(L, %d))"), *Initializer, ParamIndex);
		return InitializeParam(Param, ParamIndex);
	}
	else
	{
		return FScriptCodeGeneratorBase::InitializeFunctionDispatchParam(Function, Param, ParamIndex);
	}
}

FString FLuaScriptCodeGenerator::GenerateObjectDeclarationFromContext(const FString& ClassNameCPP)
{
	if (ClassNameCPP[0] == 'F')
		return FString::Printf(TEXT("%s* Obj = (%s*)tostruct(L,1);"), *ClassNameCPP, *ClassNameCPP);
	else
		return FString::Printf(TEXT("%s* Obj = (%s*)touobject(L,1);"), *ClassNameCPP, *ClassNameCPP);
}

FString FLuaScriptCodeGenerator::Push(const FString& ClassNameCPP, UFunction* Function, UProperty* ReturnValue, FString name, UClass* Class, bool bConsiderArrayDim)
{
	FString Initializer;
	if (ReturnValue->ArrayDim > 1 && bConsiderArrayDim)
	{
		Initializer = FString::Printf(TEXT("UTableUtil::pushcarr(L, %s);\r\n"), *name);
	}
	else if (ReturnValue->IsA(UIntProperty::StaticClass()) ||
		ReturnValue->IsA(UInt8Property::StaticClass()) ||
		ReturnValue->IsA(UUInt32Property::StaticClass()) ||
		ReturnValue->IsA(UUInt16Property::StaticClass()) )
	{
		Initializer = FString::Printf(TEXT("ue_lua_pushinteger(L, %s);"), *name);
	}
	else if (ReturnValue->IsA(UInt64Property::StaticClass()) || ReturnValue->IsA(UUInt64Property::StaticClass()))
	{
		Initializer = FString::Printf(TEXT("ue_lua_pushnumber(L, %s);"), *name);
	}
	else if (ReturnValue->IsA(UFloatProperty::StaticClass()) || ReturnValue->IsA(UDoubleProperty::StaticClass()))
	{
		Initializer = FString::Printf(TEXT("ue_lua_pushnumber(L, %s);"), *name);
	}
	else if (ReturnValue->IsA(UStrProperty::StaticClass()))
	{
		Initializer = FString::Printf(TEXT("ue_lua_pushstring(L, TCHAR_TO_UTF8(*%s));"), *name);
	}
	else if (ReturnValue->IsA(UNameProperty::StaticClass()))
	{
		Initializer = FString::Printf(TEXT("ue_lua_pushstring(L, TCHAR_TO_UTF8(*%s.ToString()));"), *name);
	}
	else if (ReturnValue->IsA(UTextProperty::StaticClass()))
	{
		Initializer = FString::Printf(TEXT("ue_lua_pushstring(L, TCHAR_TO_UTF8(*%s.ToString()));"), *name);
	}
	else if (ReturnValue->IsA(UBoolProperty::StaticClass()))
	{
		Initializer = FString::Printf(TEXT("ue_lua_pushboolean(L, %s);"), *name);
	}
	else if (ReturnValue->IsA(UClassProperty::StaticClass()))
	{
		Initializer = FString::Printf(TEXT("UTableUtil::push(L, %s);\r\n"), *name);
	}
	else if (ReturnValue->IsA(UStructProperty::StaticClass()))
	{
		FString typeName = GetPropertyTypeCPP(ReturnValue, CPPF_ArgumentOrReturnValue);
		if (ReturnValue->GetOuter()->IsA(UScriptStruct::StaticClass()) && ReturnValue->GetOffset_ForInternal() == 0)
			Initializer = FString::Printf(TEXT("pushstruct_nogc_firstmem(L, \"%s\", (void*)(&%s));"), *typeName, *name);
		else
			Initializer = FString::Printf(TEXT("pushstruct_nogc(L, \"%s\", (void*)(&%s));"), *typeName, *name);
	}
	else if (ReturnValue->IsA(UWeakObjectProperty::StaticClass()))
	{
		Initializer = FString::Printf(TEXT("pushuobject(L, (void*)%s.Get());"), *name);
	}
	else if (ReturnValue->IsA(UObjectPropertyBase::StaticClass()))
	{
		FString typeName = GetPropertyTypeCPP(ReturnValue, CPPF_ArgumentOrReturnValue);
		if (typeName.Contains("*"))
		{
			FString luatypeName = typeName;
			luatypeName.RemoveAt(luatypeName.Len() - 1);
			Initializer = FString::Printf(TEXT("pushuobject(L, (void*)%s);"), *name);
		}
		else
		{
			Initializer = FString::Printf(TEXT("pushuobject(L, (void*)(new %s(%s)), holyshit);"), *typeName, *name);
		}
	}
	else if (ReturnValue->IsA(UArrayProperty::StaticClass()))
	{
		if (Function == nullptr)
		{
			Initializer = FString::Printf(TEXT("static FName PropertyName = FName(\"%s\");\r\n"), *ReturnValue->GetName());
			if (ReturnValue->GetOuter()->IsA(UScriptStruct::StaticClass()))
				Initializer += FString::Printf(TEXT("\tstatic UArrayProperty* Property = (UArrayProperty*)%s::StaticStruct()->FindPropertyByName(PropertyName);\r\n"), *ClassNameCPP);
			else
			{
				Initializer += FString::Printf(TEXT("\tstatic UArrayProperty* Property = (UArrayProperty*)%s->FindPropertyByName(PropertyName);\r\n"), *GetUClassGlue(Class));
			}
			Initializer += FString::Printf(TEXT("\tUTableUtil::pushcontainer(L, Obj, Property);"));
		}
		else {
			Initializer = "UTableUtil::push(L, " + name + ");\r\n";
		}
	}
	else if (ReturnValue->IsA(UByteProperty::StaticClass()) || ReturnValue->IsA(UEnumProperty::StaticClass()))
	{
		Initializer = FString::Printf(TEXT("ue_lua_pushinteger(L, (int)%s);"), *name);
	}
	else if (auto DelegateProperty = Cast<UMulticastDelegateProperty>(ReturnValue))
	{
		Initializer = "auto delegateproxy = NewObject<ULuaDelegate>();\r\n";
		Initializer += "\tstatic UMulticastDelegateProperty* property = (UMulticastDelegateProperty*)" + GetUClassGlue(Class) + "->FindPropertyByName(FName(\"" + DelegateProperty->GetName() + "\"));\r\n";
		Initializer += "\tstatic UFunction* FunSig = property->SignatureFunction;\r\n";
		Initializer += FString::Printf(TEXT("\tdelegateproxy->Init(%s, FunSig);\r\n"), *name);
		Initializer += FString::Printf(TEXT("\tpushuobject(L, (void*)delegateproxy, true);"));
		return Initializer;
	}
	else if (auto p = Cast<UMapProperty>(ReturnValue))
	{
		if (Function == nullptr)
		{
			Initializer = FString::Printf(TEXT("static FName PropertyName = FName(\"%s\");\r\n"), *ReturnValue->GetName());
			if (ReturnValue->GetOuter()->IsA(UScriptStruct::StaticClass()))
				Initializer += FString::Printf(TEXT("\tstatic UMapProperty* Property = (UMapProperty*)%s::StaticStruct()->FindPropertyByName(PropertyName);\r\n"), *ClassNameCPP);
			else
			{
				Initializer += FString::Printf(TEXT("\tstatic UMapProperty* Property = (UMapProperty*)%s->FindPropertyByName(PropertyName);\r\n"), *GetUClassGlue(Class));

			}
			Initializer += FString::Printf(TEXT("\tUTableUtil::pushcontainer(L, Obj, Property);"));
		}
		else {
			Initializer = "UTableUtil::push(L, " + name + ");\r\n";
		}
	}
	else if (auto p = Cast<USetProperty>(ReturnValue))
	{
		if (Function == nullptr)
		{
			Initializer = FString::Printf(TEXT("static FName PropertyName = FName(\"%s\");\r\n"), *ReturnValue->GetName());
			if (ReturnValue->GetOuter()->IsA(UScriptStruct::StaticClass()))
				Initializer += FString::Printf(TEXT("\tstatic USetProperty* Property = (USetProperty*)%s::StaticStruct()->FindPropertyByName(PropertyName);\r\n"), *ClassNameCPP);
			else
			{
				Initializer += FString::Printf(TEXT("\tstatic USetProperty* Property = (USetProperty*)%s->FindPropertyByName(PropertyName);\r\n"), *GetUClassGlue(Class));
			}
			Initializer += FString::Printf(TEXT("\tUTableUtil::pushcontainer(L, Obj, Property);"));
		}
		else {
			Initializer = "UTableUtil::push(L, " + name + ");\r\n";
		}
	}
	else
	{
		Initializer = FString::Printf(TEXT("UTableUtil::push(L, %s);"), *name);
	}
	return Initializer;
}

bool FLuaScriptCodeGenerator::HasExportedClass(UClass* Class)
{
	return ExportedClasses.Contains(Class->GetFName());
}

bool FLuaScriptCodeGenerator::CanExportClass(UClass* Class)
{
	if (Class->ClassFlags &(CLASS_Intrinsic))
		return false;

	if (NotSupportedClass.Contains(GetClassNameCPP(Class)))
		return false;

	FString ModuleName = FModulePath::Get().GetClassModuleName(Class);
	if (ModuleName.IsEmpty() || !SupportModules.Contains(ModuleName))
		return false;

	return true;
}

bool FLuaScriptCodeGenerator::CanExportFunction(const FString& ClassNameCPP, UClass* Class, UFunction* Function)
{
	if (Class->ClassFlags &(CLASS_Interface))
		return false;
	
	bool bExport = CanExportFunctionForInterface(ClassNameCPP, Class, Function);

	if (bExport)
	{
		UClass* ClassOwner = Function->GetOwnerClass();
		if (ClassOwner != Class)
		{
			if (!CanExportFunction(GetClassNameCPP(ClassOwner), ClassOwner, Function))
				return false;
		}

	}
	return bExport;
}

bool FLuaScriptCodeGenerator::CanExportFunctionForInterface(const FString& ClassNameCPP, UClass* Class, UFunction* Function)
{
	if (NotSupportedClassFunction.Contains("*." + Function->GetName()) || NotSupportedClassFunction.Contains(ClassNameCPP + "." + Function->GetName()))
		return false;
	if (Function->GetName().Contains("DEPRECATED") || Function->HasMetaData("DeprecatedFunction"))
		return false;

	if (Function->GetName() == "ExecuteUbergraph")
		return false;

#if ENGINE_MINOR_VERSION >= 17
	if (Function->HasAnyFunctionFlags(FUNC_EditorOnly))
		return false;
#endif

	bool bExport = FScriptCodeGeneratorBase::CanExportFunction(ClassNameCPP, Class, Function);
	if (bExport)
	{
		for (TFieldIterator<UProperty> ParamIt(Function); bExport && ParamIt; ++ParamIt)
		{
			UProperty* Param = *ParamIt;
			if (!IsPropertyTypeSupported(Param))
				return false;
		}
	}

	return bExport;
}

FString CallCode(UFunction* Function, bool bIsStaticFunc, bool hasresult, int num, FString paramlist, FString ClassNameCPP, bool isfinal = false, UClass* InterfaceClass = nullptr)
{
// 	if (InterfaceClass)
// 	{
// 		paramlist = "p," + paramlist;
// 		num = num + 1;
// 	}
	if (!paramlist.IsEmpty())
		paramlist.RemoveAt(paramlist.Len() - 1);
	if (!bIsStaticFunc)
		num = num + 1;
	FString code;
	if (!isfinal)
		code += FString::Printf(TEXT("\tif (num_params == %d )\r\n\t{\r\n"), num);

	FString CallFuncName;
	if (InterfaceClass)
	{
		if (Function->HasAnyFunctionFlags(FUNC_Event))
		{
			if (Function->HasAnyFunctionFlags(FUNC_Native))
				CallFuncName = Function->GetName() + "_Implementation";
			else
				CallFuncName = Function->GetName();
		}
		else if (Function->HasAnyFunctionFlags(FUNC_BlueprintCallable))
			CallFuncName = Function->GetName();
	}
	else
		CallFuncName = Function->GetName();

	if (bIsStaticFunc)
	{
		if (hasresult)
			code += FString::Printf(TEXT("\t\tresult = %s::%s(%s);\r\n"), *ClassNameCPP, *CallFuncName, *paramlist);
		else
			code += FString::Printf(TEXT("\t\t%s::%s(%s);\r\n"), *ClassNameCPP, *CallFuncName, *paramlist);
	}
	else
	{
		if (hasresult)
			code += FString::Printf(TEXT("\t\tresult = Obj->%s(%s);\r\n"), *CallFuncName, *paramlist);
		else
			code += FString::Printf(TEXT("\t\tObj->%s(%s);\r\n"), *CallFuncName, *paramlist);
	}
	if (!isfinal)
		code += "\t\tgoto end;\r\n\t}\r\n";
	return code;
}

FString FLuaScriptCodeGenerator::GenerateFunctionDispatch_private(UFunction* Function, const FString &ClassNameCPP, UClass* Class, bool bIsStaticFunc, UClass* InterfaceClass)
{
	FString Params;
	FString paramList;
	FString returnType;
	FString FuncName = Function->GetName();
	FString args = "nullptr";
	const bool bHasParamsOrReturnValue = (Function->Children != NULL);
	if (bHasParamsOrReturnValue)
	{
		args = "&args";
		Params = TEXT("\tstruct params{\r\n");
		int32 ParamIndex = 0;
		for (TFieldIterator<UProperty> ParamIt(Function); ParamIt; ++ParamIt)
		{
			UProperty* Param = *ParamIt;
			Params += FString::Printf(TEXT("\t\t%s %s;\r\n"), *GetPropertyTypeCPP(Param, CPPF_ArgumentOrReturnValue), *Param->GetName());
		}
		Params += TEXT("\t}args;\r\n");
		for (TFieldIterator<UProperty> ParamIt(Function); ParamIt; ++ParamIt, ++ParamIndex)
		{
			int32 IndexForInit = ParamIndex;
			if (bIsStaticFunc)
				IndexForInit--;
			UProperty* Param = *ParamIt;
			FString nameCpp = GetPropertyTypeCPP(Param, CPPF_ArgumentOrReturnValue);
			if (Param->GetName() != "ReturnValue")
			{
				if (Param->IsA(UArrayProperty::StaticClass())
					|| Param->IsA(USetProperty::StaticClass())
					|| Param->IsA(UMapProperty::StaticClass())
					)
				{
					Params += InitializeParam(Param, IndexForInit, false, "args." + Param->GetName());
				}
				else
				{
					FString initParam = InitializeFunctionDispatchParam(Function, Param, IndexForInit);
					if (!nameCpp.Contains("*") && Param->IsA(UStructProperty::StaticClass()))
						Params += FString::Printf(TEXT("\targs.%s = *%s;\r\n"), *Param->GetName(), *initParam);
					else
						Params += FString::Printf(TEXT("\targs.%s = %s;\r\n"), *Param->GetName(), *initParam);
				}
			}
			else
				returnType = nameCpp;
		}

	}
	FString GetClassGlue = GetUClassGlue(Class);
	if(InterfaceClass)
		GetClassGlue = GetUClassGlue(InterfaceClass);
		
	Params += FString::Printf(TEXT("\tstatic FName FuncName(\"%s\");\r\n"), *FuncName);
	Params += FString::Printf(TEXT("\tstatic UFunction* Func = %s->FindFunctionByName(FuncName);\r\n"), *GetClassGlue);
	
	if (!bIsStaticFunc)
		Params += FString::Printf(TEXT("\tObj->ProcessEvent(Func, %s);\r\n"), *args);
	else
	{
		if (IsApiClass(Class))
			Params += FString::Printf(TEXT("\tGetMutableDefault<%s>()->ProcessEvent(Func, %s);\r\n"), *ClassNameCPP, *args);
		else
		{
			Params += FString::Printf(TEXT("\tstatic UClass* Class = %s;\r\n"), *GetClassGlue);
			Params += FString::Printf(TEXT("\tClass->GetDefaultObject()->ProcessEvent(Func, %s);\r\n"), *args);
		}
	}

	return Params;
}

bool NeedTempIns(UProperty *Param)
{
	if (Param->IsA(UStructProperty::StaticClass()))
		return true;
	if (auto p = Cast<UArrayProperty>(Param))
		return NeedTempIns(p->Inner);
	if (auto p = Cast<USetProperty>(Param))
		return NeedTempIns(p->ElementProp);
	if (auto p = Cast<UMapProperty>(Param))
		return NeedTempIns(p->KeyProp) || NeedTempIns(p->ValueProp);
	return false;
}

FString FLuaScriptCodeGenerator::GenerateFunctionDispatch(UFunction* Function, const FString &ClassNameCPP, bool bIsStaticFunc, UClass* InterfaceClass)
{
	FString Params;
	FString paramList;
	FString returnType;
	UProperty* ReturnParam = nullptr;

	FString FuncName = Function->GetName();
	int count_all_param = 0;
	int count_all_param_for_warning = 0;
	int count_default_param = 0;
	int index_first_default = 0;
	bool hasDefaultAugmentParam = false;
	const bool bHasParamsOrReturnValue = (Function->Children != NULL);
	if (bHasParamsOrReturnValue)
	{

		for (TFieldIterator<UProperty> ParamIt(Function); ParamIt; ++ParamIt)
		{
			UProperty* Param = *ParamIt;
			if (Param->GetName() == "ReturnValue")
			{
				returnType = GetPropertyTypeCPP(Param, CPPF_ArgumentOrReturnValue);
				ReturnParam = Param;
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
				if (Param->IsA(UStructProperty::StaticClass()))
				{
					FString typeName = GetPropertyTypeCPP(Param, CPPF_ArgumentOrReturnValue);
					if (!typeName.Contains("Lua_State"))
						++count_all_param_for_warning;
				}
				else
				{
					++count_all_param_for_warning;
				}
			}
		}

		int32 ParamIndex = 0;
		if (bIsStaticFunc)
			ParamIndex = -1;

// 		if (bIsInterface)
// 		{
// 			count_all_param++;
// 			ParamIndex = 1;
// 		}
		if (index_first_default != 0)
			count_default_param = count_all_param - index_first_default + 1;

		Params += TEXT("\t#ifdef LuaDebug\r\n");
		Params += TEXT("\tint totalPamCount = ue_lua_gettop(L);\r\n");
		if (bIsStaticFunc)
		{
			Params += FString::Printf(TEXT("\tif (totalPamCount < %d)\r\n"), count_all_param_for_warning - count_default_param);
		}
		else
		{
			Params += FString::Printf(TEXT("\tif (totalPamCount < %d)\r\n"), count_all_param_for_warning - count_default_param + 1);
		}

		Params += FString::Printf(TEXT("\t{\r\n\t\tensureAlwaysMsgf(0, L\"too less param in %s\");\r\n"), *Function->GetName());
		Params += FString::Printf(TEXT("\t\treturn 0;\r\n\t}\r\n"));
		Params += TEXT("\t#endif\r\n");

		if (count_default_param > 0)
		{
			Params += TEXT("\tint num_params = ue_lua_gettop(L);\r\n");
			for (TFieldIterator<UProperty> ParamIt(Function); ParamIt; ++ParamIt)
			{
				UProperty* Param = *ParamIt;
				if (!(Param->GetName() == "ReturnValue"))
				{
					if (Param->IsA(UStructProperty::StaticClass()))
						Params += FString::Printf(TEXT("\t%s* %s;\r\n"), *GetPropertyTypeCPP(Param, CPPF_ArgumentOrReturnValue), *Param->GetName());
					else if (NeedTempIns(Param))
					{
						FString TypeName = GetPropertyTypeCPP(Param, CPPF_ArgumentOrReturnValue);
						Params += FString::Printf(TEXT("\t%s& %s = UTableUtil::GetTempIns<%s>();\r\n"), *TypeName, *Param->GetName(), *TypeName);
					}
					else
						Params += FString::Printf(TEXT("\t%s %s;\r\n"), *GetPropertyTypeCPP(Param, CPPF_ArgumentOrReturnValue), *Param->GetName());
				}
			}
			if (ReturnParam)
			{
				if (NeedTempIns(ReturnParam))
					Params += FString::Printf(TEXT("\t%s& result = UTableUtil::GetTempIns<%s>();\r\n"), *returnType, *returnType);
				else
					Params += FString::Printf(TEXT("\t%s result;\r\n"), *returnType);
			}
		}
		int count_init_param = 0;
		if (count_default_param > 0 && count_default_param + count_init_param >= count_all_param)
		{
			Params += CallCode(Function, bIsStaticFunc, !returnType.IsEmpty(), count_init_param, paramList, ClassNameCPP, count_init_param == count_all_param, InterfaceClass);
		}
		for (TFieldIterator<UProperty> ParamIt(Function); ParamIt; ++ParamIt, ++ParamIndex)
		{
			UProperty* Param = *ParamIt;
			if (Param->IsA(UArrayProperty::StaticClass())
				|| Param->IsA(USetProperty::StaticClass())
				|| Param->IsA(UMapProperty::StaticClass())
				)
			{
				FString nameCpp = GetPropertyTypeCPP(Param, CPPF_ArgumentOrReturnValue);
				if (Param->GetName() != "ReturnValue")
				{
					FString PtrName = Param->GetName() + "_ptr";
					paramList += "*" + PtrName + ",";
					count_init_param++;
					if (count_default_param > 0)
					{
						Params += FString::Printf(TEXT("\t%s* %s = &%s;\r\n"), *nameCpp, *PtrName, *Param->GetName());
						Params += InitializeParam(Param, ParamIndex, false, "&" + PtrName);
						if (count_default_param + count_init_param >= count_all_param)
							Params += CallCode(Function, bIsStaticFunc, !returnType.IsEmpty(), count_init_param, paramList, ClassNameCPP, count_init_param == count_all_param, InterfaceClass);
					}
					else
					{
						if (NeedTempIns(Param))
							Params += FString::Printf(TEXT("\t%s& %s = UTableUtil::GetTempIns<%s>();\r\n"), *nameCpp, *Param->GetName(), *nameCpp);
						else
							Params += FString::Printf(TEXT("\t%s %s;\r\n"), *nameCpp, *Param->GetName());
						FString PtrName = Param->GetName() + "_ptr";
						Params += FString::Printf(TEXT("\t%s* %s = &%s;\r\n"), *nameCpp, *PtrName, *Param->GetName());
						Params += InitializeParam(Param, ParamIndex, false, "&" + PtrName);
					}
				}
			}
			else
			{
				if (Param->GetName() != "ReturnValue")
				{
					FString initParam = InitializeFunctionDispatchParam(Function, Param, ParamIndex);
					FString nameCpp = GetPropertyTypeCPP(Param, CPPF_ArgumentOrReturnValue);

					if (Param->IsA(UStructProperty::StaticClass()))
						paramList += "*" + Param->GetName() + ",";
					else
						paramList += Param->GetName() + ",";

					count_init_param++;
					if (count_default_param > 0)
					{
						Params += FString::Printf(TEXT("\t%s = %s;\r\n"), *Param->GetName(), *initParam);
						// SpawnSoundAttached ambiouscall blame UE4 not me
						if (count_default_param + count_init_param >= count_all_param && !(FuncName == "SpawnSoundAttached" && count_init_param == 4))
							Params += CallCode(Function, bIsStaticFunc, !returnType.IsEmpty(), count_init_param, paramList, ClassNameCPP, count_init_param == count_all_param, InterfaceClass);
					}
					else
					{
						if (!nameCpp.Contains("*") && Param->IsA(UStructProperty::StaticClass()))
							Params += FString::Printf(TEXT("\t%s* %s = %s;\r\n"), *nameCpp, *Param->GetName(), *initParam);
						else
							Params += FString::Printf(TEXT("\t%s %s = %s;\r\n"), *nameCpp, *Param->GetName(), *initParam);
					}
				}
			}
		}

	}
	if (count_default_param == 0)
	{
		if (ReturnParam)
		{
			if (NeedTempIns(ReturnParam))
			{
				Params += FString::Printf(TEXT("\t%s& result = UTableUtil::GetTempIns<%s>();\r\n"), *returnType, *returnType);
				Params += FString::Printf(TEXT("\tresult = "));
			}
			else
				Params += FString::Printf(TEXT("\t%s result = "), *returnType);
		}
		else
			Params += "\t";
		FString callobj = "Obj->";
// 		if (bIsInterface)
// 		{
// 			callobj = callobj + "Execute_";
// 			paramList = "p," + paramList;
// 		}
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

FString FLuaScriptCodeGenerator::FuncCode(FString  ClassNameCPP, FString classname, UFunction* Function, UClass* FuncSuper, UClass* Class, UClass* InterfaceClass)
{
	UProperty* ReturnValue = NULL;
	FString FunctionBody = GenerateWrapperFunctionDeclaration(ClassNameCPP, classname, Function);
	FunctionBody += TEXT("\r\n{\r\n");
	if (FuncSuper == NULL)
	{
		const FName ReturnCountKey = "LuaCustomReturn";
		bool HasReturnCountMeta = Function->HasMetaData(ReturnCountKey);
		if (!InterfaceClass && IsApiClass(Class) && (Function->FunctionFlags & FUNC_Public) && (!(Class->ClassFlags & CLASS_MinimalAPI) || (Function->FunctionFlags & FUNC_RequiredAPI)))
		{
			bool bIsStaticFunc = !!(Function->FunctionFlags & FUNC_Static);
			if (!bIsStaticFunc)
				FunctionBody += FString::Printf(TEXT("\t%s\r\n"), *GenerateObjectDeclarationFromContext(ClassNameCPP));

			FunctionBody += GenerateFunctionDispatch(Function, ClassNameCPP, bIsStaticFunc, InterfaceClass);
			int returnCount = 0;

			if (!HasReturnCountMeta)
			{
				for (TFieldIterator<UProperty> ParamIt(Function); !ReturnValue && ParamIt; ++ParamIt)
				{
					UProperty* Param = *ParamIt;
					if (Param->GetPropertyFlags() & CPF_ReturnParm)
					{
						ReturnValue = Param;
						FString returnValueName = "result";
						FunctionBody += FString::Printf(TEXT("\t%s\r\n"), *Push(ClassNameCPP, Function, ReturnValue, returnValueName, Class));
						returnCount++;
					}
				}
			}
			int32 ParamIndex = 2;
			if (bIsStaticFunc)
				ParamIndex = 1;
			for (TFieldIterator<UProperty> ParamIt(Function); ParamIt; ++ParamIt, ++ParamIndex)
			{
				UProperty* Param = *ParamIt;
				if ((Param->GetPropertyFlags() & (CPF_ConstParm | CPF_OutParm | CPF_ReturnParm)) == CPF_OutParm)
				{
					FString name = Param->GetName();
					if (Param->IsA(UArrayProperty::StaticClass())
						|| Param->IsA(UMapProperty::StaticClass())
						|| Param->IsA(USetProperty::StaticClass())
						)
					{
						FString PtrName = Param->GetName() + "_ptr";
						FunctionBody += FString::Printf(TEXT("\tUTableUtil::pushback(L, %d, *%s);\r\n"), ParamIndex, *PtrName);
					}
					else if (Param->IsA(UStructProperty::StaticClass()))
					{
						FunctionBody += FString::Printf(TEXT("\tUTableUtil::pushback(L, %d, *%s);\r\n"), ParamIndex, *name);
					}
					else
					{
						FunctionBody += FString::Printf(TEXT("\t%s\r\n"), *Push(ClassNameCPP, nullptr, Param, name, Class));
					}
					returnCount++;

				}
			}
			if (HasReturnCountMeta)
				FunctionBody += "\treturn result;\r\n";
			else
				FunctionBody += FString::Printf(TEXT("\treturn %d;\r\n"), returnCount);
		}
		else
		{
			bool bIsStaticFunc = !!(Function->FunctionFlags & FUNC_Static);
			if (!bIsStaticFunc)
				FunctionBody += FString::Printf(TEXT("\t%s\r\n"), *GenerateObjectDeclarationFromContext(ClassNameCPP));
			FunctionBody += GenerateFunctionDispatch_private(Function, ClassNameCPP, Class, bIsStaticFunc, InterfaceClass);

			int returnCount = 0;
			TArray<UProperty*> PushBackParms;
			UProperty* ReturnValueProperty = nullptr;
			for (TFieldIterator<UProperty> ParamIt(Function); ParamIt; ++ParamIt)
			{
				UProperty* Param = *ParamIt;
				if ((Param->GetPropertyFlags() & (CPF_ConstParm | CPF_OutParm | CPF_ReturnParm)) & CPF_ReturnParm)
				{
					FString name = "args." + Param->GetName();
					if (NeedTempIns(Param))
					{
						FString TypeName = GetPropertyTypeCPP(Param, CPPF_ArgumentOrReturnValue);
						FunctionBody += FString::Printf(TEXT("\t%s& result = UTableUtil::GetTempIns<%s>();\r\n"), *TypeName, *TypeName);
						FunctionBody += FString::Printf(TEXT("\tresult = %s;\r\n"), *name);
						FunctionBody += FString::Printf(TEXT("\t%s\r\n"), *Push(ClassNameCPP, (UFunction*)1, Param, "result", Class));
					}
					else
						FunctionBody += FString::Printf(TEXT("\t%s\r\n"), *Push(ClassNameCPP, (UFunction*)1, Param, name, Class));
					returnCount++;
					ReturnValueProperty = Param;
					break;
				}
			}
			int32 ParamIndex = 2;
			if (bIsStaticFunc)
				ParamIndex = 1;
			for (TFieldIterator<UProperty> ParamIt(Function); ParamIt; ++ParamIt, ++ParamIndex)
			{
				UProperty* Param = *ParamIt;
				if ((Param->GetPropertyFlags() & (CPF_ConstParm | CPF_OutParm | CPF_ReturnParm)) & CPF_OutParm)
				{
					if (Param != ReturnValueProperty)
					{
						FString name = "args." + Param->GetName();
						if (Param->IsA(UArrayProperty::StaticClass())
							|| Param->IsA(UMapProperty::StaticClass())
							|| Param->IsA(USetProperty::StaticClass())
							|| Param->IsA(UStructProperty::StaticClass())
							)
						{
							if (NeedTempIns(Param))
							{
								FString TypeName = GetPropertyTypeCPP(Param, CPPF_ArgumentOrReturnValue);
								FunctionBody += FString::Printf(TEXT("\t%s& %s = UTableUtil::GetTempIns<%s>();\r\n"), *TypeName, *Param->GetName(), *TypeName);
								FunctionBody += FString::Printf(TEXT("\t%s = %s;\r\n"), *Param->GetName(), *name);
								FunctionBody += FString::Printf(TEXT("\tUTableUtil::pushback_private(L, %d, %s);\r\n"), ParamIndex, *Param->GetName());
							}
							else
								FunctionBody += FString::Printf(TEXT("\tUTableUtil::pushback_private(L, %d, %s);\r\n"), ParamIndex, *name);
						}
						else
						{
							FunctionBody += FString::Printf(TEXT("\t%s\r\n"), *Push(ClassNameCPP, (UFunction*)1, Param, name, Class));
						}
						returnCount++;
					}
				}
			}
			FunctionBody += FString::Printf(TEXT("\treturn %d;\r\n"), returnCount);
		}
	}
	else
	{
		FunctionBody += FString::Printf(TEXT("\treturn %s_%s(L);\r\n"), *GetClassNameCPP(FuncSuper), *Function->GetName());
	}
	FunctionBody += TEXT("}\r\n\r\n");
	return FunctionBody;
}

FString FLuaScriptCodeGenerator::ExportInterfaceFunc(UClass* Class)
{
	FString GeneratedGlueBody;
	for (FImplementedInterface& Interface : Class->Interfaces)
	{
		UClass* InterfaceClass = Interface.Class;
		if (CanExportClass(InterfaceClass))
		{
			FString HeaderPath = FModulePath::Get().GetClassHeaderPath(InterfaceClass);
			if (!HeaderPath.IsEmpty())
				ExtraIncludeHeader.AddUnique(HeaderPath);
			FString ClassNameCPP = GetClassNameCPP(Class);

			for (TFieldIterator<UFunction> FuncIt(InterfaceClass /*, EFieldIteratorFlags::ExcludeSuper*/); FuncIt; ++FuncIt)
			{
				UFunction* Function = *FuncIt;
				if (CanExportFunctionForInterface(ClassNameCPP, Class, Function))
				{
					auto& Exports = ClassExportedFunctions.FindOrAdd(Class);
					Exports.Add(Function->GetFName());

					GeneratedGlueBody += FuncCode(ClassNameCPP, Class->GetName(), Function, nullptr, Class, InterfaceClass);
					if (bExportSbCompletion())
						AddFunctionToCompletions(Function);
				}
			}
		}
	}

	return GeneratedGlueBody;
}

FString FLuaScriptCodeGenerator::ExportFunction(const FString& ClassNameCPP, UClass* Class, UFunction* Function)
{
	UClass* FuncSuper = NULL;

	auto& Exports = ClassExportedFunctions.FindOrAdd(Class);
	Exports.Add(Function->GetFName());
	if (Function->GetOwnerClass() != Class)
	{
		if (CanExportClass(Function->GetOwnerClass()))
		{
			FuncSuper = Function->GetOwnerClass();
			FuncSuperClassName.Add(Function->GetFName(), GetClassNameCPP(FuncSuper));
			return "";
		}
	}
	if(bExportSbCompletion())
		AddFunctionToCompletions(Function);
	return FuncCode(ClassNameCPP, Class->GetName(), Function, FuncSuper, Class);
}

bool FLuaScriptCodeGenerator::IsPropertyTypeSupported(UProperty* Property)
{
	bool bSupported = true;
	if (Property->IsA(UStructProperty::StaticClass()))
	{
		UStructProperty* StructProp = CastChecked<UStructProperty>(Property);
		if (!isStructSupported(StructProp->Struct))
		{
			bSupported = false;
		}
		else
		{
			ExtraIncludeHeader.AddUnique(StructProp->Struct->GetName() + ".lua.h");
		}
	}
	else if (auto p = Cast<UArrayProperty>(Property))
	{
		if (!IsPropertyTypeSupported(p->Inner))
		{
			bSupported = false;
		}
	}
	else if (Property->IsA(ULazyObjectProperty::StaticClass()) ||
#if ENGINE_MINOR_VERSION < 18
		Property->IsA(UAssetObjectProperty::StaticClass()) ||
		Property->IsA(UAssetClassProperty::StaticClass())
#else
		Property->IsA(USoftObjectProperty::StaticClass()) ||
		Property->IsA(USoftClassProperty::StaticClass())
#endif
		)
	{
		bSupported = false;
	}
	else if (Property->IsA(UByteProperty::StaticClass()))
		bSupported = true;
	else if (auto p = Cast<UMapProperty>(Property))
	{
		if (!IsPropertyTypeSupported(p->KeyProp) || !IsPropertyTypeSupported(p->ValueProp))
		{
			bSupported = false;
		}
	}
	else if (auto p = Cast<USetProperty>(Property))
	{
		if (!IsPropertyTypeSupported(p->ElementProp))
			bSupported = false;
	}
	else if (auto p = Cast<UMulticastDelegateProperty>(Property))
	{
		ExtraIncludeHeader.AddUnique("LuaDelegate.h");
		for (TFieldIterator<UProperty> ParamIt(p->SignatureFunction); ParamIt; ++ParamIt)
		{
			if (!IsPropertyTypeSupported(*ParamIt))
			{
				bSupported = false;
				break;
			}
		}
	}
	else if (auto p = Cast<UClassProperty>(Property))
	{
		FString HeaderPath = FModulePath::Get().GetClassHeaderPath(p->MetaClass);
		if (!HeaderPath.IsEmpty())
			ExtraIncludeHeader.AddUnique(HeaderPath);

		HeaderPath = FModulePath::Get().GetClassHeaderPath(p->PropertyClass);
		if (!HeaderPath.IsEmpty())
			ExtraIncludeHeader.AddUnique(HeaderPath);
	}
	else if (auto p = Cast<UWeakObjectProperty>(Property))
	{
		if (!CanExportClass(p->PropertyClass))
		{
			bSupported = false;
		}
		else
		{
			FString HeaderPath = FModulePath::Get().GetClassHeaderPath(p->PropertyClass);
			if (!HeaderPath.IsEmpty())
				ExtraIncludeHeader.AddUnique(HeaderPath);
		}
	}
	else if (auto p = Cast<UInterfaceProperty>(Property))
	{
#if ENGINE_MINOR_VERSION < 17
// there is bug in 4.16 engine scriptinterface.blame ue4 not me.
		return false;
#endif
		if (CanExportClass(p->InterfaceClass))
		{
			bSupported = true;
			FString HeaderPath = FModulePath::Get().GetClassHeaderPath(p->InterfaceClass);
			if (!HeaderPath.IsEmpty())
				ExtraIncludeHeader.AddUnique(HeaderPath);
		}
	}
	else if (auto p = Cast<UObjectPropertyBase>(Property))
	{
		FString HeaderPath = FModulePath::Get().GetClassHeaderPath(p->PropertyClass);
		if (!HeaderPath.IsEmpty())
			ExtraIncludeHeader.AddUnique(HeaderPath);
	}
	else if (!Property->IsA(UIntProperty::StaticClass()) &&
		!Property->IsA(UInt64Property::StaticClass()) &&
		!Property->IsA(UUInt32Property::StaticClass()) &&
		!Property->IsA(UUInt16Property::StaticClass()) &&
		!Property->IsA(UFloatProperty::StaticClass()) &&
		!Property->IsA(UDoubleProperty::StaticClass()) &&
		!Property->IsA(UTextProperty::StaticClass()) &&
		!Property->IsA(UStrProperty::StaticClass()) &&
		!Property->IsA(UNameProperty::StaticClass()) &&
		!Property->IsA(UBoolProperty::StaticClass()) &&
		!Property->IsA(UEnumProperty::StaticClass()))
	{
		bSupported = false;
	}

	return bSupported;
}

bool FLuaScriptCodeGenerator::CanExportProperty(const FString& ClassNameCPP, UClass* Class, UProperty* Property)
{
	if (NotSupportedClassProperty.Contains("*." + Property->GetName()) || NotSupportedClassProperty.Contains(ClassNameCPP + "." + Property->GetName()))
		return false;

	if ((Property->PropertyFlags & CPF_Deprecated))
	{
		return false;
	}

	UClass* OwnerClass = Property->GetOwnerClass();
	if (OwnerClass != Class)
	{
		if (!CanExportProperty(GetClassNameCPP(OwnerClass), OwnerClass, Property))
			return false;
	}

	bool bsupport = FScriptCodeGeneratorBase::CanExportProperty(ClassNameCPP, Class, Property);
	if (bsupport)
	{
		if (auto p = Cast<UArrayProperty>(Property))
			if (GetPropertyType(p->Inner) == "" || !IsPropertyTypeSupported(p->Inner))
				return false;
		if (auto p = Cast<UMulticastDelegateProperty>(Property))
		{
			for (TFieldIterator<UProperty> ParamIt(p->SignatureFunction); ParamIt; ++ParamIt)
			{
				UProperty* Param = *ParamIt;
				if (!CanExportProperty("", nullptr, Param))
				{
					return false;
				}
			}

		}
		return IsPropertyTypeSupported(Property);
	}
	else
		return false;
}

FString FLuaScriptCodeGenerator::GetPropertyType(UProperty* Property) const
{
	if (Property->IsA(UStrProperty::StaticClass()))
	{
		return FString("UStrProperty");
	}
	else if (Property->IsA(UEnumProperty::StaticClass()))
	{
		return FString("UEnumProperty");
	}
	else if (Property->IsA(UInt8Property::StaticClass()))
	{
		return FString("UInt8Property");
	}
	else if (Property->IsA(UIntProperty::StaticClass()))
	{
		return FString("UIntProperty");
	}
	else if (Property->IsA(UFloatProperty::StaticClass()))
	{
		return FString("UFloatProperty");
	}
	else if (Property->IsA(UDoubleProperty::StaticClass()))
	{
		return FString("UDoubleProperty");
	}
	else if (Property->IsA(UBoolProperty::StaticClass()))
	{
		return FString("UBoolProperty");
	}
	else if (Property->IsA(UNameProperty::StaticClass()))
	{
		return FString("UNameProperty");
	}
	else if (Property->IsA(UTextProperty::StaticClass()))
	{
		return FString("UTextProperty");
	}
	else if (Property->IsA(UMulticastDelegateProperty::StaticClass()))
	{
		return FString("UMulticastDelegateProperty");
	}
	else if (Property->IsA(UWeakObjectProperty::StaticClass()))
	{
		// return FString("");
		return FString("UWeakObjectProperty");
	}
	else if (Property->IsA(UClassProperty::StaticClass()))
	{
		return FString("UClassProperty");
	}
	else if (Property->IsA(UInterfaceProperty::StaticClass()))
	{
		return FString("UInterfaceProperty");
	}
	else if (Property->IsA(UObjectPropertyBase::StaticClass()))
	{
		return FString("UObjectPropertyBase");
	}
	else if (Property->IsA(UStructProperty::StaticClass()))
	{
		return FString("UStructProperty");
	}
	else if (Property->IsA(UArrayProperty::StaticClass()))
	{
		return FString("UArrayProperty");
	}
	else if (Property->IsA(UByteProperty::StaticClass()))
	{
		return FString("UByteProperty");
	}
	else if (Property->IsA(UUInt32Property::StaticClass()))
	{
		return FString("UUInt32Property");
	}
	else if (Property->IsA(UInt64Property::StaticClass()))
	{
		return FString("UInt64Property");
	}
	else if (Property->IsA(UUInt16Property::StaticClass()))
	{
		return FString("UUInt16Property");
	}
	else if (Property->IsA(UMapProperty::StaticClass()))
	{
		return FString("UMapProperty");
	}
	else if (Property->IsA(USetProperty::StaticClass()))
	{
		return FString("USetProperty");
	}
	else
	{
		return FString("");
	}
}

FString FLuaScriptCodeGenerator::GetPropertyGetFunc(UProperty* Property) const
{
	if (Property->IsA(UClassProperty::StaticClass()))
	{
		return FString("ContainerPtrToValuePtr<void>");
	}
	if (Property->IsA(UWeakObjectProperty::StaticClass()))
	{
		return FString("GetObjectPropertyValue");
	}
	else if (Property->IsA(UObjectPropertyBase::StaticClass()))
	{
		return FString("GetObjectPropertyValInContainer");
	}
	else if (Property->IsA(UStructProperty::StaticClass()))
	{
		return FString("ContainerPtrToValuePtr<uint8>");
	}
	else if (Property->IsA(UByteProperty::StaticClass()))
	{
		return FString("GetPropertyValInContainer");
	}
	else
	{
		return FString("GetPropertyValInContainer");
	}
}

FString FLuaScriptCodeGenerator::GetPropertySetFunc(UProperty* Property) const
{
	if (Property->IsA(UWeakObjectProperty::StaticClass()))
	{
		return FString("SetObjectPropertyValue");
	}
	else if (Property->IsA(UObjectPropertyBase::StaticClass()))
	{
		return FString("SetObjectPropertyValInContainer");
	}
	else if (Property->IsA(UStructProperty::StaticClass()))
	{
		return FString("CopyCompleteValue");
	}
	else if (Property->IsA(UByteProperty::StaticClass()))
	{
		return FString("SetPropertyValInContainer");
	}
	else
	{
		return FString("SetPropertyValInContainer");
	}
}
FString FLuaScriptCodeGenerator::GetPropertyCastType(UProperty* Property)
{
	if (Property->IsA(UClassProperty::StaticClass()))
	{
		return FString("UClass*");
	}
	else if (Property->IsA(UStructProperty::StaticClass()))
	{
		FString typenamecpp = GetPropertyTypeCPP(Property, CPPF_ArgumentOrReturnValue);
		return FString::Printf(TEXT("%s*"), *typenamecpp);
	}
	else if (Property->IsA(UWeakObjectProperty::StaticClass()))
	{
		GetWeakObjType(Property, objtype)
			return typeName + "*";
	}
	else
	{
		return FString("");
	}
}
FString FLuaScriptCodeGenerator::GetPropertySetCastType(UProperty* Property)
{
	if (Property->IsA(UBoolProperty::StaticClass()))
	{
		return FString("bool");
	}
	else if (Property->IsA(UStructProperty::StaticClass()))
	{
		FString typecpp = GetPropertyTypeCPP(Property, CPPF_ArgumentOrReturnValue);
		return FString::Printf(TEXT("%s*"), *typecpp);
	}
	else if (Property->IsA(UWeakObjectProperty::StaticClass()))
	{
		return GetPropertyCastType(Property);
	}
	else
	{
		return FString("");
	}
}

bool FLuaScriptCodeGenerator::isNotCopyStruct(UScriptStruct* thestruct)
{
	if (NoCopyStruct.Contains(GetStructName(thestruct)))
		return true;
// 	auto Ops = thestruct->GetCppStructOps();
// 	if (Ops && !Ops->HasCopy())
// 		return true;

	return false;
}


bool FLuaScriptCodeGenerator::CanCopyProperty(UProperty* Property)
{
	if (UStructProperty*p = Cast<UStructProperty>(Property))
	{
		return !isNotCopyStruct(p->Struct);
	}
	else if (UArrayProperty*p = Cast<UArrayProperty>(Property))
	{
		return CanCopyProperty(p->Inner);
	}
	else if (UMapProperty*p = Cast<UMapProperty>(Property))
	{
		return CanCopyProperty(p->KeyProp) && CanCopyProperty(p->ValueProp);
	}
	else if (USetProperty*p = Cast<USetProperty>(Property))
	{
		return CanCopyProperty(p->ElementProp);
	}
	return true;
}

FString FLuaScriptCodeGenerator::GetterCode(FString ClassNameCPP, FString classname, FString FuncName, UProperty* Property, UClass* Class, UClass* PropertySuper)
{
	FString GeneratedGlue = GenerateWrapperFunctionDeclaration(ClassNameCPP, classname, FuncName);
	GeneratedGlue += TEXT("\r\n{\r\n");
	FString FunctionBody;

	if (PropertySuper == nullptr)
	{
		if (Property->PropertyFlags & CPF_EditorOnly)
			FunctionBody += TEXT("#if WITH_EDITORONLY_DATA\r\n");
		FunctionBody += FString::Printf(TEXT("\t%s\r\n"), *GenerateObjectDeclarationFromContext(ClassNameCPP));
		if (!PrivatePropertyStruct.Contains(ClassNameCPP) && 
			(Property->PropertyFlags & CPF_NativeAccessSpecifierPublic || Property->IsA(UArrayProperty::StaticClass()))
			)
		{
			if (Property->IsA(UArrayProperty::StaticClass()))
			{
			}
			else if (Property->IsA(UClassProperty::StaticClass()) && Property->HasAnyPropertyFlags(CPF_UObjectWrapper))
			{
				FunctionBody += FString::Printf(TEXT("\tconst auto& result = (Obj->%s).Get();\r\n"), *Property->GetName());
			}
			else if (auto DelegateProperty = Cast<UMulticastDelegateProperty>(Property))
			{
				FunctionBody += FString::Printf(TEXT("\tauto& result = Obj->%s;\r\n"), *Property->GetName());
			}
			else
			{
				FunctionBody += FString::Printf(TEXT("\tconst auto& result = Obj->%s;\r\n"), *Property->GetName());
			}
			FunctionBody += FString::Printf(TEXT("\t%s\r\n"), *Push(ClassNameCPP, NULL, Property, FString("result"), Class));
		}
		else
		{
			if (Property->ArrayDim <= 1)
			{
				FString statictype = GetPropertyType(Property);
// 				FunctionBody += FString::Printf(TEXT("\tstatic FName PropertyName(\"%s\");\r\n"), *Property->GetName());
				if (ClassNameCPP[0] != 'F')
				{
					FunctionBody += FString::Printf(TEXT("\tstatic %s* p = (%s*)%s->FindPropertyByName(\"%s\");\r\n"), *statictype, *statictype, *GetUClassGlue(Class), *Property->GetName());
				}
				else
				{
					if (!PrivatePropertyStruct.Contains(ClassNameCPP))
						FunctionBody += FString::Printf(TEXT("\tstatic %s* p = (%s*)%s::StaticStruct()->FindPropertyByName(\"%s\");\r\n"), *statictype, *statictype, *ClassNameCPP, *Property->GetName());
					else
					{
						FString GetStructClassStr = "FindObject<UScriptStruct>(ANY_PACKAGE, TEXT(\"" + ClassNameCPP.RightChop(1) + "\"))";
						FunctionBody += FString::Printf(TEXT("\tstatic %s* p = (%s*)%s->FindPropertyByName(\"%s\");\r\n"), *statictype, *statictype, *GetStructClassStr, *Property->GetName());
					}
				}
				if (Property->IsA(UStructProperty::StaticClass())&&
					Property->GetOuter()->IsA(UScriptStruct::StaticClass()) &&
					Property->GetOffset_ForInternal() == 0)
					FunctionBody += TEXT("\tUTableUtil::pushproperty_type_firstmem(L, p, Obj);\r\n");
				else
					FunctionBody += TEXT("\tUTableUtil::pushproperty_type(L, p, Obj);\r\n");
			}
		}
		FunctionBody += FString::Printf(TEXT("\treturn 1;\r\n"));
		if (Property->PropertyFlags & CPF_EditorOnly)
			FunctionBody += TEXT("#else\r\n\treturn 0;\r\n#endif\r\n");
	}
	else
	{
		FunctionBody += FString::Printf(TEXT("\treturn %s_%s(L);\r\n"), *GetClassNameCPP(PropertySuper), *FuncName);
	}
	GeneratedGlue += FunctionBody;
	GeneratedGlue += TEXT("}\r\n\r\n");
	return GeneratedGlue;
}

FString FLuaScriptCodeGenerator::SetterBody(UProperty* Property)
{
	int PropertyIndex = 2;
	FString Initializer = "\tObj->" + Property->GetName() + " = ";
	FString CircularModulesLuaPrefix = "";
	if (IsExportingCircularModule)
		CircularModulesLuaPrefix = "ue_";
	if (Property->ArrayDim > 1)
	{
		return "";
	}
	if (Property->IsA(UInt64Property::StaticClass()))
	{
		Initializer += TEXT("(int64)(" + CircularModulesLuaPrefix + "lua_tonumber");
	}
	else if (Property->IsA(UUInt64Property::StaticClass()))
	{
		Initializer += TEXT("(uint64)(" + CircularModulesLuaPrefix + "lua_tonumber");
	}
	else if (Property->IsA(UMulticastDelegateProperty::StaticClass()))
	{
		return "";
	}
	else if (Property->IsA(UFloatProperty::StaticClass()))
	{
		Initializer += TEXT("(float)(" + CircularModulesLuaPrefix + "lua_tonumber");
	}
	else if (Property->IsA(UDoubleProperty::StaticClass()))
	{
		Initializer += TEXT("(double)(" + CircularModulesLuaPrefix + "lua_tonumber");
	}
	else if (Property->IsA(UStrProperty::StaticClass()))
	{
		Initializer += TEXT("UTF8_TO_TCHAR(" + CircularModulesLuaPrefix + "lua_tostring");
	}
	else if (Property->IsA(UNameProperty::StaticClass()))
	{
		Initializer += TEXT("FName(" + CircularModulesLuaPrefix + "lua_tostring");
	}
	else if (Property->IsA(UTextProperty::StaticClass()))
	{
		Initializer += TEXT("FText::FromString(UTF8_TO_TCHAR(" + CircularModulesLuaPrefix + "lua_tostring");
	}
	else if (Property->IsA(UBoolProperty::StaticClass()))
	{
		Initializer += TEXT("!!(" + CircularModulesLuaPrefix + "lua_toboolean");
	}
	else if (Property->IsA(UClassProperty::StaticClass()))
	{
		FString typeName = GetPropertyTypeCPP(Property, CPPF_Implementation);

		if (typeName.StartsWith("TSubclass"))
			Initializer += TEXT("(UClass*)(touobject(L,");
		else
		{
			(Cast<UObjectProperty>(Property))->UObjectProperty::GetCPPMacroType(typeName);
			if (typeName.Contains("AnimBlueprintGeneratedClass"))
				Initializer += FString::Printf(TEXT("(%s*)(touobject(L,"), *typeName);
			else
				Initializer += TEXT("(UClass*)(touobject(L,");
		}

		return FString::Printf(TEXT("%s %d))"), *Initializer, PropertyIndex);
	}
	else if (auto ArrProperty = Cast<UArrayProperty>(Property))
	{
		return FString::Printf(TEXT("\tUTableUtil::read(Obj->%s, L, 2)"), *Property->GetName());
	}
	else if (auto p = Cast<UWeakObjectProperty>(Property))
	{
		Initializer += FString::Printf(TEXT("(%s%s*)(touobject(L,"), p->PropertyClass->GetPrefixCPP(), *p->PropertyClass->GetName());
		return FString::Printf(TEXT("%s %d))"), *Initializer, PropertyIndex);
	}
	else if (Property->IsA(UInterfaceProperty::StaticClass()))
	{
		Initializer += "(UObject*)(touobject";
	}
	else if (Property->IsA(UObjectPropertyBase::StaticClass()))
	{
		FString typeName = GetPropertyTypeCPP(Property, CPPF_ArgumentOrReturnValue);
		if (typeName.Contains("*"))
		{
			FString luatypename = GetPropertyTypeCPP(Property, CPPF_ArgumentOrReturnValue);
			luatypename.RemoveAt(luatypename.Len() - 1);
			Initializer += FString::Printf(TEXT("(%s)(touobject(L,"), *typeName);
		}
		else
		{
			Initializer += FString::Printf(TEXT("(%s*)(touobject(L,"), *typeName);
		}
		return FString::Printf(TEXT("%s %d))"), *Initializer, PropertyIndex);
	}
	else if (auto p = Cast<UStructProperty>(Property))
	{
		FString StructName = p->Struct->GetName();
		if (StructName == "ActorTickFunction"
			|| StructName == "TickFunction"
			|| StructName == "ActorComponentTickFunction"
			)
			return "";

		FString typeName = GetPropertyTypeCPP(Property, CPPF_ArgumentOrReturnValue);
		Initializer += FString::Printf(TEXT("*(%s*)(tostruct(L,"), *typeName);
		return FString::Printf(TEXT("%s %d))"), *Initializer, PropertyIndex);

	}
	else if (Property->IsA(UByteProperty::StaticClass()) || Property->IsA(UEnumProperty::StaticClass()))
	{
		FString typeName = GetPropertyTypeCPP(Property, CPPF_ArgumentOrReturnValue);
		Initializer += FString::Printf(TEXT("(%s)((int)%slua_tointeger"), *typeName, *CircularModulesLuaPrefix);
	}
	else if (Property->IsA(UMulticastDelegateProperty::StaticClass()))
	{
		FString typeName = GetPropertyTypeCPP(Property, CPPF_ArgumentOrReturnValue);
		Initializer += FString::Printf(TEXT("(%s)(%slua_tointeger"), *typeName, *CircularModulesLuaPrefix);
	}
	else if (auto p = Cast<UMapProperty>(Property))
	{
		return FString::Printf(TEXT("\tUTableUtil::read(Obj->%s, L, 2)"), *Property->GetName());
	}
	else if (auto p = Cast<USetProperty>(Property))
	{
		return FString::Printf(TEXT("\tUTableUtil::read(Obj->%s, L, 2)"), *Property->GetName());
	}
	else
	{
		Initializer += TEXT("(" + CircularModulesLuaPrefix + "lua_tointeger");
	}
	if (Property->IsA(UTextProperty::StaticClass()))
		return FString::Printf(TEXT("%s(L, %d)))"), *Initializer, PropertyIndex);
	else
		return FString::Printf(TEXT("%s(L, %d))"), *Initializer, PropertyIndex);
}

FString FLuaScriptCodeGenerator::SetterCode(FString ClassNameCPP, FString classname, FString FuncName, UProperty* Property, UClass* Class, UClass* PropertySuper)
{
	FString GeneratedGlue = GenerateWrapperFunctionDeclaration(ClassNameCPP, classname, FuncName);
	GeneratedGlue += TEXT("\r\n{\r\n");
	FString FunctionBody;

	bool bIsGenerateGlue = true;
	if (!CanCopyProperty(Property))
		bIsGenerateGlue = false;

	if (bIsGenerateGlue)
	{
		if (Property->PropertyFlags & CPF_EditorOnly)
			GeneratedGlue += TEXT("#if WITH_EDITORONLY_DATA\r\n");
		if (PropertySuper == NULL)
		{
			GeneratedGlue += FString::Printf(TEXT("\t%s\r\n"), *GenerateObjectDeclarationFromContext(ClassNameCPP));
			if (!PrivatePropertyStruct.Contains(ClassNameCPP) &&
				Property->PropertyFlags & CPF_NativeAccessSpecifierPublic)
			{
				FString Body = SetterBody(Property);
				if (!Body.IsEmpty())
					GeneratedGlue += Body + ";\r\n";
			}
			else
			{
				FString statictype = GetPropertyType(Property);
				if (ClassNameCPP[0] != 'F')
				{
					GeneratedGlue += FString::Printf(TEXT("\tstatic %s* p = (%s*)%s->FindPropertyByName(\"%s\");\r\n"), *statictype, *statictype, *GetUClassGlue(Class), *Property->GetName());
				}
				else
				{
					if (!PrivatePropertyStruct.Contains(ClassNameCPP))
						GeneratedGlue += FString::Printf(TEXT("\tstatic %s* p = (%s*)%s::StaticStruct()->FindPropertyByName(\"%s\");\r\n"), *statictype, *statictype, *ClassNameCPP, *Property->GetName());
					else
					{
						FString GetStructClassStr = "FindObject<UScriptStruct>(ANY_PACKAGE, TEXT(\"" + ClassNameCPP.RightChop(1) + "\"))";
						GeneratedGlue += FString::Printf(TEXT("\tstatic %s* p = (%s*)%s->FindPropertyByName(\"%s\");\r\n"), *statictype, *statictype, *GetStructClassStr, *Property->GetName());
					}
				}
				GeneratedGlue += L"\tUTableUtil::popproperty_type(L, 2, p, Obj);\r\n";
			}
		}
		else
		{
			FunctionBody = FString::Printf(TEXT("\treturn %s_%s(L);\r\n"), *GetClassNameCPP(PropertySuper), *FuncName);
		}

		if (Property->PropertyFlags & CPF_EditorOnly)
			GeneratedGlue += TEXT("#endif\r\n");
	}
	else
	{
		GeneratedGlue += "\tensureAlwaysMsgf(0, L\"error\");\r\n";
	}
	GeneratedGlue += "\treturn 0;\r\n";
	GeneratedGlue += TEXT("}\r\n\r\n");
	return GeneratedGlue;
}
FString FLuaScriptCodeGenerator::ExportProperty(const FString& ClassNameCPP, UClass* Class, UProperty* Property, int32 PropertyIndex)
{
	FString PropertyName = Property->GetName();
	UProperty* ReturnValue = NULL;
	UClass* PropertySuper = NULL;
	FString GeneratedGlue;
	if (Property->GetOwnerClass() != Class)
	{
		// Find the base class where this property was defined
		if (ExportedClasses.Contains(Property->GetOwnerClass()->GetFName()))
		{
			PropertySuper = Property->GetOwnerClass();
		}
	}

	// Getter	
	FString GetterName = FString::Printf(TEXT("%s_%s"), *GetterPrefixStr, *PropertyName);
	// Store the name of this getter as well as the name of the wrapper function
	FPropertyAccessor Getter;
	Getter.AccessorName = GetterName;
	if (PropertySuper)
	{
		Getter.FunctionName = FString::Printf(TEXT("%s_%s"), *GetClassNameCPP(PropertySuper), *GetterName);
	}
	else
	{
		if (bExportSbCompletion())
			AddPropertyToCompletions(Property);
		GeneratedGlue += GetterCode(ClassNameCPP, Class->GetName(), GetterName, Property, Class, PropertySuper);
		Getter.FunctionName = FString::Printf(TEXT("%s_%s"), *ClassNameCPP, *GetterName);
	}
	auto& Exports = ClassExportedProperties.FindOrAdd(Class);
	Exports.Add(Getter);

	//Setter
	FString SetterName = FString::Printf(TEXT("%s_%s"), *SetterPrefixStr, *PropertyName);
	// Store the name of this setter as well as the name of the wrapper function
	FPropertyAccessor Setter;
	Setter.AccessorName = SetterName;
	if (PropertySuper)
	{
		Setter.FunctionName = FString::Printf(TEXT("%s_%s"), *GetClassNameCPP(PropertySuper), *SetterName);
	}
	else
	{
		GeneratedGlue += SetterCode(ClassNameCPP, Class->GetName(), SetterName, Property, Class, PropertySuper);
		Setter.FunctionName = FString::Printf(TEXT("%s_%s"), *ClassNameCPP, *SetterName);
	}
	Exports.Add(Setter);
	return GeneratedGlue;
}

FString FLuaScriptCodeGenerator::ExportAdditionalClassGlue(const FString& ClassNameCPP, UClass* Class)
{
	FString GeneratedGlue;

	const FString ClassName = Class->GetName();
	if (ClassName == "UUserWidget")
	{
		GeneratedGlue += "Fuck";
		if (IsApiClass(Class))
			GeneratedGlue += " you you";

	}
	if (IsApiClass(Class))
	{
		// Constructor and destructor
		if (!(Class->ClassFlags & CLASS_Interface))
		{
			GeneratedGlue += GenerateWrapperFunctionDeclaration(ClassNameCPP, Class->GetName(), TEXT("New"));
			GeneratedGlue += TEXT("\r\n{\r\n");
			GeneratedGlue += TEXT("\tUObject* Obj = nullptr;\r\n");
			GeneratedGlue += TEXT("\tint32 len = ue_lua_gettop(L);\r\n");

			GeneratedGlue += TEXT("\tif(len == 0)\r\n");
			GeneratedGlue += FString::Printf(TEXT("\t\tObj = NewObject<%s>();\r\n"), *ClassNameCPP);
			GeneratedGlue += TEXT("\telse if(len == 1){\r\n");
			GeneratedGlue += TEXT("\t\tUObject* Outer = (UObject*)touobject(L, 1);\r\n");
			GeneratedGlue += FString::Printf(TEXT("\t\tObj = NewObject<%s>(Outer);\r\n\t}\r\n"), *ClassNameCPP);
			GeneratedGlue += TEXT("\telse if(len == 2){\r\n");
			GeneratedGlue += TEXT("\t\tUObject* Outer = (UObject*)touobject(L, 1);\r\n");
			GeneratedGlue += TEXT("\t\tUClass* Class = (UClass*)touobject(L, 2);\r\n");
			GeneratedGlue += FString::Printf(TEXT("\t\tObj = NewObject<%s>(Outer, Class);\r\n\t}\r\n"), *ClassNameCPP);
			GeneratedGlue += TEXT("\telse if(len == 3){\r\n");
			GeneratedGlue += TEXT("\t\tUObject* Outer = (UObject*)touobject(L, 1);\r\n");
			GeneratedGlue += TEXT("\t\tUClass* Class = (UClass*)touobject(L, 2);\r\n");
			GeneratedGlue += TEXT("\t\tFName Name = FName(ue_lua_tostring(L, 3));\r\n");
			GeneratedGlue += FString::Printf(TEXT("\t\tObj = NewObject<%s>(Outer, Class, Name);\r\n\t}\r\n"), *ClassNameCPP);
			GeneratedGlue += FString::Printf(TEXT("\tpushuobject(L, (void*)Obj, true);\r\n"));
			GeneratedGlue += TEXT("\treturn 1;\r\n");
			GeneratedGlue += TEXT("}\r\n\r\n");

			GeneratedGlue += GenerateWrapperFunctionDeclaration(ClassNameCPP, Class->GetName(), TEXT("Destroy"));
			GeneratedGlue += TEXT("\r\n{\r\n");
			GeneratedGlue += FString::Printf(TEXT("\t%s\r\n"), *GenerateObjectDeclarationFromContext(ClassNameCPP));
			GeneratedGlue += TEXT("\tif (Obj)\r\n\t{\r\n");
			GeneratedGlue += TEXT("\t\t\tUTableUtil::rmgcref(Obj);\r\n");
			GeneratedGlue += TEXT("\t}\r\n\treturn 0;\r\n");
			GeneratedGlue += TEXT("}\r\n\r\n");
		}
		if (!(Class->GetClassFlags() & CLASS_Abstract))
		{
			GeneratedGlue += GenerateWrapperFunctionDeclaration(ClassNameCPP, Class->GetName(), TEXT("CreateDefaultSubobject"));
			GeneratedGlue += TEXT("\r\n{\r\n");
			GeneratedGlue += TEXT("\tUObject* Outer = (UObject*)touobject(L, 1);\r\n");
			GeneratedGlue += TEXT("\tFName Name = FName(ue_lua_tostring(L, 2));\r\n");
			GeneratedGlue += FString::Printf(TEXT("\tUObject* Obj = Outer->CreateDefaultSubobject<%s>(Name);\r\n"), *ClassNameCPP);
			GeneratedGlue += TEXT("\tif (Obj)\r\n\t{\r\n");
			GeneratedGlue += TEXT("\t}\r\n");
			GeneratedGlue += FString::Printf(TEXT("\tpushuobject(L, (void*)Obj);\r\n"));
			GeneratedGlue += TEXT("\treturn 1;\r\n");
			GeneratedGlue += TEXT("}\r\n\r\n");
		}


		if (!(Class->ClassFlags & CLASS_Interface))
		{
			GeneratedGlue += GenerateWrapperFunctionDeclaration(ClassNameCPP, Class->GetName(), TEXT("FObjectFinder"));
			GeneratedGlue += TEXT("\r\n{\r\n");
			// 		GeneratedGlue += FString::Printf(TEXT("\tvoid* Obj = (void*)UTableUtil::FObjectFinder(%s::StaticClass(), luaL_checkstring(L, 1));\r\n"), *ClassNameCPP);
			GeneratedGlue += FString::Printf(TEXT("\tConstructorHelpers::FObjectFinder<%s> Obj(UTF8_TO_TCHAR(ue_lua_tostring(L, 1)));\r\n"), *ClassNameCPP);
			GeneratedGlue += FString::Printf(TEXT("\tpushuobject(L, (void*)(Obj.Object), true);\r\n"));
			GeneratedGlue += TEXT("\treturn 1;\r\n");
			GeneratedGlue += TEXT("}\r\n\r\n");

			GeneratedGlue += GenerateWrapperFunctionDeclaration(ClassNameCPP, Class->GetName(), TEXT("FClassFinder"));
			GeneratedGlue += TEXT("\r\n{\r\n");
			GeneratedGlue += FString::Printf(TEXT("\tConstructorHelpers::FClassFinder<%s> Obj(UTF8_TO_TCHAR(ue_lua_tostring(L, 1)));\r\n"), *ClassNameCPP);
			GeneratedGlue += FString::Printf(TEXT("\tpushuobject(L, Obj.Class);\r\n"));
			GeneratedGlue += TEXT("\treturn 1;\r\n");
			GeneratedGlue += TEXT("}\r\n\r\n");

			GeneratedGlue += GenerateWrapperFunctionDeclaration(ClassNameCPP, Class->GetName(), TEXT("LoadClass"));
			GeneratedGlue += TEXT("\r\n{\r\n");
			GeneratedGlue += TEXT("\tUObject* obj = (UObject*)touobject(L, 1);\r\n");
			GeneratedGlue += FString::Printf(TEXT("\tUClass* Class = LoadClass<%s>(obj, UTF8_TO_TCHAR(ue_lua_tostring(L, 2)));\r\n"), *ClassNameCPP);
			GeneratedGlue += TEXT("\tpushuobject(L, (void*)Class);\r\n");
			GeneratedGlue += TEXT("\treturn 1;\r\n");
			GeneratedGlue += TEXT("}\r\n\r\n");

			GeneratedGlue += GenerateWrapperFunctionDeclaration(ClassNameCPP, Class->GetName(), TEXT("LoadObject"));
			GeneratedGlue += TEXT("\r\n{\r\n");
			GeneratedGlue += TEXT("\tUObject* obj = (UObject*)touobject(L, 1);\r\n");
			GeneratedGlue += FString::Printf(TEXT("\t%s* result = LoadObject<%s>(obj, UTF8_TO_TCHAR(ue_lua_tostring(L, 2)));\r\n"), *ClassNameCPP, *ClassNameCPP);
			GeneratedGlue += FString::Printf(TEXT("\tpushuobject(L, (void*)result);\r\n"));
			GeneratedGlue += TEXT("\treturn 1;\r\n");
			GeneratedGlue += TEXT("}\r\n\r\n");
		}

		GeneratedGlue += GenerateWrapperFunctionDeclaration(ClassNameCPP, Class->GetName(), TEXT("Cast"));
		GeneratedGlue += TEXT("\r\n{\r\n");
		GeneratedGlue += TEXT("\tUObject* from = (UObject*)touobject(L, 1);\r\n");
		GeneratedGlue += FString::Printf(TEXT("\t%s* to = Cast<%s>(from);\r\n"), *ClassNameCPP, *ClassNameCPP);
		GeneratedGlue += FString::Printf(TEXT("\tpushuobject(L, (void*)to);\r\n"));
		GeneratedGlue += TEXT("\treturn 1;\r\n");
		GeneratedGlue += TEXT("}\r\n\r\n");
		//Library
	}

	if (!(Class->ClassFlags & CLASS_Interface))
	{
		GeneratedGlue += GenerateWrapperFunctionDeclaration(ClassNameCPP, Class->GetName(), TEXT("Lua_Class"));
		GeneratedGlue += TEXT("\r\n{\r\n");
		GeneratedGlue += FString::Printf(TEXT("\tUClass* Class = %s;\r\n"), *GetUClassGlue(Class));
		GeneratedGlue += TEXT("\tpushuobject(L, (void*)Class);\r\n");
		GeneratedGlue += TEXT("\treturn 1;\r\n");
		GeneratedGlue += TEXT("}\r\n\r\n");
	}

	GeneratedGlue += FString::Printf(TEXT("static const luaL_Reg %s_Lib[] =\r\n{\r\n"), *ClassName);

	if (IsApiClass(Class))
	{
		if (!(Class->GetClassFlags() & CLASS_Abstract))
		{
			GeneratedGlue += FString::Printf(TEXT("\t{ \"CreateDefaultSubobject\", %s_CreateDefaultSubobject },\r\n"), *ClassNameCPP);
		}

		if (!(Class->ClassFlags & CLASS_Interface))
		{
			GeneratedGlue += FString::Printf(TEXT("\t{ \"New\", %s_New },\r\n"), *ClassNameCPP);
			GeneratedGlue += FString::Printf(TEXT("\t{ \"NewObject\", %s_New },\r\n"), *ClassNameCPP);
			GeneratedGlue += FString::Printf(TEXT("\t{ \"Destroy\", %s_Destroy },\r\n"), *ClassNameCPP);
			GeneratedGlue += FString::Printf(TEXT("\t{ \"FObjectFinder\", %s_FObjectFinder },\r\n"), *ClassNameCPP);
			GeneratedGlue += FString::Printf(TEXT("\t{ \"FClassFinder\", %s_FClassFinder },\r\n"), *ClassNameCPP);
			GeneratedGlue += FString::Printf(TEXT("\t{ \"LoadClass\", %s_LoadClass },\r\n"), *ClassNameCPP);
			GeneratedGlue += FString::Printf(TEXT("\t{ \"LoadObject\", %s_LoadObject },\r\n"), *ClassNameCPP);
			
		}
		GeneratedGlue += FString::Printf(TEXT("\t{ \"Cast\", %s_Cast },\r\n"), *ClassNameCPP);
	}

	if (!(Class->ClassFlags & CLASS_Interface))
	{
		GeneratedGlue += FString::Printf(TEXT("\t{ \"Class\", %s_Lua_Class },\r\n"), *ClassNameCPP);
	}
	auto FunctionExports = ClassExportedFunctions.Find(Class);
	if (FunctionExports)
	{
		for (auto& FunctionName : *FunctionExports)
		{
			if (auto SuperClassNameCpp = FuncSuperClassName.Find(FunctionName))
				GeneratedGlue += FString::Printf(TEXT("\t{ \"%s\", %s_%s },\r\n"), *FunctionName.ToString(), **SuperClassNameCpp, *FunctionName.ToString());
			else
				GeneratedGlue += FString::Printf(TEXT("\t{ \"%s\", %s_%s },\r\n"), *FunctionName.ToString(), *ClassNameCPP, *FunctionName.ToString());
		}
	}
	auto PropertyExports = ClassExportedProperties.Find(Class);
	if (PropertyExports)
	{
		for (auto& Accessor : *PropertyExports)
		{
			GeneratedGlue += FString::Printf(TEXT("\t{ \"%s\", %s },\r\n"), *Accessor.AccessorName, *Accessor.FunctionName);
		}
	}
	GeneratedGlue += TEXT("\t{ NULL, NULL }\r\n};\r\n\r\n");

	return GeneratedGlue;
}

bool FLuaScriptCodeGenerator::isStructSupported(UScriptStruct* thestruct) const
{
	FString ModuleName = FModulePath::Get().GetClassModuleName(thestruct);

	if (!SupportModules.Contains(ModuleName))
		return false;

	if (SupportedStruct.Contains(GetStructName(thestruct)))
		return true;

	if (thestruct->StructFlags & STRUCT_NoExport)
		return false;

	if (NoSupportedStruct.Contains(GetStructName(thestruct)))
		return false;

	return true;
}

void FLuaScriptCodeGenerator::ExportStruct()
{
	TArray<FString> AllStructFileArr;
	for (TObjectIterator<UScriptStruct> It; It; ++It)
	{
		FString name = *It->GetName();
		if (!isStructSupported(*It))
			continue;
		FString ModuleName = FModulePath::Get().GetClassModuleName(*It);
		IsExportingCircularModule = CircularModules.Contains(ModuleName);
		TArray<UScriptStruct*>& StructSet = ModuleStruct.FindOrAdd(ModuleName);
		StructSet.Add(*It);
		// 		auto x = name == "AIRequestID";
		FString namecpp = "F" + name;
		if (bExportSbCompletion())
			SublimeCompletions.Add(namecpp);
		const FString ClassGlueFilename = GeneratedCodePath / (name + TEXT(".lua.h"));
		AllStructFileArr.Add(name + TEXT(".lua.h"));

		FString GeneratedGlue(TEXT("#pragma once\r\n\r\n"));
		ExtraIncludeHeader.Reset();
		FString HeaderSourceFile = FModulePath::Get().GetClassHeaderPath(*It);
		if (!HeaderSourceFile.IsEmpty())
			ExtraIncludeHeader.Add(HeaderSourceFile);
		ExtraIncludeHeader.Add("ExtraHeaders_ustruct.h");
		int32 PropertyIndex = 0;
		TArray<FString> allPropertyName;
		FString PropertyGlue;
// 		if (!PrivatePropertyStruct.Contains(namecpp))
// 		{
			for (TFieldIterator<UProperty> PropertyIt(*It/*, EFieldIteratorFlags::ExcludeSuper*/); PropertyIt; ++PropertyIt, ++PropertyIndex)
			{
				UProperty* Property = *PropertyIt;
				if (CanExportProperty(namecpp, nullptr, Property))
				{
					allPropertyName.Add(Property->GetName());
					FString GetterName = FString::Printf(TEXT("%s_%s"), *GetterPrefixStr, *Property->GetName());
					PropertyGlue += GetterCode(namecpp, namecpp, GetterName, Property);
					FString SetterName = FString::Printf(TEXT("%s_%s"), *SetterPrefixStr, *Property->GetName());
					PropertyGlue += SetterCode(namecpp, namecpp, SetterName, Property);
				}
			}
// 		}
		FString addition;
		bool bIsNoNewStruct = NoNewStruct.Contains(namecpp);
		if (!bIsNoNewStruct)
		{
			addition += FString::Printf(TEXT("static int32 %s_New(lua_State* L)\r\n{\r\n"), *namecpp);
			addition += FString::Printf(TEXT("\t%s* Obj = new %s;\r\n"), *namecpp, *namecpp);
			addition += FString::Printf(TEXT("\tpushstruct_gc(L,\"%s\", (void*)Obj);\r\n"), *namecpp);
			addition += FString::Printf(TEXT("\treturn 1;\r\n}\r\n\r\n"));

			addition += FString::Printf(TEXT("static int32 %s_Destroy(lua_State* L)\r\n{\r\n"), *namecpp);
			addition += FString::Printf(TEXT("\t%s* Obj = (%s*)tostruct(L,1);\r\n"), *namecpp, *namecpp);
			addition += FString::Printf(TEXT("\tdelete Obj;\r\n"));
			addition += TEXT("\treturn 0;\r\n}\r\n\r\n");
		}
		bool bExportCopyFunction = !NoCopyStruct.Contains(namecpp);
		if (bExportCopyFunction)
		{
			addition += FString::Printf(TEXT("static int32 %s_Temp(lua_State* L)\r\n{\r\n"), *namecpp);
			addition += FString::Printf(TEXT("\t%s& Ins = UTableUtil::GetTempInsInit<%s>();\r\n"), *namecpp, *namecpp);
			addition += FString::Printf(TEXT("\tpushstruct_nogc(L, \"%s\", (void*)(&Ins));\r\n"), *namecpp);
			addition += TEXT("\treturn 1;\r\n}\r\n\r\n");

			addition += FString::Printf(TEXT("static int32 %s_Copy(lua_State* L)\r\n{\r\n"), *namecpp);
			addition += FString::Printf(TEXT("\t%s* Obj = (%s*)tostruct(L,1);\r\n"), *namecpp, *namecpp);
			addition += FString::Printf(TEXT("\tpushstruct_gc(L,\"%s\", (void*)(new %s(*Obj)));\r\n"), *namecpp, *namecpp);
			addition += TEXT("\treturn 1;\r\n}\r\n\r\n");
		}
		for (FString& FilePath : ExtraIncludeHeader)
		{
			GeneratedGlue += "#include \"" + FilePath + "\"\r\n";
		}
		GeneratedGlue += PropertyGlue;
		GeneratedGlue += addition;

		GeneratedGlue += FString::Printf(TEXT("static const luaL_Reg %s_Lib[] =\r\n{\r\n"), *namecpp);
		if (!bIsNoNewStruct)
		{
			GeneratedGlue += FString::Printf(TEXT("\t{ \"New\", %s_New },\r\n"), *namecpp);
			GeneratedGlue += FString::Printf(TEXT("\t{ \"Destroy\", %s_Destroy },\r\n"), *namecpp);
		}
		if (bExportCopyFunction)
		{
			GeneratedGlue += FString::Printf(TEXT("\t{ \"Temp\", %s_Temp},\r\n"), *namecpp);
			GeneratedGlue += FString::Printf(TEXT("\t{ \"Copy\", %s_Copy },\r\n"), *namecpp);
		}


		for (auto& PropertyName : allPropertyName)
		{
			GeneratedGlue += FString::Printf(TEXT("\t{ \"%s_%s\", %s_%s_%s },\r\n"), *GetterPrefixStr, *PropertyName, *namecpp, *GetterPrefixStr, *PropertyName);
			GeneratedGlue += FString::Printf(TEXT("\t{ \"%s_%s\", %s_%s_%s },\r\n"), *SetterPrefixStr, *PropertyName, *namecpp, *SetterPrefixStr, *PropertyName);
		}
		GeneratedGlue += TEXT("\t{ NULL, NULL }\r\n};\r\n");

		GeneratedGlue += "template<>\r\nclass traitstructclass<" + namecpp + ">{\r\n";
		GeneratedGlue += "public:\r\ninline static const char* name(){ return \"" + namecpp + "\";}\r\n";
		GeneratedGlue += "using NotStructType = NeedTempInsType;\r\n";
		GeneratedGlue += "using value = " + namecpp + ";\r\n};\r\n";

		SaveHeaderIfChanged(ClassGlueFilename, GeneratedGlue);
	}
	FString IncludeAllStructGlue = "#pragma once\r\n";
	for (FString &FilePath : AllStructFileArr)
	{
		IncludeAllStructGlue += FString::Printf(L"#include \"%s\"\r\n", *FilePath);
	}
	const FString IncludeAllStructFile = GeneratedCodePath / TEXT("all_struct.lua.h");
	SaveHeaderIfChanged(IncludeAllStructFile, IncludeAllStructGlue);
}

void FLuaScriptCodeGenerator::ExportEnum()
{
	const FString ClassGlueFilename = GeneratedCodePath / TEXT("allEnum.lua.h");
	FString GeneratedGlue;
	GeneratedGlue += TEXT("#pragma once\n");
	// 	GeneratedGlue += TEXT("\tconst char* key;\r\n");
	// 	GeneratedGlue += TEXT("\tconst int32 value;\r\n};\r\n");
	for (TObjectIterator<UEnum> It; It; ++It)
	{
		FString ModuleName = FModulePath::Get().GetClassModuleName(*It);
		if (!SupportModules.Contains(ModuleName))
			continue;
		UEnum* e = *It;
		FString name = e->GetName();
		EnumtNames.Add(name);
		GeneratedGlue += FString::Printf(TEXT("static const EnumItem %s_Enum[] =\r\n{\r\n"), *name);
		
		for (int32 i = 0; i <= e->GetMaxEnumValue(); ++i)
		{
			FString ValueName = e->GetNameStringByIndex(i);
			if (ValueName.IsEmpty())
				continue;
			GeneratedGlue += FString::Printf(TEXT("\t{ \"%s\", %d },\r\n"), *ValueName, i);
			if (bExportSbCompletion())
				SublimeCompletions.Add(name + "." + ValueName+"--[[ZZ]]");
		}
		GeneratedGlue += TEXT("\t{ NULL, -1 }\r\n};\r\n\r\n");
	}
	AllScriptHeaders.Add(ClassGlueFilename);
	SaveHeaderIfChanged(ClassGlueFilename, GeneratedGlue);
}

void FLuaScriptCodeGenerator::ExportClass(UClass* Class, const FString& SourceHeaderFilename, const FString& GeneratedHeaderFilename, bool bHasChanged)
{
	if (!CanExportClass(Class) || HasExportedClass(Class))
	{
		return;
	}
	FString ClassModuleName = FModulePath::Get().GetClassModuleName(Class);
	IsExportingCircularModule = CircularModules.Contains(ClassModuleName);
	FuncSuperClassName.Reset();

	ExportedClasses.Add(Class->GetFName());
	const FString ClassGlueFilename = GetScriptHeaderForClass(Class);
	Class2ScriptName.Add(Class, ClassGlueFilename);

	ExportingClassSourcefile = SourceHeaderFilename;

	const FString ClassNameCPP = GetClassNameCPP(Class);
	if(bExportSbCompletion())
		SublimeCompletions.Add(ClassNameCPP);
	FString GeneratedGlue(TEXT("#pragma once\r\n\r\n"));
	ExtraIncludeHeader.Reset();
	FString FixPath = SourceHeaderFilename;
	FPaths::MakePlatformFilename(FixPath);
	ExtraIncludeHeader.Add(FixPath);
	ExtraIncludeHeader.Add("ExtraHeaders_uclass.h");
	if (UClass* Super = Class->GetSuperClass())
	{
		do
		{
			if (CanExportClass(Super))
			{
				GeneratedGlue += "#include \"" + GetScriptHeaderForClass(Super) + "\"\r\n";
				break;
			}
			Super = Super->GetSuperClass();
		} while (Super);
	}

	FString GeneratedGlueBody = "PRAGMA_DISABLE_DEPRECATION_WARNINGS\r\n";
	for (TFieldIterator<UFunction> FuncIt(Class /*, EFieldIteratorFlags::ExcludeSuper*/); FuncIt; ++FuncIt)
	{
		UFunction* Function = *FuncIt;
		if (CanExportFunction(ClassNameCPP, Class, Function))
		{
			GeneratedGlueBody += ExportFunction(ClassNameCPP, Class, Function);
		}
	}

	GeneratedGlueBody += ExportInterfaceFunc(Class);

	// Export properties that are owned by this class
	int32 PropertyIndex = 0;
	for (TFieldIterator<UProperty> PropertyIt(Class /*, EFieldIteratorFlags::ExcludeSuper*/); PropertyIt; ++PropertyIt, ++PropertyIndex)
	{
		UProperty* Property = *PropertyIt;
		if (CanExportProperty(ClassNameCPP, Class, Property))
		{
			GeneratedGlueBody += ExportProperty(ClassNameCPP, Class, Property, PropertyIndex);
		}
	}

	for (FString& HeaderToInclude : ExtraIncludeHeader)
	{
		GeneratedGlue += "#include \"" + HeaderToInclude + "\"\r\n";
	}
	GeneratedGlue += GeneratedGlueBody;

	GeneratedGlue += ExportAdditionalClassGlue(ClassNameCPP, Class);
	GeneratedGlue += "PRAGMA_ENABLE_DEPRECATION_WARNINGS\r\n";
	Class2Sourcefile.Add(Class, SourceHeaderFilename);
	TArray<UClass*>& ClassSet = ModulesClass.FindOrAdd(ClassModuleName);
	ClassSet.Add(Class);
	SaveHeaderIfChanged(ClassGlueFilename, GeneratedGlue);
}

void FLuaScriptCodeGenerator::FinishExport()
{
	ExportEnum();
	ExportStruct();
	GlueAllGeneratedFiles();
	GlueAllGeneratedFiles_CircularModule();
	GenerateSBCompletion();
	RenameTempFiles();
}

void FLuaScriptCodeGenerator::AddFunctionToCompletions(UFunction* Function)
{
	FString Snip = Function->GetName()+"(";
	bool HasParam = false;
	UProperty* ReturnValue = nullptr;
	for (TFieldIterator<UProperty> ParamIt(Function); ParamIt; ++ParamIt)
	{
		UProperty* Param = *ParamIt;
		if (Param->GetPropertyFlags() & CPF_ReturnParm)
		{
			ReturnValue = Param;
			continue;
		}
		HasParam = true;
		FString TypeName = GetPropertyTypeCPP(Param, CPPF_ArgumentOrReturnValue);
		Snip += "--[[" + TypeName;
		if ((Param->GetPropertyFlags() & (CPF_ConstParm | CPF_OutParm | CPF_ReturnParm)) == CPF_OutParm)
		{
			Snip += "&";
		}
		Snip += "]]" + Param->GetName() + ",";
	}
	if (HasParam)
	{
		Snip = Snip.LeftChop(1);
	}
	Snip += ")";
	if (ReturnValue)
	{
		FString TypeName = GetPropertyTypeCPP(ReturnValue, CPPF_ArgumentOrReturnValue);
		Snip += "--[[" + TypeName + " ZZ]]";
	}
	else
	{
		Snip += "--[[ZZ]]";
	}
	SublimeCompletions.Add(Snip);
}

void FLuaScriptCodeGenerator::AddPropertyToCompletions(UProperty* Property)
{
	FString TypeName = GetPropertyTypeCPP(Property, CPPF_ArgumentOrReturnValue);
	FString Snip = Property->GetName()+"--[["+TypeName+" ZZ]]";
	SublimeCompletions.Add(Snip);
}

void FLuaScriptCodeGenerator::GenerateSBCompletion()
{
	if (bExportSbCompletion())
	{
		FString All = "{\"completions\":[\r\n";
		for (FString& str : SublimeCompletions)
		{
			All += "\"" + str + "\",\r\n";
		}
		All += "]}";
		FString CppFileName = SublimeCompletionDir / "ue4lua.sublime-completions";
		SaveHeaderIfChanged(CppFileName, All);
	}
}

void FLuaScriptCodeGenerator::GlueAllGeneratedFiles()
{
	FString OutputDir = FModulePath::Get().GetModulePathByName("LuaPluginRuntime");
	TArray<FString> AllLuaGenCpp;
	IFileManager::Get().FindFilesRecursive(AllLuaGenCpp, *(OutputDir / LuaGeneratedCodeDir), TEXT("*.cpp"), true, false, false);

	TMap<FString, FString> FileName2FilePath;
	for (FString& Path : AllLuaGenCpp)
	{
		FString FileName = FPaths::GetCleanFilename(Path);
		FileName2FilePath.Add(FileName, Path);
	}

	FString CommonInclude;

	CommonInclude += "#include \"tableutil.h\"\r\n";
	TArray<FString> FixLinkFuncs;
	for (const auto& ModuleClassSet : ModulesClass)
	{
		if (!CircularModules.Contains(ModuleClassSet.Key))
		{
			if (ChangeableModules.Contains(ModuleClassSet.Key))
			{
				for (auto Class : ModuleClassSet.Value)
				{
					FString CppFileName = OutputDir / LuaGeneratedCodeDir / ModuleClassSet.Key / Class->GetName() + "_lua.cpp";

					FString CppGlue = CommonInclude;
					CppGlue += FString::Printf(TEXT("#include \"%s.lua.h\"\r\n"), *Class->GetName());
					CppGlue += FString::Printf(TEXT("struct lua_static_load_%s_struct\r\n{\r\n"), *Class->GetName());
					CppGlue += FString::Printf(TEXT("\tstatic void load()\r\n\t{\r\n"));
					CppGlue += FString::Printf(TEXT("\t\tUTableUtil::loadlib(%s_Lib, \"%s\");\r\n\t}\r\n"), *Class->GetName(), *GetClassNameCPP(Class));
					CppGlue += FString::Printf(TEXT("\tlua_static_load_%s_struct(){UTableUtil::GetInitDelegates().AddStatic(&load);}\r\n};\r\n"), *Class->GetName());
					CppGlue += FString::Printf(TEXT("static lua_static_load_%s_struct lua_%s_static_var;\r\n"), *Class->GetName(), *Class->GetName());
					FString FixFuncName = FString::Printf(TEXT("FixLinkFunc_%s_%s"), *ModuleClassSet.Key, *Class->GetName());
					CppGlue += FString::Printf(TEXT("void %s(){};\r\n"), *FixFuncName);
					FixLinkFuncs.AddUnique(FixFuncName);
					AllLuaGenCpp.Remove(CppFileName);
					SaveHeaderIfChanged(CppFileName, CppGlue);
				}
			}
			else
			{
				FString StructNamePrefix = ModuleClassSet.Key + "_uclass_all";
				FString CppFileName = OutputDir / LuaGeneratedCodeDir / ModuleClassSet.Key / StructNamePrefix + "_lua.cpp";
				FString IncludeStr = CommonInclude;
				FString LoadStr;
				for (auto Class : ModuleClassSet.Value)
				{
					IncludeStr += FString::Printf(TEXT("#include \"%s.lua.h\"\r\n"), *Class->GetName());
					LoadStr += FString::Printf(TEXT("\t\tUTableUtil::loadlib(%s_Lib, \"%s\");\r\n"), *Class->GetName(), *GetClassNameCPP(Class));
				}

				FString CppGlue = IncludeStr;
				CppGlue += FString::Printf(TEXT("struct lua_static_load_%s_struct\r\n{\r\n"), *StructNamePrefix);
				CppGlue += FString::Printf(TEXT("\tstatic void load()\r\n\t{\r\n"));
				CppGlue += LoadStr + "\t}\r\n";
				CppGlue += FString::Printf(TEXT("\tlua_static_load_%s_struct(){UTableUtil::GetInitDelegates().AddStatic(&load);}\r\n};\r\n"), *StructNamePrefix);
				CppGlue += FString::Printf(TEXT("static lua_static_load_%s_struct lua_%s_static_var;\r\n"), *StructNamePrefix, *StructNamePrefix);
				FString FixFuncName = FString::Printf(TEXT("FixLinkFunc_%s_uclass_all"), *ModuleClassSet.Key);
				CppGlue += FString::Printf(TEXT("void %s(){};\r\n"), *FixFuncName);
				FixLinkFuncs.AddUnique(FixFuncName);
				AllLuaGenCpp.Remove(CppFileName);
				SaveHeaderIfChanged(CppFileName, CppGlue);
			}
		}
	}
	for (const auto& ModuleClassSet : ModuleStruct)
	{
		if (!CircularModules.Contains(ModuleClassSet.Key))
		{
			if (ChangeableModules.Contains(ModuleClassSet.Key))
			{
				for (auto Struct : ModuleClassSet.Value)
				{
					FString CppFileName = OutputDir / LuaGeneratedCodeDir / ModuleClassSet.Key / Struct->GetName() + "_lua.cpp";

					FString CppGlue = CommonInclude;
					CppGlue += FString::Printf(TEXT("#include \"%s.lua.h\"\r\n"), *Struct->GetName());
					CppGlue += FString::Printf(TEXT("struct lua_static_load_%s_struct\r\n{\r\n"), *Struct->GetName());
					CppGlue += FString::Printf(TEXT("\tstatic void load()\r\n\t{\r\n"));
					CppGlue += FString::Printf(TEXT("\t\tUTableUtil::loadstruct(%s_Lib, \"%s\");\r\n\t}\r\n"), *GetStructName(Struct), *GetStructName(Struct));
					CppGlue += FString::Printf(TEXT("\tlua_static_load_%s_struct(){UTableUtil::GetInitDelegates().AddStatic(&load);}\r\n};\r\n"), *Struct->GetName());
					CppGlue += FString::Printf(TEXT("static lua_static_load_%s_struct lua_%s_static_var;\r\n"), *Struct->GetName(), *Struct->GetName());
					FString FixFuncName = FString::Printf(TEXT("FixLinkFunc_%s_%s"), *ModuleClassSet.Key, *Struct->GetName());
					CppGlue += FString::Printf(TEXT("void %s(){};\r\n"), *FixFuncName);
					FixLinkFuncs.AddUnique(FixFuncName);
					AllLuaGenCpp.Remove(CppFileName);
					SaveHeaderIfChanged(CppFileName, CppGlue);
				}
			}
			else
			{
				FString StructNamePrefix = ModuleClassSet.Key + "_ustruct_all";
				FString CppFileName = OutputDir / LuaGeneratedCodeDir / ModuleClassSet.Key / StructNamePrefix + "_lua.cpp";
				FString IncludeStr = CommonInclude;
				FString LoadStr;

				for (auto Struct : ModuleClassSet.Value)
				{
					IncludeStr += FString::Printf(TEXT("#include \"%s.lua.h\"\r\n"), *Struct->GetName());
					LoadStr += FString::Printf(TEXT("\t\tUTableUtil::loadstruct(F%s_Lib, \"F%s\");\r\n"), *Struct->GetName(), *Struct->GetName());
				}

				FString CppGlue = IncludeStr;
				CppGlue += FString::Printf(TEXT("struct lua_static_load_%s_struct\r\n{\r\n"), *StructNamePrefix);
				CppGlue += FString::Printf(TEXT("\tstatic void load()\r\n\t{\r\n"));
				CppGlue += LoadStr + "\t}\r\n";
				CppGlue += FString::Printf(TEXT("\tlua_static_load_%s_struct(){UTableUtil::GetInitDelegates().AddStatic(&load);}\r\n};\r\n"), *StructNamePrefix);
				CppGlue += FString::Printf(TEXT("static lua_static_load_%s_struct lua_%s_static_var;\r\n"), *StructNamePrefix, *StructNamePrefix);
				FString FixFuncName = FString::Printf(TEXT("FixLinkFunc_%s_ustruct_all"), *ModuleClassSet.Key);
				CppGlue += FString::Printf(TEXT("void %s(){};\r\n"), *FixFuncName);
				FixLinkFuncs.AddUnique(FixFuncName);
				AllLuaGenCpp.Remove(CppFileName);
				SaveHeaderIfChanged(CppFileName, CppGlue);
			}
		}
	}

	FString EnumCppFileName = OutputDir / LuaGeneratedCodeDir / "allenum_lua.cpp";
	FString EnumGlue = CommonInclude;
	EnumGlue += "#include \"allenum.lua.h\"\r\n";
	EnumGlue += "struct lua_static_load_allenum_struct\r\n{\r\n";
	EnumGlue += FString::Printf(TEXT("\tstatic void load()\r\n\t{\r\n"));
	for (auto Name : EnumtNames)
	{
		EnumGlue += FString::Printf(TEXT("\t\tUTableUtil::loadEnum(%s_Enum, \"%s\");\r\n"), *Name, *Name);
	}
	EnumGlue += "\t}\r\n";
	EnumGlue += "\tlua_static_load_allenum_struct(){UTableUtil::GetInitDelegates().AddStatic(&load);}\r\n};\r\n";
	EnumGlue += "static lua_static_load_allenum_struct lua_allenum_static_var;\r\n";
	FString FixFuncName = FString::Printf(TEXT("FixLinkFunc_uenum_all"));
	EnumGlue += FString::Printf(TEXT("void %s(){};\r\n"), *FixFuncName);
	FixLinkFuncs.AddUnique(FixFuncName);
	AllLuaGenCpp.Remove(EnumCppFileName);
	SaveHeaderIfChanged(EnumCppFileName, EnumGlue);
	//
	FString FixLinkCppFileName = OutputDir / "private" / "LuaFixLink.cpp";
	FString FixLinkCppGlue = "#include \"LuaFixLink.h\"\r\n";
// 	FixLinkCppGlue += "extern void LuaFixLink1();\r\n";
	for (FString& Func : FixLinkFuncs)
	{
		FixLinkCppGlue += "extern void " + Func + "();\r\n";
	}
	FixLinkCppGlue += "void LuaFixLink()\r\n{\r\n";
	for (FString& Func : FixLinkFuncs)
	{
		FixLinkCppGlue += "\t" + Func + "();\r\n";
	}
// 	FixLinkCppGlue += "\tLuaFixLink1();\r\n";
	FixLinkCppGlue += "}";

	SaveHeaderIfChanged(FixLinkCppFileName, FixLinkCppGlue);

	for (FString& Path : AllLuaGenCpp)
	{
		SaveHeaderIfChanged(Path, "//you should delete this file. In order to let UBT run again, you should make tiny change to project's build.cs.");
	}
}
//later refactor, for now ,just this, hehe.
void FLuaScriptCodeGenerator::GlueAllGeneratedFiles_CircularModule()
{
	FString OutputDir = FModulePath::Get().GetModulePathByName("LuaCircularModules");
	TArray<FString> AllLuaGenCpp;
	IFileManager::Get().FindFilesRecursive(AllLuaGenCpp, *(OutputDir / LuaGeneratedCodeDir), TEXT("*.cpp"), true, false, false);

	TMap<FString, FString> FileName2FilePath;
	for (FString& Path : AllLuaGenCpp)
	{
		FString FileName = FPaths::GetCleanFilename(Path);
		FileName2FilePath.Add(FileName, Path);
	}

	FString CommonInclude;
	CommonInclude += "#include \"tableutil.h\"\r\n";
	TArray<FString> FixLinkFuncs;
	for (const auto& ModuleClassSet : ModulesClass)
	{
		if (CircularModules.Contains(ModuleClassSet.Key))
		{
			if (ChangeableModules.Contains(ModuleClassSet.Key))
			{
				for (auto Class : ModuleClassSet.Value)
				{
					FString CppFileName = OutputDir / LuaGeneratedCodeDir / ModuleClassSet.Key / Class->GetName() + "_lua.cpp";

					FString CppGlue = CommonInclude;
					CppGlue += FString::Printf(TEXT("#include \"%s.lua.h\"\r\n"), *Class->GetName());
					CppGlue += FString::Printf(TEXT("struct lua_static_load_%s_struct\r\n{\r\n"), *Class->GetName());
					CppGlue += FString::Printf(TEXT("\tstatic void load()\r\n\t{\r\n"));
					CppGlue += FString::Printf(TEXT("\t\tUTableUtil::loadlib(%s_Lib, \"%s\");\r\n\t}\r\n"), *Class->GetName(), *GetClassNameCPP(Class));
					CppGlue += FString::Printf(TEXT("\tlua_static_load_%s_struct(){UTableUtil::GetInitDelegates().AddStatic(&load);}\r\n};\r\n"), *Class->GetName());
					CppGlue += FString::Printf(TEXT("static lua_static_load_%s_struct lua_%s_static_var;\r\n"), *Class->GetName(), *Class->GetName());
					FString FixFuncName = FString::Printf(TEXT("FixLinkFunc_%s_%s"), *ModuleClassSet.Key, *Class->GetName());
					CppGlue += FString::Printf(TEXT("void %s(){};\r\n"), *FixFuncName);
					FixLinkFuncs.AddUnique(FixFuncName);
					AllLuaGenCpp.Remove(CppFileName);
					SaveHeaderIfChanged(CppFileName, CppGlue);
				}
			}
			else
			{
				FString StructNamePrefix = ModuleClassSet.Key + "_uclass_all";
				FString CppFileName = OutputDir / LuaGeneratedCodeDir / ModuleClassSet.Key / StructNamePrefix + "_lua.cpp";
				FString IncludeStr = CommonInclude;
				FString LoadStr;
				for (auto Class : ModuleClassSet.Value)
				{
					IncludeStr += FString::Printf(TEXT("#include \"%s.lua.h\"\r\n"), *Class->GetName());
					LoadStr += FString::Printf(TEXT("\t\tUTableUtil::loadlib(%s_Lib, \"%s\");\r\n"), *Class->GetName(), *GetClassNameCPP(Class));
				}

				FString CppGlue = IncludeStr;
				CppGlue += FString::Printf(TEXT("struct lua_static_load_%s_struct\r\n{\r\n"), *StructNamePrefix);
				CppGlue += FString::Printf(TEXT("\tstatic void load()\r\n\t{\r\n"));
				CppGlue += LoadStr + "\t}\r\n";
				CppGlue += FString::Printf(TEXT("\tlua_static_load_%s_struct(){UTableUtil::GetInitDelegates().AddStatic(&load);}\r\n};\r\n"), *StructNamePrefix);
				CppGlue += FString::Printf(TEXT("static lua_static_load_%s_struct lua_%s_static_var;\r\n"), *StructNamePrefix, *StructNamePrefix);
				FString FixFuncName = FString::Printf(TEXT("FixLinkFunc_%s_uclass_all"), *ModuleClassSet.Key);
				CppGlue += FString::Printf(TEXT("void %s(){};\r\n"), *FixFuncName);
				FixLinkFuncs.AddUnique(FixFuncName);
				AllLuaGenCpp.Remove(CppFileName);
				SaveHeaderIfChanged(CppFileName, CppGlue);
			}
		}
	}
	for (const auto& ModuleClassSet : ModuleStruct)
	{
		if (CircularModules.Contains(ModuleClassSet.Key))
		{
			if (ChangeableModules.Contains(ModuleClassSet.Key))
			{
				for (auto Struct : ModuleClassSet.Value)
				{
					FString CppFileName = OutputDir / LuaGeneratedCodeDir / ModuleClassSet.Key / Struct->GetName() + "_lua.cpp";

					FString CppGlue = CommonInclude;
					CppGlue += FString::Printf(TEXT("#include \"%s.lua.h\"\r\n"), *Struct->GetName());
					CppGlue += FString::Printf(TEXT("struct lua_static_load_%s_struct\r\n{\r\n"), *Struct->GetName());
					CppGlue += FString::Printf(TEXT("\tstatic void load()\r\n\t{\r\n"));
					CppGlue += FString::Printf(TEXT("\t\tUTableUtil::loadstruct(%s_Lib, \"%s\");\r\n\t}\r\n"), *GetStructName(Struct), *GetStructName(Struct));
					CppGlue += FString::Printf(TEXT("\tlua_static_load_%s_struct(){UTableUtil::GetInitDelegates().AddStatic(&load);}\r\n};\r\n"), *Struct->GetName());
					CppGlue += FString::Printf(TEXT("static lua_static_load_%s_struct lua_%s_static_var;\r\n"), *Struct->GetName(), *Struct->GetName());
					FString FixFuncName = FString::Printf(TEXT("FixLinkFunc_%s_%s"), *ModuleClassSet.Key, *Struct->GetName());
					CppGlue += FString::Printf(TEXT("void %s(){};\r\n"), *FixFuncName);
					FixLinkFuncs.AddUnique(FixFuncName);
					AllLuaGenCpp.Remove(CppFileName);
					SaveHeaderIfChanged(CppFileName, CppGlue);
				}
			}
			else
			{
				FString StructNamePrefix = ModuleClassSet.Key + "_ustruct_all";
				FString CppFileName = OutputDir / LuaGeneratedCodeDir / ModuleClassSet.Key / StructNamePrefix + "_lua.cpp";
				FString IncludeStr = CommonInclude;
				FString LoadStr;

				for (auto Struct : ModuleClassSet.Value)
				{
					IncludeStr += FString::Printf(TEXT("#include \"%s.lua.h\"\r\n"), *Struct->GetName());
					LoadStr += FString::Printf(TEXT("\t\tUTableUtil::loadstruct(F%s_Lib, \"F%s\");\r\n"), *Struct->GetName(), *Struct->GetName());
				}

				FString CppGlue = IncludeStr;
				CppGlue += FString::Printf(TEXT("struct lua_static_load_%s_struct\r\n{\r\n"), *StructNamePrefix);
				CppGlue += FString::Printf(TEXT("\tstatic void load()\r\n\t{\r\n"));
				CppGlue += LoadStr + "\t}\r\n";
				CppGlue += FString::Printf(TEXT("\tlua_static_load_%s_struct(){UTableUtil::GetInitDelegates().AddStatic(&load);}\r\n};\r\n"), *StructNamePrefix);
				CppGlue += FString::Printf(TEXT("static lua_static_load_%s_struct lua_%s_static_var;\r\n"), *StructNamePrefix, *StructNamePrefix);
				FString FixFuncName = FString::Printf(TEXT("FixLinkFunc_%s_ustruct_all"), *ModuleClassSet.Key);
				CppGlue += FString::Printf(TEXT("void %s(){};\r\n"), *FixFuncName);
				FixLinkFuncs.AddUnique(FixFuncName);
				AllLuaGenCpp.Remove(CppFileName);
				SaveHeaderIfChanged(CppFileName, CppGlue);
			}
		}
	}

	FString FixLinkCppFileName = OutputDir / "LuaFixLink1.cpp";
	FString FixLinkCppGlue = "#include \"LuaFixLink1.h\"\r\n";
	for (FString& Func : FixLinkFuncs)
	{
		FixLinkCppGlue += "extern void " + Func + "();\r\n";
	}
	FixLinkCppGlue += "void LuaFixLink1()\r\n{\r\n";
	for (FString& Func : FixLinkFuncs)
	{
		FixLinkCppGlue += "\t" + Func + "();\r\n";
	}
	FixLinkCppGlue += "}";

	SaveHeaderIfChanged(FixLinkCppFileName, FixLinkCppGlue);

	for (FString& Path : AllLuaGenCpp)
	{
		SaveHeaderIfChanged(Path, "//you should delete this file. In order to let UBT run again, you should make tiny change to project's build.cs.");
	}
}
