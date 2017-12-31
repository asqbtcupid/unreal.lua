#include "tableutil.h"
#include "SkillInfo.lua.h"
struct lua_static_load_SkillInfo_struct
{
	static void load()
	{
		UTableUtil::loadstruct(FSkillInfo_Lib, "FSkillInfo");
	}
	lua_static_load_SkillInfo_struct(){UTableUtil::GetInitDelegates().AddStatic(&load);}
};
static lua_static_load_SkillInfo_struct lua_SkillInfo_static_var;
void FixLinkFunc_CatchMe_SkillInfo(){};
