// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TableUtil.h"
#include "CoreUObject.h"
#include "Runtime/Launch/Resources/Version.h"


class LUAPLUGINRUNTIME_API LuaFunctionInjectHelper
{
public:
	static LuaFunctionInjectHelper* SingletonIns;
	static LuaFunctionInjectHelper* Get();
	static void Destroy();

	LuaFunctionInjectHelper();
	~LuaFunctionInjectHelper();

	void ReplaceUClassFunction(lua_State*inL, UClass* Class, const char* LuaClassPath);
	void ReplaceNativeClassFunction(lua_State*inL, UClass* Class, const char* LuaClassPath);

	void AddFunction(UFunction *TemplateFunction, UClass *OuterClass, FName NewFuncName);
	void ReplaceFunction(UFunction *TemplateFunction, UClass *OuterClass, FName NewFuncName);
	UFunction* DuplicateFunction(UFunction *TemplateFunction, UClass *OuterClass, FName NewFuncName);

	TMap<UFunction*, FNativeFuncPtr> CachedNatives;
	TMap<FName, TMap<FName, UFunction*>> CachedClassNewUFunctions;
	TMap<FName, TMap<FName, UClass*>> HasReplaceClass;
	TSet<UClass*> HasReplaceClass_ForGame;
};
