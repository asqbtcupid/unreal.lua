// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Animation/AnimInstance.h"
#include "LuaAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class CATCHME_API ULuaAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	ULuaAnimInstance();
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float WalkSpeed;
};
