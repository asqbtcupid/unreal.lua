#include "TableUtil.h"
#include "LuaAnimInstance.lua.h"
struct lua_static_load_LuaAnimInstance_struct
{
	static void load()
	{
		UTableUtil::loadlib(LuaAnimInstance_Lib, "ULuaAnimInstance");
	}
	lua_static_load_LuaAnimInstance_struct(){UTableUtil::GetInitDelegates().AddStatic(&load);}
};
static lua_static_load_LuaAnimInstance_struct lua_LuaAnimInstance_static_var;
void FixLinkFunc_CatchMe_LuaAnimInstance(){};
