// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameFramework/Actor.h"
#include "DummyForLink.generated.h"

UCLASS(BlueprintType, Blueprintable)
class LUACIRCULARMODULES_API ADummyForLink : public AActor
{
	GENERATED_BODY()

		UFUNCTION(BlueprintCallable, Category = CircularModule)
		void NothingForlink();
};
