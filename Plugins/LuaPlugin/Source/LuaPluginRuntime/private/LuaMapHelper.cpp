// Fill out your copyright notice in the Description page of Project Settings.

#include "LuaMapHelper.h"
#include "TableUtil.h"
#include "UnrealType.h"

ULuaMapHelper::ULuaMapHelper()
{
	 LuaCtor("frame.luamaphelper", this);
}

void ULuaMapHelper::Init(void* _Obj, UMapProperty* _Property)
{
	Property = _Property;
	Obj = Property->ContainerPtrToValuePtr<void>(_Obj);
}

void ULuaMapHelper::Init_ValuePtr(void* _Obj, UMapProperty* _Property)
{
	Property = _Property;
	Obj = _Obj;
}

ULuaMapHelper* ULuaMapHelper::GetHelper(UObject* _Obj, const FName& PropertyName)
{
	UMapProperty* P = Cast<UMapProperty>(_Obj->GetClass()->FindPropertyByName(PropertyName));
	if (P)
	{
		return GetHelperCPP(_Obj, P);
	}
	else
		return nullptr;
}

ULuaMapHelper* ULuaMapHelper::GetHelperCPP(void* _Obj, UMapProperty* Property)
{
	ULuaMapHelper* Result = NewObject<ULuaMapHelper>();
	Result->Init(_Obj, Property);
	return Result;
}

ULuaMapHelper* ULuaMapHelper::GetHelperCPP_ValuePtr(void* _Obj, UMapProperty* Property)
{
	ULuaMapHelper* Result = NewObject<ULuaMapHelper>();
	Result->Init_ValuePtr(_Obj, Property);
	return Result;
}

void ULuaMapHelper::Copy(FScriptMapHelper& SrcMapHelper, FScriptMapHelper& DestMapHelper, UMapProperty* p)
{
	int32 Num = SrcMapHelper.Num();
	DestMapHelper.EmptyValues(Num);

	if (Num == 0)
	{
		return;
	}

	for (int32 SrcIndex = 0; Num; ++SrcIndex)
	{
		if (SrcMapHelper.IsValidIndex(SrcIndex))
		{
			int32 DestIndex = DestMapHelper.AddDefaultValue_Invalid_NeedsRehash();

			uint8* SrcData = SrcMapHelper.GetKeyPtr(SrcIndex);
			uint8* DestData = DestMapHelper.GetKeyPtr(DestIndex);

			p->KeyProp->CopyCompleteValue_InContainer(DestData, SrcData);
			p->ValueProp->CopyCompleteValue_InContainer(DestData, SrcData);

			--Num;
		}
	}

	DestMapHelper.Rehash();
}

void ULuaMapHelper::CopyTo(UMapProperty* p, void* ptr)
{
	if (ptr == Obj && p == Property)
		return;
	FScriptMapHelper SrcMapHelper(Property, Obj);
	FScriptMapHelper DestMapHelper(p, ptr);
	Copy(SrcMapHelper, DestMapHelper, p);
}

void ULuaMapHelper::CopyFrom(UMapProperty* p, void* ptr)
{
	if (ptr == Obj && p == Property)
		return;
	FScriptMapHelper SrcMapHelper(p, ptr);
	FScriptMapHelper DestMapHelper(Property, Obj);
	Copy(SrcMapHelper, DestMapHelper, Property);
}

int32 ULuaMapHelper::Num()
{
	FScriptMapHelper result(Property, Obj);
	return result.Num();
}

int32 ULuaMapHelper::Get(Flua_State inL)
{
#ifdef LuaDebug
	if (lua_gettop(inL) < 3)
	{
		ensureAlwaysMsgf(0, L"no key");
		return 0;
	}
#endif
	FScriptMapHelper result(Property, Obj);
	UProperty* CurrKeyProp = Property->KeyProp;
	const int32 KeyPropertySize = CurrKeyProp->ElementSize * CurrKeyProp->ArrayDim;
	void* KeyStorageSpace = FMemory_Alloca(KeyPropertySize);
	CurrKeyProp->InitializeValue(KeyStorageSpace);
	UTableUtil::popproperty(inL, 3, CurrKeyProp, KeyStorageSpace);
	uint8* Result = result.FindValueFromHash(KeyStorageSpace);
	CurrKeyProp->DestroyValue(KeyStorageSpace);
	if (Result)
	{
		UTableUtil::pushproperty(inL, Property->ValueProp, Result - Property->MapLayout.ValueOffset);
		return 1;
	}
	else
		return 0;
}

void ULuaMapHelper::Set(Flua_State inL)
{
#ifdef LuaDebug
	if (lua_gettop(inL) < 4)
	{
		ensureAlwaysMsgf(0, L"too less param");
		return ;
	}
#endif
	FScriptMapHelper result(Property, Obj);
	UProperty* CurrKeyProp = Property->KeyProp;
	const int32 KeyPropertySize = CurrKeyProp->ElementSize * CurrKeyProp->ArrayDim;
	void* KeyStorageSpace = FMemory_Alloca(KeyPropertySize);
	CurrKeyProp->InitializeValue(KeyStorageSpace);
	UTableUtil::popproperty(inL, 3, CurrKeyProp, KeyStorageSpace);

	if (lua_isnil(inL, 4))
	{
		result.RemovePair(KeyStorageSpace);
	}
	else
	{
		uint8* Result = result.FindValueFromHash(KeyStorageSpace);
		if (Result)
		{
			UTableUtil::popproperty(inL, 4, Property->ValueProp, Result - Property->MapLayout.ValueOffset);
		}
		else
			Add(inL);
	}
	CurrKeyProp->DestroyValue(KeyStorageSpace);

}

void ULuaMapHelper::Add(Flua_State inL)
{
#ifdef LuaDebug
	if (lua_gettop(inL) < 4)
	{
		ensureAlwaysMsgf(0, L"too less param");
		return;
	}
#endif
	FScriptMapHelper result(Property, Obj);

	UProperty* CurrKeyProp = Property->KeyProp;
	const int32 KeyPropertySize = CurrKeyProp->ElementSize * CurrKeyProp->ArrayDim;
	void* KeyStorageSpace = FMemory_Alloca(KeyPropertySize);
	CurrKeyProp->InitializeValue(KeyStorageSpace);
	UTableUtil::popproperty(inL, 3, CurrKeyProp, KeyStorageSpace);
	

	UProperty* CurrValueProp = Property->ValueProp;
	const int32 ValuePropertySize = CurrValueProp->ElementSize * CurrValueProp->ArrayDim;
	uint8* ValueStorageSpace = (uint8*)FMemory_Alloca(ValuePropertySize);
	CurrValueProp->InitializeValue(ValueStorageSpace);
	UTableUtil::popproperty(inL, 4, CurrValueProp, ValueStorageSpace-Property->MapLayout.ValueOffset);


	result.AddPair(KeyStorageSpace, ValueStorageSpace);

	CurrValueProp->DestroyValue(ValueStorageSpace);
	CurrKeyProp->DestroyValue(KeyStorageSpace);

}

void ULuaMapHelper::Reset()
{
	FScriptMapHelper result(Property, Obj);
	result.EmptyValues(0);
}

int32 ULuaMapHelper::Table(Flua_State inL)
{
	FScriptMapHelper result(Property, Obj);
	lua_newtable(inL);
	for (int32 i = 0; i < result.Num(); ++i)
	{
		uint8* PairPtr = result.GetPairPtr(i);
		UTableUtil::pushproperty(inL, Property->KeyProp, PairPtr + Property->MapLayout.KeyOffset);
		UTableUtil::pushproperty(inL, Property->ValueProp, PairPtr);
		lua_rawset(inL, -3);
	}
	return 1;
}
