#include "tableutil.h"
#include "DebuggerVarNode.lua.h"
#include "BreakPointNode.lua.h"
struct lua_static_load_LuaDebugger_ustruct_all_struct
{
	static void load()
	{
		UTableUtil::loadstruct(FDebuggerVarNode_Lib, "FDebuggerVarNode");
		UTableUtil::loadstruct(FBreakPointNode_Lib, "FBreakPointNode");
	}
	lua_static_load_LuaDebugger_ustruct_all_struct(){UTableUtil::GetInitDelegates().AddStatic(&load);}
};
static lua_static_load_LuaDebugger_ustruct_all_struct lua_LuaDebugger_ustruct_all_static_var;
void FixLinkFunc_LuaDebugger_ustruct_all(){};
