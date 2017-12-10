#include "tableutil.h"
#include "CatchMeGameMode.lua.h"
struct lua_static_load_CatchMeGameMode_struct
{
	static void load()
	{
		UTableUtil::loadlib(CatchMeGameMode_Lib, "ACatchMeGameMode");
	}
	lua_static_load_CatchMeGameMode_struct(){UTableUtil::GetInitDelegates().AddStatic(&load);}
};
static lua_static_load_CatchMeGameMode_struct lua_CatchMeGameMode_static_var;
void FixLinkFunc_CatchMe_CatchMeGameMode(){};
