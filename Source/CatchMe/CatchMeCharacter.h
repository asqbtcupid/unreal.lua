// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "character/CMCharacterBase.h"
#include "CatchMeCharacter.generated.h"
     
USTRUCT(meta = (Lua=1))
struct FCharacterInfo {
	GENERATED_BODY()
	UPROPERTY()
	int32 MaxHp;
	  
	UPROPERTY()
	int32 Hp;

	UPROPERTY()
	int32 MaxMp;

	UPROPERTY()
	int32 Mp;

	UPROPERTY()
	int32 AttackPower;

	UPROPERTY()
	int32 MoveSpeed;
};
USTRUCT(meta = (lua=1))
struct FSkillInfo {

	GENERATED_BODY()

	UPROPERTY()
	int32 SkillId;

	UPROPERTY()
	int32 Level;

	UPROPERTY()
	float LastTime;
};

UCLASS(Blueprintable, meta=(lua=1))
class ACatchMeCharacter : public ACMCharacterBase
{
	GENERATED_BODY()

public:
	ACatchMeCharacter();

	UPROPERTY(EditAnywhere)
		UTexture2D *TestTexture;


	UPROPERTY(replicated)
	FCharacterInfo BaseInfo;

	UPROPERTY(ReplicatedUsing = OnRep_SkillInfo)
	TArray<FSkillInfo> SkillInfo;

	UPROPERTY(ReplicatedUsing = OnRep_CharacterId)
	int32 CharacterId;

	UPROPERTY(ReplicatedUsing = OnRep_HP)
	int32 HP;

	UPROPERTY(ReplicatedUsing = OnRep_HPPercent)
	float HPPercent;

	UPROPERTY(ReplicatedUsing = OnRep_MP)
	int32 MP;

	UPROPERTY(ReplicatedUsing = OnRep_EXP)
	int32 EXP;

	UPROPERTY(ReplicatedUsing = OnRep_Level)
	int32 Level;
	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Test)
	UStaticMeshComponent* TestMesh;

	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	
	virtual bool IsReplicationPausedForConnection(const FNetViewer& ConnectionOwnerNetViewer) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	UFUNCTION(netmulticast, reliable)
	void M_PlaySkillMontage(int32 SKillId);

	UFUNCTION()
	virtual void OnRep_SkillInfo();

	UFUNCTION()
	virtual void OnRep_HP();

	UFUNCTION()
	virtual void OnRep_HPPercent();


	UFUNCTION()
	virtual void OnRep_MP();

	UFUNCTION()
	virtual void OnRep_EXP();

	UFUNCTION()
	virtual void OnRep_Level();

	UFUNCTION()
	virtual void OnRep_CharacterId();
};

