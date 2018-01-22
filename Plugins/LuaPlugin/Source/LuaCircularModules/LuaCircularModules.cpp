// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
#include "CoreMinimal.h"
#include "ILuaCircularModules.h"
#include "LuaFixLink1.h"

class FLuaCircularModules : public ILuaCircularModules
{
	
};

IMPLEMENT_MODULE(FLuaCircularModules, LuaCircularModules)
