// Fill out your copyright notice in the Description page of Project Settings.

#include "LuaSetHelper.h"
#include "TableUtil.h"
#include "../Launch/Resources/Version.h"

ULuaSetHelper::ULuaSetHelper()
{
	if(!HasAnyFlags(RF_ClassDefaultObject))
		LuaCtor("frame.luasethelper", this);
}

void ULuaSetHelper::Init(void* _Obj, USetProperty* _Property)
{
	Property = _Property;
	Obj = Property->ContainerPtrToValuePtr<void>(_Obj);
}

void ULuaSetHelper::Init_ValuePtr(void* _Obj, USetProperty* _Property)
{
	Property = _Property;
	Obj = _Obj;
}

ULuaSetHelper* ULuaSetHelper::GetHelper(UObject* _Obj, const FName& PropertyName)
{
	USetProperty* P = Cast<USetProperty>(_Obj->GetClass()->FindPropertyByName(PropertyName));
	if (P)
	{
		return GetHelperCPP(_Obj, P);
	}
	else
		return nullptr;
}

ULuaSetHelper* ULuaSetHelper::GetHelperCPP(void* _Obj, USetProperty* Property)
{
	ULuaSetHelper* Result = NewObject<ULuaSetHelper>();
	Result->Init(_Obj, Property);
	return Result;
}

ULuaSetHelper* ULuaSetHelper::GetHelperCPP_ValuePtr(void* _Obj, USetProperty* Property)
{
	ULuaSetHelper* Result = NewObject<ULuaSetHelper>();
	Result->Init_ValuePtr(_Obj, Property);
	return Result;
}

void ULuaSetHelper::Copy(FScriptSetHelper& SrcSetHelper, FScriptSetHelper& DestSetHelper, USetProperty* p)
{
	int32 Num = SrcSetHelper.Num();
	DestSetHelper.EmptyElements(Num);

	if (Num == 0)
	{
		return;
	}

	for (int32 SrcIndex = 0; Num; ++SrcIndex)
	{
		if (SrcSetHelper.IsValidIndex(SrcIndex))
		{
			const int32 DestIndex = DestSetHelper.AddDefaultValue_Invalid_NeedsRehash();

			uint8* SrcData = SrcSetHelper.GetElementPtr(SrcIndex);
			uint8* DestData = DestSetHelper.GetElementPtr(DestIndex);

			p->ElementProp->CopyCompleteValue_InContainer(DestData, SrcData);

			--Num;
		}
	}

	DestSetHelper.Rehash();
}

void ULuaSetHelper::CopyTo(USetProperty* p, void* ptr)
{
	if (ptr == Obj && p == Property)
		return;
	FScriptSetHelper SrcSetHelper(Property, Obj);
	FScriptSetHelper DestSetHelper(p, ptr);
	Copy(SrcSetHelper, DestSetHelper, p);
}

void ULuaSetHelper::CopyFrom(USetProperty* p, void* ptr)
{
	if (ptr == Obj && p == Property)
		return;
	FScriptSetHelper SrcSetHelper(p, ptr);
	FScriptSetHelper DestSetHelper(Property, Obj);
	Copy(SrcSetHelper, DestSetHelper, Property);
}

int32 ULuaSetHelper::Num()
{
	FScriptSetHelper result(Property, Obj);
	return result.Num();
}

int32 ULuaSetHelper::Get(Flua_State inL)
{
#ifdef LuaDebug
	if (lua_gettop(inL) < 3)
	{
		ensureAlwaysMsgf(0, L"no key");
		return 0;
	}
#endif
	FScriptSetHelper result(Property, Obj);
	UProperty* CurrKeyProp = Property->ElementProp;
	const int32 KeyPropertySize = CurrKeyProp->ElementSize * CurrKeyProp->ArrayDim;
	void* KeyStorageSpace = FMemory_Alloca(KeyPropertySize);
	CurrKeyProp->InitializeValue(KeyStorageSpace);
	UTableUtil::popproperty(inL, 3, CurrKeyProp, KeyStorageSpace);
#if ENGINE_MINOR_VERSION < 18
	uint8* keyptr = result.FindElementFromHash(KeyStorageSpace);
#else
	uint8* keyptr = nullptr;
	int32 Index = result.FindElementIndexFromHash(KeyStorageSpace);
	if (Index != INDEX_NONE)
	{
		keyptr = result.GetElementPtr(Index);
	}
#endif
	CurrKeyProp->DestroyValue(KeyStorageSpace);

	if (keyptr)
	{
		UTableUtil::push(inL, true);
		return 1;
	}
	else
		return 0;
}

void ULuaSetHelper::Set(Flua_State inL)
{
#ifdef LuaDebug
	if (lua_gettop(inL) < 4)
	{
		ensureAlwaysMsgf(0, L"too less param");
		return;
	}
#endif
	FScriptSetHelper result(Property, Obj);
	UProperty* CurrKeyProp = Property->ElementProp;
	const int32 KeyPropertySize = CurrKeyProp->ElementSize * CurrKeyProp->ArrayDim;
	void* KeyStorageSpace = FMemory_Alloca(KeyPropertySize);
	CurrKeyProp->InitializeValue(KeyStorageSpace);
	UTableUtil::popproperty(inL, 3, CurrKeyProp, KeyStorageSpace);
#if ENGINE_MINOR_VERSION < 18
	uint8* keyptr = result.FindElementFromHash(KeyStorageSpace);
#else
	uint8* keyptr = nullptr;
	int32 Index = result.FindElementIndexFromHash(KeyStorageSpace);
	if (Index != INDEX_NONE)
	{
		keyptr = result.GetElementPtr(Index);
	}
#endif
	if (lua_isnil(inL, -1))
	{
		if(keyptr)
			result.RemoveElement(keyptr);
	}
	else if (lua_toboolean(inL, -1) == false)
	{
		if (keyptr)
			result.RemoveElement(keyptr);
	}
	else
	{
		if (!keyptr)
			result.AddElement(KeyStorageSpace);
	}
	CurrKeyProp->DestroyValue(KeyStorageSpace);
}

void ULuaSetHelper::Add(Flua_State inL)
{
#ifdef LuaDebug
	if (lua_gettop(inL) < 4)
	{
		ensureAlwaysMsgf(0, L"too less param");
		return;
	}
#endif
}

void ULuaSetHelper::Reset()
{
	FScriptSetHelper result(Property, Obj);
	result.EmptyElements(0);
}

int32 ULuaSetHelper::Table(Flua_State inL)
{
	FScriptSetHelper result(Property, Obj);
	lua_newtable(inL);
	for (int32 i = 0; i < result.Num(); ++i)
	{
		UTableUtil::pushproperty(inL, Property->ElementProp, result.GetElementPtr(i));
		lua_pushboolean(inL, true);
		lua_rawset(inL, -3);
	}
	return 1;
}

