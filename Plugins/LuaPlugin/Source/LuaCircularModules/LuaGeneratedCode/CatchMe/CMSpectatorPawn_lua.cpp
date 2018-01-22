#include "tableutil.h"
#include "CMSpectatorPawn.lua.h"
struct lua_static_load_CMSpectatorPawn_struct
{
	static void load()
	{
		UTableUtil::loadlib(CMSpectatorPawn_Lib, "ACMSpectatorPawn");
	}
	lua_static_load_CMSpectatorPawn_struct(){UTableUtil::GetInitDelegates().AddStatic(&load);}
};
static lua_static_load_CMSpectatorPawn_struct lua_CMSpectatorPawn_static_var;
void FixLinkFunc_CatchMe_CMSpectatorPawn(){};
