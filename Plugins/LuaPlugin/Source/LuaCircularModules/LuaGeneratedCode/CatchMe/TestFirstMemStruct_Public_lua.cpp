#include "tableutil.h"
#include "TestFirstMemStruct_Public.lua.h"
struct lua_static_load_TestFirstMemStruct_Public_struct
{
	static void load()
	{
		UTableUtil::loadstruct(FTestFirstMemStruct_Public_Lib, "FTestFirstMemStruct_Public");
	}
	lua_static_load_TestFirstMemStruct_Public_struct(){UTableUtil::GetInitDelegates().AddStatic(&load);}
};
static lua_static_load_TestFirstMemStruct_Public_struct lua_TestFirstMemStruct_Public_static_var;
void FixLinkFunc_CatchMe_TestFirstMemStruct_Public(){};
