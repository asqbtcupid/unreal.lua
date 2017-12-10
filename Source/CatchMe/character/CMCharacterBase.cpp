// Fill out your copyright notice in the Description page of Project Settings.

#include "CMCharacterBase.h"
#include "TableUtil.h"


// Sets default values
ACMCharacterBase::ACMCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	LuaCtor("character.cmcharacterbase", this);
}

// Called when the game starts or when spawned
void ACMCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	LuaCall("BeginPlay", this);
}

void ACMCharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	LuaCall("EndPlay", this, int(EndPlayReason));
}

// Called every frame
void ACMCharacterBase::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	LuaCall("Tick", this, DeltaTime);
}

// Called to bind functionality to input
void ACMCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ACMCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	LuaCall("PostInitializeComponents", this);
}

