#pragma once

#include "TableUtil.h"
#include "UserDefineClassMacro.h"
#include "ArrGlueMacro.h"
#include "MapGlueMacro.h"
#include "SetGlueMacro.h"
#include "SharePtrGlueMacro.h"
#include "ShareRefGlueMacro.h"
#include "StringTableCoreFwd.h"
#include "UnrealLuaFunctor.h"
#include "DelegateGlueMacro.h"
#include "UnrealLuaInterface.h"
#include "LuaFunctionInjectHelper.h"

LUA_GLUE_STRUCT_TRAIT(FText)

template<>
struct TCheckArgIsFit<FText> {
	static UnrealLua::CheckResult ToCheck(UnrealLua::ArgType _Type, lua_State*inL = nullptr, int32 Index = 0)
	{
		return _Type == UnrealLua::Type::TYPE_TSTRING 
			|| _Type == GetStructNewType<FText>::GetType()
			|| _Type == UnrealLua::Type::TYPE_Text
			;
	}
};
