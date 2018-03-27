#include "TableUtil.h"
#include "CatchMePlayerController.lua.h"
struct lua_static_load_CatchMePlayerController_struct
{
	static void load()
	{
		UTableUtil::loadlib(CatchMePlayerController_Lib, "ACatchMePlayerController");
	}
	lua_static_load_CatchMePlayerController_struct(){UTableUtil::GetInitDelegates().AddStatic(&load);}
};
static lua_static_load_CatchMePlayerController_struct lua_CatchMePlayerController_static_var;
void FixLinkFunc_CatchMe_CatchMePlayerController(){};
