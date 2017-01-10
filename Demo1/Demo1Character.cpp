// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "Demo1.h"
#include "Demo1Character.h"
#include "Demo1Projectile.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "MotionControllerComponent.h"
#include "TableUtil.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ADemo1Character

ADemo1Character::ADemo1Character()
{
	//init in lua
 	UTableUtil::call("CtorCpp", this, "luacharacter");
}

void ADemo1Character::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	//call lua beginplay
	UTableUtil::call("CppCallBack", "luacharacter", "BeginPlayLua", this);
	
}

//////////////////////////////////////////////////////////////////////////

