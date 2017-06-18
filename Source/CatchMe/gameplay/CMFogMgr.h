#pragma once

#include "UObject/NoExportTypes.h"
#include "CMFogMgr.generated.h"

UCLASS(meta=(lua=1))
class CATCHME_API UCMFogMgr : public UObject
{
	GENERATED_BODY()
public:
	UCMFogMgr();
	UPROPERTY()
		UTexture2D *Tx_Fog;

	UPROPERTY()
		UTexture2D *Tx_Last_Fog;
	TMap<int32, FVector> TreeToCheckSee;
	TArray<FColor> Data;
	TArray<FColor> LastData;
	TArray<bool> CanSeeData;
	FUpdateTextureRegion2D* textureRegions;
	FUpdateTextureRegion2D* LasttextureRegions;
	FColor FogColor;
	UFUNCTION()
	void Init(FColor Color);

	UPROPERTY()
	FColor ThroughColor;

	void UpdateTextureRegions(
		UTexture2D* Texture,
		int32 MipIndex,
		uint32 NumRegions,
		FUpdateTextureRegion2D* Regions,
		uint32 SrcPitch,
		uint32 SrcBpp,
		uint8* SrcData,
		bool bFreeData);

	bool CanSee(FVector& Pos, int32 Targetx, int32 Targety);

	UFUNCTION()
	bool CanSeeNow(FVector& TargetPos);

	UFUNCTION()
	void UpdateTexture();

	UFUNCTION()
	void UpdateLastTexture();
	
	UFUNCTION()
	void UpdateFOV(FVector CharacterPos);

	UPROPERTY()
	int32 MapSize;
	UPROPERTY()
	int32 LandscapeSize;
	UPROPERTY()
	int32 EyeLen;
	UPROPERTY()
	APlayerController* Controller;

};
