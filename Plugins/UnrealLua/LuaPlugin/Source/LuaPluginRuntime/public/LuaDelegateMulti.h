// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ScriptDelegates.h"
#include "Runtime/Launch/Resources/Version.h"
#include "LuaDelegateMulti.generated.h"
 
struct lua_State;
UCLASS()
class LUAPLUGINRUNTIME_API ULuaDelegateMulti : public UObject
{
	GENERATED_BODY()

	struct IDelegateWrapper
	{
		virtual void ProcessMulticastDelegate(void* Buffer) = 0;
		virtual void Add(TScriptDelegate<FWeakObjectPtr>& Handle) = 0;
		virtual void Clear() = 0;
		virtual void RemoveUObject(UObject* Ptr) = 0;
		virtual void RemoveUFunction(UObject* Ptr, FName InFunctionName) = 0;
	};

	struct FInlineDelegateWrapper :public IDelegateWrapper
	{
		UObject* Parent;
		TMulticastScriptDelegate<FWeakObjectPtr>* TheDelegatePtr;
		FInlineDelegateWrapper(UObject* InParent, TMulticastScriptDelegate<FWeakObjectPtr>* InTheDelegatePtr):Parent(InParent),TheDelegatePtr(InTheDelegatePtr){}

		virtual void ProcessMulticastDelegate(void* Buffer) override
		{
			TheDelegatePtr->ProcessMulticastDelegate<UObject>(Buffer);
		}
		virtual void Add(TScriptDelegate<FWeakObjectPtr>& Handle) override
		{
			TheDelegatePtr->Add(Handle);
		}
		virtual void Clear() override
		{
			TheDelegatePtr->Clear();
		}
		virtual void RemoveUObject(UObject* Ptr) override
		{
			TheDelegatePtr->RemoveAll(Ptr);
		}
		virtual void RemoveUFunction(UObject* Ptr, FName InFunctionName) override
		{
			TheDelegatePtr->Remove(Ptr, InFunctionName);
		}
	};

#if ENGINE_MINOR_VERSION >= 23
#include "UnrealType.h"
	struct FSparseDelegateWrapper :public IDelegateWrapper
	{
		UObject* Parent;
		UMulticastSparseDelegateProperty* Property;
		FSparseDelegateWrapper(UObject* InParent, UMulticastSparseDelegateProperty* InProperty) :Parent(InParent), Property(InProperty) {}
		const FMulticastScriptDelegate* GetMD()
		{
			void* PropertyValue = Property->GetPropertyValuePtr_InContainer(Parent);
			const FMulticastScriptDelegate* MD = Property->GetMulticastDelegate(PropertyValue);
			return MD;
		}
		virtual void ProcessMulticastDelegate(void* Buffer) override
		{
			const FMulticastScriptDelegate* MD = GetMD();
			if(MD)
				MD->ProcessMulticastDelegate<UObject>(Buffer);
		}
		virtual void Add(TScriptDelegate<FWeakObjectPtr>& Handle) override
		{
			Property->AddDelegate(Handle, Parent);
		}
		virtual void Clear() override
		{
			Property->ClearDelegate(Parent);
		}
		virtual void RemoveUObject(UObject* Ptr) override
		{
			const FMulticastScriptDelegate* MD = GetMD();
			if (MD)
				const_cast<FMulticastScriptDelegate*>(MD)->RemoveAll(Ptr);
		}
		virtual void RemoveUFunction(UObject* Ptr, FName InFunctionName) override
		{
			const FMulticastScriptDelegate* MD = GetMD();
			if (MD)
				const_cast<FMulticastScriptDelegate*>(MD)->Remove(Ptr, InFunctionName);
		}
	};
#endif

// 	TMulticastScriptDelegate<FWeakObjectPtr>* TheDelegatePtr;
	TSharedPtr<IDelegateWrapper> TheDelegatePtr;
public:
	TMap<lua_State*, TSet<int>> LuaCallBacks;
	TMap<lua_State*, TSet<int>> LuaCallBacksToRemove;
	bool bIsProcessingEvent;
	UFunction* FunSig;

	void Init(TMulticastScriptDelegate<FWeakObjectPtr>& Delegate, UFunction* _FunSig);
	void Init(void* Delegate, UFunction* _FunSig);

#if ENGINE_MINOR_VERSION >= 23
	void Init(UMulticastSparseDelegateProperty* Property, UObject* Parent);
#endif

	UFUNCTION()
		void NoUseAtAll();

	virtual void ProcessEvent(UFunction* Function, void* Parms) override;

		int Add(lua_State* inL);
		void Remove(lua_State* inL);
		void RemoveAll();
		void Destroy();
		void Clear();
		void Fire(lua_State* inL);
		void RemoveUObject(UObject* Ptr);
		void RemoveUFunction(UObject* Ptr, FName InFunctionName);
};
