// Fill out your copyright notice in the Description page of Project Settings.

#include "LuaArrayHelper.h"
#include "TableUtil.h"

void ULuaArrayHelper::Init(void* _Obj, UArrayProperty* _Property)
{
	Obj = _Obj;
	Property = _Property;
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

int32 ULuaArrayHelper::Num()
{
	FScriptArrayHelper_InContainer result(Property, Obj);
	return result.Num();
}

int32 ULuaArrayHelper::Get(Flua_State inL, int32 Index)
{
	//lua arr start at 1
	--Index;
	FScriptArrayHelper_InContainer result(Property, Obj);
	if (!result.IsValidIndex(Index))
		return 0;
	UProperty* InnerProperty = Property->Inner;
	UTableUtil::pushproperty(inL, InnerProperty, result.GetRawPtr(Index));
	return 1;
}

void ULuaArrayHelper::Set(Flua_State inL, int32 Index, Flua_Index Value)
{
	//lua arr start at 1
	--Index;
	FScriptArrayHelper_InContainer result(Property, Obj);
	result.ExpandForIndex(Index);
	UProperty* InnerProperty = Property->Inner;
	UTableUtil::popproperty(inL, Value, InnerProperty, result.GetRawPtr(Index));
}

void ULuaArrayHelper::Add(Flua_State inL, Flua_Index Value)
{
	FScriptArrayHelper_InContainer result(Property, Obj);
	UProperty* InnerProperty = Property->Inner;
	int32 Index = result.AddValue();
	UTableUtil::popproperty(inL, Value, InnerProperty, result.GetRawPtr(Index));
}

int32 ULuaArrayHelper::Remove(Flua_State inL, int32 Index)
{
	--Index;
	FScriptArrayHelper_InContainer result(Property, Obj);
	UProperty* InnerProperty = Property->Inner;
	if (!result.IsValidIndex(Index))
		return 0;

	UTableUtil::pushproperty(inL, InnerProperty, result.GetRawPtr(Index));
	result.RemoveValues(Index);
	return 1;
}

int32 ULuaArrayHelper::Pop(Flua_State inL)
{
	FScriptArrayHelper_InContainer result(Property, Obj);
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
	FScriptArrayHelper_InContainer result(Property, Obj);
	UProperty* InnerProperty = Property->Inner;
	result.InsertValues(Index);
	UTableUtil::popproperty(inL, Value, InnerProperty, result.GetRawPtr(Index));
}

void ULuaArrayHelper::Empty(int32 Slack /*= 0*/)
{
	FScriptArrayHelper_InContainer result(Property, Obj);
	result.EmptyValues(Slack);
}

int32 ULuaArrayHelper::Table(Flua_State inL)
{
	UTableUtil::pushproperty(inL, Property, Obj);
	return 1;
}
