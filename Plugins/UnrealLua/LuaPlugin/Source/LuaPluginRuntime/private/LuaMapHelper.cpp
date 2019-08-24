// Fill out your copyright notice in the Description page of Project Settings.

#include "LuaMapHelper.h"
#include "UnrealLua.h"
#include "UnrealType.h"
#include "UserDefineClassMacro.h"

ULuaMapHelper::ULuaMapHelper()
{
// 	if (!HasAnyFlags(RF_ClassDefaultObject))
// 		 LuaCtor("frame.luamaphelper", this);
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

int32 ULuaMapHelper::__index(lua_State* inL)
{
	const char* key = lua_tostring(inL, 2);
	lua_getmetatable(inL, 1);
	lua_pushvalue(inL, 2);
	lua_rawget(inL, -2);
	if (lua_isnil(inL, -1))
	{
		ULuaMapHelper* p = (ULuaMapHelper*)tovoidtype<ULuaMapHelper>(inL, 1);
		return p->Get(inL);
	}
	else
	{
		return 1;
	}
}

int32 ULuaMapHelper::__newindex(lua_State* inL)
{
	ULuaMapHelper* p = (ULuaMapHelper*)tovoidtype<ULuaMapHelper>(inL, 1);
	p->Set(inL);
	return 0;
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
	ULuaMapHelper* Result = new ULuaMapHelper;
	Result->Init(_Obj, Property);
	return Result;
}

ULuaMapHelper* ULuaMapHelper::GetHelperCPP_ValuePtr(void* _Obj, UMapProperty* Property)
{
	ULuaMapHelper* Result = new ULuaMapHelper;
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

void ULuaMapHelper::GlueMapCopyTo(UMapProperty* p, const void* src, const void* dest)
{
	FScriptMapHelper SrcHelper(p, src);
	FScriptMapHelper DestHelper(p, dest);
	Copy(SrcHelper, DestHelper, p);
}

int32 ULuaMapHelper::Num()
{
	FScriptMapHelper result(Property, Obj);
	return result.Num();
}

int32 ULuaMapHelper::Get(lua_State* inL)
{
#if LuaDebug
	if (lua_gettop(inL) < 2)
	{
		ensureAlwaysMsgf(0, TEXT("no key"));
		return 0;
	}
#endif
	FScriptMapHelper result(Property, Obj);
	UProperty* CurrKeyProp = Property->KeyProp;
	const int32 KeyPropertySize = CurrKeyProp->ElementSize * CurrKeyProp->ArrayDim;
	void* KeyStorageSpace = FMemory_Alloca(KeyPropertySize);
	CurrKeyProp->InitializeValue(KeyStorageSpace);
	UTableUtil::popproperty(inL, 2, CurrKeyProp, KeyStorageSpace);
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

void ULuaMapHelper::Set(lua_State* inL)
{
#if LuaDebug
	if (lua_gettop(inL) < 3)
	{
		ensureAlwaysMsgf(0, TEXT("too less param"));
		return ;
	}
#endif
	FScriptMapHelper result(Property, Obj);
	UProperty* CurrKeyProp = Property->KeyProp;
	const int32 KeyPropertySize = CurrKeyProp->ElementSize * CurrKeyProp->ArrayDim;
	void* KeyStorageSpace = FMemory_Alloca(KeyPropertySize);
	CurrKeyProp->InitializeValue(KeyStorageSpace);
	UTableUtil::popproperty(inL, 2, CurrKeyProp, KeyStorageSpace);

	if (lua_isnil(inL, 3))
	{
		result.RemovePair(KeyStorageSpace);
	}
	else
	{
		uint8* Result = result.FindValueFromHash(KeyStorageSpace);
		if (Result)
		{
			UTableUtil::popproperty(inL, 3, Property->ValueProp, Result - Property->MapLayout.ValueOffset);
		}
		else
			Add(inL);
	}
	CurrKeyProp->DestroyValue(KeyStorageSpace);

}

void ULuaMapHelper::Add(lua_State* inL)
{
#if LuaDebug
	if (lua_gettop(inL) < 3)
	{
		ensureAlwaysMsgf(0, TEXT("too less param"));
		return;
	}
#endif
	FScriptMapHelper result(Property, Obj);

	UProperty* CurrKeyProp = Property->KeyProp;
	const int32 KeyPropertySize = CurrKeyProp->ElementSize * CurrKeyProp->ArrayDim;
	void* KeyStorageSpace = FMemory_Alloca(KeyPropertySize);
	CurrKeyProp->InitializeValue(KeyStorageSpace);
	UTableUtil::popproperty(inL, 2, CurrKeyProp, KeyStorageSpace);
	

	UProperty* CurrValueProp = Property->ValueProp;
	const int32 ValuePropertySize = CurrValueProp->ElementSize * CurrValueProp->ArrayDim;
	uint8* ValueStorageSpace = (uint8*)FMemory_Alloca(ValuePropertySize);
	CurrValueProp->InitializeValue(ValueStorageSpace);
	UTableUtil::popproperty(inL, 3, CurrValueProp, ValueStorageSpace-Property->MapLayout.ValueOffset);


	result.AddPair(KeyStorageSpace, ValueStorageSpace);

	CurrValueProp->DestroyValue(ValueStorageSpace);
	CurrKeyProp->DestroyValue(KeyStorageSpace);

}

void ULuaMapHelper::Reset()
{
	FScriptMapHelper result(Property, Obj);
	result.EmptyValues(0);
}

int32 ULuaMapHelper::Table(lua_State* inL)
{
	FScriptMapHelper result(Property, Obj);
	lua_newtable(inL);
	for (int32 i = 0; i < result.Num(); ++i)
	{
		uint8* PairPtr = result.GetPairPtr(i);
		UTableUtil::pushproperty(inL, Property->KeyProp, PairPtr);
		UTableUtil::pushproperty(inL, Property->ValueProp, PairPtr);
		lua_rawset(inL, -3);
	}
	return 1;
}


int32 ULuaMapHelper::__pairs(lua_State* inL)
{
	lua_pushinteger(inL, 0);
	lua_pushcclosure(inL, travel, 1);
	lua_pushvalue(inL, 1);
	lua_pushinteger(inL, 0);
	return 3;
}

int32 ULuaMapHelper::travel(lua_State* inL)
{
	ULuaMapHelper* p = (ULuaMapHelper*)tovoidtype<ULuaMapHelper>(inL, 1);
	int32 Index = lua_tointeger(inL, lua_upvalueindex(1));
	FScriptMapHelper result(p->Property, p->Obj);
	if (Index < result.Num())
	{
		uint8* PairPtr = result.GetPairPtr(Index);
		UTableUtil::pushproperty(inL, p->Property->KeyProp, PairPtr);
		UTableUtil::pushproperty(inL, p->Property->ValueProp, PairPtr);
		lua_pushinteger(inL, Index + 1);
		lua_replace(inL, lua_upvalueindex(1));
		return 2;
	}
	else
		return 0;
}

LUA_GLUE_BEGIN(ULuaMapHelper)
LUA_GLUE_FUNCTION(Table)
LUA_GLUE_FUNCTION(Get)
LUA_GLUE_FUNCTION(Set)
LUA_GLUE_FUNCTION(Add)
LUA_GLUE_FUNCTION(Num)
LUA_GLUE_FUNCTION(Reset)
LUA_GLUE_FUNCTION(__index)
LUA_GLUE_FUNCTION(__newindex)
LUA_GLUE_FUNCTION(__pairs)
LUA_GLUE_END()


