// Fill out your copyright notice in the Description page of Project Settings.

#include "LuaArrayHelper.h"
#include "TableUtil.h"

ULuaArrayHelper::ULuaArrayHelper()
{
	LuaCtor("frame.luaarrayhelper", this);
}

void ULuaArrayHelper::Init(void* _Obj, UArrayProperty* _Property)
{
	Property = _Property;
	Obj = Property->ContainerPtrToValuePtr<void>(_Obj);
	// ValuePtr = Property->ContainerPtrToValuePtr<void>(Obj);
}

void ULuaArrayHelper::Init_ValuePtr(void* _Obj, UArrayProperty* _Property)
{
	Property = _Property;
	Obj = _Obj;
}

ULuaArrayHelper* ULuaArrayHelper::GetHelper(UObject* _Obj, const FName& PropertyName)
{
	UArrayProperty* P = Cast<UArrayProperty>(_Obj->GetClass()->FindPropertyByName(PropertyName));
	if (P)
	{
		return GetHelperCPP(_Obj, P);
	}
	else
		return nullptr;
}

ULuaArrayHelper* ULuaArrayHelper::GetHelperCPP(void* _Obj, UArrayProperty* Property)
{
	ULuaArrayHelper* Result = NewObject<ULuaArrayHelper>();
	Result->Init(_Obj, Property);
	return Result;
}

ULuaArrayHelper* ULuaArrayHelper::GetHelperCPP_ValuePtr(void* _Obj, UArrayProperty* Property)
{
	ULuaArrayHelper* Result = NewObject<ULuaArrayHelper>();
	Result->Init_ValuePtr(_Obj, Property);
	return Result;
}

void ULuaArrayHelper::Copy(FScriptArrayHelper& SrcArrayHelper, FScriptArrayHelper& DestArrayHelper, UArrayProperty* p)
{
	int32 Num = SrcArrayHelper.Num();
	if (!(p->Inner->PropertyFlags & CPF_IsPlainOldData))
	{
		DestArrayHelper.EmptyAndAddValues(Num);
	}
	else
	{
		DestArrayHelper.EmptyAndAddUninitializedValues(Num);
	}
	if (Num)
	{
		int32 Size = p->Inner->ElementSize;
		uint8* SrcData = (uint8*)SrcArrayHelper.GetRawPtr();
		uint8* DestData = (uint8*)DestArrayHelper.GetRawPtr();
		if (!(p->Inner->PropertyFlags & CPF_IsPlainOldData))
		{
			for (int32 i = 0; i < Num; i++)
			{
				p->Inner->CopyCompleteValue(DestData + i * Size, SrcData + i * Size);
			}
		}
		else
		{
			FMemory::Memcpy(DestData, SrcData, Num*Size);
		}
	}
}

void ULuaArrayHelper::CopyTo(UArrayProperty* p, const void* ptr)
{
	if (ptr == Obj && p == Property)
		return;
	FScriptArrayHelper SrcArrayHelper(Property, Obj);
	FScriptArrayHelper DestArrayHelper(p, ptr);
	Copy(SrcArrayHelper, DestArrayHelper, p);
}

void ULuaArrayHelper::CopyFrom(UArrayProperty* p, const void* ptr)
{
	if (ptr == Obj && p == Property)
		return;
	FScriptArrayHelper SrcArrayHelper(p, ptr);
	FScriptArrayHelper DestArrayHelper(Property, Obj);
	Copy(SrcArrayHelper, DestArrayHelper, Property);
}

int32 ULuaArrayHelper::Num()
{
	FScriptArrayHelper result(Property, Obj);
	return result.Num();
}

int32 ULuaArrayHelper::Get(Flua_State inL, int32 Index)
{
	//lua arr start at 1
	--Index;
	FScriptArrayHelper result(Property, Obj);
	if (!result.IsValidIndex(Index))
	{
		ensureMsgf(0, L"Index Invalid");
		return 0;
	}
	
	UProperty* InnerProperty = Property->Inner;
	UTableUtil::pushproperty(inL, InnerProperty, result.GetRawPtr(Index));
	return 1;
}

void ULuaArrayHelper::Set(Flua_State inL, int32 Index, Flua_Index Value)
{
	//lua arr start at 1
	--Index;
	FScriptArrayHelper result(Property, Obj);
	result.ExpandForIndex(Index);
	UProperty* InnerProperty = Property->Inner;
	UTableUtil::popproperty(inL, Value, InnerProperty, result.GetRawPtr(Index));
}

void ULuaArrayHelper::Add(Flua_State inL, Flua_Index Value)
{
	FScriptArrayHelper result(Property, Obj);
	UProperty* InnerProperty = Property->Inner;
	int32 Index = result.AddValue();
	UTableUtil::popproperty(inL, Value, InnerProperty, result.GetRawPtr(Index));
}

int32 ULuaArrayHelper::Remove(Flua_State inL, int32 Index)
{
	--Index;
	FScriptArrayHelper result(Property, Obj);
	UProperty* InnerProperty = Property->Inner;
	if (!result.IsValidIndex(Index))
	{
		ensureMsgf(0, L"Index Invalid");
		return 0;
	}

	UTableUtil::pushproperty(inL, InnerProperty, result.GetRawPtr(Index));
	result.RemoveValues(Index);
	return 1;
}

int32 ULuaArrayHelper::Pop(Flua_State inL)
{
	FScriptArrayHelper result(Property, Obj);
	if (result.Num() > 0)
	{
		UProperty* InnerProperty = Property->Inner;
		int32 Index = result.Num() - 1;
		UTableUtil::pushproperty(inL, InnerProperty, result.GetRawPtr(Index));
		result.RemoveValues(Index);
		return 1;
	}
	else
		return 0;
}

void ULuaArrayHelper::Insert(Flua_State inL, int32 Index, Flua_Index Value)
{
	--Index;
	FScriptArrayHelper result(Property, Obj);
	UProperty* InnerProperty = Property->Inner;
	result.InsertValues(Index);
	UTableUtil::popproperty(inL, Value, InnerProperty, result.GetRawPtr(Index));
}

void ULuaArrayHelper::Empty(int32 Slack /*= 0*/)
{
	FScriptArrayHelper result(Property, Obj);
	result.EmptyValues(Slack);
}

void ULuaArrayHelper::Reset()
{
	FScriptArrayHelper result(Property, Obj);
	result.EmptyValues(0);
}

int32 ULuaArrayHelper::Table(Flua_State inL)
{
	FScriptArrayHelper result(Property, Obj);
	lua_newtable(inL);
	for (int32 i = 0; i < result.Num(); ++i)
	{
		lua_pushinteger(inL, i + 1);
		UTableUtil::pushproperty(inL, Property->Inner, result.GetRawPtr(i));
		lua_rawset(inL, -3);
	}
	return 1;
}
