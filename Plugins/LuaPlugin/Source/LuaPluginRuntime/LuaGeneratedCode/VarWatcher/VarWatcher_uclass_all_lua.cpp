#include "TableUtil.h"
#include "VarNode.lua.h"
struct lua_static_load_VarWatcher_uclass_all_struct
{
	static void load()
	{
		UTableUtil::loadlib(VarNode_Lib, "UVarNode");
	}
	lua_static_load_VarWatcher_uclass_all_struct(){UTableUtil::GetInitDelegates().AddStatic(&load);}
};
static lua_static_load_VarWatcher_uclass_all_struct lua_VarWatcher_uclass_all_static_var;
void FixLinkFunc_VarWatcher_uclass_all(){};
