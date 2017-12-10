#include "tableutil.h"
#include "PropertyPathSegment.lua.h"
#include "DynamicPropertyPath.lua.h"
#include "PaintContext.lua.h"
#include "NamedSlotBinding.lua.h"
#include "WidgetNavigationData.lua.h"
#include "ShapedTextOptions.lua.h"
struct lua_static_load_UMG_ustruct_all_struct
{
	static void load()
	{
		UTableUtil::loadstruct(FPropertyPathSegment_Lib, "FPropertyPathSegment");
		UTableUtil::loadstruct(FDynamicPropertyPath_Lib, "FDynamicPropertyPath");
		UTableUtil::loadstruct(FPaintContext_Lib, "FPaintContext");
		UTableUtil::loadstruct(FNamedSlotBinding_Lib, "FNamedSlotBinding");
		UTableUtil::loadstruct(FWidgetNavigationData_Lib, "FWidgetNavigationData");
		UTableUtil::loadstruct(FShapedTextOptions_Lib, "FShapedTextOptions");
	}
	lua_static_load_UMG_ustruct_all_struct(){UTableUtil::GetInitDelegates().AddStatic(&load);}
};
static lua_static_load_UMG_ustruct_all_struct lua_UMG_ustruct_all_static_var;
void FixLinkFunc_UMG_ustruct_all(){};
