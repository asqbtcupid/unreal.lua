#include "tableutil.h"
#include "CatchMeGameInstance.lua.h"
struct lua_static_load_CatchMeGameInstance_struct
{
	static void load()
	{
		UTableUtil::loadlib(CatchMeGameInstance_Lib, "UCatchMeGameInstance");
	}
	lua_static_load_CatchMeGameInstance_struct(){UTableUtil::GetInitDelegates().AddStatic(&load);}
};
static lua_static_load_CatchMeGameInstance_struct lua_CatchMeGameInstance_static_var;
void FixLinkFunc_CatchMe_CatchMeGameInstance(){};
