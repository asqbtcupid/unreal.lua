#include "TableUtil.h"
#include "ForTestInterface.lua.h"
struct lua_static_load_ForTestInterface_struct
{
	static void load()
	{
		UTableUtil::loadlib(ForTestInterface_Lib, "IForTestInterface");
	}
	lua_static_load_ForTestInterface_struct(){UTableUtil::GetInitDelegates().AddStatic(&load);}
};
static lua_static_load_ForTestInterface_struct lua_ForTestInterface_static_var;
void FixLinkFunc_CatchMe_ForTestInterface(){};
