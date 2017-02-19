#pragma once
#include "TableUtil.h"
#include "DelegateLuaProxy.generated.h"
UCLASS(meta=(Lua=1))
class UDelegate_AActor_OnTakeAnyDamage : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_AActor_OnTakeAnyDamage() {};
	using delegatetype = decltype(((AActor*)0)->OnTakeAnyDamage);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_AActor_OnTakeAnyDamage::CallBack);
	}

	UFUNCTION()
	void CallBack( AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  DamagedActor, Damage, DamageType, InstigatedBy, DamageCauser);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_AActor_OnTakePointDamage : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_AActor_OnTakePointDamage() {};
	using delegatetype = decltype(((AActor*)0)->OnTakePointDamage);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_AActor_OnTakePointDamage::CallBack);
	}

	UFUNCTION()
	void CallBack( AActor* DamagedActor, float Damage, AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  DamagedActor, Damage, InstigatedBy, HitLocation, FHitComponent, BoneName, ShotFromDirection, DamageType, DamageCauser);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_AActor_OnActorBeginOverlap : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_AActor_OnActorBeginOverlap() {};
	using delegatetype = decltype(((AActor*)0)->OnActorBeginOverlap);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_AActor_OnActorBeginOverlap::CallBack);
	}

	UFUNCTION()
	void CallBack( AActor* OverlappedActor, AActor* OtherActor){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  OverlappedActor, OtherActor);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_AActor_OnActorEndOverlap : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_AActor_OnActorEndOverlap() {};
	using delegatetype = decltype(((AActor*)0)->OnActorEndOverlap);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_AActor_OnActorEndOverlap::CallBack);
	}

	UFUNCTION()
	void CallBack( AActor* OverlappedActor, AActor* OtherActor){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  OverlappedActor, OtherActor);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_AActor_OnBeginCursorOver : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_AActor_OnBeginCursorOver() {};
	using delegatetype = decltype(((AActor*)0)->OnBeginCursorOver);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_AActor_OnBeginCursorOver::CallBack);
	}

	UFUNCTION()
	void CallBack( AActor* TouchedActor){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  TouchedActor);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_AActor_OnEndCursorOver : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_AActor_OnEndCursorOver() {};
	using delegatetype = decltype(((AActor*)0)->OnEndCursorOver);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_AActor_OnEndCursorOver::CallBack);
	}

	UFUNCTION()
	void CallBack( AActor* TouchedActor){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  TouchedActor);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_AActor_OnClicked : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_AActor_OnClicked() {};
	using delegatetype = decltype(((AActor*)0)->OnClicked);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_AActor_OnClicked::CallBack);
	}

	UFUNCTION()
	void CallBack( AActor* TouchedActor, FKey ButtonPressed){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  TouchedActor, ButtonPressed);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_AActor_OnReleased : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_AActor_OnReleased() {};
	using delegatetype = decltype(((AActor*)0)->OnReleased);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_AActor_OnReleased::CallBack);
	}

	UFUNCTION()
	void CallBack( AActor* TouchedActor, FKey ButtonReleased){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  TouchedActor, ButtonReleased);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_AActor_OnInputTouchBegin : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_AActor_OnInputTouchBegin() {};
	using delegatetype = decltype(((AActor*)0)->OnInputTouchBegin);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_AActor_OnInputTouchBegin::CallBack);
	}

	UFUNCTION()
	void CallBack( ETouchIndex::Type FingerIndex, AActor* TouchedActor){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  (int)(FingerIndex), TouchedActor);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_AActor_OnInputTouchEnd : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_AActor_OnInputTouchEnd() {};
	using delegatetype = decltype(((AActor*)0)->OnInputTouchEnd);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_AActor_OnInputTouchEnd::CallBack);
	}

	UFUNCTION()
	void CallBack( ETouchIndex::Type FingerIndex, AActor* TouchedActor){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  (int)(FingerIndex), TouchedActor);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_AActor_OnInputTouchEnter : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_AActor_OnInputTouchEnter() {};
	using delegatetype = decltype(((AActor*)0)->OnInputTouchEnter);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_AActor_OnInputTouchEnter::CallBack);
	}

	UFUNCTION()
	void CallBack( ETouchIndex::Type FingerIndex, AActor* TouchedActor){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  (int)(FingerIndex), TouchedActor);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_AActor_OnInputTouchLeave : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_AActor_OnInputTouchLeave() {};
	using delegatetype = decltype(((AActor*)0)->OnInputTouchLeave);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_AActor_OnInputTouchLeave::CallBack);
	}

	UFUNCTION()
	void CallBack( ETouchIndex::Type FingerIndex, AActor* TouchedActor){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  (int)(FingerIndex), TouchedActor);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_AActor_OnActorHit : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_AActor_OnActorHit() {};
	using delegatetype = decltype(((AActor*)0)->OnActorHit);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_AActor_OnActorHit::CallBack);
	}

	UFUNCTION()
	void CallBack( AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  SelfActor, OtherActor, NormalImpulse, Hit);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_AActor_OnDestroyed : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_AActor_OnDestroyed() {};
	using delegatetype = decltype(((AActor*)0)->OnDestroyed);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_AActor_OnDestroyed::CallBack);
	}

	UFUNCTION()
	void CallBack( AActor* DestroyedActor){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  DestroyedActor);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_AActor_OnEndPlay : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_AActor_OnEndPlay() {};
	using delegatetype = decltype(((AActor*)0)->OnEndPlay);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_AActor_OnEndPlay::CallBack);
	}

	UFUNCTION()
	void CallBack( AActor* Actor, EEndPlayReason::Type EndPlayReason){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  Actor, (int)(EndPlayReason));
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UActorComponent_OnComponentActivated : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UActorComponent_OnComponentActivated() {};
	using delegatetype = decltype(((UActorComponent*)0)->OnComponentActivated);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UActorComponent_OnComponentActivated::CallBack);
	}

	UFUNCTION()
	void CallBack( UActorComponent* Component, bool bReset){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  Component, bReset);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UActorComponent_OnComponentDeactivated : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UActorComponent_OnComponentDeactivated() {};
	using delegatetype = decltype(((UActorComponent*)0)->OnComponentDeactivated);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UActorComponent_OnComponentDeactivated::CallBack);
	}

	UFUNCTION()
	void CallBack( UActorComponent* Component){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  Component);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UAnimInstance_OnMontageBlendingOut : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UAnimInstance_OnMontageBlendingOut() {};
	using delegatetype = decltype(((UAnimInstance*)0)->OnMontageBlendingOut);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UAnimInstance_OnMontageBlendingOut::CallBack);
	}

	UFUNCTION()
	void CallBack( UAnimMontage* Montage, bool bInterrupted){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  Montage, bInterrupted);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UAnimInstance_OnMontageStarted : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UAnimInstance_OnMontageStarted() {};
	using delegatetype = decltype(((UAnimInstance*)0)->OnMontageStarted);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UAnimInstance_OnMontageStarted::CallBack);
	}

	UFUNCTION()
	void CallBack( UAnimMontage* Montage){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  Montage);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UAnimInstance_OnMontageEnded : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UAnimInstance_OnMontageEnded() {};
	using delegatetype = decltype(((UAnimInstance*)0)->OnMontageEnded);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UAnimInstance_OnMontageEnded::CallBack);
	}

	UFUNCTION()
	void CallBack( UAnimMontage* Montage, bool bInterrupted){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  Montage, bInterrupted);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_ULevelStreaming_OnLevelLoaded : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_ULevelStreaming_OnLevelLoaded() {};
	using delegatetype = decltype(((ULevelStreaming*)0)->OnLevelLoaded);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_ULevelStreaming_OnLevelLoaded::CallBack);
	}

	UFUNCTION()
	void CallBack(){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_ULevelStreaming_OnLevelUnloaded : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_ULevelStreaming_OnLevelUnloaded() {};
	using delegatetype = decltype(((ULevelStreaming*)0)->OnLevelUnloaded);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_ULevelStreaming_OnLevelUnloaded::CallBack);
	}

	UFUNCTION()
	void CallBack(){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_ULevelStreaming_OnLevelShown : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_ULevelStreaming_OnLevelShown() {};
	using delegatetype = decltype(((ULevelStreaming*)0)->OnLevelShown);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_ULevelStreaming_OnLevelShown::CallBack);
	}

	UFUNCTION()
	void CallBack(){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_ULevelStreaming_OnLevelHidden : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_ULevelStreaming_OnLevelHidden() {};
	using delegatetype = decltype(((ULevelStreaming*)0)->OnLevelHidden);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_ULevelStreaming_OnLevelHidden::CallBack);
	}

	UFUNCTION()
	void CallBack(){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_ADestructibleActor_OnActorFracture : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_ADestructibleActor_OnActorFracture() {};
	using delegatetype = decltype(((ADestructibleActor*)0)->OnActorFracture);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_ADestructibleActor_OnActorFracture::CallBack);
	}

	UFUNCTION()
	void CallBack( const FVector& HitPoint, const FVector& HitDirection){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  HitPoint, HitDirection);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_AEmitter_OnParticleSpawn : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_AEmitter_OnParticleSpawn() {};
	using delegatetype = decltype(((AEmitter*)0)->OnParticleSpawn);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_AEmitter_OnParticleSpawn::CallBack);
	}

	UFUNCTION()
	void CallBack( FName EventName, float EmitterTime, FVector Location, FVector Velocity){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  EventName, EmitterTime, Location, Velocity);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_AEmitter_OnParticleBurst : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_AEmitter_OnParticleBurst() {};
	using delegatetype = decltype(((AEmitter*)0)->OnParticleBurst);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_AEmitter_OnParticleBurst::CallBack);
	}

	UFUNCTION()
	void CallBack( FName EventName, float EmitterTime, int32 ParticleCount){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  EventName, EmitterTime, ParticleCount);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_AEmitter_OnParticleDeath : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_AEmitter_OnParticleDeath() {};
	using delegatetype = decltype(((AEmitter*)0)->OnParticleDeath);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_AEmitter_OnParticleDeath::CallBack);
	}

	UFUNCTION()
	void CallBack( FName EventName, float EmitterTime, int32 ParticleTime, FVector Location, FVector Velocity, FVector Direction){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  EventName, EmitterTime, ParticleTime, Location, Velocity, Direction);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_AEmitter_OnParticleCollide : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_AEmitter_OnParticleCollide() {};
	using delegatetype = decltype(((AEmitter*)0)->OnParticleCollide);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_AEmitter_OnParticleCollide::CallBack);
	}

	UFUNCTION()
	void CallBack( FName EventName, float EmitterTime, int32 ParticleTime, FVector Location, FVector Velocity, FVector Direction, FVector Normal, FName BoneName, UPhysicalMaterial* PhysMat){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  EventName, EmitterTime, ParticleTime, Location, Velocity, Direction, Normal, BoneName, PhysMat);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_AMatineeActor_OnPlay : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_AMatineeActor_OnPlay() {};
	using delegatetype = decltype(((AMatineeActor*)0)->OnPlay);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_AMatineeActor_OnPlay::CallBack);
	}

	UFUNCTION()
	void CallBack(){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_AMatineeActor_OnStop : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_AMatineeActor_OnStop() {};
	using delegatetype = decltype(((AMatineeActor*)0)->OnStop);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_AMatineeActor_OnStop::CallBack);
	}

	UFUNCTION()
	void CallBack(){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_AMatineeActor_OnPause : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_AMatineeActor_OnPause() {};
	using delegatetype = decltype(((AMatineeActor*)0)->OnPause);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_AMatineeActor_OnPause::CallBack);
	}

	UFUNCTION()
	void CallBack(){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_ACharacter_OnReachedJumpApex : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_ACharacter_OnReachedJumpApex() {};
	using delegatetype = decltype(((ACharacter*)0)->OnReachedJumpApex);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_ACharacter_OnReachedJumpApex::CallBack);
	}

	UFUNCTION()
	void CallBack(){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_ACharacter_OnCharacterMovementUpdated : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_ACharacter_OnCharacterMovementUpdated() {};
	using delegatetype = decltype(((ACharacter*)0)->OnCharacterMovementUpdated);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_ACharacter_OnCharacterMovementUpdated::CallBack);
	}

	UFUNCTION()
	void CallBack( float DeltaSeconds, FVector OldLocation, FVector OldVelocity){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  DeltaSeconds, OldLocation, OldVelocity);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UApplicationLifecycleComponent_ApplicationWillDeactivateDelegate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UApplicationLifecycleComponent_ApplicationWillDeactivateDelegate() {};
	using delegatetype = decltype(((UApplicationLifecycleComponent*)0)->ApplicationWillDeactivateDelegate);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UApplicationLifecycleComponent_ApplicationWillDeactivateDelegate::CallBack);
	}

	UFUNCTION()
	void CallBack(){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UApplicationLifecycleComponent_ApplicationHasReactivatedDelegate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UApplicationLifecycleComponent_ApplicationHasReactivatedDelegate() {};
	using delegatetype = decltype(((UApplicationLifecycleComponent*)0)->ApplicationHasReactivatedDelegate);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UApplicationLifecycleComponent_ApplicationHasReactivatedDelegate::CallBack);
	}

	UFUNCTION()
	void CallBack(){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UApplicationLifecycleComponent_ApplicationWillEnterBackgroundDelegate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UApplicationLifecycleComponent_ApplicationWillEnterBackgroundDelegate() {};
	using delegatetype = decltype(((UApplicationLifecycleComponent*)0)->ApplicationWillEnterBackgroundDelegate);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UApplicationLifecycleComponent_ApplicationWillEnterBackgroundDelegate::CallBack);
	}

	UFUNCTION()
	void CallBack(){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UApplicationLifecycleComponent_ApplicationHasEnteredForegroundDelegate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UApplicationLifecycleComponent_ApplicationHasEnteredForegroundDelegate() {};
	using delegatetype = decltype(((UApplicationLifecycleComponent*)0)->ApplicationHasEnteredForegroundDelegate);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UApplicationLifecycleComponent_ApplicationHasEnteredForegroundDelegate::CallBack);
	}

	UFUNCTION()
	void CallBack(){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UApplicationLifecycleComponent_ApplicationWillTerminateDelegate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UApplicationLifecycleComponent_ApplicationWillTerminateDelegate() {};
	using delegatetype = decltype(((UApplicationLifecycleComponent*)0)->ApplicationWillTerminateDelegate);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UApplicationLifecycleComponent_ApplicationWillTerminateDelegate::CallBack);
	}

	UFUNCTION()
	void CallBack(){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UPlatformEventsComponent_PlatformChangedToLaptopModeDelegate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPlatformEventsComponent_PlatformChangedToLaptopModeDelegate() {};
	using delegatetype = decltype(((UPlatformEventsComponent*)0)->PlatformChangedToLaptopModeDelegate);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UPlatformEventsComponent_PlatformChangedToLaptopModeDelegate::CallBack);
	}

	UFUNCTION()
	void CallBack(){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UPlatformEventsComponent_PlatformChangedToTabletModeDelegate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPlatformEventsComponent_PlatformChangedToTabletModeDelegate() {};
	using delegatetype = decltype(((UPlatformEventsComponent*)0)->PlatformChangedToTabletModeDelegate);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UPlatformEventsComponent_PlatformChangedToTabletModeDelegate::CallBack);
	}

	UFUNCTION()
	void CallBack(){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_USceneComponent_PhysicsVolumeChangedDelegate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_USceneComponent_PhysicsVolumeChangedDelegate() {};
	using delegatetype = decltype(((USceneComponent*)0)->PhysicsVolumeChangedDelegate);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_USceneComponent_PhysicsVolumeChangedDelegate::CallBack);
	}

	UFUNCTION()
	void CallBack( APhysicsVolume* NewVolume){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  NewVolume);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UInterpToMovementComponent_OnInterpToReverse : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UInterpToMovementComponent_OnInterpToReverse() {};
	using delegatetype = decltype(((UInterpToMovementComponent*)0)->OnInterpToReverse);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UInterpToMovementComponent_OnInterpToReverse::CallBack);
	}

	UFUNCTION()
	void CallBack( const FHitResult& ImpactResult, float Time){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  ImpactResult, Time);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UInterpToMovementComponent_OnInterpToStop : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UInterpToMovementComponent_OnInterpToStop() {};
	using delegatetype = decltype(((UInterpToMovementComponent*)0)->OnInterpToStop);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UInterpToMovementComponent_OnInterpToStop::CallBack);
	}

	UFUNCTION()
	void CallBack( const FHitResult& ImpactResult, float Time){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  ImpactResult, Time);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UInterpToMovementComponent_OnWaitBeginDelegate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UInterpToMovementComponent_OnWaitBeginDelegate() {};
	using delegatetype = decltype(((UInterpToMovementComponent*)0)->OnWaitBeginDelegate);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UInterpToMovementComponent_OnWaitBeginDelegate::CallBack);
	}

	UFUNCTION()
	void CallBack( const FHitResult& ImpactResult, float Time){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  ImpactResult, Time);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UInterpToMovementComponent_OnWaitEndDelegate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UInterpToMovementComponent_OnWaitEndDelegate() {};
	using delegatetype = decltype(((UInterpToMovementComponent*)0)->OnWaitEndDelegate);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UInterpToMovementComponent_OnWaitEndDelegate::CallBack);
	}

	UFUNCTION()
	void CallBack( const FHitResult& ImpactResult, float Time){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  ImpactResult, Time);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UInterpToMovementComponent_OnResetDelegate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UInterpToMovementComponent_OnResetDelegate() {};
	using delegatetype = decltype(((UInterpToMovementComponent*)0)->OnResetDelegate);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UInterpToMovementComponent_OnResetDelegate::CallBack);
	}

	UFUNCTION()
	void CallBack( const FHitResult& ImpactResult, float Time){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  ImpactResult, Time);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UProjectileMovementComponent_OnProjectileBounce : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UProjectileMovementComponent_OnProjectileBounce() {};
	using delegatetype = decltype(((UProjectileMovementComponent*)0)->OnProjectileBounce);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UProjectileMovementComponent_OnProjectileBounce::CallBack);
	}

	UFUNCTION()
	void CallBack( const FHitResult& ImpactResult, const FVector& ImpactVelocity){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  ImpactResult, ImpactVelocity);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UProjectileMovementComponent_OnProjectileStop : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UProjectileMovementComponent_OnProjectileStop() {};
	using delegatetype = decltype(((UProjectileMovementComponent*)0)->OnProjectileStop);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UProjectileMovementComponent_OnProjectileStop::CallBack);
	}

	UFUNCTION()
	void CallBack( const FHitResult& ImpactResult){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  ImpactResult);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UAudioComponent_OnAudioFinished : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UAudioComponent_OnAudioFinished() {};
	using delegatetype = decltype(((UAudioComponent*)0)->OnAudioFinished);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UAudioComponent_OnAudioFinished::CallBack);
	}

	UFUNCTION()
	void CallBack(){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UAudioComponent_OnAudioPlaybackPercent : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UAudioComponent_OnAudioPlaybackPercent() {};
	using delegatetype = decltype(((UAudioComponent*)0)->OnAudioPlaybackPercent);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UAudioComponent_OnAudioPlaybackPercent::CallBack);
	}

	UFUNCTION()
	void CallBack( const USoundWave* PlayingSoundWave, const float PlaybackPercent){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  PlayingSoundWave, PlaybackPercent);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UPhysicsConstraintComponent_OnConstraintBroken : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPhysicsConstraintComponent_OnConstraintBroken() {};
	using delegatetype = decltype(((UPhysicsConstraintComponent*)0)->OnConstraintBroken);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UPhysicsConstraintComponent_OnConstraintBroken::CallBack);
	}

	UFUNCTION()
	void CallBack( int32 ConstraintIndex){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  ConstraintIndex);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UPrimitiveComponent_OnComponentHit : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPrimitiveComponent_OnComponentHit() {};
	using delegatetype = decltype(((UPrimitiveComponent*)0)->OnComponentHit);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UPrimitiveComponent_OnComponentHit::CallBack);
	}

	UFUNCTION()
	void CallBack( UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UPrimitiveComponent_OnComponentBeginOverlap : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPrimitiveComponent_OnComponentBeginOverlap() {};
	using delegatetype = decltype(((UPrimitiveComponent*)0)->OnComponentBeginOverlap);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UPrimitiveComponent_OnComponentBeginOverlap::CallBack);
	}

	UFUNCTION()
	void CallBack( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UPrimitiveComponent_OnComponentEndOverlap : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPrimitiveComponent_OnComponentEndOverlap() {};
	using delegatetype = decltype(((UPrimitiveComponent*)0)->OnComponentEndOverlap);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UPrimitiveComponent_OnComponentEndOverlap::CallBack);
	}

	UFUNCTION()
	void CallBack( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UPrimitiveComponent_OnComponentWake : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPrimitiveComponent_OnComponentWake() {};
	using delegatetype = decltype(((UPrimitiveComponent*)0)->OnComponentWake);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UPrimitiveComponent_OnComponentWake::CallBack);
	}

	UFUNCTION()
	void CallBack( UPrimitiveComponent* WakingComponent, FName BoneName){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  WakingComponent, BoneName);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UPrimitiveComponent_OnComponentSleep : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPrimitiveComponent_OnComponentSleep() {};
	using delegatetype = decltype(((UPrimitiveComponent*)0)->OnComponentSleep);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UPrimitiveComponent_OnComponentSleep::CallBack);
	}

	UFUNCTION()
	void CallBack( UPrimitiveComponent* SleepingComponent, FName BoneName){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  SleepingComponent, BoneName);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UPrimitiveComponent_OnBeginCursorOver : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPrimitiveComponent_OnBeginCursorOver() {};
	using delegatetype = decltype(((UPrimitiveComponent*)0)->OnBeginCursorOver);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UPrimitiveComponent_OnBeginCursorOver::CallBack);
	}

	UFUNCTION()
	void CallBack( UPrimitiveComponent* TouchedComponent){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  TouchedComponent);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UPrimitiveComponent_OnEndCursorOver : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPrimitiveComponent_OnEndCursorOver() {};
	using delegatetype = decltype(((UPrimitiveComponent*)0)->OnEndCursorOver);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UPrimitiveComponent_OnEndCursorOver::CallBack);
	}

	UFUNCTION()
	void CallBack( UPrimitiveComponent* TouchedComponent){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  TouchedComponent);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UPrimitiveComponent_OnClicked : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPrimitiveComponent_OnClicked() {};
	using delegatetype = decltype(((UPrimitiveComponent*)0)->OnClicked);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UPrimitiveComponent_OnClicked::CallBack);
	}

	UFUNCTION()
	void CallBack( UPrimitiveComponent* TouchedComponent, FKey ButtonPressed){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  TouchedComponent, ButtonPressed);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UPrimitiveComponent_OnReleased : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPrimitiveComponent_OnReleased() {};
	using delegatetype = decltype(((UPrimitiveComponent*)0)->OnReleased);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UPrimitiveComponent_OnReleased::CallBack);
	}

	UFUNCTION()
	void CallBack( UPrimitiveComponent* TouchedComponent, FKey ButtonReleased){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  TouchedComponent, ButtonReleased);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UPrimitiveComponent_OnInputTouchBegin : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPrimitiveComponent_OnInputTouchBegin() {};
	using delegatetype = decltype(((UPrimitiveComponent*)0)->OnInputTouchBegin);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UPrimitiveComponent_OnInputTouchBegin::CallBack);
	}

	UFUNCTION()
	void CallBack( ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  (int)(FingerIndex), TouchedComponent);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UPrimitiveComponent_OnInputTouchEnd : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPrimitiveComponent_OnInputTouchEnd() {};
	using delegatetype = decltype(((UPrimitiveComponent*)0)->OnInputTouchEnd);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UPrimitiveComponent_OnInputTouchEnd::CallBack);
	}

	UFUNCTION()
	void CallBack( ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  (int)(FingerIndex), TouchedComponent);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UPrimitiveComponent_OnInputTouchEnter : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPrimitiveComponent_OnInputTouchEnter() {};
	using delegatetype = decltype(((UPrimitiveComponent*)0)->OnInputTouchEnter);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UPrimitiveComponent_OnInputTouchEnter::CallBack);
	}

	UFUNCTION()
	void CallBack( ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  (int)(FingerIndex), TouchedComponent);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UPrimitiveComponent_OnInputTouchLeave : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPrimitiveComponent_OnInputTouchLeave() {};
	using delegatetype = decltype(((UPrimitiveComponent*)0)->OnInputTouchLeave);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UPrimitiveComponent_OnInputTouchLeave::CallBack);
	}

	UFUNCTION()
	void CallBack( ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  (int)(FingerIndex), TouchedComponent);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UParticleSystemComponent_OnParticleSpawn : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UParticleSystemComponent_OnParticleSpawn() {};
	using delegatetype = decltype(((UParticleSystemComponent*)0)->OnParticleSpawn);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UParticleSystemComponent_OnParticleSpawn::CallBack);
	}

	UFUNCTION()
	void CallBack( FName EventName, float EmitterTime, FVector Location, FVector Velocity){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  EventName, EmitterTime, Location, Velocity);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UParticleSystemComponent_OnParticleBurst : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UParticleSystemComponent_OnParticleBurst() {};
	using delegatetype = decltype(((UParticleSystemComponent*)0)->OnParticleBurst);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UParticleSystemComponent_OnParticleBurst::CallBack);
	}

	UFUNCTION()
	void CallBack( FName EventName, float EmitterTime, int32 ParticleCount){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  EventName, EmitterTime, ParticleCount);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UParticleSystemComponent_OnParticleDeath : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UParticleSystemComponent_OnParticleDeath() {};
	using delegatetype = decltype(((UParticleSystemComponent*)0)->OnParticleDeath);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UParticleSystemComponent_OnParticleDeath::CallBack);
	}

	UFUNCTION()
	void CallBack( FName EventName, float EmitterTime, int32 ParticleTime, FVector Location, FVector Velocity, FVector Direction){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  EventName, EmitterTime, ParticleTime, Location, Velocity, Direction);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UParticleSystemComponent_OnParticleCollide : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UParticleSystemComponent_OnParticleCollide() {};
	using delegatetype = decltype(((UParticleSystemComponent*)0)->OnParticleCollide);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UParticleSystemComponent_OnParticleCollide::CallBack);
	}

	UFUNCTION()
	void CallBack( FName EventName, float EmitterTime, int32 ParticleTime, FVector Location, FVector Velocity, FVector Direction, FVector Normal, FName BoneName, UPhysicalMaterial* PhysMat){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  EventName, EmitterTime, ParticleTime, Location, Velocity, Direction, Normal, BoneName, PhysMat);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UParticleSystemComponent_OnSystemFinished : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UParticleSystemComponent_OnSystemFinished() {};
	using delegatetype = decltype(((UParticleSystemComponent*)0)->OnSystemFinished);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UParticleSystemComponent_OnSystemFinished::CallBack);
	}

	UFUNCTION()
	void CallBack( UParticleSystemComponent* PSystem){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  PSystem);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UDestructibleComponent_OnComponentFracture : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UDestructibleComponent_OnComponentFracture() {};
	using delegatetype = decltype(((UDestructibleComponent*)0)->OnComponentFracture);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UDestructibleComponent_OnComponentFracture::CallBack);
	}

	UFUNCTION()
	void CallBack( const FVector& HitPoint, const FVector& HitDirection){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  HitPoint, HitDirection);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_USkeletalMeshComponent_OnConstraintBroken : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_USkeletalMeshComponent_OnConstraintBroken() {};
	using delegatetype = decltype(((USkeletalMeshComponent*)0)->OnConstraintBroken);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_USkeletalMeshComponent_OnConstraintBroken::CallBack);
	}

	UFUNCTION()
	void CallBack( int32 ConstraintIndex){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  ConstraintIndex);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UNavigationSystem_OnNavDataRegisteredEvent : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UNavigationSystem_OnNavDataRegisteredEvent() {};
	using delegatetype = decltype(((UNavigationSystem*)0)->OnNavDataRegisteredEvent);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UNavigationSystem_OnNavDataRegisteredEvent::CallBack);
	}

	UFUNCTION()
	void CallBack( ANavigationData* NavData){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  NavData);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UNavigationSystem_OnNavigationGenerationFinishedDelegate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UNavigationSystem_OnNavigationGenerationFinishedDelegate() {};
	using delegatetype = decltype(((UNavigationSystem*)0)->OnNavigationGenerationFinishedDelegate);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UNavigationSystem_OnNavigationGenerationFinishedDelegate::CallBack);
	}

	UFUNCTION()
	void CallBack( ANavigationData* NavData){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  NavData);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UPlatformGameInstance_ApplicationWillDeactivateDelegate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPlatformGameInstance_ApplicationWillDeactivateDelegate() {};
	using delegatetype = decltype(((UPlatformGameInstance*)0)->ApplicationWillDeactivateDelegate);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UPlatformGameInstance_ApplicationWillDeactivateDelegate::CallBack);
	}

	UFUNCTION()
	void CallBack(){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UPlatformGameInstance_ApplicationHasReactivatedDelegate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPlatformGameInstance_ApplicationHasReactivatedDelegate() {};
	using delegatetype = decltype(((UPlatformGameInstance*)0)->ApplicationHasReactivatedDelegate);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UPlatformGameInstance_ApplicationHasReactivatedDelegate::CallBack);
	}

	UFUNCTION()
	void CallBack(){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UPlatformGameInstance_ApplicationWillEnterBackgroundDelegate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPlatformGameInstance_ApplicationWillEnterBackgroundDelegate() {};
	using delegatetype = decltype(((UPlatformGameInstance*)0)->ApplicationWillEnterBackgroundDelegate);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UPlatformGameInstance_ApplicationWillEnterBackgroundDelegate::CallBack);
	}

	UFUNCTION()
	void CallBack(){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UPlatformGameInstance_ApplicationHasEnteredForegroundDelegate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPlatformGameInstance_ApplicationHasEnteredForegroundDelegate() {};
	using delegatetype = decltype(((UPlatformGameInstance*)0)->ApplicationHasEnteredForegroundDelegate);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UPlatformGameInstance_ApplicationHasEnteredForegroundDelegate::CallBack);
	}

	UFUNCTION()
	void CallBack(){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UPlatformGameInstance_ApplicationWillTerminateDelegate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPlatformGameInstance_ApplicationWillTerminateDelegate() {};
	using delegatetype = decltype(((UPlatformGameInstance*)0)->ApplicationWillTerminateDelegate);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UPlatformGameInstance_ApplicationWillTerminateDelegate::CallBack);
	}

	UFUNCTION()
	void CallBack(){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UPlatformGameInstance_ApplicationRegisteredForRemoteNotificationsDelegate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPlatformGameInstance_ApplicationRegisteredForRemoteNotificationsDelegate() {};
	using delegatetype = decltype(((UPlatformGameInstance*)0)->ApplicationRegisteredForRemoteNotificationsDelegate);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UPlatformGameInstance_ApplicationRegisteredForRemoteNotificationsDelegate::CallBack);
	}

	UFUNCTION()
	void CallBack( TArray<uint8> inArray){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  inArray);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UPlatformGameInstance_ApplicationRegisteredForUserNotificationsDelegate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPlatformGameInstance_ApplicationRegisteredForUserNotificationsDelegate() {};
	using delegatetype = decltype(((UPlatformGameInstance*)0)->ApplicationRegisteredForUserNotificationsDelegate);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UPlatformGameInstance_ApplicationRegisteredForUserNotificationsDelegate::CallBack);
	}

	UFUNCTION()
	void CallBack( int32 inInt){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  inInt);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UPlatformGameInstance_ApplicationFailedToRegisterForRemoteNotificationsDelegate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPlatformGameInstance_ApplicationFailedToRegisterForRemoteNotificationsDelegate() {};
	using delegatetype = decltype(((UPlatformGameInstance*)0)->ApplicationFailedToRegisterForRemoteNotificationsDelegate);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UPlatformGameInstance_ApplicationFailedToRegisterForRemoteNotificationsDelegate::CallBack);
	}

	UFUNCTION()
	void CallBack( FString inString){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  inString);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UPlatformGameInstance_ApplicationReceivedRemoteNotificationDelegate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPlatformGameInstance_ApplicationReceivedRemoteNotificationDelegate() {};
	using delegatetype = decltype(((UPlatformGameInstance*)0)->ApplicationReceivedRemoteNotificationDelegate);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UPlatformGameInstance_ApplicationReceivedRemoteNotificationDelegate::CallBack);
	}

	UFUNCTION()
	void CallBack( FString inString){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  inString);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UPlatformGameInstance_ApplicationReceivedLocalNotificationDelegate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPlatformGameInstance_ApplicationReceivedLocalNotificationDelegate() {};
	using delegatetype = decltype(((UPlatformGameInstance*)0)->ApplicationReceivedLocalNotificationDelegate);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UPlatformGameInstance_ApplicationReceivedLocalNotificationDelegate::CallBack);
	}

	UFUNCTION()
	void CallBack( FString inString, int32 inInt){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  inString, inInt);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UPlatformGameInstance_ApplicationReceivedScreenOrientationChangedNotificationDelegate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPlatformGameInstance_ApplicationReceivedScreenOrientationChangedNotificationDelegate() {};
	using delegatetype = decltype(((UPlatformGameInstance*)0)->ApplicationReceivedScreenOrientationChangedNotificationDelegate);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UPlatformGameInstance_ApplicationReceivedScreenOrientationChangedNotificationDelegate::CallBack);
	}

	UFUNCTION()
	void CallBack( EScreenOrientation::Type inScreenOrientation){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  (int)(inScreenOrientation));
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UCanvasRenderTarget2D_OnCanvasRenderTargetUpdate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UCanvasRenderTarget2D_OnCanvasRenderTargetUpdate() {};
	using delegatetype = decltype(((UCanvasRenderTarget2D*)0)->OnCanvasRenderTargetUpdate);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UCanvasRenderTarget2D_OnCanvasRenderTargetUpdate::CallBack);
	}

	UFUNCTION()
	void CallBack( UCanvas* Canvas, int32 Width, int32 Height){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  Canvas, Width, Height);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UAsyncTaskDownloadImage_OnSuccess : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UAsyncTaskDownloadImage_OnSuccess() {};
	using delegatetype = decltype(((UAsyncTaskDownloadImage*)0)->OnSuccess);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UAsyncTaskDownloadImage_OnSuccess::CallBack);
	}

	UFUNCTION()
	void CallBack( UTexture2D* Texture){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  Texture);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UAsyncTaskDownloadImage_OnFail : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UAsyncTaskDownloadImage_OnFail() {};
	using delegatetype = decltype(((UAsyncTaskDownloadImage*)0)->OnFail);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UAsyncTaskDownloadImage_OnFail::CallBack);
	}

	UFUNCTION()
	void CallBack( UTexture2D* Texture){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  Texture);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UDragDropOperation_OnDrop : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UDragDropOperation_OnDrop() {};
	using delegatetype = decltype(((UDragDropOperation*)0)->OnDrop);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UDragDropOperation_OnDrop::CallBack);
	}

	UFUNCTION()
	void CallBack( UDragDropOperation* Operation){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  Operation);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UDragDropOperation_OnDragCancelled : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UDragDropOperation_OnDragCancelled() {};
	using delegatetype = decltype(((UDragDropOperation*)0)->OnDragCancelled);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UDragDropOperation_OnDragCancelled::CallBack);
	}

	UFUNCTION()
	void CallBack( UDragDropOperation* Operation){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  Operation);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UDragDropOperation_OnDragged : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UDragDropOperation_OnDragged() {};
	using delegatetype = decltype(((UDragDropOperation*)0)->OnDragged);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UDragDropOperation_OnDragged::CallBack);
	}

	UFUNCTION()
	void CallBack( UDragDropOperation* Operation){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  Operation);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UWidgetAnimation_OnAnimationStarted : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UWidgetAnimation_OnAnimationStarted() {};
	using delegatetype = decltype(((UWidgetAnimation*)0)->OnAnimationStarted);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UWidgetAnimation_OnAnimationStarted::CallBack);
	}

	UFUNCTION()
	void CallBack(){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UWidgetAnimation_OnAnimationFinished : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UWidgetAnimation_OnAnimationFinished() {};
	using delegatetype = decltype(((UWidgetAnimation*)0)->OnAnimationFinished);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UWidgetAnimation_OnAnimationFinished::CallBack);
	}

	UFUNCTION()
	void CallBack(){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UWidgetInteractionComponent_OnHoveredWidgetChanged : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UWidgetInteractionComponent_OnHoveredWidgetChanged() {};
	using delegatetype = decltype(((UWidgetInteractionComponent*)0)->OnHoveredWidgetChanged);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UWidgetInteractionComponent_OnHoveredWidgetChanged::CallBack);
	}

	UFUNCTION()
	void CallBack( UWidgetComponent* WidgetComponent, UWidgetComponent* PreviousWidgetComponent){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  WidgetComponent, PreviousWidgetComponent);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UComboBoxString_OnSelectionChanged : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UComboBoxString_OnSelectionChanged() {};
	using delegatetype = decltype(((UComboBoxString*)0)->OnSelectionChanged);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UComboBoxString_OnSelectionChanged::CallBack);
	}

	UFUNCTION()
	void CallBack( FString SelectedItem, ESelectInfo::Type SelectionType){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  SelectedItem, (int)(SelectionType));
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UComboBoxString_OnOpening : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UComboBoxString_OnOpening() {};
	using delegatetype = decltype(((UComboBoxString*)0)->OnOpening);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UComboBoxString_OnOpening::CallBack);
	}

	UFUNCTION()
	void CallBack(){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UEditableText_OnTextChanged : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UEditableText_OnTextChanged() {};
	using delegatetype = decltype(((UEditableText*)0)->OnTextChanged);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UEditableText_OnTextChanged::CallBack);
	}

	UFUNCTION()
	void CallBack( const FText& Text){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  Text);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UEditableText_OnTextCommitted : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UEditableText_OnTextCommitted() {};
	using delegatetype = decltype(((UEditableText*)0)->OnTextCommitted);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UEditableText_OnTextCommitted::CallBack);
	}

	UFUNCTION()
	void CallBack( const FText& Text, ETextCommit::Type CommitMethod){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  Text, (int)(CommitMethod));
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UEditableTextBox_OnTextChanged : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UEditableTextBox_OnTextChanged() {};
	using delegatetype = decltype(((UEditableTextBox*)0)->OnTextChanged);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UEditableTextBox_OnTextChanged::CallBack);
	}

	UFUNCTION()
	void CallBack( const FText& Text){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  Text);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UEditableTextBox_OnTextCommitted : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UEditableTextBox_OnTextCommitted() {};
	using delegatetype = decltype(((UEditableTextBox*)0)->OnTextCommitted);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UEditableTextBox_OnTextCommitted::CallBack);
	}

	UFUNCTION()
	void CallBack( const FText& Text, ETextCommit::Type CommitMethod){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  Text, (int)(CommitMethod));
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UExpandableArea_OnExpansionChanged : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UExpandableArea_OnExpansionChanged() {};
	using delegatetype = decltype(((UExpandableArea*)0)->OnExpansionChanged);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UExpandableArea_OnExpansionChanged::CallBack);
	}

	UFUNCTION()
	void CallBack( UExpandableArea* Area, bool bIsExpanded){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  Area, bIsExpanded);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UInputKeySelector_OnKeySelected : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UInputKeySelector_OnKeySelected() {};
	using delegatetype = decltype(((UInputKeySelector*)0)->OnKeySelected);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UInputKeySelector_OnKeySelected::CallBack);
	}

	UFUNCTION()
	void CallBack( FInputChord SelectedKey){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  SelectedKey);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UInputKeySelector_OnIsSelectingKeyChanged : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UInputKeySelector_OnIsSelectingKeyChanged() {};
	using delegatetype = decltype(((UInputKeySelector*)0)->OnIsSelectingKeyChanged);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UInputKeySelector_OnIsSelectingKeyChanged::CallBack);
	}

	UFUNCTION()
	void CallBack(){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_USlider_OnMouseCaptureBegin : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_USlider_OnMouseCaptureBegin() {};
	using delegatetype = decltype(((USlider*)0)->OnMouseCaptureBegin);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_USlider_OnMouseCaptureBegin::CallBack);
	}

	UFUNCTION()
	void CallBack(){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_USlider_OnMouseCaptureEnd : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_USlider_OnMouseCaptureEnd() {};
	using delegatetype = decltype(((USlider*)0)->OnMouseCaptureEnd);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_USlider_OnMouseCaptureEnd::CallBack);
	}

	UFUNCTION()
	void CallBack(){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_USlider_OnControllerCaptureBegin : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_USlider_OnControllerCaptureBegin() {};
	using delegatetype = decltype(((USlider*)0)->OnControllerCaptureBegin);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_USlider_OnControllerCaptureBegin::CallBack);
	}

	UFUNCTION()
	void CallBack(){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_USlider_OnControllerCaptureEnd : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_USlider_OnControllerCaptureEnd() {};
	using delegatetype = decltype(((USlider*)0)->OnControllerCaptureEnd);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_USlider_OnControllerCaptureEnd::CallBack);
	}

	UFUNCTION()
	void CallBack(){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_USlider_OnValueChanged : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_USlider_OnValueChanged() {};
	using delegatetype = decltype(((USlider*)0)->OnValueChanged);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_USlider_OnValueChanged::CallBack);
	}

	UFUNCTION()
	void CallBack( float Value){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  Value);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_USpinBox_OnValueChanged : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_USpinBox_OnValueChanged() {};
	using delegatetype = decltype(((USpinBox*)0)->OnValueChanged);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_USpinBox_OnValueChanged::CallBack);
	}

	UFUNCTION()
	void CallBack( float InValue){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  InValue);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_USpinBox_OnValueCommitted : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_USpinBox_OnValueCommitted() {};
	using delegatetype = decltype(((USpinBox*)0)->OnValueCommitted);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_USpinBox_OnValueCommitted::CallBack);
	}

	UFUNCTION()
	void CallBack( float InValue, ETextCommit::Type CommitMethod){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  InValue, (int)(CommitMethod));
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_USpinBox_OnBeginSliderMovement : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_USpinBox_OnBeginSliderMovement() {};
	using delegatetype = decltype(((USpinBox*)0)->OnBeginSliderMovement);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_USpinBox_OnBeginSliderMovement::CallBack);
	}

	UFUNCTION()
	void CallBack(){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_USpinBox_OnEndSliderMovement : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_USpinBox_OnEndSliderMovement() {};
	using delegatetype = decltype(((USpinBox*)0)->OnEndSliderMovement);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_USpinBox_OnEndSliderMovement::CallBack);
	}

	UFUNCTION()
	void CallBack( float InValue){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  InValue);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UButton_OnClicked : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UButton_OnClicked() {};
	using delegatetype = decltype(((UButton*)0)->OnClicked);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UButton_OnClicked::CallBack);
	}

	UFUNCTION()
	void CallBack(){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UButton_OnPressed : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UButton_OnPressed() {};
	using delegatetype = decltype(((UButton*)0)->OnPressed);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UButton_OnPressed::CallBack);
	}

	UFUNCTION()
	void CallBack(){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UButton_OnReleased : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UButton_OnReleased() {};
	using delegatetype = decltype(((UButton*)0)->OnReleased);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UButton_OnReleased::CallBack);
	}

	UFUNCTION()
	void CallBack(){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UButton_OnHovered : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UButton_OnHovered() {};
	using delegatetype = decltype(((UButton*)0)->OnHovered);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UButton_OnHovered::CallBack);
	}

	UFUNCTION()
	void CallBack(){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UButton_OnUnhovered : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UButton_OnUnhovered() {};
	using delegatetype = decltype(((UButton*)0)->OnUnhovered);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UButton_OnUnhovered::CallBack);
	}

	UFUNCTION()
	void CallBack(){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UCheckBox_OnCheckStateChanged : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UCheckBox_OnCheckStateChanged() {};
	using delegatetype = decltype(((UCheckBox*)0)->OnCheckStateChanged);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UCheckBox_OnCheckStateChanged::CallBack);
	}

	UFUNCTION()
	void CallBack( bool bIsChecked){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  bIsChecked);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UMenuAnchor_OnMenuOpenChanged : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UMenuAnchor_OnMenuOpenChanged() {};
	using delegatetype = decltype(((UMenuAnchor*)0)->OnMenuOpenChanged);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UMenuAnchor_OnMenuOpenChanged::CallBack);
	}

	UFUNCTION()
	void CallBack( bool bIsOpen){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  bIsOpen);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UMultiLineEditableText_OnTextChanged : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UMultiLineEditableText_OnTextChanged() {};
	using delegatetype = decltype(((UMultiLineEditableText*)0)->OnTextChanged);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UMultiLineEditableText_OnTextChanged::CallBack);
	}

	UFUNCTION()
	void CallBack( const FText& Text){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  Text);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UMultiLineEditableText_OnTextCommitted : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UMultiLineEditableText_OnTextCommitted() {};
	using delegatetype = decltype(((UMultiLineEditableText*)0)->OnTextCommitted);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UMultiLineEditableText_OnTextCommitted::CallBack);
	}

	UFUNCTION()
	void CallBack( const FText& Text, ETextCommit::Type CommitMethod){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  Text, (int)(CommitMethod));
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UMultiLineEditableTextBox_OnTextChanged : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UMultiLineEditableTextBox_OnTextChanged() {};
	using delegatetype = decltype(((UMultiLineEditableTextBox*)0)->OnTextChanged);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UMultiLineEditableTextBox_OnTextChanged::CallBack);
	}

	UFUNCTION()
	void CallBack( const FText& Text){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  Text);
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

UCLASS(meta=(Lua=1))
class UDelegate_UMultiLineEditableTextBox_OnTextCommitted : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UMultiLineEditableTextBox_OnTextCommitted() {};
	using delegatetype = decltype(((UMultiLineEditableTextBox*)0)->OnTextCommitted);
	void Init(delegatetype& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UMultiLineEditableTextBox_OnTextCommitted::CallBack);
	}

	UFUNCTION()
	void CallBack( const FText& Text, ETextCommit::Type CommitMethod){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  Text, (int)(CommitMethod));
		}
	}

	UFUNCTION()
	int Add() {int r = UTableUtil::pushluafunc(2);LuaCallBacks.Add(r);return r;}
	UFUNCTION()
	void Remove(int32 r)
	{
		if(LuaCallBacks.Contains(r)){
			UTableUtil::unref(r);
			LuaCallBacks.Remove(r);
		}
	}
	UFUNCTION()
	void Destroy() {
		for(auto v : LuaCallBacks){
			UTableUtil::unref(v);
		}
		LuaCallBacks.Reset();
		UTableUtil::rmgcref(this);
	}
};

