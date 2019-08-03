// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
#include "ILuaManuallyGlue.h"
#include "JustForLinkGlue.h"
#include "UnrealLua.h"
#include "CoreMinimal.h"

void UJustForLinkGlue::LinkFixFunctin1()
{
}



#define PushDefineToLua(TheMacroPush) UTableUtil::push(inL, true);lua_setglobal(inL, #TheMacroPush);

struct RegisterDefineToLua_Struct
{
	static void load(lua_State*inL)
	{
#if PLATFORM_WINDOWS
		PushDefineToLua(PLATFORM_WINDOWS);
#endif
#if PLATFORM_ANDROID
		PushDefineToLua(PLATFORM_ANDROID);
#endif
#if PLATFORM_IOS
		PushDefineToLua(PLATFORM_IOS);
#endif
#if PLATFORM_IOS
		PushDefineToLua(PLATFORM_IOS);
#endif
#if PLATFORM_MAC
		PushDefineToLua(PLATFORM_MAC);
#endif
#if UE_BUILD_SHIPPING
		PushDefineToLua(UE_BUILD_SHIPPING);
#endif
	}

	RegisterDefineToLua_Struct()
	{
		UTableUtil::GetOnPowerStateDelegate().AddStatic(&load);
	}
};

static RegisterDefineToLua_Struct RegisterDefineToLua_Struct_var;