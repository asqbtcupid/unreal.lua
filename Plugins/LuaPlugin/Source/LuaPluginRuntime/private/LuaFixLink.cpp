#include "LuaFixLink.h"
extern void FixLinkFunc_CoreUObject_uclass_all();
extern void FixLinkFunc_InputCore_uclass_all();
extern void FixLinkFunc_AssetRegistry_uclass_all();
extern void FixLinkFunc_Engine_uclass_all();
extern void FixLinkFunc_UMG_uclass_all();
extern void FixLinkFunc_Foliage_uclass_all();
extern void FixLinkFunc_AIModule_uclass_all();
extern void FixLinkFunc_VarWatcher_uclass_all();
extern void FixLinkFunc_LuaPluginRuntime_uclass_all();
extern void FixLinkFunc_CoreUObject_ustruct_all();
extern void FixLinkFunc_InputCore_ustruct_all();
extern void FixLinkFunc_AssetRegistry_ustruct_all();
extern void FixLinkFunc_Engine_ustruct_all();
extern void FixLinkFunc_UMG_ustruct_all();
extern void FixLinkFunc_Foliage_ustruct_all();
extern void FixLinkFunc_AIModule_ustruct_all();
extern void FixLinkFunc_LuaPluginRuntime_ustruct_all();
extern void FixLinkFunc_uenum_all();
void LuaFixLink()
{
	FixLinkFunc_CoreUObject_uclass_all();
	FixLinkFunc_InputCore_uclass_all();
	FixLinkFunc_AssetRegistry_uclass_all();
	FixLinkFunc_Engine_uclass_all();
	FixLinkFunc_UMG_uclass_all();
	FixLinkFunc_Foliage_uclass_all();
	FixLinkFunc_AIModule_uclass_all();
	FixLinkFunc_VarWatcher_uclass_all();
	FixLinkFunc_LuaPluginRuntime_uclass_all();
	FixLinkFunc_CoreUObject_ustruct_all();
	FixLinkFunc_InputCore_ustruct_all();
	FixLinkFunc_AssetRegistry_ustruct_all();
	FixLinkFunc_Engine_ustruct_all();
	FixLinkFunc_UMG_ustruct_all();
	FixLinkFunc_Foliage_ustruct_all();
	FixLinkFunc_AIModule_ustruct_all();
	FixLinkFunc_LuaPluginRuntime_ustruct_all();
	FixLinkFunc_uenum_all();
}