#include "TableUtil.h"
#include "BPAndLuaBridge.lua.h"
#include "LuaArrayHelper.lua.h"
#include "LuaDelegate.lua.h"
#include "LuaMapHelper.lua.h"
#include "LuaScript.lua.h"
#include "LuaSetHelper.lua.h"
#include "LuaStructHelper.lua.h"
#include "Luautils.lua.h"
#include "TableUtil.lua.h"
struct lua_static_load_LuaPluginRuntime_uclass_all_struct
{
	static void load()
	{
		UTableUtil::loadlib(BPAndLuaBridge_Lib, "UBPAndLuaBridge");
		UTableUtil::loadlib(LuaArrayHelper_Lib, "ULuaArrayHelper");
		UTableUtil::loadlib(LuaDelegate_Lib, "ULuaDelegate");
		UTableUtil::loadlib(LuaMapHelper_Lib, "ULuaMapHelper");
		UTableUtil::loadlib(LuaScript_Lib, "ULuaScript");
		UTableUtil::loadlib(LuaSetHelper_Lib, "ULuaSetHelper");
		UTableUtil::loadlib(LuaStructHelper_Lib, "ULuaStructHelper");
		UTableUtil::loadlib(Luautils_Lib, "ULuautils");
		UTableUtil::loadlib(TableUtil_Lib, "UTableUtil");
	}
	lua_static_load_LuaPluginRuntime_uclass_all_struct(){UTableUtil::GetInitDelegates().AddStatic(&load);}
};
static lua_static_load_LuaPluginRuntime_uclass_all_struct lua_LuaPluginRuntime_uclass_all_static_var;
void FixLinkFunc_LuaPluginRuntime_uclass_all(){};
