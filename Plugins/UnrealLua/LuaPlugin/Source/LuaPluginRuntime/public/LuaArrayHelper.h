// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UnrealType.h"
#include "LuaDelegateMulti.h"

class LUAPLUGINRUNTIME_API ULuaArrayHelper
{
public:
	ULuaArrayHelper();
	void* Obj;

	UArrayProperty* Property;
	void Init(void* _Obj, UArrayProperty* _Property);
	void Init_ValuePtr(void* _Obj, UArrayProperty* _Property);
	uint32 GetElementSize() { return Property->Inner->ElementSize; }
	template<class T>
	TArray<T>* ValuePtr()
	{
		return (TArray<T>*)(Obj);
	}
	static ULuaArrayHelper* GetHelper(UObject* _Obj, const FName& PropertyName);

	static ULuaArrayHelper* GetHelperCPP(void* _Obj, UArrayProperty* Property);
	static ULuaArrayHelper* GetHelperCPP_ValuePtr(void* _Obj, UArrayProperty* Property);

	static void Copy(FScriptArrayHelper& SrcArrayHelper, FScriptArrayHelper& DestArrayHelper, UArrayProperty* p);
	void CopyTo(UArrayProperty* p, const void* ptr);
	void CopyFrom(UArrayProperty* p, const void* ptr);

	static void GlueArrCopyTo(UArrayProperty* p, const void* src, const void* dest);

		int32 Num();


		int32 Get(lua_State* inL);
		int32 Set(lua_State*inL);
		void Add(lua_State*inL);
		int32 Remove(lua_State* inL);
		int32 Pop(lua_State* inL);
		void Insert(lua_State* inL);
		void Empty(int32 Slack = 0);
		void Reset();
		int32 Table(lua_State* inL);
		int32 GetInnerType();

		static int32 __index(lua_State* inL);
		static int32 __newindex(lua_State* inL);
		static int32 __len(lua_State* inL);
		static int32 __pairs(lua_State* inL);
		static int32 travel(lua_State* inL);
};
