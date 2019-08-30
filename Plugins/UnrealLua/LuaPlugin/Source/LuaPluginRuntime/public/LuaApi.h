// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#ifdef USE_LUA53 
#include "fix_lua.hpp"
#define lua_objlen lua_rawlen 
#else
#include "lua_tinker.h"
#endif

const int32 CppInstanceIndex = 1;
const int32 ExistTableIndex = LUA_RIDX_LAST + 1;

#ifdef USE_LUA53 
#define ue_lua_gettop lua_gettop
#define ue_lua_close lua_close
#define ue_lua_gettop lua_gettop
#define ue_lua_settop lua_settop
#define ue_lua_pushvalue lua_pushvalue
#define ue_lua_remove lua_remove
#define ue_lua_insert lua_insert
#define ue_lua_replace lua_replace
#define ue_lua_tonumber lua_tonumber
#define ue_lua_tointeger lua_tointeger
#define ue_lua_toboolean lua_toboolean
#define ue_lua_tolstring lua_tolstring
#define ue_lua_objlen lua_objlen
#define ue_lua_pushnil lua_pushnil
#define ue_lua_pushnumber lua_pushnumber
#define ue_lua_pushinteger lua_pushinteger
#define ue_lua_pushlstring lua_pushlstring
#define ue_lua_pushstring lua_pushstring
#define ue_lua_pushboolean lua_pushboolean
#define ue_lua_gettable lua_gettable
#define ue_lua_getfield lua_getfield
#define ue_lua_rawget lua_rawget
#define ue_lua_rawgeti lua_rawgeti
#define ue_lua_createtable lua_createtable
#define ue_lua_rawset lua_rawset
#define ue_lua_rawseti lua_rawseti
#define ue_lua_setmetatable lua_setmetatable
#define ue_lua_pcall lua_pcall
#define ue_lua_pushcclosure lua_pushcclosure
#define ue_lua_next lua_next
#define ue_lua_error lua_error
#define ue_lua_type lua_type
#define ue_lua_newuserdata lua_newuserdata
#define ue_lua_getmetatable lua_getmetatable
#define ue_lua_isuserdata lua_isuserdata
#define ue_lua_getglobal lua_getglobal
#define ue_lua_isinteger lua_isinteger
#define ue_lua_isnumber lua_isnumber
#define ue_lua_isstring lua_isstring
#define ue_lua_isfunction lua_isfunction
#define ue_lua_iscfunction lua_iscfunction
#define ue_lua_touserdata lua_touserdata
#define ue_lua_pushlightuserdata lua_pushlightuserdata
#define ue_lua_settable lua_settable
#define ue_lua_gettable lua_gettable
#define ue_lua_call lua_call
#define ue_lua_rawequal lua_rawequal

#define ue_lua_pop(L,n)		ue_lua_settop(L, -(n)-1)
#define ue_lua_newtable(L)		ue_lua_createtable(L, 0, 0)
#define ue_lua_pushcfunction(L,f)	ue_lua_pushcclosure(L, (f), 0)
#define ue_lua_tostring(L,i)	ue_lua_tolstring(L, (i), NULL)
#define ue_lua_istable(L,n)	(ue_lua_type(L, (n)) == LUA_TTABLE)
#define ue_lua_islightuserdata(L,n)	(ue_lua_type(L, (n)) == LUA_TLIGHTUSERDATA)
#define ue_lua_isnil(L,n)		(ue_lua_type(L, (n)) == LUA_TNIL)
#else
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
LUAPLUGINRUNTIME_API lua_Integer (ue_lua_tointeger) (lua_State *L, int idx);
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
LUAPLUGINRUNTIME_API void  (ue_lua_rawseti)(lua_State *L, int idx, int i);
LUAPLUGINRUNTIME_API int   (ue_lua_setmetatable)(lua_State *L, int objindex);
LUAPLUGINRUNTIME_API int   (ue_lua_pcall)(lua_State *L, int nargs, int nresults, int errfunc);
LUAPLUGINRUNTIME_API void  (ue_lua_pushcclosure)(lua_State *L, lua_CFunction fn, int n);
LUAPLUGINRUNTIME_API int   (ue_lua_next)(lua_State *L, int idx);
LUAPLUGINRUNTIME_API int   (ue_lua_error)(lua_State *L);
LUAPLUGINRUNTIME_API int   (ue_lua_type)(lua_State *L, int idx);
LUAPLUGINRUNTIME_API void*   (ue_lua_newuserdata)(lua_State *L, size_t sz);
LUAPLUGINRUNTIME_API int   (ue_lua_getmetatable)(lua_State *L, int objindex);
LUAPLUGINRUNTIME_API int ue_lua_isuserdata(lua_State *L, int idx);
LUAPLUGINRUNTIME_API void (ue_lua_getglobal)(lua_State *L, const char *name);
LUAPLUGINRUNTIME_API int ue_lua_isinteger(lua_State *L, int idx);
LUAPLUGINRUNTIME_API int ue_lua_isnumber(lua_State *L, int idx);
LUAPLUGINRUNTIME_API int ue_lua_isstring(lua_State *L, int idx);
LUAPLUGINRUNTIME_API bool ue_lua_isfunction(lua_State *L, int n);
LUAPLUGINRUNTIME_API bool ue_lua_iscfunction(lua_State *L, int n);
LUAPLUGINRUNTIME_API void* ue_lua_touserdata(lua_State *L, int idx);
LUAPLUGINRUNTIME_API void ue_lua_pushlightuserdata(lua_State *L, void *p);

#define ue_lua_pop(L,n)		ue_lua_settop(L, -(n)-1)
#define ue_lua_newtable(L)		ue_lua_createtable(L, 0, 0)
#define ue_lua_pushcfunction(L,f)	ue_lua_pushcclosure(L, (f), 0)
#define ue_lua_tostring(L,i)	ue_lua_tolstring(L, (i), NULL)
#define ue_lua_istable(L,n)	(ue_lua_type(L, (n)) == LUA_TTABLE)
#define ue_lua_islightuserdata(L,n)	(ue_lua_type(L, (n)) == LUA_TLIGHTUSERDATA)
#define ue_lua_isnil(L,n)		(ue_lua_type(L, (n)) == LUA_TNIL)
#endif
