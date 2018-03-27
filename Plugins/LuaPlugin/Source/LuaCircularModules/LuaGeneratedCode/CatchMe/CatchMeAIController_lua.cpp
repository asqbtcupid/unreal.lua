#include "TableUtil.h"
#include "CatchMeAIController.lua.h"
struct lua_static_load_CatchMeAIController_struct
{
	static void load()
	{
		UTableUtil::loadlib(CatchMeAIController_Lib, "ACatchMeAIController");
	}
	lua_static_load_CatchMeAIController_struct(){UTableUtil::GetInitDelegates().AddStatic(&load);}
};
static lua_static_load_CatchMeAIController_struct lua_CatchMeAIController_static_var;
void FixLinkFunc_CatchMe_CatchMeAIController(){};
