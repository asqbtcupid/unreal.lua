#include "tableutil.h"
#include "InputCoreTypes.lua.h"
struct lua_static_load_InputCore_uclass_all_struct
{
	static void load()
	{
		UTableUtil::loadlib(InputCoreTypes_Lib, "UInputCoreTypes");
	}
	lua_static_load_InputCore_uclass_all_struct(){UTableUtil::GetInitDelegates().AddStatic(&load);}
};
static lua_static_load_InputCore_uclass_all_struct lua_InputCore_uclass_all_static_var;
void FixLinkFunc_InputCore_uclass_all(){};
