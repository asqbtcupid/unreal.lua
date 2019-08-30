#include "TableUtil.h"
#include "LuaScript.h"
#include "Paths.h"
#include "TextProperty.h"
#include "LuaMapHelper.h"
#include "../Launch/Resources/Version.h"
#include "BPAndLuaBridge.h"
#include "Engine/World.h"
#include "Engine/LevelScriptActor.h"

#if WITH_EDITOR
#include "Editor.h"
#endif
#include "GameDelegates.h"
#include "Platform.h"
#include "FileHelper.h"
#include "MetaInterface.h"
#include <list>

static int32 GeneralGetProperty(lua_State*inL)
{
	LuaBasePropertyInterface* p = (LuaBasePropertyInterface*)lua_touserdata(inL, lua_upvalueindex(1));
	p->push_container(inL, tostruct(inL, 1));
	return 1;
}

static int32 GeneralSetProperty(lua_State*inL)
{
	LuaBasePropertyInterface* p = (LuaBasePropertyInterface*)lua_touserdata(inL, lua_upvalueindex(1));
	p->pop_container(inL, 2, tostruct(inL, 1));
	return 0;
}


static int32 LuaPanic(lua_State *L)
{
	UTableUtil::log(FString::Printf(TEXT("PANIC: unprotected error in call to Lua API(%s)"), ANSI_TO_TCHAR(lua_tostring(L, -1))));
	return 0;
}

#if LuaDebug
void* tovoid(lua_State* L, int i)
{
	int LuaType = ue_lua_type(L, i);
	void **u = nullptr;
	switch (LuaType)
	{
		case LUA_TUSERDATA:
			u = static_cast<void**>(lua_touserdata(L, i));
			return *u;
		case LUA_TTABLE:
			if (i < 0)
				i = lua_gettop(L) + i + 1;
			lua_rawgeti(L, i, CppInstanceIndex);
			if (lua_isnil(L, -1))
			{
				lua_pop(L, 1);
				ensureAlwaysMsgf(0, TEXT("Bug"));
				UnrealLua::ReportError(L, "tovoid Bug1");
				return nullptr;
			}
			else
			{
				if (!lua_isuserdata(L, -1))
				{
					ensureAlwaysMsgf(0, TEXT("Bug"));
					UnrealLua::ReportError(L, "tovoid Bug2");
					lua_pop(L, 1);
					return nullptr;
				}
				u = static_cast<void**>(lua_touserdata(L, -1));
				lua_pop(L, 1);
				return *u;
			}
		case LUA_TNIL:
			return nullptr;
		case LUA_TNONE:
			return nullptr;
		default:
			ensureAlwaysMsgf(0, TEXT("Can't Be Another Type, Bug!!!"));
			UnrealLua::ReportError(L, "Can't Be Another Type, Bug!!!");
			return nullptr;
	};
}
#endif

FString GetLuaCodeFromPath(FString FilePath)
{
	FString FirstLine = FString("--[[") + FilePath + FString("]]");

	const static FString Prefix = "/Game/LuaSource/";
	int32 IndexStart = 0;
	FilePath.FindLastChar('.', IndexStart);
	if (IndexStart != INDEX_NONE)
	{
		FString FileName = FilePath.Mid(IndexStart + 1, FilePath.Len() - IndexStart);
		FilePath = FilePath.Replace(TEXT("."), TEXT("/"));
		FilePath = FilePath + "." + FileName;
	}
	else
	{
		FilePath = FilePath + FString(".") + FilePath;
	}
	FilePath = Prefix + FilePath;
	ULuaScript* CodeObject = LoadObject<ULuaScript>(nullptr, *FilePath);
	if (CodeObject) {
		return FirstLine + CodeObject->Code;
	}
	return "";
}

int CustomLuaLoader(lua_State* L)
{
	FString FilePath = lua_tostring(L, -1);
	FString Code = GetLuaCodeFromPath(FilePath);
	if (!Code.IsEmpty())
	{
		luaL_loadstring(L, TCHAR_TO_UTF8(*Code));
		return 1;
	}
	return 0;
}

bool GetLuaCodeFromPathFromIO(const FString& FolderPath, FString& FilePath, FString& Code)
{
	FString FirstLine = FString("--[[") + FilePath + FString("]]");

	FilePath = FilePath.Replace(TEXT("."), TEXT("/")) + TEXT(".lua") ;
	FilePath = FolderPath + "LuaSource/" + FilePath;
	if (FPaths::FileExists(FilePath))
	{
		FFileHelper::LoadFileToString(Code, *FilePath);
		Code = FirstLine + Code;
		return true;
	}
	else
		return false;
}

int CustomLuaLoader_SearchSaved(lua_State* L)
{
	FString FilePath = lua_tostring(L, -1);
	FString Code;

	if (GetLuaCodeFromPathFromIO(FPaths::ProjectSavedDir(), FilePath, Code))
	{
		if (!Code.IsEmpty())
		{
			luaL_loadstring(L, TCHAR_TO_UTF8(*Code));
			return 1;
		}
	}
	return 0;
}


int SetExistTable(lua_State* inL)
{
	lua_geti(inL, LUA_REGISTRYINDEX, ExistTableIndex);
	auto u = static_cast<void**>(lua_touserdata(inL, 1));
	if (u == nullptr)
	{
		ensureAlwaysMsgf(0, TEXT("Can't Be null, Bug"));
		UnrealLua::ReportError(inL,"Can't Be null, Bug");
		return 0;
	}
	lua_pushlightuserdata(inL, *u);
	lua_pushvalue(inL, 2);
	lua_rawset(inL, -3);
	return 0;
}

static int32 LuaCallCpp(lua_State* inL)
{
	LuaUFunctionInterface* FuncInterface = (LuaUFunctionInterface*)lua_touserdata(inL, lua_upvalueindex(1));
	return FuncInterface->JustCall(inL);
}

template<bool HasGlueFunc>
int32 index_reflection_uobject_func_withexpand(lua_State* inL)
{
	lua_pushvalue(inL, 2);
	int32 Type = lua_rawget(inL, lua_upvalueindex(2));
	if (HasGlueFunc && Type == LUA_TLIGHTUSERDATA)
	{
		auto f = (lua_CFunction)lua_touserdata(inL, -1);
		return f(inL);
	}
	else if (Type == LUA_TTABLE)
	{
		lua_rawgeti(inL, -1, 2);//cached
		lua_pushvalue(inL, 1);
		int32 Type1 = lua_rawget(inL, -2);
		if (Type1 == LUA_TNIL)
		{
			lua_rawgeti(inL, -3, 1);//func
			lua_pushvalue(inL, 1);
			lua_pushvalue(inL, 2);
			lua_call(inL, 2, 1);

			lua_pushvalue(inL, 1);
			lua_pushvalue(inL, -2);
			lua_rawset(inL, -5);
		}

		return 1;
	}
	else if (Type == LUA_TUSERDATA)
	{
		auto f = (LuaBasePropertyInterface**)lua_touserdata(inL, -1);
		auto ptr = tovoid(inL, 1);
		(*f)->push_container(inL, ptr);
		return 1;
	}
	else if (Type == LUA_TFUNCTION)
	{
		return 1;
	}
	else if (Type != LUA_TNIL)
	{
		return 1;
	}

	lua_pushvalue(inL, 2);
	Type = lua_rawget(inL, lua_upvalueindex(1));
	if (Type != LUA_TNIL)
	{
		return 1;
	}

	UObject* p = (UObject*)tovoid(inL, 1);
#if LuaDebug
	if (p == nullptr)
	{
		ensureAlwaysMsgf(0, TEXT("Bug"));
		UnrealLua::ReportError(inL, "index_reflection_uobject_func_withexpand Bug");
		lua_pushstring(inL, "Bug");
		lua_error(inL);
		return 0;
	}
#endif
	UClass* Class = p->GetClass();
	FString OriginKey = FString(lua_tostring(inL, 2));
	UFunction* Func = Class->FindFunctionByName(*OriginKey);
	if (Func)
	{
		void* LuaFunction = UTableUtil::GetBpFuncInterface(inL, Func);
		lua_pushlightuserdata(inL, LuaFunction);
		lua_pushcclosure(inL, LuaCallCpp, 1);
// cached
		lua_pushvalue(inL, lua_upvalueindex(2));
		lua_pushvalue(inL, 2);
		lua_pushvalue(inL, -3);
		lua_rawset(inL, -3);
		lua_pop(inL, 1);

		return 1;
	}

	UProperty* Property = Class->FindPropertyByName(*OriginKey);
	if (Property)
	{
		void* PropertyInterface = UTableUtil::GetBpPropertyInterface(inL, Property);
		lua_pushvalue(inL, lua_upvalueindex(2));
		lua_pushvalue(inL, 2);
		*(void**)lua_newuserdata(inL, sizeof(void *)) = PropertyInterface;
		lua_rawset(inL, -3);
		LuaBasePropertyInterface* p = (LuaBasePropertyInterface*)PropertyInterface;
		p->push_container(inL, p);
		return 1;
	}
	return 0;
}

template<bool HasGlueFunc>
int32 newindex_reflection_uobject_func_withexpand(lua_State* inL)
{
	lua_pushvalue(inL, 2);
	int32 Type = lua_rawget(inL, lua_upvalueindex(2));

	if (HasGlueFunc && Type == LUA_TLIGHTUSERDATA)
	{
		auto f = (lua_CFunction)lua_touserdata(inL, -1);
		f(inL);
		return 0;
	}
	else if (Type == LUA_TUSERDATA)
	{
		auto f = (LuaBasePropertyInterface**)lua_touserdata(inL, -1);
		auto ptr = tovoid(inL, 1);
		(*f)->pop_container(inL, 3, ptr);
		return 0;
	}
	else if (Type == LUA_TFUNCTION)
	{
		lua_pushvalue(inL, 1);
		lua_pushvalue(inL, 2);
		lua_pushvalue(inL, 3);
		lua_call(inL, 3, 0);
		return 0;
	}
#if LuaDebug
	else if (Type != LUA_TNIL)
	{
		checkf(0, TEXT("shouldn't come here"));
	}
#endif

	UObject* p = (UObject*)tovoid(inL, 1);
#if LuaDebug
	if (p == nullptr)
	{
		ensureAlwaysMsgf(0, TEXT("Bug"));
		UnrealLua::ReportError(inL, "newindex_reflection_uobject_func_withexpand Bug");
		lua_pushstring(inL, "Bug");
		lua_error(inL);
		return 0;
	}
#endif
	UClass* Class = p->GetClass();
	UProperty* Property = Class->FindPropertyByName(UTF8_TO_TCHAR(lua_tostring(inL, 2)));
	if (Property)
	{
		void* PropertyInterface = UTableUtil::GetBpPropertyInterface(inL, Property);
		lua_pushvalue(inL, lua_upvalueindex(1));
		lua_pushvalue(inL, 2);
		*(void**)lua_newuserdata(inL, sizeof(void *)) = PropertyInterface;
		lua_rawset(inL, -3);
		LuaBasePropertyInterface* p = (LuaBasePropertyInterface*)PropertyInterface;
		p->pop_container(inL, 3, p);
	}
	ensureAlwaysMsgf(0, TEXT("Set value to not exist key: %s"), lua_tostring(inL, 2));
	UnrealLua::ReportError(inL,FString::Printf(TEXT("Set value to not exist key: %s"), lua_tostring(inL, 2)));
	return 0;
}


template<bool HasGlueFunc>
int32 try_newindex_reflection_uobject_func_withexpand(lua_State* inL)
{
	lua_pushvalue(inL, 2);
	int32 Type = lua_rawget(inL, lua_upvalueindex(1));

	if (HasGlueFunc && Type == LUA_TLIGHTUSERDATA)
	{
		auto f = (lua_CFunction)lua_touserdata(inL, -1);
		f(inL);
		lua_pushboolean(inL, true);
		return 1;
	}
	else if (Type == LUA_TUSERDATA)
	{
		auto f = (LuaBasePropertyInterface**)lua_touserdata(inL, -1);
		auto ptr = tovoid(inL, 1);
		(*f)->pop_container(inL, 3, ptr);
		lua_pushboolean(inL, true);
		return 1;
	}
	else if (Type == LUA_TFUNCTION)
	{
		lua_pushvalue(inL, 1);
		lua_pushvalue(inL, 2);
		lua_pushvalue(inL, 3);
		lua_call(inL, 3, 0);
		lua_pushboolean(inL, true);
		return 1;
	}
	else if (Type == LUA_TBOOLEAN)
	{
		return 1;
	}

	UObject* p = (UObject*)tovoid(inL, 1);
	UClass* Class = p->GetClass();
	UProperty* Property = Class->FindPropertyByName(UTF8_TO_TCHAR(lua_tostring(inL, 2)));
	if (Property)
	{
		void* PropertyInterface = UTableUtil::GetBpPropertyInterface(inL, Property);
		lua_pushvalue(inL, lua_upvalueindex(1));
		lua_pushvalue(inL, 2);
		*(void**)lua_newuserdata(inL, sizeof(void *)) = PropertyInterface;
		lua_rawset(inL, -3);
		LuaBasePropertyInterface* p = (LuaBasePropertyInterface*)PropertyInterface;
		p->pop_container(inL, 3, p);
		lua_pushboolean(inL, true);
		return 1;
	}
	else
	{
		lua_pushvalue(inL, lua_upvalueindex(1));
		lua_pushvalue(inL, 2);
		lua_pushboolean(inL, false);
		lua_rawset(inL, -3);
		lua_pushboolean(inL, false);
		return 0;
	}
}

int32 IndexStaticProperty(lua_State* inL)
{
	int32 Type = lua_rawget(inL, lua_upvalueindex(1));
	if (Type == LUA_TFUNCTION)
	{
		auto f = lua_tocfunction(inL, -1);
		f(inL);
		return 1;
	}
	else if (Type == LUA_TUSERDATA) //the struct
	{
		return 1;
	}
	else
		return 0;
}

int32 ObjectIndexStaticProperty(lua_State* inL)
{
	lua_pushvalue(inL, 2);
	int32 Type = lua_rawget(inL, lua_upvalueindex(2));

	if (Type == LUA_TFUNCTION)
	{
		auto f = lua_tocfunction(inL, -1);
		lua_pop(inL, 3);
		f(inL);
		return 1;
	}
	else if (Type == LUA_TUSERDATA) //the struct
	{
		return 1;
	}
	
	void** p = (void**)lua_touserdata(inL, lua_upvalueindex(3));
	UClass* Class = (UClass*)(*p);

	UFunction* Func = Class->FindFunctionByName(UTF8_TO_TCHAR(lua_tostring(inL, 2)));
	if (Func)
	{
		void* LuaFunction = UTableUtil::GetBpFuncInterface(inL, Func);
		lua_pushlightuserdata(inL, LuaFunction);
		lua_pushcclosure(inL, LuaCallCpp, 1);
		// cached
		lua_pushvalue(inL, lua_upvalueindex(1));
		lua_pushvalue(inL, 2);
		lua_pushvalue(inL, -3);
		lua_rawset(inL, -3);
		lua_pop(inL, 1);
		return 1;
	}

	return 0;
}

int32 NewindexStaticProperty(lua_State* inL)
{
	lua_pushvalue(inL, 2);
	int32 Type = lua_rawget(inL, lua_upvalueindex(1));
	if (Type == LUA_TFUNCTION)
	{
		auto f = lua_tocfunction(inL, -1);
		f(inL);
		return 0;
	}
	else
	{
		lua_pop(inL, 1);
		lua_rawset(inL, -3);
		return 0;
	}
}

int32 ObjectNewindexStaticProperty(lua_State* inL)
{
	lua_pushvalue(inL, 2);
	int32 Type = lua_rawget(inL, lua_upvalueindex(1));
	if (Type == LUA_TFUNCTION)
	{
		auto f = lua_tocfunction(inL, -1);
		lua_pushnil(inL);
		lua_replace(inL, 1);
		f(inL);
		return 0;
	}
	else
	{
		lua_pop(inL, 1);
		lua_rawset(inL, -3);
		return 0;
	}
}

int32 index_struct_func(lua_State* inL)
{
	int32 Type = lua_rawget(inL, lua_upvalueindex(1));
	if (Type == LUA_TLIGHTUSERDATA)
	{
		auto f = (lua_CFunction)lua_touserdata(inL, -1);
		f(inL);
	}
	else if (Type == LUA_TTABLE)
	{
		lua_rawgeti(inL, -1, 2);//cached
		lua_pushvalue(inL, 1);
		int32 Type1 = lua_rawget(inL, -2);
		if (Type1 == LUA_TNIL)
		{
			lua_rawgeti(inL, -3, 1);//func
			auto f = (lua_CFunction)lua_touserdata(inL, -1);
			f(inL);
			lua_pushvalue(inL, 1);
			lua_pushvalue(inL, -2);
			lua_rawset(inL, -6);
		}
	}
	return 1;
}

template<bool HasGlueFunc>
int32 index_struct_func_with_class_with_glue(lua_State* inL)
{
	lua_pushvalue(inL, 2);
	int32 Type = lua_rawget(inL, lua_upvalueindex(2));
	if (HasGlueFunc && Type == LUA_TLIGHTUSERDATA)
	{
		auto f = (lua_CFunction)lua_touserdata(inL, -1);
		return f(inL);
	}
	else if (Type == LUA_TTABLE)
	{
		lua_rawgeti(inL, -1, 2);//cached
		lua_pushvalue(inL, 1);
		int32 Type1 = lua_rawget(inL, -2);
		if (Type1 == LUA_TNIL)
		{
			lua_rawgeti(inL, -3, 1);//func
			lua_pushvalue(inL, 1);
			lua_pushvalue(inL, 2);
			lua_call(inL, 2, 1);

			lua_pushvalue(inL, 1);
			lua_pushvalue(inL, -2);
			lua_rawset(inL, -5);
		}

		return 1;
	}
	else if (Type == LUA_TUSERDATA)
	{
		auto f = (LuaBasePropertyInterface**)lua_touserdata(inL, -1);
		auto ptr = tostruct(inL, 1);
		(*f)->push_container(inL, ptr);
		return 1;
	}
	else if (HasGlueFunc && Type == LUA_TFUNCTION)
	{
		return 1;
	}
	else if (Type != LUA_TNIL)
	{
		return 1;
	}
	
	lua_pushvalue(inL, 2);
	Type = lua_rawget(inL, lua_upvalueindex(1));
	if (Type == LUA_TNIL)
	{
		lua_pushvalue(inL, lua_upvalueindex(3));
		void** p = (void**)lua_touserdata(inL, -1);
		UScriptStruct* Class = (UScriptStruct*)(*p);
		UProperty* Property = Class->FindPropertyByName(UTF8_TO_TCHAR(lua_tostring(inL, 2)));
		if (Property)
		{
			void* PropertyInterface = UTableUtil::GetBpPropertyInterface(inL, Property);
			lua_pushvalue(inL, lua_upvalueindex(2));
			lua_pushvalue(inL, 2);
			*(void**)lua_newuserdata(inL, sizeof(void *)) = PropertyInterface;
			lua_rawset(inL, -3);
			LuaBasePropertyInterface* p = (LuaBasePropertyInterface*)PropertyInterface;
			p->push_container(inL, tostruct(inL, 1));
			return 1;
		}
		return 0;
	}
	return 1;
}

int32 index_struct_func_with_extend(lua_State*inL)
{
	lua_pushvalue(inL, 2);
	int32 Type = lua_rawget(inL, lua_upvalueindex(1));
	if (Type == LUA_TLIGHTUSERDATA)
	{
		auto f = (lua_CFunction)lua_touserdata(inL, -1);
		f(inL);
	}
	else if (Type == LUA_TTABLE)
	{
		lua_rawgeti(inL, -1, 2);//cached
		lua_pushvalue(inL, 1);
		int32 Type1 = lua_rawget(inL, -2);
		if (Type1 == LUA_TNIL)
		{
			lua_rawgeti(inL, -3, 1);//func
			auto f = (lua_CFunction)lua_touserdata(inL, -1);
			f(inL);
			lua_pushvalue(inL, 1);
			lua_pushvalue(inL, -2);
			lua_rawset(inL, -6);
		}
	}
	else if (Type == LUA_TNIL)
	{
		lua_pushvalue(inL, lua_upvalueindex(2));
		lua_CFunction Func = (lua_CFunction)lua_touserdata(inL, -1);
		return Func(inL);
	}
	return 1;

}

int32 newindex_struct_func(lua_State* inL)
{
	lua_pushvalue(inL, 2);
	lua_rawget(inL, lua_upvalueindex(1));
	auto f = lua_tocfunction(inL, -1);
	f(inL);
	return 0;
}

template<bool HasGlueFunc>
int32 newindex_struct_Func_with_class_with_glue(lua_State* inL)
{
	lua_pushvalue(inL, 2);
	int32 Type = lua_rawget(inL, lua_upvalueindex(1));
	
	if (HasGlueFunc && Type == LUA_TLIGHTUSERDATA)
	{
		auto f = (lua_CFunction)lua_touserdata(inL, -1);
		f(inL);
		return 0;
	}
	else if (Type == LUA_TUSERDATA)
	{
		auto f = (LuaBasePropertyInterface**)lua_touserdata(inL, -1);
		auto ptr = tostruct(inL, 1);
		(*f)->pop_container(inL, 3, ptr);
		return 0;
	}
#if LuaDebug
	else if (Type != LUA_TNIL)
	{
		checkf(0, TEXT("shouldn't come here"));
	}
#endif

	lua_pushvalue(inL, lua_upvalueindex(2));
	
	void** p = (void**)lua_touserdata(inL, -1);
	UScriptStruct* Class = (UScriptStruct*)(*p);
	UProperty* Property = Class->FindPropertyByName(UTF8_TO_TCHAR(lua_tostring(inL, 2)));
	if (Property)
	{
		void* PropertyInterface = UTableUtil::GetBpPropertyInterface(inL, Property);
		lua_pushvalue(inL, lua_upvalueindex(1));
		lua_pushvalue(inL, 2);
		*(void**)lua_newuserdata(inL, sizeof(void *)) = PropertyInterface;
		lua_rawset(inL, -3);
		LuaBasePropertyInterface* p = (LuaBasePropertyInterface*)PropertyInterface;
		p->pop_container(inL, 3, tostruct(inL, 1));
	}

	return 0;
}

int32 newindex_struct_func_with_extend(lua_State* inL)
{
	lua_pushvalue(inL, 2);
	int32 Type = lua_rawget(inL, lua_upvalueindex(1));
	if (Type == LUA_TFUNCTION)
	{
		auto f = lua_tocfunction(inL, -1);
		f(inL);
		return 0;
	}
	else
	{
		lua_CFunction Func = (lua_CFunction)lua_touserdata(inL, lua_upvalueindex(2));
		return Func(inL);
	}
}
int32 uobjcet_gcfunc(lua_State *inL)
{
	auto u = (void**)lua_touserdata(inL, -1);
	UTableUtil::rmgcref(inL, static_cast<UObject*>(*u));
	lua_getmetatable(inL, 1);
	lua_getfield(inL, -1, "Destroy");
	if (lua_isfunction(inL, -1))
	{
		lua_pushvalue(inL, 1);
		if (lua_pcall(inL, 1, 0, 0))
		{
			UTableUtil::log(FString(lua_tostring(inL, -1)));
		}
	}
#if LuaDebug
	else
		lua_pop(inL, 1);
	lua_getfield(inL, -1, "classname");
	FString n = lua_tostring(inL, -1);
	UTableUtil::SubGcCount(inL, n);
#endif
	return 0;
}
int32 struct_gcfunc(lua_State *inL)
{
	lua_getmetatable(inL, 1);
	lua_getfield(inL, -1, "Destroy");
	lua_pushvalue(inL, 1);
	lua_call(inL, 1, 0);
#if LuaDebug
	lua_getmetatable(inL, 1);
	lua_getfield(inL, -1, "classname");
	FString n = lua_tostring(inL, -1);
	UTableUtil::SubGcCount(inL, n);
#endif
	return 0;
}

int32 serialize_table(lua_State *inL)
{
	int top = lua_gettop(inL);
	if (top == 1)
	{
		lua_newtable(inL);
		lua_getmetatable(inL, 1);
		lua_pushnil(inL);
		while (lua_next(inL, -2))
		{
			FString Key = lua_tostring(inL, -2);
			if (Key.Contains("LuaGet_"))
			{
				lua_pushvalue(inL, 1);
				lua_call(inL, 1, 1);
				UTableUtil::push(inL, Key.Mid(7));
				lua_insert(inL, -2);
				lua_rawset(inL, -5);
				continue;
			}
			lua_pop(inL, 1);
		}
		lua_pop(inL, 1);
		return 1;
	}
	else
	{
		lua_getmetatable(inL, 1);
		lua_pushnil(inL);
		while (lua_next(inL, 2))
		{
			FString Key = "LuaSet_" + FString(lua_tostring(inL, -2));
			lua_getfield(inL, 3, TCHAR_TO_UTF8(*Key));
			lua_pushvalue(inL, 1);
			lua_pushvalue(inL, 2);
			lua_pushvalue(inL, -3);
			lua_call(inL, 3, 0);
			lua_pop(inL, 1);
		}
		return 0;
	}
}

int32 EnsureNew(lua_State* inL)
{
	ensureAlwaysMsgf(0, TEXT("This class not api class, so don't have new, make it become api class if you want one."));
	UnrealLua::ReportError(inL,TEXT("This class not api class, so don't have new, make it become api class if you want one."));
	return 0;
}

int32 EnsureDestroy(lua_State* inL)
{
	UObject* Obj = (UObject*)touobject(inL, 1);
	if (Obj)
	{
		UTableUtil::rmgcref(inL, Obj);
	}
	return 0;
}

int32 UObject_GetName(lua_State* inL)
{
	UObject* Obj = (UObject*)touobject(inL, 1);
#if LuaDebug
	if (Obj == nullptr)
	{
		ensureAlwaysMsgf(0, TEXT("Bug"));
		UnrealLua::ReportError(inL,TEXT("UObject_GetName Bug"));
		lua_pushstring(inL, "Bug");
		lua_error(inL);
		return 0;
	}
#endif
	UTableUtil::push(inL, Obj->GetName());
	return 1;
}

int32 UObject_GetClass(lua_State* inL)
{
	UObject* Obj = (UObject*)touobject(inL, 1);
#if LuaDebug
	if (Obj == nullptr)
	{
		ensureAlwaysMsgf(0, TEXT("Bug"));
		UnrealLua::ReportError(inL,TEXT("UObject_GetClass Bug"));
		lua_pushstring(inL, "Bug");
		lua_error(inL);
		return 0;
	}
#endif
	pushuobject(inL, Obj->GetClass());
	return 1;
}

int32 UObject_GetOuter(lua_State* inL)
{
	UObject* Obj = (UObject*)touobject(inL, 1);
#if LuaDebug
	if (Obj == nullptr)
	{
		ensureAlwaysMsgf(0, TEXT("Bug"));
		UnrealLua::ReportError(inL,TEXT("UObject_GetOuter Bug"));
		lua_pushstring(inL, "Bug");
		lua_error(inL);
		return 0;
	}
#endif
	pushuobject(inL, Obj->GetOuter());
	return 1;
}

int32 UObject_IsPendingKill(lua_State* inL)
{
	UObject* Obj = (UObject*)touobject(inL, 1);
#if LuaDebug
	if (Obj == nullptr)
	{
		ensureAlwaysMsgf(0, TEXT("Bug"));
		UnrealLua::ReportError(inL,TEXT("UObject_IsPendingKill Bug"));
		lua_pushstring(inL, "Bug");
		lua_error(inL);
		return 0;
	}
#endif
	UTableUtil::push(inL, Obj->IsPendingKill());
	return 1;
}

int32 UObject_MarkPendingKill(lua_State* inL)
{
	UObject* Obj = (UObject*)touobject(inL, 1);
#if LuaDebug
	if (Obj == nullptr)
	{
		ensureAlwaysMsgf(0, TEXT("Bug"));
		UnrealLua::ReportError(inL,TEXT("UObject_MarkPendingKill Bug"));
		lua_pushstring(inL, "Bug");
		lua_error(inL);
		return 0;
	}
#endif
	Obj->MarkPendingKill();
	return 0;
}

int32 UObject_AddToRoot(lua_State* inL)
{
	UObject* Obj = (UObject*)touobject(inL, 1);
#if LuaDebug
	if (Obj == nullptr)
	{
		ensureAlwaysMsgf(0, TEXT("Bug"));
		UnrealLua::ReportError(inL,TEXT("UObject_AddToRoot Bug"));
		lua_pushstring(inL, "Bug");
		lua_error(inL);
		return 0;
	}
#endif
	Obj->AddToRoot();
	return 0;
}

int32 UObject_RemoveFromRoot(lua_State* inL)
{
	UObject* Obj = (UObject*)touobject(inL, 1);
	Obj->RemoveFromRoot();
	return 0;
}

int ErrHandleFunc(lua_State*L)
{
	lua_getglobal(L, "debug");
	lua_getfield(L, -1, "traceback");
	lua_pushthread(L);
	lua_pushvalue(L, 1);
	lua_pushinteger(L, 2);
	lua_call(L, 3, 1);
	lua_remove(L, -2);
	lua_concat(L, 2);
	lua_getglobal(L, "ErrHandleInLua");
	if (lua_isnil(L, -1))
	{
		lua_pop(L, 1);
	}
	else
	{
		lua_pushvalue(L, -2);
		lua_call(L, 1, 0);
#if LuaDebug
		FString error = lua_tostring(L, -2);
		ensureAlwaysMsgf(0, *error);
		UnrealLua::ReportError(L, error);
#endif
	}
	return 1;
}

#define BpPropertyGetterName(PropertyType) GetBpProp_##PropertyType
#define BpPropertySetterName(PropertyType) SetBpProp_##PropertyType

#define BpPropertyGetSet(PropertyType) static int32 BpPropertyGetterName(PropertyType)(lua_State* inL)\
					{\
						void* p = (void*)lua_touserdata(inL, lua_upvalueindex(1));\
						Lua##PropertyType* LuaProperty = (Lua##PropertyType*)(p);\
						LuaProperty->push_container_novirtual(inL, tovoid(inL, 1));\
						return 1;\
					}\
					static int32 BpPropertySetterName(PropertyType)(lua_State* inL)\
					{\
						void* p = (void*)lua_touserdata(inL, lua_upvalueindex(1));\
						Lua##PropertyType* LuaProperty = (Lua##PropertyType*)(p);\
						LuaProperty->pop_container_novirtual(inL, 3, tovoid(inL, 1));\
						return 0;\
					}

#define BpStructPropertyGetterName(PropertyType) BpStructGetProp_##PropertyType
#define BpStructPropertySetterName(PropertyType) BpStructSetProp_##PropertyType

#define BpStructPropertyGetSet(PropertyType) static int32 BpStructPropertyGetterName(PropertyType)(lua_State* inL)\
					{\
						void* p = (void*)lua_touserdata(inL, lua_upvalueindex(1));\
						Lua##PropertyType* LuaProperty = (Lua##PropertyType*)(p);\
						LuaProperty->push_container_novirtual(inL, tostruct(inL, 1));\
						return 1;\
					}\
					static int32 BpStructPropertySetterName(PropertyType)(lua_State* inL)\
					{\
						void* p = (void*)lua_touserdata(inL, lua_upvalueindex(1));\
						Lua##PropertyType* LuaProperty = (Lua##PropertyType*)(p);\
						LuaProperty->pop_container_novirtual(inL, 3, tostruct(inL, 1));\
						return 0;\
					}

#define BpSetterAndGetter(PropertyType) BpPropertyGetSet(PropertyType) \
										BpStructPropertyGetSet(PropertyType)

BpSetterAndGetter(UBoolProperty)
BpSetterAndGetter(UIntProperty)
BpSetterAndGetter(UUInt16Property)
BpSetterAndGetter(UInt16Property)
BpSetterAndGetter(UUInt32Property)
BpSetterAndGetter(UInt64Property)
BpSetterAndGetter(UUInt64Property)
BpSetterAndGetter(UFloatProperty)
BpSetterAndGetter(UDoubleProperty)
BpSetterAndGetter(UObjectPropertyBase)
BpSetterAndGetter(UObjectProperty)
BpSetterAndGetter(UClassProperty)
BpSetterAndGetter(UStrProperty)
BpSetterAndGetter(UNameProperty)
BpSetterAndGetter(UTextProperty)
BpSetterAndGetter(UByteProperty)
BpSetterAndGetter(UEnumProperty)
BpSetterAndGetter(UStructProperty)
BpSetterAndGetter(UMulticastDelegateProperty)
BpSetterAndGetter(UDelegateProperty)
BpSetterAndGetter(UWeakObjectProperty)
BpSetterAndGetter(UArrayProperty)
BpSetterAndGetter(UMapProperty)
BpSetterAndGetter(USetProperty)
BpSetterAndGetter(UInterfaceProperty)


static int32 GetBpClass(lua_State* inL)
{
	void** p = (void**)lua_touserdata(inL, lua_upvalueindex(1));
	UClass* Class = (UClass*)(*p);
	pushuobject(inL, Class);
	return 1;
}

static int32 BpStructGetProp(lua_State* inL)
{
	void** p = (void**)lua_touserdata(inL, lua_upvalueindex(1));
	UProperty* Property = (UProperty*)(*p);
	UTableUtil::pushproperty(inL, Property, tostruct(inL, 1));
	return 1;
}

static int32 BpStructSetProp(lua_State* inL)
{
	void** p = (void**)lua_touserdata(inL, lua_upvalueindex(1));
	UProperty* Property = (UProperty*)(*p);
	UTableUtil::popproperty(inL, 2, Property, tostruct(inL, 1));
	return 0;
}

static int32 BpStructNew(lua_State* inL)
{
	void** p = (void**)lua_touserdata(inL, lua_upvalueindex(1));
	int Size = lua_tointeger(inL, lua_upvalueindex(2));
	const char* name = lua_tostring(inL, lua_upvalueindex(3));
	UScriptStruct* Struct = (UScriptStruct*)(*p);
	uint8* result = (uint8*)FMemory::Malloc(Size);
	Struct->InitializeStruct(result);
	pushstruct_gc(inL, name, result);
	return 1;
}

static int32 BpStructTemp(lua_State* inL)
{
	void** p = (void**)lua_touserdata(inL, lua_upvalueindex(1));
	int Size = lua_tointeger(inL, lua_upvalueindex(2));
	const char* name = lua_tostring(inL, lua_upvalueindex(3));
	const char* name_nogc = lua_tostring(inL, lua_upvalueindex(4));
	UScriptStruct* Struct = (UScriptStruct*)(*p);
	uint8* result = (uint8*)GetBpStructTempIns(name, Size);
	Struct->InitializeStruct(result);
	pushstruct_nogc(inL, name, name_nogc, result);
	return 1;
}

static int32 BpStructDestroy(lua_State* inL)
{
	void* ptr = tostruct(inL, 1);
	void** p = (void**)lua_touserdata(inL, lua_upvalueindex(1));
	UScriptStruct* Struct = (UScriptStruct*)(*p);

	Struct->DestroyStruct(ptr);
	FMemory::Free(ptr);
	return 0;
}

static int32 BpStruct__eq(lua_State* inL)
{
	void* a = tostruct(inL, 1);
	void* b = tostruct(inL, 2);
	if (a == b)
		UTableUtil::push(inL, true);
	else
	{
		void** p = (void**)lua_touserdata(inL, lua_upvalueindex(1));
		UScriptStruct* Struct = (UScriptStruct*)(*p);
		if (Struct->GetCppStructOps())
		{
			bool IsSame;
			Struct->GetCppStructOps()->Identical(a, b, 0, IsSame);
			UTableUtil::push(inL, IsSame);
		}
		else
			UTableUtil::push(inL, false);
	}
	return 1;
}


static int32 BpStructCopy(lua_State* inL)
{
	void** p = (void**)lua_touserdata(inL, lua_upvalueindex(1));
	int Size = lua_tointeger(inL, lua_upvalueindex(2));
	const char* name = lua_tostring(inL, lua_upvalueindex(3));
	UScriptStruct* Struct = (UScriptStruct*)(*p);
	uint8* result = (uint8*)FMemory::Malloc(Size);
	Struct->InitializeStruct(result);
	void* ptr = tostruct(inL, 1);
	Struct->CopyScriptStruct(result, ptr);
	pushstruct_gc(inL, name, result);
	return 1;
}

static int32 GeneralLoadObject(lua_State* inL)
{
	void** p = (void**)lua_touserdata(inL, lua_upvalueindex(1));
	UClass* Class = (UClass*)(*p);
	ensureAlwaysMsgf(Class != nullptr, TEXT("Bug"));
	UObject* obj = (UObject*)touobject(inL, 1);
	FString Path = UTF8_TO_TCHAR(luaL_checkstring(inL, 2));
	UObject* result = StaticLoadObject(Class, obj, *Path);
	ensureAlwaysMsgf(result != nullptr, TEXT("Bug"));
	pushuobject(inL, (void*)result);
	return 1;
}

static int32 GeneralLoadClass(lua_State* inL)
{
	void** p = (void**)lua_touserdata(inL, lua_upvalueindex(1));
	UClass* Class = (UClass*)(*p);
	if (Class == nullptr)
	{
		ensureAlwaysMsgf(0, TEXT("Bug"));
		UnrealLua::ReportError(inL,"GeneralLoadClass Bug1");
	}
	UObject* obj = (UObject*)touobject(inL, 1);
	FString Path = UTF8_TO_TCHAR(luaL_checkstring(inL, 2));
	UObject* result = StaticLoadClass(Class, obj, *Path);
	if (result == nullptr)
	{
		ensureAlwaysMsgf(0, TEXT("Bug"));
		UnrealLua::ReportError(inL,"GeneralLoadClass Bug2");
	}
	pushuobject(inL, (void*)result);
	return 1;
}

static int32 GeneralGetClass(lua_State* inL)
{
	void** p = (void**)lua_touserdata(inL, lua_upvalueindex(1));
	UClass* Class = (UClass*)(*p);
	if (Class == nullptr)
	{
		ensureAlwaysMsgf(0, TEXT("Bug"));
		UnrealLua::ReportError(inL,"GeneralGetClass Bug1");
	}
	pushuobject(inL, (void*)Class);
	return 1;
}

static int32 GeneralGetDefaultObject(lua_State*inL)
{
	void** p = (void**)lua_touserdata(inL, lua_upvalueindex(1));
	UClass* Class = (UClass*)(*p);
	UObject* DefaultObject = Class->GetDefaultObject();
	ensureAlwaysMsgf(DefaultObject != nullptr, TEXT("Bug"));
	if (DefaultObject == nullptr)
	{
		ensureAlwaysMsgf(0, TEXT("Bug"));
		UnrealLua::ReportError(inL,"GeneralGetDefaultObject Bug1");
	}
	pushuobject(inL, DefaultObject);
	return 1;
}

static int32 GeneralNewObject(lua_State*inL)
{
	void** p = (void**)lua_touserdata(inL, lua_upvalueindex(1));
	UClass* Class = (UClass*)(*p);
	ensureAlwaysMsgf(Class != nullptr, TEXT("Bug"));
	UObject* Obj = nullptr;
	int32 len = lua_gettop(inL);
	if (len == 0)
		Obj = NewObject<UObject>(GetTransientPackage(), Class);
	else if (len == 1) {
		UObject* Outer = (UObject*)touobject(inL, 1);
		Obj = NewObject<UObject>(Outer, Class);
	}
	else if (len == 2) {
		UObject* Outer = (UObject*)touobject(inL, 1);
		Class = (UClass*)touobject(inL, 2);
		Obj = NewObject<UObject>(Outer, Class);
	}
	else if (len == 3) {
		UObject* Outer = (UObject*)touobject(inL, 1);
		Class = (UClass*)touobject(inL, 2);
		FName Name = FName(lua_tostring(inL, 3));
		Obj = NewObject<UObject>(Outer, Class, Name);
	}
	pushuobject(inL, (void*)Obj, true);
	return 1;
}


static int32 GeneralCast(lua_State* inL)
{
	void** p = (void**)lua_touserdata(inL, lua_upvalueindex(1));
	UClass* Class = (UClass*)(*p);
	UObject* from = (UObject*)touobject(inL, 1);
#if LuaDebug
	if (from == nullptr)
	{
		ensureAlwaysMsgf(0, TEXT("Bug"));
		return 0;
	}
#endif
	if (from->IsA(Class))
	{
		pushuobject(inL, (void*)from);
		return 1;
	}
	else
		return 0;
}

static int32 GeneralFClassFinder(lua_State* inL)
{
	void** p = (void**)lua_touserdata(inL, lua_upvalueindex(1));
	UClass* Class = (UClass*)(*p);
	FString PathName = lua_tostring(inL, 1);
	UClass* ResultClass = ConstructorHelpersInternal::FindOrLoadClass(PathName, Class);
	pushuobject(inL, (void*)ResultClass);
	return 1;
}
static int32 GetUObjectAllProperty(lua_State* inL)
{
	UObject* Obj = (UObject*)tovoid(inL, 1);
	UClass* Class = Obj->GetClass();
	lua_newtable(inL);
	for (TFieldIterator<UProperty> PropertyIt(Class); PropertyIt; ++PropertyIt)
	{
		UProperty* Property = *PropertyIt;
		UTableUtil::push(inL, Property->GetName());
		UTableUtil::pushproperty(inL, Property, (void*)Obj);
		lua_rawset(inL, -3);
	}
	return 1;
}


static int32 GlobalLoadObject(lua_State* inL)
{
	UObject* obj = (UObject*)touobject(inL, 1);
	FString Path = UTF8_TO_TCHAR(luaL_checkstring(inL, 2));
	UObject* result = LoadObject<UObject>(obj, *Path);
	ensureAlwaysMsgf(result != nullptr, TEXT("Bug"));
	pushuobject(inL, (void*)result);
	return 1;
}

static int32 GlobalLoadClass(lua_State* inL)
{
	UObject* obj = (UObject*)touobject(inL, 1);
	FString Path = UTF8_TO_TCHAR(luaL_checkstring(inL, 2));
	UObject* result = LoadClass<UObject>(obj, *Path);
	ensureAlwaysMsgf(result != nullptr, TEXT("Bug"));
	pushuobject(inL, (void*)result);
	return 1;
}

static int32 GeneralLoadConfig(lua_State*inL)
{
	UObject* Obj = (UObject*)tovoid(inL, 1);
	if (Obj)
	{
		int32 Top = lua_gettop(inL);
		if (Top == 1)
		{
			Obj->LoadConfig();
		}
		UClass* ConfigClass = (UClass*)tovoid(inL, 2);
		if (Top == 2)
		{
			Obj->LoadConfig(ConfigClass);
		}
		FString FileName;
		UTableUtil::read(FileName, inL, 3);
		if (Top == 3)
			Obj->LoadConfig(ConfigClass, *FileName);
		if (Top > 3)
		{
			ensureAlwaysMsgf(0, TEXT("didn't processed"));
		}
	}
	return 0;
}

static int32 GeneralReloadConfig(lua_State*inL)
{
	UObject* Obj = (UObject*)tovoid(inL, 1);
	if (Obj&& lua_gettop(inL) == 1)
	{
		Obj->ReloadConfig();
	}
	else
	{
		ensureAlwaysMsgf(0, TEXT("bug"));
	}
	return 0;
}

static int32 GeneralSaveConfig(lua_State *inL)
{
	UObject* Obj = (UObject*)tovoid(inL, 1);
	if (Obj && lua_gettop(inL) == 1)
	{
		Obj->SaveConfig();
	}
	else
	{
		ensureAlwaysMsgf(0, TEXT("bug"));
	}
	return 0;
}

struct LuaCandicateFuncs
{
	lua_CFunction func;
	UnrealLuaBlueFunc::CheckFuncsType TheCheckFuncs;
};

void FindCandidate(TArray<UnrealLuaBlueFunc>& CandidateFuncs, std::list<int32>& CandidateIndex, UnrealLua::ArgType TheArgType, int32 Index, int32 IndexOffset, lua_State*inL)
{
	for (std::list<int32>::iterator i = CandidateIndex.begin(); i != CandidateIndex.end();)
	{
		UnrealLuaBlueFunc& Candidate = CandidateFuncs[*i];
		if (!Candidate.TheCheckFuncs.CheckFuncs[Index - IndexOffset](TheArgType, inL, Index))
			i = CandidateIndex.erase(i);
		else
			i++;
	}
}

static int32 CallOverLoadFuncs(lua_State*inL)
{
	TArray<UnrealLuaBlueFunc>& CandidateFuncs = *(TArray<UnrealLuaBlueFunc>*) lua_touserdata(inL, lua_upvalueindex(1));;
	std::list<int32> CandidateIndex;
	
	int32 IndexOffset = CandidateFuncs[0].TheCheckFuncs.IndexOffset;

	int StackTop = lua_gettop(inL);
	int ParamCount = StackTop - (IndexOffset-1);
	int Len = CandidateFuncs.Num();
	for (int i = 0; i < Len; i++)
	{
		UnrealLuaBlueFunc& Candidate = CandidateFuncs[i];
		if (Candidate.TheCheckFuncs.ValidParamCount[ParamCount] == true)
			CandidateIndex.push_back(i);
	}
	
	auto CallCandicate = [inL](UnrealLuaBlueFunc& Func)
	{
		return Func.func(inL);
	};

	if (CandidateIndex.size() == 0)
	{
		ensureAlwaysMsgf(0, TEXT("Bug"));
		return 0;
	}
	if (CandidateIndex.size() == 1)
	{
		return CallCandicate(CandidateFuncs[CandidateIndex.front()]);
	}

	for (int i = IndexOffset; i <= StackTop; i++)
	{
		UnrealLua::ArgType TheArgType = UnrealLua::GetArgType(inL, i);
		FindCandidate(CandidateFuncs, CandidateIndex, TheArgType, i, IndexOffset, inL);
		if (CandidateIndex.size() == 1)
		{
			return CallCandicate(CandidateFuncs[CandidateIndex.front()]);
		}
	}
	if (CandidateIndex.size() >= 1)
	{
#if LuaDebug
		if (CandidateIndex.size() > 1)
		{
			ensureAlwaysMsgf(0, TEXT("ambious call"));
		}
#endif
		return CallCandicate(CandidateFuncs[CandidateIndex.front()]);
	}
	else
	{
		ensureAlwaysMsgf(0, TEXT("Bug"));
		return 0;
	}
}