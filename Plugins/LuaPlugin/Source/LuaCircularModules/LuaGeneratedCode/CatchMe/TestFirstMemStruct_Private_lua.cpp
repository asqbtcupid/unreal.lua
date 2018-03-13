#include "tableutil.h"
#include "TestFirstMemStruct_Private.lua.h"
struct lua_static_load_TestFirstMemStruct_Private_struct
{
	static void load()
	{
		UTableUtil::loadstruct(FTestFirstMemStruct_Private_Lib, "FTestFirstMemStruct_Private");
	}
	lua_static_load_TestFirstMemStruct_Private_struct(){UTableUtil::GetInitDelegates().AddStatic(&load);}
};
static lua_static_load_TestFirstMemStruct_Private_struct lua_TestFirstMemStruct_Private_static_var;
void FixLinkFunc_CatchMe_TestFirstMemStruct_Private(){};
