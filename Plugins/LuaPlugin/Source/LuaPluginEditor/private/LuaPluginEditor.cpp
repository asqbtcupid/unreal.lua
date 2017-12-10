// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
#include "ILuaPluginEditor.h"
#include "LuaFactory.h"
#include "CoreMinimal.h"


class FLuaPluginEditor : public ILuaPluginEditor
{

};

IMPLEMENT_MODULE(FLuaPluginEditor, LuaPluginEditor)
