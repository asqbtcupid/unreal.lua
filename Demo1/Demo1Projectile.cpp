// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "Demo1.h"
#include "Demo1Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "TableUtil.h"

ADemo1Projectile::ADemo1Projectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ADemo1Projectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;
	UTableUtil::call("CtorCpp", this, "luaprojectile");

}

void ADemo1Projectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//process logic is in lua
 	UTableUtil::call("CppCallBack", "luaprojectile", "ReceiveHit", this, HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}