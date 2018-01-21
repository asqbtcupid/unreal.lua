#include "tableutil.h"
#include "WidgetTransform.lua.h"
#include "EventReply.lua.h"
#include "SlateChildSize.lua.h"
#include "PropertyPathSegment.lua.h"
#include "DynamicPropertyPath.lua.h"
#include "DelegateRuntimeBinding.lua.h"
#include "PaintContext.lua.h"
#include "NamedSlotBinding.lua.h"
#include "WidgetAnimationBinding.lua.h"
#include "WidgetNavigationData.lua.h"
#include "AnchorData.lua.h"
#include "ShapedTextOptions.lua.h"
#include "SlateMeshVertex.lua.h"
struct lua_static_load_UMG_ustruct_all_struct
{
	static void load()
	{
		UTableUtil::loadstruct(FWidgetTransform_Lib, "FWidgetTransform");
		UTableUtil::loadstruct(FEventReply_Lib, "FEventReply");
		UTableUtil::loadstruct(FSlateChildSize_Lib, "FSlateChildSize");
		UTableUtil::loadstruct(FPropertyPathSegment_Lib, "FPropertyPathSegment");
		UTableUtil::loadstruct(FDynamicPropertyPath_Lib, "FDynamicPropertyPath");
		UTableUtil::loadstruct(FDelegateRuntimeBinding_Lib, "FDelegateRuntimeBinding");
		UTableUtil::loadstruct(FPaintContext_Lib, "FPaintContext");
		UTableUtil::loadstruct(FNamedSlotBinding_Lib, "FNamedSlotBinding");
		UTableUtil::loadstruct(FWidgetAnimationBinding_Lib, "FWidgetAnimationBinding");
		UTableUtil::loadstruct(FWidgetNavigationData_Lib, "FWidgetNavigationData");
		UTableUtil::loadstruct(FAnchorData_Lib, "FAnchorData");
		UTableUtil::loadstruct(FShapedTextOptions_Lib, "FShapedTextOptions");
		UTableUtil::loadstruct(FSlateMeshVertex_Lib, "FSlateMeshVertex");
	}
	lua_static_load_UMG_ustruct_all_struct(){UTableUtil::GetInitDelegates().AddStatic(&load);}
};
static lua_static_load_UMG_ustruct_all_struct lua_UMG_ustruct_all_static_var;
void FixLinkFunc_UMG_ustruct_all(){};
