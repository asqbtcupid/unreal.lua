// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "IScriptPlugin.h"
#include "CoreUObject.h"
#include "ModuleManager.h"
#include "Engine.h"
#include "ScriptPluginComponent.h"
#include "ScriptContext.h"
#include "ScriptContextComponent.h"
#include "ScriptTestActor.h"

#if WITH_EDITOR
#include "UnrealEd.h"
#endif

#if WITH_LUA
#include "LuaIntegration.h"
#endif // WITH_LUA

DECLARE_LOG_CATEGORY_EXTERN(LogScriptPlugin, Log, All);

