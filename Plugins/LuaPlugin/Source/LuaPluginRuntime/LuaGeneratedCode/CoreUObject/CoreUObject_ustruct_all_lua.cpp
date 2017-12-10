#include "tableutil.h"
#include "Guid.lua.h"
#include "Vector.lua.h"
#include "Vector4.lua.h"
#include "Vector2D.lua.h"
#include "TwoVectors.lua.h"
#include "Plane.lua.h"
#include "Rotator.lua.h"
#include "Quat.lua.h"
#include "PackedNormal.lua.h"
#include "IntPoint.lua.h"
#include "IntVector.lua.h"
#include "Color.lua.h"
#include "LinearColor.lua.h"
#include "Box.lua.h"
#include "Box2D.lua.h"
#include "BoxSphereBounds.lua.h"
#include "OrientedBox.lua.h"
#include "Matrix.lua.h"
#include "Transform.lua.h"
struct lua_static_load_CoreUObject_ustruct_all_struct
{
	static void load()
	{
		UTableUtil::loadstruct(FGuid_Lib, "FGuid");
		UTableUtil::loadstruct(FVector_Lib, "FVector");
		UTableUtil::loadstruct(FVector4_Lib, "FVector4");
		UTableUtil::loadstruct(FVector2D_Lib, "FVector2D");
		UTableUtil::loadstruct(FTwoVectors_Lib, "FTwoVectors");
		UTableUtil::loadstruct(FPlane_Lib, "FPlane");
		UTableUtil::loadstruct(FRotator_Lib, "FRotator");
		UTableUtil::loadstruct(FQuat_Lib, "FQuat");
		UTableUtil::loadstruct(FPackedNormal_Lib, "FPackedNormal");
		UTableUtil::loadstruct(FIntPoint_Lib, "FIntPoint");
		UTableUtil::loadstruct(FIntVector_Lib, "FIntVector");
		UTableUtil::loadstruct(FColor_Lib, "FColor");
		UTableUtil::loadstruct(FLinearColor_Lib, "FLinearColor");
		UTableUtil::loadstruct(FBox_Lib, "FBox");
		UTableUtil::loadstruct(FBox2D_Lib, "FBox2D");
		UTableUtil::loadstruct(FBoxSphereBounds_Lib, "FBoxSphereBounds");
		UTableUtil::loadstruct(FOrientedBox_Lib, "FOrientedBox");
		UTableUtil::loadstruct(FMatrix_Lib, "FMatrix");
		UTableUtil::loadstruct(FTransform_Lib, "FTransform");
	}
	lua_static_load_CoreUObject_ustruct_all_struct(){UTableUtil::GetInitDelegates().AddStatic(&load);}
};
static lua_static_load_CoreUObject_ustruct_all_struct lua_CoreUObject_ustruct_all_static_var;
void FixLinkFunc_CoreUObject_ustruct_all(){};
