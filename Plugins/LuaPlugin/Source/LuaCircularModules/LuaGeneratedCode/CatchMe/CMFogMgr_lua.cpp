#include "tableutil.h"
#include "CMFogMgr.lua.h"
struct lua_static_load_CMFogMgr_struct
{
	static void load()
	{
		UTableUtil::loadlib(CMFogMgr_Lib, "UCMFogMgr");
	}
	lua_static_load_CMFogMgr_struct(){UTableUtil::GetInitDelegates().AddStatic(&load);}
};
static lua_static_load_CMFogMgr_struct lua_CMFogMgr_static_var;
void FixLinkFunc_CatchMe_CMFogMgr(){};
