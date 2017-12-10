// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "lua_tinker.h"
//gamemodule.dll and LuaPluginRuntime will link lua.lib separately,That will cause bug.
LUAPLUGINRUNTIME_API int ue_lua_gettop(lua_State*L);
LUAPLUGINRUNTIME_API void       (ue_lua_close)(lua_State *L);
LUAPLUGINRUNTIME_API int   (ue_lua_gettop)(lua_State *L);
LUAPLUGINRUNTIME_API void  (ue_lua_settop)(lua_State *L, int idx);
LUAPLUGINRUNTIME_API void  (ue_lua_pushvalue)(lua_State *L, int idx);
LUAPLUGINRUNTIME_API void  (ue_lua_remove)(lua_State *L, int idx);
LUAPLUGINRUNTIME_API void  (ue_lua_insert)(lua_State *L, int idx);
LUAPLUGINRUNTIME_API void  (ue_lua_replace)(lua_State *L, int idx);
LUAPLUGINRUNTIME_API lua_Number(ue_lua_tonumber) (lua_State *L, int idx);
LUAPLUGINRUNTIME_API int	(ue_lua_tointeger) (lua_State *L, int idx);
LUAPLUGINRUNTIME_API int             (ue_lua_toboolean)(lua_State *L, int idx);
LUAPLUGINRUNTIME_API const char     *(ue_lua_tolstring)(lua_State *L, int idx, size_t *len);
LUAPLUGINRUNTIME_API size_t(ue_lua_objlen) (lua_State *L, int idx);
LUAPLUGINRUNTIME_API void  (ue_lua_pushnil)(lua_State *L);
LUAPLUGINRUNTIME_API void  (ue_lua_pushnumber)(lua_State *L, lua_Number n);
LUAPLUGINRUNTIME_API void  (ue_lua_pushinteger)(lua_State *L, lua_Integer n);
LUAPLUGINRUNTIME_API void  (ue_lua_pushlstring)(lua_State *L, const char *s, size_t l);
LUAPLUGINRUNTIME_API void  (ue_lua_pushstring)(lua_State *L, const char *s);
LUAPLUGINRUNTIME_API void  (ue_lua_pushboolean)(lua_State *L, int b);
LUAPLUGINRUNTIME_API void  (ue_lua_gettable)(lua_State *L, int idx);
LUAPLUGINRUNTIME_API void  (ue_lua_getfield)(lua_State *L, int idx, const char *k);
LUAPLUGINRUNTIME_API void  (ue_lua_rawget)(lua_State *L, int idx);
LUAPLUGINRUNTIME_API void  (ue_lua_rawgeti)(lua_State *L, int idx, int n);
LUAPLUGINRUNTIME_API void  (ue_lua_createtable)(lua_State *L, int narr, int nrec);
LUAPLUGINRUNTIME_API void  (ue_lua_rawset)(lua_State *L, int idx);
LUAPLUGINRUNTIME_API int   (ue_lua_setmetatable)(lua_State *L, int objindex);
LUAPLUGINRUNTIME_API int   (ue_lua_setfenv)(lua_State *L, int idx);
LUAPLUGINRUNTIME_API int   (ue_lua_pcall)(lua_State *L, int nargs, int nresults, int errfunc);
LUAPLUGINRUNTIME_API void  (ue_lua_pushcclosure)(lua_State *L, lua_CFunction fn, int n);
LUAPLUGINRUNTIME_API int   (ue_lua_next)(lua_State *L, int idx);
LUAPLUGINRUNTIME_API int   (ue_lua_error)(lua_State *L);
LUAPLUGINRUNTIME_API int   (ue_lua_type)(lua_State *L, int idx);


#define ue_lua_pop(L,n)		ue_lua_settop(L, -(n)-1)
#define ue_lua_newtable(L)		ue_lua_createtable(L, 0, 0)
#define ue_lua_pushcfunction(L,f)	ue_lua_pushcclosure(L, (f), 0)
#define ue_lua_tostring(L,i)	ue_lua_tolstring(L, (i), NULL)
#define ue_lua_istable(L,n)	(ue_lua_type(L, (n)) == LUA_TTABLE)
#define ue_lua_islightuserdata(L,n)	(ue_lua_type(L, (n)) == LUA_TLIGHTUSERDATA)
#define ue_lua_isnil(L,n)		(ue_lua_type(L, (n)) == LUA_TNIL)