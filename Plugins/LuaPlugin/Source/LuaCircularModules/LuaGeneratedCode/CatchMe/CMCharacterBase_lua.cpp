#include "TableUtil.h"
#include "CMCharacterBase.lua.h"
struct lua_static_load_CMCharacterBase_struct
{
	static void load()
	{
		UTableUtil::loadlib(CMCharacterBase_Lib, "ACMCharacterBase");
	}
	lua_static_load_CMCharacterBase_struct(){UTableUtil::GetInitDelegates().AddStatic(&load);}
};
static lua_static_load_CMCharacterBase_struct lua_CMCharacterBase_static_var;
void FixLinkFunc_CatchMe_CMCharacterBase(){};
