#include "tableutil.h"
#include "DebuggerSetting.lua.h"
struct lua_static_load_LuaDebugger_uclass_all_struct
{
	static void load()
	{
		UTableUtil::loadlib(DebuggerSetting_Lib, "UDebuggerSetting");
	}
	lua_static_load_LuaDebugger_uclass_all_struct(){UTableUtil::GetInitDelegates().AddStatic(&load);}
};
static lua_static_load_LuaDebugger_uclass_all_struct lua_LuaDebugger_uclass_all_static_var;
void FixLinkFunc_LuaDebugger_uclass_all(){};
