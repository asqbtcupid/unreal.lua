#pragma once
#include "allheader.inl"
#include "extraheader.h"
#include "tableutil.h"
#include "DelegateLuaProxy.generated.h"
UCLASS(meta=(Lua=2))
class UDelegate_AActor_OnTakeAnyDamage : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_AActor_OnTakeAnyDamage() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_AActor_OnTakePointDamage : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_AActor_OnTakePointDamage() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_AActor_OnActorBeginOverlap : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_AActor_OnActorBeginOverlap() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_AActor_OnActorEndOverlap : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_AActor_OnActorEndOverlap() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_AActor_OnBeginCursorOver : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_AActor_OnBeginCursorOver() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_AActor_OnEndCursorOver : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_AActor_OnEndCursorOver() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_AActor_OnClicked : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_AActor_OnClicked() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_AActor_OnReleased : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_AActor_OnReleased() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_AActor_OnInputTouchBegin : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_AActor_OnInputTouchBegin() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_AActor_OnInputTouchEnd : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_AActor_OnInputTouchEnd() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_AActor_OnInputTouchEnter : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_AActor_OnInputTouchEnter() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_AActor_OnInputTouchLeave : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_AActor_OnInputTouchLeave() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_AActor_OnActorHit : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_AActor_OnActorHit() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_AActor_OnDestroyed : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_AActor_OnDestroyed() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_AActor_OnEndPlay : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_AActor_OnEndPlay() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UActorComponent_OnComponentActivated : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UActorComponent_OnComponentActivated() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UActorComponent_OnComponentDeactivated : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UActorComponent_OnComponentDeactivated() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UAnimInstance_OnMontageBlendingOut : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UAnimInstance_OnMontageBlendingOut() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UAnimInstance_OnMontageStarted : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UAnimInstance_OnMontageStarted() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UAnimInstance_OnMontageEnded : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UAnimInstance_OnMontageEnded() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UAnimInstance_OnAllMontageInstancesEnded : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UAnimInstance_OnAllMontageInstancesEnded() {};
	template<typename T>
	void Init(T& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UAnimInstance_OnAllMontageInstancesEnded::CallBack);
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

UCLASS(meta=(Lua=2))
class UDelegate_ULevelStreaming_OnLevelLoaded : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_ULevelStreaming_OnLevelLoaded() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_ULevelStreaming_OnLevelUnloaded : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_ULevelStreaming_OnLevelUnloaded() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_ULevelStreaming_OnLevelShown : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_ULevelStreaming_OnLevelShown() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_ULevelStreaming_OnLevelHidden : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_ULevelStreaming_OnLevelHidden() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_ADestructibleActor_OnActorFracture : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_ADestructibleActor_OnActorFracture() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_AEmitter_OnParticleSpawn : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_AEmitter_OnParticleSpawn() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_AEmitter_OnParticleBurst : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_AEmitter_OnParticleBurst() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_AEmitter_OnParticleDeath : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_AEmitter_OnParticleDeath() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_AEmitter_OnParticleCollide : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_AEmitter_OnParticleCollide() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_AMatineeActor_OnPlay : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_AMatineeActor_OnPlay() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_AMatineeActor_OnStop : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_AMatineeActor_OnStop() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_AMatineeActor_OnPause : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_AMatineeActor_OnPause() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_ACharacter_OnReachedJumpApex : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_ACharacter_OnReachedJumpApex() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_ACharacter_MovementModeChangedDelegate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_ACharacter_MovementModeChangedDelegate() {};
	template<typename T>
	void Init(T& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_ACharacter_MovementModeChangedDelegate::CallBack);
	}

	UFUNCTION()
	void CallBack( ACharacter* Character, EMovementMode PrevMovementMode, uint8 PreviousCustomMode){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  Character, (int)(PrevMovementMode), PreviousCustomMode);
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

UCLASS(meta=(Lua=2))
class UDelegate_ACharacter_OnCharacterMovementUpdated : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_ACharacter_OnCharacterMovementUpdated() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UApplicationLifecycleComponent_ApplicationWillDeactivateDelegate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UApplicationLifecycleComponent_ApplicationWillDeactivateDelegate() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UApplicationLifecycleComponent_ApplicationHasReactivatedDelegate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UApplicationLifecycleComponent_ApplicationHasReactivatedDelegate() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UApplicationLifecycleComponent_ApplicationWillEnterBackgroundDelegate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UApplicationLifecycleComponent_ApplicationWillEnterBackgroundDelegate() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UApplicationLifecycleComponent_ApplicationHasEnteredForegroundDelegate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UApplicationLifecycleComponent_ApplicationHasEnteredForegroundDelegate() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UApplicationLifecycleComponent_ApplicationWillTerminateDelegate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UApplicationLifecycleComponent_ApplicationWillTerminateDelegate() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UPlatformEventsComponent_PlatformChangedToLaptopModeDelegate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPlatformEventsComponent_PlatformChangedToLaptopModeDelegate() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UPlatformEventsComponent_PlatformChangedToTabletModeDelegate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPlatformEventsComponent_PlatformChangedToTabletModeDelegate() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_USceneComponent_PhysicsVolumeChangedDelegate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_USceneComponent_PhysicsVolumeChangedDelegate() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UInterpToMovementComponent_OnInterpToReverse : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UInterpToMovementComponent_OnInterpToReverse() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UInterpToMovementComponent_OnInterpToStop : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UInterpToMovementComponent_OnInterpToStop() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UInterpToMovementComponent_OnWaitBeginDelegate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UInterpToMovementComponent_OnWaitBeginDelegate() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UInterpToMovementComponent_OnWaitEndDelegate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UInterpToMovementComponent_OnWaitEndDelegate() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UInterpToMovementComponent_OnResetDelegate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UInterpToMovementComponent_OnResetDelegate() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UProjectileMovementComponent_OnProjectileBounce : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UProjectileMovementComponent_OnProjectileBounce() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UProjectileMovementComponent_OnProjectileStop : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UProjectileMovementComponent_OnProjectileStop() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UAudioComponent_OnAudioFinished : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UAudioComponent_OnAudioFinished() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UAudioComponent_OnAudioPlaybackPercent : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UAudioComponent_OnAudioPlaybackPercent() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UForceFeedbackComponent_OnForceFeedbackFinished : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UForceFeedbackComponent_OnForceFeedbackFinished() {};
	template<typename T>
	void Init(T& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UForceFeedbackComponent_OnForceFeedbackFinished::CallBack);
	}

	UFUNCTION()
	void CallBack( UForceFeedbackComponent* ForceFeedbackComponent){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  ForceFeedbackComponent);
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

UCLASS(meta=(Lua=2))
class UDelegate_UPhysicsConstraintComponent_OnConstraintBroken : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPhysicsConstraintComponent_OnConstraintBroken() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UPrimitiveComponent_OnComponentHit : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPrimitiveComponent_OnComponentHit() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UPrimitiveComponent_OnComponentBeginOverlap : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPrimitiveComponent_OnComponentBeginOverlap() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UPrimitiveComponent_OnComponentEndOverlap : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPrimitiveComponent_OnComponentEndOverlap() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UPrimitiveComponent_OnComponentWake : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPrimitiveComponent_OnComponentWake() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UPrimitiveComponent_OnComponentSleep : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPrimitiveComponent_OnComponentSleep() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UPrimitiveComponent_OnBeginCursorOver : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPrimitiveComponent_OnBeginCursorOver() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UPrimitiveComponent_OnEndCursorOver : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPrimitiveComponent_OnEndCursorOver() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UPrimitiveComponent_OnClicked : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPrimitiveComponent_OnClicked() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UPrimitiveComponent_OnReleased : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPrimitiveComponent_OnReleased() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UPrimitiveComponent_OnInputTouchBegin : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPrimitiveComponent_OnInputTouchBegin() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UPrimitiveComponent_OnInputTouchEnd : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPrimitiveComponent_OnInputTouchEnd() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UPrimitiveComponent_OnInputTouchEnter : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPrimitiveComponent_OnInputTouchEnter() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UPrimitiveComponent_OnInputTouchLeave : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPrimitiveComponent_OnInputTouchLeave() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UParticleSystemComponent_OnParticleSpawn : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UParticleSystemComponent_OnParticleSpawn() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UParticleSystemComponent_OnParticleBurst : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UParticleSystemComponent_OnParticleBurst() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UParticleSystemComponent_OnParticleDeath : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UParticleSystemComponent_OnParticleDeath() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UParticleSystemComponent_OnParticleCollide : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UParticleSystemComponent_OnParticleCollide() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UParticleSystemComponent_OnSystemFinished : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UParticleSystemComponent_OnSystemFinished() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UDestructibleComponent_OnComponentFracture : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UDestructibleComponent_OnComponentFracture() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_USkeletalMeshComponent_OnConstraintBroken : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_USkeletalMeshComponent_OnConstraintBroken() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_USkeletalMeshComponent_OnAnimInitialized : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_USkeletalMeshComponent_OnAnimInitialized() {};
	template<typename T>
	void Init(T& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_USkeletalMeshComponent_OnAnimInitialized::CallBack);
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

UCLASS(meta=(Lua=2))
class UDelegate_UNavigationSystem_OnNavDataRegisteredEvent : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UNavigationSystem_OnNavDataRegisteredEvent() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UNavigationSystem_OnNavigationGenerationFinishedDelegate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UNavigationSystem_OnNavigationGenerationFinishedDelegate() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UPlatformGameInstance_ApplicationWillDeactivateDelegate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPlatformGameInstance_ApplicationWillDeactivateDelegate() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UPlatformGameInstance_ApplicationHasReactivatedDelegate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPlatformGameInstance_ApplicationHasReactivatedDelegate() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UPlatformGameInstance_ApplicationWillEnterBackgroundDelegate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPlatformGameInstance_ApplicationWillEnterBackgroundDelegate() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UPlatformGameInstance_ApplicationHasEnteredForegroundDelegate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPlatformGameInstance_ApplicationHasEnteredForegroundDelegate() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UPlatformGameInstance_ApplicationWillTerminateDelegate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPlatformGameInstance_ApplicationWillTerminateDelegate() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UPlatformGameInstance_ApplicationRegisteredForRemoteNotificationsDelegate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPlatformGameInstance_ApplicationRegisteredForRemoteNotificationsDelegate() {};
	template<typename T>
	void Init(T& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UPlatformGameInstance_ApplicationRegisteredForRemoteNotificationsDelegate::CallBack);
	}

	UFUNCTION()
	void CallBack( const TArray<uint8>& inArray){
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

UCLASS(meta=(Lua=2))
class UDelegate_UPlatformGameInstance_ApplicationRegisteredForUserNotificationsDelegate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPlatformGameInstance_ApplicationRegisteredForUserNotificationsDelegate() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UPlatformGameInstance_ApplicationFailedToRegisterForRemoteNotificationsDelegate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPlatformGameInstance_ApplicationFailedToRegisterForRemoteNotificationsDelegate() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UPlatformGameInstance_ApplicationReceivedRemoteNotificationDelegate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPlatformGameInstance_ApplicationReceivedRemoteNotificationDelegate() {};
	template<typename T>
	void Init(T& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UPlatformGameInstance_ApplicationReceivedRemoteNotificationDelegate::CallBack);
	}

	UFUNCTION()
	void CallBack( FString inString, EApplicationState::Type inAppState){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  inString, (int)(inAppState));
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

UCLASS(meta=(Lua=2))
class UDelegate_UPlatformGameInstance_ApplicationReceivedLocalNotificationDelegate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPlatformGameInstance_ApplicationReceivedLocalNotificationDelegate() {};
	template<typename T>
	void Init(T& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UPlatformGameInstance_ApplicationReceivedLocalNotificationDelegate::CallBack);
	}

	UFUNCTION()
	void CallBack( FString inString, int32 inInt, EApplicationState::Type inAppState){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  inString, inInt, (int)(inAppState));
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

UCLASS(meta=(Lua=2))
class UDelegate_UPlatformGameInstance_ApplicationReceivedScreenOrientationChangedNotificationDelegate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UPlatformGameInstance_ApplicationReceivedScreenOrientationChangedNotificationDelegate() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UCanvasRenderTarget2D_OnCanvasRenderTargetUpdate : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UCanvasRenderTarget2D_OnCanvasRenderTargetUpdate() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UAsyncTaskDownloadImage_OnSuccess : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UAsyncTaskDownloadImage_OnSuccess() {};
	template<typename T>
	void Init(T& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UAsyncTaskDownloadImage_OnSuccess::CallBack);
	}

	UFUNCTION()
	void CallBack( UTexture2DDynamic* Texture){
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

UCLASS(meta=(Lua=2))
class UDelegate_UAsyncTaskDownloadImage_OnFail : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UAsyncTaskDownloadImage_OnFail() {};
	template<typename T>
	void Init(T& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UAsyncTaskDownloadImage_OnFail::CallBack);
	}

	UFUNCTION()
	void CallBack( UTexture2DDynamic* Texture){
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

UCLASS(meta=(Lua=2))
class UDelegate_UDragDropOperation_OnDrop : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UDragDropOperation_OnDrop() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UDragDropOperation_OnDragCancelled : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UDragDropOperation_OnDragCancelled() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UDragDropOperation_OnDragged : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UDragDropOperation_OnDragged() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UWidgetAnimation_OnAnimationStarted : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UWidgetAnimation_OnAnimationStarted() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UWidgetAnimation_OnAnimationFinished : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UWidgetAnimation_OnAnimationFinished() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UWidgetInteractionComponent_OnHoveredWidgetChanged : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UWidgetInteractionComponent_OnHoveredWidgetChanged() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UComboBoxString_OnSelectionChanged : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UComboBoxString_OnSelectionChanged() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UComboBoxString_OnOpening : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UComboBoxString_OnOpening() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UEditableText_OnTextChanged : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UEditableText_OnTextChanged() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UEditableText_OnTextCommitted : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UEditableText_OnTextCommitted() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UEditableTextBox_OnTextChanged : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UEditableTextBox_OnTextChanged() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UEditableTextBox_OnTextCommitted : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UEditableTextBox_OnTextCommitted() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UExpandableArea_OnExpansionChanged : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UExpandableArea_OnExpansionChanged() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UInputKeySelector_OnKeySelected : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UInputKeySelector_OnKeySelected() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UInputKeySelector_OnIsSelectingKeyChanged : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UInputKeySelector_OnIsSelectingKeyChanged() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_USlider_OnMouseCaptureBegin : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_USlider_OnMouseCaptureBegin() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_USlider_OnMouseCaptureEnd : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_USlider_OnMouseCaptureEnd() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_USlider_OnControllerCaptureBegin : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_USlider_OnControllerCaptureBegin() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_USlider_OnControllerCaptureEnd : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_USlider_OnControllerCaptureEnd() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_USlider_OnValueChanged : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_USlider_OnValueChanged() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_USpinBox_OnValueChanged : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_USpinBox_OnValueChanged() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_USpinBox_OnValueCommitted : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_USpinBox_OnValueCommitted() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_USpinBox_OnBeginSliderMovement : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_USpinBox_OnBeginSliderMovement() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_USpinBox_OnEndSliderMovement : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_USpinBox_OnEndSliderMovement() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UScrollBox_OnUserScrolled : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UScrollBox_OnUserScrolled() {};
	template<typename T>
	void Init(T& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UScrollBox_OnUserScrolled::CallBack);
	}

	UFUNCTION()
	void CallBack( float CurrentOffset){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  CurrentOffset);
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

UCLASS(meta=(Lua=2))
class UDelegate_UButton_OnClicked : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UButton_OnClicked() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UButton_OnPressed : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UButton_OnPressed() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UButton_OnReleased : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UButton_OnReleased() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UButton_OnHovered : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UButton_OnHovered() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UButton_OnUnhovered : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UButton_OnUnhovered() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UCheckBox_OnCheckStateChanged : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UCheckBox_OnCheckStateChanged() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UMenuAnchor_OnMenuOpenChanged : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UMenuAnchor_OnMenuOpenChanged() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UMultiLineEditableText_OnTextChanged : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UMultiLineEditableText_OnTextChanged() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UMultiLineEditableText_OnTextCommitted : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UMultiLineEditableText_OnTextCommitted() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UMultiLineEditableTextBox_OnTextChanged : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UMultiLineEditableTextBox_OnTextChanged() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UMultiLineEditableTextBox_OnTextCommitted : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UMultiLineEditableTextBox_OnTextCommitted() {};
	template<typename T>
	void Init(T& theDelegate){
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

UCLASS(meta=(Lua=2))
class UDelegate_UFoliageInstancedStaticMeshComponent_OnInstanceTakePointDamage : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UFoliageInstancedStaticMeshComponent_OnInstanceTakePointDamage() {};
	template<typename T>
	void Init(T& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UFoliageInstancedStaticMeshComponent_OnInstanceTakePointDamage::CallBack);
	}

	UFUNCTION()
	void CallBack( int32 InstanceIndex, float Damage, AController* InstigatedBy, FVector HitLocation, FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  InstanceIndex, Damage, InstigatedBy, HitLocation, ShotFromDirection, DamageType, DamageCauser);
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

UCLASS(meta=(Lua=2))
class UDelegate_UFoliageInstancedStaticMeshComponent_OnInstanceTakeRadialDamage : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_UFoliageInstancedStaticMeshComponent_OnInstanceTakeRadialDamage() {};
	template<typename T>
	void Init(T& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_UFoliageInstancedStaticMeshComponent_OnInstanceTakeRadialDamage::CallBack);
	}

	UFUNCTION()
	void CallBack( const TArray<int32>& Instances, const TArray<float>& Damages, AController* InstigatedBy, FVector Origin, float MaxRadius, const UDamageType* DamageType, AActor* DamageCauser){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  Instances, Damages, InstigatedBy, Origin, MaxRadius, DamageType, DamageCauser);
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
class UDelegate_ACatchMeAIController_ReceiveMoveCompleted : public UObject{
	GENERATED_BODY()
public:
	TSet<int> LuaCallBacks;
	UDelegate_ACatchMeAIController_ReceiveMoveCompleted() {};
	template<typename T>
	void Init(T& theDelegate){
		UTableUtil::addgcref((UObject*)this);
		theDelegate.AddDynamic(this, &UDelegate_ACatchMeAIController_ReceiveMoveCompleted::CallBack);
	}

	UFUNCTION()
	void CallBack( FAIRequestID RequestID, EPathFollowingResult::Type Result){
		for (auto v : LuaCallBacks){
			UTableUtil::call(v,  RequestID, (int)(Result));
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

