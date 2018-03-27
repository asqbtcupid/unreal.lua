#include "TableUtil.h"
#include "FoliageVertexColorChannelMask.lua.h"
#include "FoliageTypeObject.lua.h"
#include "ProceduralFoliageInstance.lua.h"
struct lua_static_load_Foliage_ustruct_all_struct
{
	static void load()
	{
		UTableUtil::loadstruct(FFoliageVertexColorChannelMask_Lib, "FFoliageVertexColorChannelMask");
		UTableUtil::loadstruct(FFoliageTypeObject_Lib, "FFoliageTypeObject");
		UTableUtil::loadstruct(FProceduralFoliageInstance_Lib, "FProceduralFoliageInstance");
	}
	lua_static_load_Foliage_ustruct_all_struct(){UTableUtil::GetInitDelegates().AddStatic(&load);}
};
static lua_static_load_Foliage_ustruct_all_struct lua_Foliage_ustruct_all_static_var;
void FixLinkFunc_Foliage_ustruct_all(){};
