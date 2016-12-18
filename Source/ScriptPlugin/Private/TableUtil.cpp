// Fill out your copyright notice in the Description page of Project Settings.

// #include "TestCamera.h"
#include "ScriptPluginPrivatePCH.h"
#include "TableUtil.h"
lua_State* UTableUtil::L = nullptr;
void UTableUtil::init()
{
	if (L != nullptr)
		lua_close(L);
	auto l = lua_open();
	luaL_openlibs(l);
	L = l;
	if (luaL_dofile(l, "G:\\luacode\\main.lua"))
	{
		int i = 10;
	}
}

void UTableUtil::initmeta()
{
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);
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
	lua_rawset(L, -3);
	lua_pop(L, 1);
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
	auto u = static_cast<void**>(lua_touserdata(L, i));
	return *u;
}

int UTableUtil::toint(int i)
{
	return lua_tointeger(L, i);
}


void UTableUtil::push(const char* classname, void* p)
{
	*(void**)lua_newuserdata(L, sizeof(void *)) = p;
	luaL_getmetatable(L, classname);
	if (lua_istable(L, -1))
		lua_setmetatable(L, -2);
	else
	{
		UTableUtil::addmodule(classname);
		luaL_getmetatable(L, classname);
		lua_setmetatable(L, -2);
	}
}

UTableUtil::UTableUtil()
{
	
}

int32 UTableUtil::test()
{
	return 5;
}

int UTableUtil::tick(float delta)
{
	lua_pushvalue(L, LUA_GLOBALSINDEX);
	lua_pushstring(L, "tick");
	lua_rawget(L, -2);
	lua_pushnumber(L, delta);
	lua_call(L, 1, 1);
	int result = lua_tointeger(L, -1);
	lua_pop(L, 2);
	return result;
}
	
