#include "TableUtil.h"
#include "CMPlayerControllerBase.lua.h"
struct lua_static_load_CMPlayerControllerBase_struct
{
	static void load()
	{
		UTableUtil::loadlib(CMPlayerControllerBase_Lib, "ACMPlayerControllerBase");
	}
	lua_static_load_CMPlayerControllerBase_struct(){UTableUtil::GetInitDelegates().AddStatic(&load);}
};
static lua_static_load_CMPlayerControllerBase_struct lua_CMPlayerControllerBase_static_var;
void FixLinkFunc_CatchMe_CMPlayerControllerBase(){};
