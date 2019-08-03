// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "CMPlayerControllerBase.generated.h"

UCLASS()
class CATCHME_API ACMPlayerControllerBase : public APlayerController
{
	GENERATED_BODY()
public:
	ACMPlayerControllerBase();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void PlayerTick(float DeltaTime) override;

	/** Method called prior to processing input */
	virtual void PreProcessInput(const float DeltaTime, const bool bGamePaused) override;

	/** Method called after processing input */
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;

	/** update input detection */
	virtual void ProcessPlayerInput(const float DeltaTime, const bool bGamePaused) override;

	virtual void SetPawn(APawn* aPawn) override;

	UFUNCTION(BlueprintImplementableEvent)
		void TestBpImp(int i, float j, FVector l);
	
	UFUNCTION()
	void GetInputState(UPARAM(ref) TArray<float>& result);

	UFUNCTION(BlueprintCallable, Category = "PlayerControllerBase")
	bool GetHitResult(float x, float y, UPARAM(ref) FHitResult& HitResult, int TraceChannel = -1, bool bTraceComplex = true);
};
