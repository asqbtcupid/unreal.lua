#include "tableutil.h"
#include "CatchMeCharacter.lua.h"
struct lua_static_load_CatchMeCharacter_struct
{
	static void load()
	{
		UTableUtil::loadlib(CatchMeCharacter_Lib, "ACatchMeCharacter");
	}
	lua_static_load_CatchMeCharacter_struct(){UTableUtil::GetInitDelegates().AddStatic(&load);}
};
static lua_static_load_CatchMeCharacter_struct lua_CatchMeCharacter_static_var;
void FixLinkFunc_CatchMe_CatchMeCharacter(){};
