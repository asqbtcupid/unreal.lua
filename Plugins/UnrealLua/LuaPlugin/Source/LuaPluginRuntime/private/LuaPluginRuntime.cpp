// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
#include "TableUtil.h"
#include "CoreMinimal.h"
#include "ILuaPluginRuntime.h"


class FLuaPluginRuntime : public ILuaPluginRuntime
{

};

IMPLEMENT_MODULE(FLuaPluginRuntime, LuaPluginRuntime)
