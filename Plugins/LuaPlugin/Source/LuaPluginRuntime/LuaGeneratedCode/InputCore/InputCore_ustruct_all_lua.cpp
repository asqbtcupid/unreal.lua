#include "tableutil.h"
#include "Key.lua.h"
struct lua_static_load_InputCore_ustruct_all_struct
{
	static void load()
	{
		UTableUtil::loadstruct(FKey_Lib, "FKey");
	}
	lua_static_load_InputCore_ustruct_all_struct(){UTableUtil::GetInitDelegates().AddStatic(&load);}
};
static lua_static_load_InputCore_ustruct_all_struct lua_InputCore_ustruct_all_static_var;
void FixLinkFunc_InputCore_ustruct_all(){};
