#include "TableUtil.h"
#include "lua_State.lua.h"
#include "lua_Index.lua.h"
#include "ReplifetimeCond.lua.h"
struct lua_static_load_LuaPluginRuntime_ustruct_all_struct
{
	static void load()
	{
		UTableUtil::loadstruct(Flua_State_Lib, "Flua_State");
		UTableUtil::loadstruct(Flua_Index_Lib, "Flua_Index");
		UTableUtil::loadstruct(FReplifetimeCond_Lib, "FReplifetimeCond");
	}
	lua_static_load_LuaPluginRuntime_ustruct_all_struct(){UTableUtil::GetInitDelegates().AddStatic(&load);}
};
static lua_static_load_LuaPluginRuntime_ustruct_all_struct lua_LuaPluginRuntime_ustruct_all_static_var;
void FixLinkFunc_LuaPluginRuntime_ustruct_all(){};
