#include "tableutil.h"
#include "CMFogMgr_Test.lua.h"
struct lua_static_load_CMFogMgr_Test_struct
{
	static void load()
	{
		UTableUtil::loadlib(CMFogMgr_Test_Lib, "UCMFogMgr_Test");
	}
	lua_static_load_CMFogMgr_Test_struct(){UTableUtil::GetInitDelegates().AddStatic(&load);}
};
static lua_static_load_CMFogMgr_Test_struct lua_CMFogMgr_Test_static_var;
void FixLinkFunc_CatchMe_CMFogMgr_Test(){};
