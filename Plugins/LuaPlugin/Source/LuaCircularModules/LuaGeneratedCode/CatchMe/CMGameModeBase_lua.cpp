#include "TableUtil.h"
#include "CMGameModeBase.lua.h"
struct lua_static_load_CMGameModeBase_struct
{
	static void load()
	{
		UTableUtil::loadlib(CMGameModeBase_Lib, "ACMGameModeBase");
	}
	lua_static_load_CMGameModeBase_struct(){UTableUtil::GetInitDelegates().AddStatic(&load);}
};
static lua_static_load_CMGameModeBase_struct lua_CMGameModeBase_static_var;
void FixLinkFunc_CatchMe_CMGameModeBase(){};
