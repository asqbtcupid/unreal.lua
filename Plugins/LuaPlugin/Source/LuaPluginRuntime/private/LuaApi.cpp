#include "LuaApi.h"

int ue_lua_gettop(lua_State*L)
{
	return lua_gettop(L);
}
void       ue_lua_close(lua_State *L)
{
	return lua_close(L);
}

void  ue_lua_settop(lua_State *L, int idx)
{
	return lua_settop(L, idx);
}
void  ue_lua_pushvalue(lua_State *L, int idx)
{
	return lua_pushvalue(L, idx);
}
void  ue_lua_remove(lua_State *L, int idx)
{
	return lua_remove(L, idx);
}
void  ue_lua_insert(lua_State *L, int idx)
{
	return lua_insert(L, idx);
}
void  ue_lua_replace(lua_State *L, int idx)
{
	return lua_replace(L, idx);
}
lua_Number      ue_lua_tonumber(lua_State *L, int idx)
{
	return lua_tonumber(L, idx);
}
int     ue_lua_tointeger(lua_State *L, int idx)
{
	return (int)lua_tointeger(L, idx);
}
int             ue_lua_toboolean(lua_State *L, int idx)
{
	return lua_toboolean(L, idx);
}
size_t          ue_lua_objlen(lua_State *L, int idx)
{
	return lua_objlen(L, idx);
}
void  ue_lua_pushnil(lua_State *L)
{
	return lua_pushnil(L);
}
void  ue_lua_pushnumber(lua_State *L, lua_Number n)
{
	return lua_pushnumber(L, n);
}
void  ue_lua_pushinteger(lua_State *L, lua_Integer n)
{
	return lua_pushinteger(L, n);
}
void  ue_lua_pushlstring(lua_State *L, const char *s, size_t l)
{
	return lua_pushlstring(L, s, l);
}
void  ue_lua_pushstring(lua_State *L, const char *s)
{
	return lua_pushstring(L, s);
}
void  ue_lua_pushboolean(lua_State *L, int b)
{
	return lua_pushboolean(L, b);
}
void  ue_lua_gettable(lua_State *L, int idx)
{
	return lua_gettable(L, idx);
}
void  ue_lua_getfield(lua_State *L, int idx, const char *k)
{
	return lua_getfield(L, idx, k);
}
void  ue_lua_rawget(lua_State *L, int idx)
{
	return lua_rawget(L, idx);
}
void  ue_lua_rawgeti(lua_State *L, int idx, int n)
{
	return lua_rawgeti(L, idx, n);
}
void  ue_lua_createtable(lua_State *L, int narr, int nrec)
{
	return lua_createtable(L, narr, nrec);
}
void  ue_lua_rawset(lua_State *L, int idx)
{
	return lua_rawset(L, idx);
}

void  ue_lua_rawseti(lua_State *L, int idx, int i)
{
	return lua_rawseti(L, idx, i);
}
int   ue_lua_setmetatable(lua_State *L, int objindex)
{
	return lua_setmetatable(L, objindex);
}
int   ue_lua_setfenv(lua_State *L, int idx)
{
	return lua_setfenv(L, idx);
}
int   ue_lua_pcall(lua_State *L, int nargs, int nresults, int errfunc)
{
	return lua_pcall(L, nargs, nresults, errfunc);
}
const char *ue_lua_tolstring(lua_State *L, int idx, size_t *len)
{
	return lua_tolstring(L, idx, len);
}
void  ue_lua_pushcclosure(lua_State *L, lua_CFunction fn, int n)
{
	return lua_pushcclosure(L, fn, n);
}

int   ue_lua_next(lua_State *L, int idx)
{
	return lua_next(L, idx);
}
int   ue_lua_error(lua_State *L)
{
	return lua_error(L);
}

int   ue_lua_type(lua_State *L, int idx)
{
	return lua_type(L, idx);
}
