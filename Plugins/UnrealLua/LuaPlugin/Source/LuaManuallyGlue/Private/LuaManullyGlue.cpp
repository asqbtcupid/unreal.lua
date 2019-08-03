// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
#include "TableUtil.h"
#include "CoreMinimal.h"
#include "ILuaManuallyGlue.h"


class FLuaManuallyGlue : public ILuaManuallyGlue
{

};

IMPLEMENT_MODULE(FLuaManuallyGlue, LuaManuallyGlue)
