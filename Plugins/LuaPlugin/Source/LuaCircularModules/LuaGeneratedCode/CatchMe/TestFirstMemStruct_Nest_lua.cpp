#include "TableUtil.h"
#include "TestFirstMemStruct_Nest.lua.h"
struct lua_static_load_TestFirstMemStruct_Nest_struct
{
	static void load()
	{
		UTableUtil::loadstruct(FTestFirstMemStruct_Nest_Lib, "FTestFirstMemStruct_Nest");
	}
	lua_static_load_TestFirstMemStruct_Nest_struct(){UTableUtil::GetInitDelegates().AddStatic(&load);}
};
static lua_static_load_TestFirstMemStruct_Nest_struct lua_TestFirstMemStruct_Nest_static_var;
void FixLinkFunc_CatchMe_TestFirstMemStruct_Nest(){};
