// Fill out your copyright notice in the Description page of Project Settings.

#include "LuaArrayHelper.h"
#include "UnrealLua.h"
#include "UserDefineClassMacro.h"

ULuaArrayHelper::ULuaArrayHelper()
{
// 	if (!HasAnyFlags(RF_ClassDefaultObject))
// 		LuaCtor("frame.luaarrayhelper", this);
}

void ULuaArrayHelper::Init(void* _Obj, UArrayProperty* _Property)
{
	Property = _Property;
	Obj = Property->ContainerPtrToValuePtr<void>(_Obj);
// 	Ptr_ArrHelper = MakeUnique<FScriptArrayHelper>(Property, Obj);
	// ValuePtr = Property->ContainerPtrToValuePtr<void>(Obj);
}

void ULuaArrayHelper::Init_ValuePtr(void* _Obj, UArrayProperty* _Property)
{
	Property = _Property;
	Obj = _Obj;
// 	Ptr_ArrHelper = MakeUnique<FScriptArrayHelper>(Property, Obj);
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
	ULuaArrayHelper* Result = new ULuaArrayHelper;
	Result->Init(_Obj, Property);
	return Result;
}

ULuaArrayHelper* ULuaArrayHelper::GetHelperCPP_ValuePtr(void* _Obj, UArrayProperty* Property)
{
	ULuaArrayHelper* Result = new ULuaArrayHelper;
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

void ULuaArrayHelper::GlueArrCopyTo(UArrayProperty* p, const void* src, const void* dest)
{
	FScriptArrayHelper SrcArrayHelper(p, src);
	FScriptArrayHelper DestArrayHelper(p, dest);
	Copy(SrcArrayHelper, DestArrayHelper, p);
}

int32 ULuaArrayHelper::Num()
{
	FScriptArrayHelper result(Property, Obj);
	return result.Num();
}

int32 ULuaArrayHelper::Get(lua_State* inL)
{
	//lua arr start at 1
	int32 Index = lua_tointeger(inL, 2) - 1;
	FScriptArrayHelper result(Property, Obj);
	if (!result.IsValidIndex(Index))
	{
// 		ensureMsgf(0, TEXT("Index Invalid"));
		return 0;
	}
	
	UProperty* InnerProperty = Property->Inner;
	UTableUtil::pushproperty(inL, InnerProperty, result.GetRawPtr(Index));
	return 1;
}

int32 ULuaArrayHelper::__index(lua_State* inL)
{
	if (lua_isinteger(inL, 2))
	{
		ULuaArrayHelper* ptr = (ULuaArrayHelper *)tovoidtype<ULuaArrayHelper>(inL, 1);
		return ptr->Get(inL);
	}
	else
	{
		lua_rawget(inL, lua_upvalueindex(1)); 
		return 1;
	}
}

int32 ULuaArrayHelper::__newindex(lua_State* inL)
{
	ULuaArrayHelper* ptr = (ULuaArrayHelper*)tovoidtype<ULuaArrayHelper>(inL, 1);
	ptr->Set(inL);
	return 0;
}

int32 ULuaArrayHelper::__len(lua_State* inL)
{
	ULuaArrayHelper* ptr = (ULuaArrayHelper*)tovoidtype<ULuaArrayHelper>(inL, 1);
	UTableUtil::push(inL, ptr->Num());
	return 1;
}

int32 ULuaArrayHelper::__pairs(lua_State* inL)
{
	lua_pushcclosure(inL, travel, 0);
	lua_pushvalue(inL, 1);
	lua_pushinteger(inL, 0);
	return 3;
}

int32 ULuaArrayHelper::travel(lua_State* inL)
{
	ULuaArrayHelper* ptr = (ULuaArrayHelper*)tovoidtype<ULuaArrayHelper>(inL, 1);
	int32 Index;
	UTableUtil::read(Index, inL, 2);
	if (Index < ptr->Num())
	{
		lua_pushinteger(inL, Index + 1);
		FScriptArrayHelper result(ptr->Property, ptr->Obj);
		UProperty* InnerProperty = ptr->Property->Inner;
		UTableUtil::pushproperty(inL, InnerProperty, result.GetRawPtr(Index));
		return 2;
	}
	else
		return 0;
}

int32 ULuaArrayHelper::Set(lua_State*inL)
{
	int32 Index = lua_tointeger(inL, 2) - 1;
	FScriptArrayHelper result(Property, Obj);
	result.ExpandForIndex(Index);
	UProperty* InnerProperty = Property->Inner;
	UTableUtil::popproperty(inL, 3, InnerProperty, result.GetRawPtr(Index));
	return 0;
}

void ULuaArrayHelper::Add(lua_State* inL)
{
	FScriptArrayHelper result(Property, Obj);
	UProperty* InnerProperty = Property->Inner;
	int32 Index = result.AddValue();
	UTableUtil::popproperty(inL, 2, InnerProperty, result.GetRawPtr(Index));
}

int32 ULuaArrayHelper::Remove(lua_State* inL)
{
	int32 Index = lua_tointeger(inL, 2) - 1;
	FScriptArrayHelper result(Property, Obj);
	UProperty* InnerProperty = Property->Inner;
	if (!result.IsValidIndex(Index))
	{
		ensureMsgf(0, TEXT("Index Invalid"));
		return 0;
	}

	UTableUtil::pushproperty(inL, InnerProperty, result.GetRawPtr(Index));
	result.RemoveValues(Index);
	return 1;
}

int32 ULuaArrayHelper::Pop(lua_State* inL)
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

void ULuaArrayHelper::Insert(lua_State* inL)
{
	int32 Index = lua_tointeger(inL, 2) - 1;
	FScriptArrayHelper result(Property, Obj);
	UProperty* InnerProperty = Property->Inner;
	result.InsertValues(Index);
	UTableUtil::popproperty(inL, 3, InnerProperty, result.GetRawPtr(Index));
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

int32 ULuaArrayHelper::Table(lua_State* inL)
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

int32 ULuaArrayHelper::GetInnerType()
{
	return UTableUtil::GetTypeOfProperty(Property->Inner);
}

LUA_GLUE_BEGIN(ULuaArrayHelper)
LUA_GLUE_FUNCTION(Table)
LUA_GLUE_FUNCTION(Get)
LUA_GLUE_FUNCTION(Remove)
LUA_GLUE_FUNCTION(Pop)
LUA_GLUE_FUNCTION(__index)
LUA_GLUE_FUNCTION(__newindex)
LUA_GLUE_FUNCTION(__len)
LUA_GLUE_FUNCTION(__pairs)
LUA_GLUE_FUNCTION(Num)
LUA_GLUE_FUNCTION(Add)
LUA_GLUE_FUNCTION(Insert)
LUA_GLUE_FUNCTION(Set)
LUA_GLUE_FUNCTION(Empty)
LUA_GLUE_FUNCTION(Reset)
LUA_GLUE_FUNCTION(GetInnerType)
LUA_GLUE_END()


