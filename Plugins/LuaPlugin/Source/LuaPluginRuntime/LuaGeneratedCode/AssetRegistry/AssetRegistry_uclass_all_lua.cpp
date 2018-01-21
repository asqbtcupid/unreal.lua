#include "tableutil.h"
#include "AssetRegistry.lua.h"
#include "AssetRegistryHelpers.lua.h"
#include "AssetRegistryImpl.lua.h"
struct lua_static_load_AssetRegistry_uclass_all_struct
{
	static void load()
	{
		UTableUtil::loadlib(AssetRegistry_Lib, "IAssetRegistry");
		UTableUtil::loadlib(AssetRegistryHelpers_Lib, "UAssetRegistryHelpers");
		UTableUtil::loadlib(AssetRegistryImpl_Lib, "UAssetRegistryImpl");
	}
	lua_static_load_AssetRegistry_uclass_all_struct(){UTableUtil::GetInitDelegates().AddStatic(&load);}
};
static lua_static_load_AssetRegistry_uclass_all_struct lua_AssetRegistry_uclass_all_static_var;
void FixLinkFunc_AssetRegistry_uclass_all(){};
