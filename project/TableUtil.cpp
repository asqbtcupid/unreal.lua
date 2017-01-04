// Fill out your copyright notice in the Description page of Project Settings.

// #include "TestCamera.h"
// #include "ScriptPluginPrivatePCH.h"
#include "firstperson_1228.h"
#include "TableUtil.h"
#include "GeneratedScriptLibraries.inl"
lua_State* UTableUtil::L = nullptr;

TMap<FString, TMap<FString, UProperty*>> UTableUtil::propertyMap;
FLuaGcObj UTableUtil::gcobjs;
void UTableUtil::InitClassMap()
{
	for (TObjectIterator<UClass> uClass; uClass; ++uClass)
	{
		UClass* theClass = *uClass;
		FString className = FString::Printf(TEXT("%s%s"), theClass->GetPrefixCPP(), *theClass->GetName());
		TMap<FString, UProperty*> m;
		for (TFieldIterator<UProperty> PropertyIt(theClass); PropertyIt; ++PropertyIt)
		{
			UProperty* Property = *PropertyIt;
			m.Add(Property->GetName(), Property);
		}
		propertyMap.Add(className ,m);
	}
}

UProperty* UTableUtil::GetPropertyByName(FString classname, FString propertyname)
{
	return propertyMap[classname][propertyname];
}

static void* LuaAlloc(void *Ud, void *Ptr, size_t OldSize, size_t NewSize)
{
	if (NewSize != 0)
	{
		return FMemory::Realloc(Ptr, NewSize);
	}
	else
	{
		FMemory::Free(Ptr);
		return NULL;
	}
}

static int32 LuaPanic(lua_State *L)
{
	UTableUtil::log(FString::Printf(TEXT("PANIC: unprotected error in call to Lua API(%s)"), ANSI_TO_TCHAR(lua_tostring(L, -1))));
	return 0;
}

void UTableUtil::init()
{
	InitClassMap();
	if (L != nullptr)
		lua_close(L);
	auto l = lua_newstate(LuaAlloc, nullptr);
	if (l) lua_atpanic(l, &LuaPanic);
	luaL_openlibs(l);
	L = l;
	FString contentDir = FPaths::GameContentDir();
	FString luaDir = contentDir / TEXT("luacode");
	FString mainFilePath = luaDir / TEXT("main.lua");
	if (luaL_dofile(l, TCHAR_TO_ANSI(*mainFilePath)))
	{
		UTableUtil::log(FString(lua_tostring(L, -1)));
	}
	else
	{
		//set table for index exist userdata
		lua_newtable(L);
		lua_newtable(L);
		lua_pushstring(L, "v");
		lua_setfield(L, -2, "__mode");
		lua_setmetatable(L, -2);
		lua_setfield(L, LUA_REGISTRYINDEX, "_existuserdata");
		push(luaDir);
		lua_setfield(L, LUA_GLOBALSINDEX, "_luadir");
		Call_void("Init");
		//register all function
		LuaRegisterExportedClasses(L);

	}
}
void UTableUtil::shutdown()
{
	if (L != nullptr)
	{
		lua_close(L);
		L = nullptr;
	}
}

int32 indexFunc(lua_State* L)
{
	lua_getmetatable(L, 1);
	lua_pushvalue(L, 2);
	lua_rawget(L,-2);
	if (lua_isnil(L, -1))
	{
		FString property = FString(lua_tostring(L, 2));
		lua_pop(L, 1);
		FString propertyKey = FString::Printf(TEXT("Get_%s"), *property);
		UTableUtil::push(propertyKey);
		lua_rawget(L, -2);
		if (!lua_isnil(L, -1))
		{
			lua_pushvalue(L, 1);
			lua_pcall(L, 1, 1, 0);
		}
	}
	return 1;
}

int32 newindexFunc(lua_State* L)
{
	lua_getmetatable(L, 1);
	FString property = FString(lua_tostring(L, 2));
	FString propertyKey = FString::Printf(TEXT("Set_%s"), *property);
	UTableUtil::push(propertyKey);
	lua_rawget(L, -2);
	if (!lua_isnil(L, -1))
	{
		lua_pushvalue(L, 1);
		lua_pushvalue(L, 3);
		lua_call(L, 2, 1);
	}
	return 0;
}

int32 cast(lua_State* L)
{
	if (lua_isnil(L, 2))
	{
		UTableUtil::log(FString("cast error, nil"));
		return 1;
	}
	lua_pushstring(L, "classname");
	lua_rawget(L, 1);
	luaL_getmetatable(L, lua_tostring(L, -1));
	lua_setmetatable(L, 2);
	lua_pushvalue(L, 2);
	return 1;
}

int32 gcfunc(lua_State *L)
{
	auto u = (void**)lua_touserdata(L, -1);
	if (*u != nullptr)
	{
		UTableUtil::rmgcref((UObject*)(*u));
	}
	lua_getmetatable(L, -1);
	lua_pushstring(L, "Destroy");
	lua_gettable(L, -2);
	if (lua_iscfunction(L, -1))
	{
		lua_remove(L, -2);
		lua_pushvalue(L, 1);
		if (lua_pcall(L, 1, 0, 0))
		{
			UTableUtil::log(FString(lua_tostring(L, -1)));
		}
	}
	else
	{
// 		lua_getmetatable(L, 1);
// 		log("memoryleak")
	}
	return 0;
}

void UTableUtil::initmeta()
{
	lua_pushstring(L, "__index");
	lua_pushcfunction(L, indexFunc);
	lua_rawset(L, -3);
	lua_pushstring(L, "cast");
	lua_pushcfunction(L, cast);
	lua_rawset(L, -3);
	lua_pushstring(L, "__newindex");
	lua_pushcfunction(L, newindexFunc);
	lua_rawset(L, -3);
	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, gcfunc);
	lua_rawset(L, -3);
}

void UTableUtil::addmodule(const char* name)
{
	lua_getglobal(L, name);
	if (lua_istable(L, -1))
	{
		lua_pop(L, 1);
		return;
	}
	lua_pushvalue(L, LUA_GLOBALSINDEX);
	lua_pushstring(L, name);
	luaL_newmetatable(L, name);
	initmeta();
	lua_pushstring(L, "classname");
	lua_pushstring(L, name);
	lua_rawset(L, -3);
	lua_rawset(L, -3);
	lua_pop(L, 2);
}

void UTableUtil::openmodule(const char* name)
{
	lua_pushvalue(L, LUA_GLOBALSINDEX);
	lua_pushstring(L, name);
	lua_rawget(L, -2);
}


void UTableUtil::addfunc(const char* name, luafunc f)
{
	lua_pushstring(L, name);
	lua_pushcfunction(L, f);
	lua_rawset(L, -3);
}

void UTableUtil::closemodule()
{
	lua_pop(L, 2);
}

void* UTableUtil::tousertype(const char* classname, int i)
{
	if (lua_isnil(L, i))
		return nullptr;
	else if (lua_istable(L, i))
	{
		lua_pushstring(L,  "_cppinstance_");
		lua_rawget(L, i);
		if (lua_isnil(L, -1))
		{
			lua_pushstring(L, "suck in tousertype");
			lua_error(L);
			return nullptr;
		}
		else
		{
			lua_replace(L, i);
			return tousertype(classname, i);
		}
	}
	else if (lua_isuserdata(L, i))
	{
		auto u = static_cast<void**>(lua_touserdata(L, i));
		return *u;
	}
	else
		return nullptr;
}

int UTableUtil::toint(int i)
{
	return lua_tointeger(L, i);
}


void UTableUtil::push(const char* classname, void* p, bool bgcrecord)
{
	if (p == nullptr)
	{
		lua_pushnil(L);
		return;
	}
	if (!existdata(p))
	{
		if (bgcrecord)
			addgcref((UObject*)p);
		*(void**)lua_newuserdata(L, sizeof(void *)) = p;

		lua_getfield(L, LUA_REGISTRYINDEX, "_existuserdata");
		lua_pushlightuserdata(L, p);
		lua_pushvalue(L, -3);
		lua_rawset(L, -3);
		lua_pop(L, 1);

		luaL_getmetatable(L, classname);
		if (lua_istable(L, -1))
		{
			lua_setmetatable(L, -2);
		}
		else
		{
			lua_pop(L, 1);
			UTableUtil::addmodule(classname);
			luaL_getmetatable(L, classname);
			lua_setmetatable(L, -2);
		}
	}
}

UTableUtil::UTableUtil()
{
	
}

void UTableUtil::loadlib(const luaL_Reg funclist[], const char* classname)
{
	int i = 0;
	UTableUtil::addmodule(classname);
	UTableUtil::openmodule(classname);
	while (true)
	{
		
		luaL_Reg temp = funclist[i];
		if (temp.name == nullptr)
		{
			break;
		}
		else if (temp.func != nullptr)
		{
			UTableUtil::addfunc(temp.name, temp.func);
		}
		i++;
	}
	UTableUtil::closemodule();
	// int j = lua_gettop(L);
	// UE_LOG(LogScriptPlugin, Warning, TEXT("lalala %d"), j);
}

void UTableUtil::loadEnum(const EnumItem list[], const char* enumname)
{
	lua_newtable(L);
	int i = 0;
	while (true)
	{
		EnumItem temp = list[i];
		if (temp.key == nullptr)
			break;
		else
		{
			lua_pushstring(L, temp.key);
			lua_pushinteger(L, temp.value);
			lua_rawset(L, -3);
		}
		++i;
	}
	lua_setfield(L, LUA_GLOBALSINDEX, enumname);
}

void UTableUtil::setpawn(ADefaultPawn *p)
{
	lua_pushvalue(L, LUA_GLOBALSINDEX);
	lua_pushstring(L, "PawnIns");
	push("ADefaultPawn", (void*)p);
	lua_rawset(L, -3);
	lua_pop(L, 1);
}

void UTableUtil::executeFunc(FString funcName)
{
	int nargs = lua_gettop(L);
	lua_pushvalue(L, LUA_GLOBALSINDEX);
	push(funcName);
	lua_rawget(L, -2);
	for (int i = 1; i <= nargs; i++)
	{
		lua_pushvalue(L, i);
	}
	if (lua_pcall(L, nargs, 1, 0))
		log(lua_tostring(L, -1));
}

void UTableUtil::clearStack()
{
	lua_pop(L, lua_gettop(L));
}
// api for blueprint start:
void UTableUtil::Push_obj(UObject *p)
{
	auto Class = p->StaticClass();
	FString ClassName = FString::Printf(TEXT("%s%s"), Class->GetPrefixCPP(), *Class->GetName());
	push(TCHAR_TO_ANSI(*ClassName), (void*)p, true);
}

void UTableUtil::Push_float(float value)
{
	lua_pushnumber(L, value);
}

void UTableUtil::Push_str(FString value)
{
	push(value);
}


void UTableUtil::Call_void(FString funcName)
{
	executeFunc(funcName);
	clearStack();
	// lua_tinker::call<void>(L, TCHAR_TO_ANSI(*funcName));
}

float UTableUtil::Call_float(FString funcName)
{
	executeFunc(funcName);
	auto result = lua_tonumber(L, -1);
	clearStack();
	return result;
}

UObject* UTableUtil::Call_obj(FString funcName)
{
	executeFunc(funcName);
	auto result = (UObject*)tousertype("", -1);
	clearStack();
	return result;
}

FString UTableUtil::Call_str(FString funcName)
{
	executeFunc(funcName);
	FString result = ANSI_TO_TCHAR(luaL_checkstring(L, -1));
	clearStack();
	return result;
}

bool UTableUtil::existdata(void * p)
{
	lua_getfield(L, LUA_REGISTRYINDEX, "_existuserdata");
	lua_pushlightuserdata(L, p);
	lua_rawget(L, -2);
	if (lua_isnil(L, -1))
	{
		lua_pop(L, 2);
		return false;
	}
	else
	{
		lua_replace(L, 1);
		return true;
	}
}

void UTableUtil::log(FString content)
{
	//UE_LOG(LogScriptPlugin, Warning, TEXT("[lua error] %s"), *content);
}

void UTableUtil::addgcref(UObject* p)
{
	gcobjs.objs.Add(p);
}

void UTableUtil::rmgcref(UObject* p)
{
	gcobjs.objs.Remove(p);
}