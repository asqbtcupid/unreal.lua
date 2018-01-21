#include "tableutil.h"
#include "Object.lua.h"
struct lua_static_load_CoreUObject_uclass_all_struct
{
	static void load()
	{
		UTableUtil::loadlib(Object_Lib, "UObject");
	}
	lua_static_load_CoreUObject_uclass_all_struct(){UTableUtil::GetInitDelegates().AddStatic(&load);}
};
static lua_static_load_CoreUObject_uclass_all_struct lua_CoreUObject_uclass_all_static_var;
void FixLinkFunc_CoreUObject_uclass_all(){};
