#pragma once
#include "TableUtil.h"
#include "UserDefineClassMacro.h"

template<class K, class V>
struct popiml< TMap<K, V> > {
	static TMap<K, V> pop(lua_State *L, int index)
	{
		ue_lua_pushvalue(L, index);
		TMap<K, V> result;
		ue_lua_pushnil(L);
		while (ue_lua_next(L, -2) != 0)
		{
			V value = popiml<V>::pop(L, -1);
			ue_lua_pop(L, 1);
			K key = popiml<K>::pop(L, -1);
			result.Add(key, value);
		}
		ue_lua_pop(L, 1);
		return result;
	}
};

template<class K, class V>
void ReadMapFromLuaStack(TMap<K, V>& Result, lua_State*inL, int index)
{
	ULuaMapHelper* Helper = (ULuaMapHelper*)UnrealLua::IsCppPtr(inL, index);
	if (Helper)
	{
		TMap<K, V>*Right = Helper->ValuePtr<K, V>();
		if (Right != &Result)
			Result = *(TMap<K, V>*)Right;
	}
	else if (ue_lua_istable(inL, index))
	{
		Result = popiml<TMap<K, V>>::pop(inL, index);
	}
	else
	{
		ensureAlwaysMsgf(0, TEXT("not map"));
		UnrealLua::ReportError(inL, "not map");
	}
}

template<class K, class V>
static void PushBackMapToTable(lua_State*inL, TMap<K, V>& Map, int32 index)
{
	const TMap<K, V>& Temp = UnrealLua::MayNeedTempIns(Map);
	ue_lua_newtable(inL);
	ue_lua_pushvalue(inL, index);
	ue_lua_pushnil(inL);
	int i = 1;
	while (ue_lua_next(inL, -2) != 0)
	{
		ue_lua_pop(inL, 1);
		K key = popiml<K>::pop(inL, -1);
		if (!Temp.Contains(key))
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

	for (auto& Pair : Map)
	{
		UTableUtil::push(inL, Pair.Key);
		UTableUtil::push(inL, Pair.Value);
		ue_lua_rawset(inL, -3);
	}
}

template<class K, class V>
static int PushBackMapToLuaStack(lua_State*inL, TMap<K, V>& Map, int32 index)
{
	ULuaMapHelper* Helper = (ULuaMapHelper*)UnrealLua::IsCppPtr(inL, index);
	if (Helper)
	{
		TMap<K, V>* MapPtr = Helper->ValuePtr<K, V>();
		*MapPtr = Map;
		ue_lua_pushvalue(inL, index);
	}
	else if (ue_lua_istable(inL, index))
	{
		PushBackMapToTable(inL, Map, index);
	}
	else
	{
		ensureAlwaysMsgf(0, TEXT("not map"));
		UnrealLua::ReportError(inL, "not map");
	}
	return 1;
}

template<class K, class V>
struct CustomTypeToLua<TMap<K, V>>:UnrealLua::DefaultCustomEnum
{
	enum {
		Value = true, 
		PopFromLuaStackByPointer = false, 
		NeedCreateTempInsWhenPushRet = true, 
	}; 
	static void ReadValueFromLuaStack(TMap<K, V>& Result, lua_State*inL, int32 Index)
	{
		ReadMapFromLuaStack(Result, inL, Index);
	}
	static int PushBackValueToLuaStack(lua_State*inL, TMap<K, V>& Map, int32 index)
	{
		return PushBackMapToLuaStack(inL, Map, index);
	}
// 	static int PushRetValueToLuaStack(lua_State*inL, const TMap<K, V>& Map)
// 	{
// 		const TMap<K, V>& Temp = UnrealLua::MayNeedTempIns(Map);
// 		return PushValueToLuaStack(inL, Temp);
// 	}
	static int PushValueToLuaStack(lua_State*inL, const TMap<K, V>& Map)
	{
		ue_lua_newtable(inL);
		for (auto& ele : Map)
		{
			UTableUtil::push(inL, ele.Key);
			UTableUtil::push(inL, ele.Value);
			ue_lua_rawset(inL, -3);
		}
		return 1;
	}
};

#define LUA_GLUE_MAP_TRAIT(NAME_AFTER_FIX, KEY_TYPE, VALUE_TYPE) \
	template<>\
	struct TIsPopByPointer<TMap<KEY_TYPE, VALUE_TYPE> > {\
		enum { Value = true }; \
	};\
	template<>\
	struct CustomTypeToLua<TMap<KEY_TYPE, VALUE_TYPE>>:UnrealLua::DefaultCustomEnum\
	{\
		enum{\
			Value = true,\
			PopFromLuaStackByPointer = true,\
			NeedCreateTempInsWhenPushRet = true,\
		};\
		static void ReadValueFromLuaStack(TMap<KEY_TYPE, VALUE_TYPE>& Result, lua_State*inL, int32 Index) \
		{\
			if (UnrealLua::IsGlueTMap(inL, Index))\
			{\
				Result = *((TMap<KEY_TYPE, VALUE_TYPE>*)tovoidtype<TMap<KEY_TYPE, VALUE_TYPE> >(inL, Index));\
			}\
			else\
				ReadMapFromLuaStack(Result, inL, Index);\
		}\
		static void ReadPtrFromLuaStack(TMap<KEY_TYPE, VALUE_TYPE>*& Value, lua_State*inL, int32 Index) \
		{\
			if (UnrealLua::IsGlueTMap(inL, Index))\
			{\
				Value = (TMap<KEY_TYPE, VALUE_TYPE>*)tovoidtype<TMap<KEY_TYPE, VALUE_TYPE> >(inL, Index);\
			}\
			else\
			{\
				ULuaMapHelper* Helper = (ULuaMapHelper*)UnrealLua::IsCppPtr(inL, Index);\
				if (Helper)\
				{\
					Value = Helper->ValuePtr<KEY_TYPE, VALUE_TYPE>();\
					return;\
				}\
				TMap<KEY_TYPE, VALUE_TYPE>& TempIns = GetTempIns<TMap< KEY_TYPE, VALUE_TYPE >>(); \
				Value = &TempIns;\
				if (ue_lua_istable(inL, Index))\
				{\
					TempIns = popiml<TMap<KEY_TYPE, VALUE_TYPE>>::pop(inL, Index);\
				}\
				else\
				{\
					ensureAlwaysMsgf(0, TEXT("not map"));\
					UnrealLua::ReportError(inL, "not map");\
				}\
			}\
		}\
		static int PushBackPtrToLuaStack(lua_State*inL, TMap<KEY_TYPE, VALUE_TYPE>* Value, int32 StackIndex) \
		{\
			if (ue_lua_isuserdata(inL, StackIndex))\
			{\
				ue_lua_pushvalue(inL, StackIndex); \
			}\
			else\
			{\
				PushBackMapToTable(inL, *Value, StackIndex);\
			}\
			return 1;\
		}\
	};\
	template<>\
	struct PushPointerToStack<TMap<KEY_TYPE, VALUE_TYPE>>\
	{\
		static void Push(lua_State* inL, TMap<KEY_TYPE, VALUE_TYPE>* Value, int32 StackIndex)\
		{\
			CustomTypeToLua<TMap<KEY_TYPE, VALUE_TYPE>>::PushBackPtrToLuaStack(inL, Value, StackIndex);\
		}\
	};\

#define LUA_GLUE_MAP_BODY(NAME_AFTER_FIX, KEY_TYPE, VALUE_TYPE)\
	namespace MapGlue##NAME_AFTER_FIX{\
		static void JustForArrGlue(){}\
		using MapType = TMap<KEY_TYPE,VALUE_TYPE>;\
		static int32 Add(lua_State*inL)\
		{\
			MapType* MapValue = (MapType*)tovoidtype<MapType>(inL, 1);\
			KEY_TYPE Key;\
			VALUE_TYPE Value;\
			UTableUtil::read(Key, inL, 2);\
			UTableUtil::read(Value, inL, 3);\
			MapValue->Add(Key, Value);\
			return 0;\
		}\
		static int32 Remove(lua_State*inL)\
		{\
			MapType* MapValue = (MapType*)tovoidtype<MapType>(inL, 1);\
			KEY_TYPE Key;\
			UTableUtil::read(Key, inL, 2);\
			MapValue->Remove(Key);\
			return 0;\
		}\
		static int32 __index(lua_State*inL)\
		{\
			ue_lua_pushvalue(inL, 2);\
			int32 Type = ue_lua_rawget(inL, lua_upvalueindex(1));\
			if (Type == LUA_TNIL)\
			{\
				MapType* MapValue = (MapType*)tovoidtype<MapType>(inL, 1); \
				KEY_TYPE Key;\
				UTableUtil::read(Key, inL, 2);\
				VALUE_TYPE* ptr = MapValue->Find(Key);\
				if (ptr)\
					UTableUtil::push(inL, *ptr);\
				else\
					ue_lua_pushnil(inL);\
				return 1;\
			}\
			else\
			{\
				return 1;\
			}\
		}\
		static int32 Table(lua_State* inL)\
		{\
			ue_lua_newtable(inL);\
			MapType* MapValue = (MapType*)tovoidtype<MapType>(inL, 1); \
			for (auto& Pairs : *MapValue)\
			{\
				UTableUtil::push(inL, Pairs.Key);\
				UTableUtil::push(inL, Pairs.Value);\
				ue_lua_rawset(inL, -3);\
			}\
			return 1;\
		}\
		static int32 New(lua_State*inL)\
		{\
			MapType* p = new MapType();\
			ensureAlwaysMsgf(ue_lua_gettop(inL) <= 1, TEXT(""));\
			if (ue_lua_gettop(inL) == 1)\
			{\
				UTableUtil::read(*p, inL, 1);\
			}\
			const char* name = LUA_GLUE_STR(TMap_##NAME_AFTER_FIX);\
			pushstruct_gc(inL, name, p);\
			return 1;\
		}\
	}\
	LUA_GLUE_BEGIN_NAME(TMap_##NAME_AFTER_FIX, MapGlue##NAME_AFTER_FIX::MapType)\
	LUA_GLUE_FUNCTION_NAME(__len, Num)\
	LUA_GLUE_FUNCTION_OUT(Add, MapGlue##NAME_AFTER_FIX::Add)\
	LUA_GLUE_FUNCTION_OUT(Remove, MapGlue##NAME_AFTER_FIX::Remove)\
	LUA_GLUE_FUNCTION_OUT(Glue, MapGlue##NAME_AFTER_FIX::JustForArrGlue)\
	LUA_GLUE_FUNCTION_OUT(__index, MapGlue##NAME_AFTER_FIX::__index)\
	LUA_GLUE_FUNCTION_OUT(__newindex, MapGlue##NAME_AFTER_FIX::Add)\
	LUA_GLUE_FUNCTION_OUT(Table, MapGlue##NAME_AFTER_FIX::Table)\
	LUA_GLUE_FUNCTION_OUT(New, MapGlue##NAME_AFTER_FIX::New)\
	LUA_GLUE_END()


#define LUA_GLUE_MAP(NAME_AFTER_FIX, KEY_TYPE, VALUE_TYPE) \
		LUA_GLUE_MAP_TRAIT(NAME_AFTER_FIX, KEY_TYPE, VALUE_TYPE)\
		LUA_GLUE_MAP_BODY(NAME_AFTER_FIX, KEY_TYPE, VALUE_TYPE)

template<class K, class V>
struct TCheckArgIsFit<TMap<K, V>> {
	static UnrealLua::CheckResult ToCheck(UnrealLua::ArgType _Type, lua_State*inL = nullptr, int32 Index = 0)
	{
		if (_Type == GetStructNewType<TMap<K, V>>::GetType())
			return true;
		else {
			if (_Type == UTableUtil::GetNewType("ULuaMapHelper"))
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