#include "tableutil.h"
#include "CharacterInfo.lua.h"
struct lua_static_load_CharacterInfo_struct
{
	static void load()
	{
		UTableUtil::loadstruct(FCharacterInfo_Lib, "FCharacterInfo");
	}
	lua_static_load_CharacterInfo_struct(){UTableUtil::GetInitDelegates().AddStatic(&load);}
};
static lua_static_load_CharacterInfo_struct lua_CharacterInfo_static_var;
void FixLinkFunc_CatchMe_CharacterInfo(){};
