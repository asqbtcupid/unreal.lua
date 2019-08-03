#pragma once

#include "UserDefineClassMacro.h"
#include "UnrealLua.h"

#define CAST_TO_VALUE_FUNC(TYPE_NAME) TYPE_NAME To_##TYPE_NAME(){return *((TYPE_NAME*)ThePtr);}\
										static int32 To_##TYPE_NAME##Raw(lua_State* inL){ UTableUtil::push(inL, *((TYPE_NAME*)(lua_touserdata(inL, 1)))); return 1; }	

#define LUA_GLUE_CAST_FUNCTION(TYPE_NAME) LUA_GLUE_FUNCTION(To_##TYPE_NAME)\
											LUA_GLUE_FUNCTION(To_##TYPE_NAME##Raw)

struct VoidPtrStruct
{
	VoidPtrStruct():ThePtr(nullptr){}
	VoidPtrStruct(const void* _ptr):ThePtr(_ptr){}
	VoidPtrStruct(const VoidPtrStruct& Right):ThePtr(Right.ThePtr){}
	const void* ThePtr;

// 	int ToInt() { return *((int*)ThePtr); }
	CAST_TO_VALUE_FUNC(int32)
		CAST_TO_VALUE_FUNC(int)
		CAST_TO_VALUE_FUNC(float)
		CAST_TO_VALUE_FUNC(double)
		CAST_TO_VALUE_FUNC(FString)
		CAST_TO_VALUE_FUNC(bool)
		CAST_TO_VALUE_FUNC(int64)
		CAST_TO_VALUE_FUNC(uint64)
		CAST_TO_VALUE_FUNC(uint32)
		CAST_TO_VALUE_FUNC(int8)
		CAST_TO_VALUE_FUNC(uint8)
		CAST_TO_VALUE_FUNC(int16)
		CAST_TO_VALUE_FUNC(uint16)

	static int32 To_Struct(lua_State* inL)
	{
#if LuaDebug
		if (ue_lua_gettop(inL) < 2 )
		{
			ensureAlwaysMsgf(0, TEXT(""));
			return 0;
		}
#endif
		VoidPtrStruct* Ptr = (VoidPtrStruct*)tovoid(inL, 1);
		FString Type_nogc = ue_lua_tostring(inL, 2);
		Type_nogc += "_nogc";
		pushstruct_nogc(inL, ue_lua_tostring(inL, 2), TCHAR_TO_UTF8(*Type_nogc), (void*)Ptr->ThePtr);
		return 1;
	}

	static int32 To_UObject(lua_State* inL)
	{
#if LuaDebug
		if (ue_lua_gettop(inL) < 1)
		{
			ensureAlwaysMsgf(0, TEXT(""));
			return 0;
		}
#endif
		VoidPtrStruct* Ptr = (VoidPtrStruct*)tovoid(inL, 1);
		pushuobject(inL, (void*)Ptr->ThePtr);
		return 1;
	}

	static int32 To_StructRaw(lua_State* inL)
	{
#if LuaDebug
		if (ue_lua_gettop(inL) < 2)
		{
			ensureAlwaysMsgf(0, TEXT(""));
			return 0;
		}
#endif
		void* Ptr = lua_touserdata(inL, 1);
		FString Type_nogc = ue_lua_tostring(inL, 2);
		Type_nogc += "_nogc";
		pushstruct_nogc(inL, ue_lua_tostring(inL, 2), TCHAR_TO_UTF8(*Type_nogc), Ptr);
		return 1;
	}

	static int32 To_UObjectRaw(lua_State* inL)
	{
#if LuaDebug
		if (ue_lua_gettop(inL) < 1)
		{
			ensureAlwaysMsgf(0, TEXT(""));
			return 0;
		}
#endif
		void* Ptr = lua_touserdata(inL, 1);
		pushuobject(inL, Ptr);
		return 1;
	}

	static int32 To_lightuserdata(lua_State*inL)
	{
#if LuaDebug
		if (ue_lua_gettop(inL) < 1)
		{
			ensureAlwaysMsgf(0, TEXT(""));
			return 0;
		}
#endif
		VoidPtrStruct* Ptr = (VoidPtrStruct*)tovoid(inL, 1);
		ue_lua_pushlightuserdata(inL, (void*)Ptr->ThePtr);
		return 1;
	}

	static int32 Index_Raw(lua_State*inL);

	static int32 __index(lua_State*inL);

};

LUA_GLUE_BEGIN(VoidPtrStruct)
LUA_GLUE_CAST_FUNCTION(int32)
LUA_GLUE_CAST_FUNCTION(int)
LUA_GLUE_CAST_FUNCTION(float)
LUA_GLUE_CAST_FUNCTION(double)
LUA_GLUE_CAST_FUNCTION(FString)
LUA_GLUE_CAST_FUNCTION(bool)
LUA_GLUE_CAST_FUNCTION(int64)
LUA_GLUE_CAST_FUNCTION(uint64)
LUA_GLUE_CAST_FUNCTION(uint32)
LUA_GLUE_CAST_FUNCTION(int8)
LUA_GLUE_CAST_FUNCTION(uint8)
LUA_GLUE_CAST_FUNCTION(int16)
LUA_GLUE_CAST_FUNCTION(uint16)
LUA_GLUE_CUSTOM_FUNCTION(To_Struct)
LUA_GLUE_CUSTOM_FUNCTION(To_UObject)
LUA_GLUE_CUSTOM_FUNCTION(To_StructRaw)
LUA_GLUE_CUSTOM_FUNCTION(To_UObjectRaw)
LUA_GLUE_CUSTOM_FUNCTION(To_lightuserdata)
LUA_GLUE_CUSTOM_FUNCTION(Index_Raw)
LUA_GLUE_FUNCTION(__index)
LUA_GLUE_END()