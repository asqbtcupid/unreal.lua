#include "TableUtil.h"
#include "FoliageInstancedStaticMeshComponent.lua.h"
#include "FoliageStatistics.lua.h"
#include "FoliageType.lua.h"
#include "InstancedFoliageActor.lua.h"
#include "InteractiveFoliageActor.lua.h"
#include "InteractiveFoliageComponent.lua.h"
#include "ProceduralFoliageBlockingVolume.lua.h"
#include "ProceduralFoliageComponent.lua.h"
#include "ProceduralFoliageSpawner.lua.h"
#include "ProceduralFoliageTile.lua.h"
#include "ProceduralFoliageVolume.lua.h"
#include "FoliageType_InstancedStaticMesh.lua.h"
struct lua_static_load_Foliage_uclass_all_struct
{
	static void load()
	{
		UTableUtil::loadlib(FoliageInstancedStaticMeshComponent_Lib, "UFoliageInstancedStaticMeshComponent");
		UTableUtil::loadlib(FoliageStatistics_Lib, "UFoliageStatistics");
		UTableUtil::loadlib(FoliageType_Lib, "UFoliageType");
		UTableUtil::loadlib(InstancedFoliageActor_Lib, "AInstancedFoliageActor");
		UTableUtil::loadlib(InteractiveFoliageActor_Lib, "AInteractiveFoliageActor");
		UTableUtil::loadlib(InteractiveFoliageComponent_Lib, "UInteractiveFoliageComponent");
		UTableUtil::loadlib(ProceduralFoliageBlockingVolume_Lib, "AProceduralFoliageBlockingVolume");
		UTableUtil::loadlib(ProceduralFoliageComponent_Lib, "UProceduralFoliageComponent");
		UTableUtil::loadlib(ProceduralFoliageSpawner_Lib, "UProceduralFoliageSpawner");
		UTableUtil::loadlib(ProceduralFoliageTile_Lib, "UProceduralFoliageTile");
		UTableUtil::loadlib(ProceduralFoliageVolume_Lib, "AProceduralFoliageVolume");
		UTableUtil::loadlib(FoliageType_InstancedStaticMesh_Lib, "UFoliageType_InstancedStaticMesh");
	}
	lua_static_load_Foliage_uclass_all_struct(){UTableUtil::GetInitDelegates().AddStatic(&load);}
};
static lua_static_load_Foliage_uclass_all_struct lua_Foliage_uclass_all_static_var;
void FixLinkFunc_Foliage_uclass_all(){};
