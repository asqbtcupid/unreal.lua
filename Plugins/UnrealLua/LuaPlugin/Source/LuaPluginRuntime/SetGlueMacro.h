#pragma once

#include "TableUtil.h"
#include "UserDefineClassMacro.h"

template<class T>
struct popiml< TSet<T> > {
	static TSet<T> pop(lua_State *L, int index)
	{
		ue_lua_pushvalue(L, index);
		TSet<T> result;
		ue_lua_pushnil(L);
		while (ue_lua_next(L, -2) != 0)
		{
			ue_lua_pop(L, 1);
			result.Add(popiml<T>::pop(L, -1));
		}
		ue_lua_pop(L, 1);
		return result;
	}
};

template<class T>
static void PushBackSetToTable(lua_State*inL, const TSet<T>& Set, int index)
{
	ue_lua_newtable(inL);
	ue_lua_pushvalue(inL, index);
	ue_lua_pushnil(inL);
	int i = 1;
	while (ue_lua_next(inL, -2) != 0)
	{
		ue_lua_pop(inL, 1);
		T key = popiml<T>::pop(inL, -1);
		if (!Set.Contains(key))
		{
			ue_lua_pushvalue(inL, -1);
			ue_lua_rawseti(inL, -4, i);
			i++;
		}
	}
	ue_lua_pushnil(inL);
	while (ue_lua_next(inL, -3) != 0)
	{
		ue_lua_pushnil(inL);
		ue_lua_rawset(inL, -4);
	}
	ue_lua_remove(inL, -2);

	for (auto& Key : Set)
	{
		UTableUtil::push(inL, Key);
		UTableUtil::push(inL, true);
		ue_lua_rawset(inL, -3);
	}
}

template<class K>
static void ReadSetFromLuaStack(TSet<K>& Result, lua_State*inL, int32 Index)
{
	ULuaSetHelper* Helper = (ULuaSetHelper*)UnrealLua::IsCppPtr(inL, Index);
	if (Helper)
	{
		TSet<K>*Right = Helper->ValuePtr<K>();
		if (Right != &Result)
			Result = *Right;
	}
	else if (ue_lua_istable(inL, Index))
	{
		Result = popiml<TSet<K>>::pop(inL, Index);
	}
	else
	{
		ensureAlwaysMsgf(0, TEXT("not set"));
		UnrealLua::ReportError(inL, "not set");
	}
}

template<class K>
struct CustomTypeToLua<TSet<K>>:UnrealLua::DefaultCustomEnum
{
	enum {
		Value = true,
		PopFromLuaStackByPointer = false,
		NeedCreateTempInsWhenPushRet = true,
	};
	static void ReadValueFromLuaStack(TSet<K>& Result, lua_State*inL, int32 Index)
	{
		ReadSetFromLuaStack(Result, inL, Index);
	}
	static int PushValueToLuaStack(lua_State*inL, const TSet<K>& Value)
	{
		ue_lua_newtable(inL);
		for (auto& ele : Value)
		{
			UTableUtil::push(inL, ele);
			UTableUtil::push(inL, true);
			ue_lua_rawset(inL, -3);
		}
		return 1;
	}
	static int PushBackValueToLuaStack(lua_State*inL, TSet<K>& Value, int32 index)
	{
		ULuaSetHelper* Helper = (ULuaSetHelper*)UnrealLua::IsCppPtr(inL, index);
		if (Helper)
		{
			TSet<K>* Ptr = Helper->ValuePtr<K>();
			*Ptr = Value;
			ue_lua_pushvalue(inL, index);
		}
		else if (ue_lua_istable(inL, index))
		{
			PushBackSetToTable(inL, Value, index);
		}
		else
		{
			ensureAlwaysMsgf(0, TEXT("not map"));
			UnrealLua::ReportError(inL, "not map");
		}
		return 1;
	}
};

#define LUA_GLUE_SET_TRAIT(NAME_AFTER_FIX, KEY_TYPE) \
	template<>\
	struct CustomTypeToLua<TSet<KEY_TYPE>>:UnrealLua::DefaultCustomEnum\
	{\
		enum{\
			Value = true,\
			PopFromLuaStackByPointer = true,\
			NeedCreateTempInsWhenPushRet = true,\
		};\
		static void ReadValueFromLuaStack(TSet<KEY_TYPE>& Result, lua_State*inL, int32 Index) \
		{\
			if (UnrealLua::IsGlueTSet(inL, Index))\
			{\
				TSet<KEY_TYPE>* WrapPtr = (TSet<KEY_TYPE>*)tovoid(inL, Index);\
				Result = *WrapPtr;\
			}\
			else\
				ReadSetFromLuaStack(Result, inL, Index);\
		}\
		static void ReadPtrFromLuaStack(TSet<KEY_TYPE>*& Value, lua_State*inL, int32 Index) \
		{\
			if (UnrealLua::IsGlueTSet(inL, Index))\
			{\
				TSet<KEY_TYPE>* WrapPtr = (TSet<KEY_TYPE>*)tovoid(inL, Index);\
				Value = WrapPtr;\
			}\
			else\
			{\
				ULuaSetHelper* Helper = (ULuaSetHelper*)UnrealLua::IsCppPtr(inL, Index);\
				if (Helper)\
				{\
					Value = Helper->ValuePtr<KEY_TYPE>();\
					return;\
				}\
				TSet<KEY_TYPE>& TempIns = GetTempIns<TSet<KEY_TYPE>>(); \
				Value = &TempIns;\
				if (ue_lua_istable(inL, Index))\
				{\
					TempIns = popiml<TSet<KEY_TYPE>>::pop(inL, Index);\
				}\
				else\
				{\
					ensureAlwaysMsgf(0, TEXT("not set"));\
					UnrealLua::ReportError(inL, "not set");\
				}\
			}\
		}\
		static int PushBackPtrToLuaStack(lua_State*inL, TSet<KEY_TYPE>* Value, int32 StackIndex) \
		{\
			if (ue_lua_isuserdata(inL, StackIndex))\
			{\
				ue_lua_pushvalue(inL, StackIndex); \
			}\
			else\
			{\
				PushBackSetToTable(inL, *Value, StackIndex);\
			}\
			return 1;\
		}\
	};\
	template<>\
	struct PushPointerToStack<TSet<KEY_TYPE>>\
	{\
		static void Push(lua_State* inL, TSet<KEY_TYPE>* Value, int32 StackIndex)\
		{\
			CustomTypeToLua<TSet<KEY_TYPE>>::PushBackPtrToLuaStack(inL, Value, StackIndex);\
		}\
	};

#define LUA_GLUE_SET_BODY(NAME_AFTER_FIX, KEY_TYPE) \
	namespace SetGlue##NAME_AFTER_FIX{\
		static void JustForArrGlue(){}\
		using SetType = TSet<KEY_TYPE>;\
		static int32 Add(lua_State*inL)\
		{\
			SetType* Value = (SetType*)tovoid(inL, 1);\
			KEY_TYPE Key;\
			UTableUtil::read(Key, inL, 2);\
			Value->Add(Key);\
			return 0;\
		}\
		static int32 __index(lua_State*inL)\
		{\
			const char* key = ue_lua_tostring(inL, 2);\
			ue_lua_getmetatable(inL, 1);\
			ue_lua_pushvalue(inL, 2);\
			ue_lua_rawget(inL, -2);\
			if (ue_lua_isnil(inL, -1))\
			{\
				SetType* ArrWrapPtr = (SetType*) tovoid(inL, 1);\
				KEY_TYPE Key;\
				UTableUtil::read(Key, inL, 2);\
				if (ArrWrapPtr->Contains(Key))\
					UTableUtil::push(inL, true);\
				else\
					ue_lua_pushnil(inL);\
				return 1;\
			}\
			else\
			{\
				return 1;\
			}\
		}\
		static int32 New(lua_State*inL)\
		{\
			SetType* p = new SetType();\
			ensureAlwaysMsgf(ue_lua_gettop(inL) <= 1, TEXT(""));\
			if (ue_lua_gettop(inL) == 1)\
			{\
				UTableUtil::read(*p, inL, 1);\
			}\
			const char* name = LUA_GLUE_STR(TSet_##NAME_AFTER_FIX);\
			pushstruct_gc(inL, name, p);\
			return 1;\
		}\
		static int32 Remove(lua_State*inL)\
		{\
			SetType* Value = (SetType*)tovoid(inL, 1);\
			KEY_TYPE Key;\
			UTableUtil::read(Key, inL, 2);\
			Value->Remove(Key);\
			return 0;\
		}\
		static int32 Table(lua_State* inL)\
		{\
			ue_lua_newtable(inL);\
			SetType* Value = (SetType*)tovoid(inL, 1);\
			for (auto& Key : *Value)\
			{\
				UTableUtil::push(inL, Key);\
				UTableUtil::push(inL, true);\
				ue_lua_rawset(inL, -3);\
			}\
			return 1;\
		}\
	}\
	LUA_GLUE_BEGIN_NAME(TSet_##NAME_AFTER_FIX, TSet<KEY_TYPE>)\
	LUA_GLUE_FUNCTION_OUT(Glue, SetGlue##NAME_AFTER_FIX::JustForArrGlue)\
	LUA_GLUE_FUNCTION_OUT(New, SetGlue##NAME_AFTER_FIX::New)\
	LUA_GLUE_FUNCTION_OUT(Add, SetGlue##NAME_AFTER_FIX::Add)\
	LUA_GLUE_FUNCTION_OUT(Remove, SetGlue##NAME_AFTER_FIX::Remove)\
	LUA_GLUE_FUNCTION_OUT(__index, SetGlue##NAME_AFTER_FIX::__index)\
	LUA_GLUE_FUNCTION_OUT(__newindex, SetGlue##NAME_AFTER_FIX::Add)\
	LUA_GLUE_FUNCTION_NAME(__len, Num)\
	LUA_GLUE_FUNCTION_OUT(Table, SetGlue##NAME_AFTER_FIX::Table)\
	LUA_GLUE_END()


#define LUA_GLUE_SET(NAME_AFTER_FIX, KEY_TYPE) \
		LUA_GLUE_SET_TRAIT(NAME_AFTER_FIX, KEY_TYPE)\
		LUA_GLUE_SET_BODY(NAME_AFTER_FIX, KEY_TYPE)

template<class T>
struct TCheckArgIsFit<TSet<T>> {
	static UnrealLua::CheckResult ToCheck(UnrealLua::ArgType _Type, lua_State*inL = nullptr, int32 Index = 0)
	{
		if (_Type == GetStructNewType<TSet<T>>::GetType())
			return true;
		else {
			if (_Type == UTableUtil::GetNewType("ULuaSetHelper"))
			{
				// todo
				return true;
			}
			else if (_Type == UnrealLua::Type::TYPE_TTABLE)
				return true;
			else
				return false;
		}
	}
};