#include "TableUtil.h"
#include "ARFilter.lua.h"
#include "AssetBundleEntry.lua.h"
#include "AssetBundleData.lua.h"
#include "AssetData.lua.h"
#include "TagAndValue.lua.h"
struct lua_static_load_AssetRegistry_ustruct_all_struct
{
	static void load()
	{
		UTableUtil::loadstruct(FARFilter_Lib, "FARFilter");
		UTableUtil::loadstruct(FAssetBundleEntry_Lib, "FAssetBundleEntry");
		UTableUtil::loadstruct(FAssetBundleData_Lib, "FAssetBundleData");
		UTableUtil::loadstruct(FAssetData_Lib, "FAssetData");
		UTableUtil::loadstruct(FTagAndValue_Lib, "FTagAndValue");
	}
	lua_static_load_AssetRegistry_ustruct_all_struct(){UTableUtil::GetInitDelegates().AddStatic(&load);}
};
static lua_static_load_AssetRegistry_ustruct_all_struct lua_AssetRegistry_ustruct_all_static_var;
void FixLinkFunc_AssetRegistry_ustruct_all(){};
