// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TableUtil.h"
#include "JustForLinkGlue.generated.h"

UCLASS()
class LUAMANUALLYGLUE_API UJustForLinkGlue : public UObject
{
	GENERATED_BODY()


	UFUNCTION()
	void LinkFixFunctino();
	UFUNCTION()
		void LinkFixFunctin1();
	UFUNCTION()
		void LinkFixFunction2();
};
