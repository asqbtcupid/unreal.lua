#include "LuaFunctionInjectHelper.h"
#include "UnrealLuaInterface.h"
#include "UnrealLua.h"

LuaFunctionInjectHelper* LuaFunctionInjectHelper::SingletonIns = nullptr;
LuaFunctionInjectHelper* LuaFunctionInjectHelper::Get()
{
	if (SingletonIns == nullptr)
		SingletonIns = new LuaFunctionInjectHelper;
	return SingletonIns;
}

void LuaFunctionInjectHelper::Destroy()
{
	if (SingletonIns)
	{
		delete SingletonIns;
		SingletonIns = nullptr;
	}
}

LuaFunctionInjectHelper::LuaFunctionInjectHelper()
{
}

LuaFunctionInjectHelper::~LuaFunctionInjectHelper()
{
}

void LuaFunctionInjectHelper::ReplaceUClassFunction(lua_State*inL, UClass* Class, const char* LuaClassPath)
{
}

void LuaFunctionInjectHelper::AddFunction(UFunction *TemplateFunction, UClass *OuterClass, FName NewFuncName)
{
}

void ReplaceNativeFunc(UFunction *Function, FNativeFuncPtr NativeFunc)
{
}

void LuaFunctionInjectHelper::ReplaceFunction(UFunction *TemplateFunction, UClass *OuterClass, FName NewFuncName)
{
}

UFunction* DuplicateUFunction(UFunction *TemplateFunction, UClass *OuterClass, FName NewFuncName)
{
	return nullptr;
}
UFunction* LuaFunctionInjectHelper::DuplicateFunction(UFunction *TemplateFunction, UClass *OuterClass, FName NewFuncName)
{
	return nullptr;
}
