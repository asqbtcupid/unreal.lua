#pragma once
#include "UserDefineClassMacro.h"
#include "LuaArrayHelper.h"

namespace UnrealLuaCheck
{
	template<typename T>
	struct __eq_Exists<TArray<T>>
	{
		enum {Value = UnrealLuaCheck::__eq_Exists<T>::Value};
	};
}

template<class T>
struct popiml< TArray<T> > {
	static TArray<T> pop(lua_State *L, int index)
	{
		ue_lua_pushvalue(L, index);
		TArray<T> result;
		ue_lua_pushnil(L);
		while (ue_lua_next(L, -2) != 0)
		{
			result.Add(popiml<T>::pop(L, -1));
			ue_lua_pop(L, 1);
		}
		ue_lua_pop(L, 1);
		return result;
	}
};

template<class INNER_TYPE>
static void PushTableBack(lua_State*inL, const TArray<INNER_TYPE>& Arr, int32 StackIndex)
{
	ue_lua_pushvalue(inL, StackIndex);
	int table_len = ue_lua_objlen(inL, -1);
	for (int i = 1; i <= FMath::Max(table_len, Arr.Num()); i++)
	{
		ue_lua_pushinteger(inL, i);
		if (i <= Arr.Num())
			UTableUtil::push(inL, Arr[i - 1]);
		else
			ue_lua_pushnil(inL);
		ue_lua_rawset(inL, -3);
	}
}

template<bool, class T>
struct PushBackValueToLuaStack_Internal
{
	static void Call(lua_State*inL, TArray<T>& Value, int32 StackIndex)
	{
		PushTableBack(inL, Value, StackIndex);
	}
};

template<class T>
struct PushBackValueToLuaStack_Internal<true, T>
{
	static void Call(lua_State*inL, TArray<T>& Value, int32 StackIndex)
	{
		TArray<T>& TempIns = GetTempIns<TArray<T>>();
		TempIns = Value; 
		PushTableBack(inL, TempIns, StackIndex); 
	}
};

template<class T>
inline void ReadArrFromPropertyOrTable(TArray<T>& Result, lua_State*inL, int index)
{
	ULuaArrayHelper* ArrHelper = (ULuaArrayHelper*)UnrealLua::IsCppPtr(inL, index);
	if (ArrHelper)
	{
		ensureAlwaysMsgf(sizeof(T) == ArrHelper->GetElementSize(), TEXT("SomeBug"));
		TArray<T>*Right = ArrHelper->ValuePtr<T>();
		if (Right != &Result)
			Result = *(TArray<T>*)Right;
	}
	else if (ue_lua_istable(inL, index))
	{
		Result = popiml<TArray<T>>::pop(inL, index);
	}
	else
	{
		ensureAlwaysMsgf(0, TEXT("not arr"));
		UnrealLua::ReportError(inL, "not arr");
	}
}

template<class T>
struct CustomTypeToLua<TArray<T>>:UnrealLua::DefaultCustomEnum
{
	using INNER_TYPE = T;
	enum {
		Value = true,
		PopFromLuaStackByPointer = false,
		NeedCreateTempInsWhenPushRet = true,
	};
	static void ReadValueFromLuaStack(TArray<INNER_TYPE>& Result, lua_State*inL, int32 Index) 
	{
		ReadArrFromPropertyOrTable(Result, inL, Index); 
	}
	static int PushBackValueToLuaStack(lua_State*inL, TArray<T>& Value, int32 Index)
	{
		ULuaArrayHelper* ArrHelper = (ULuaArrayHelper*)UnrealLua::IsCppPtr(inL, Index);
		if (ArrHelper)
		{
			ensureAlwaysMsgf(sizeof(T) == ArrHelper->GetElementSize(), TEXT("SomeBug"));
			TArray<T>*Right = ArrHelper->ValuePtr<T>();
			if (Right != &Value)
				*(TArray<T>*)Right = Value;
		}
		else if (ue_lua_istable(inL, Index))
		{
			const TArray<T>& Temp = UnrealLua::MayNeedTempIns(Value);
			PushTableBack(inL, Temp, Index);
		}
		else
		{
			ensureAlwaysMsgf(0, TEXT("not arr"));
			UnrealLua::ReportError(inL, "not arr");
		}
		return 1;
	}
	static int PushRetValueToLuaStack(lua_State*inL, const TArray<T>& Value)
	{
		const TArray<T>& Temp = UnrealLua::MayNeedTempIns(Value);
		return PushValueToLuaStack(inL, Temp);
	}
	static int PushValueToLuaStack(lua_State*inL, const TArray<T>& Value)
	{
		ue_lua_newtable(inL);
		for (int i = 0; i < Value.Num(); i++)
		{
			UTableUtil::push(inL, i + 1);
			UTableUtil::push(inL, Value[i]);
			ue_lua_rawset(inL, -3);
		}
		return 1;
	}
};

#define LUA_GLUE_ARR_TRAIT(NAME_AFTER_FIX, INNER_TYPE) \
	template<>\
	struct CustomTypeToLua<TArray<INNER_TYPE>>:UnrealLua::DefaultCustomEnum\
	{\
		enum{\
			Value = true,\
			PopFromLuaStackByPointer = true,\
			NeedCreateTempInsWhenPushRet = true,\
		};\
		static void ReadValueFromLuaStack(TArray<INNER_TYPE>& Result, lua_State*inL, int32 Index) \
		{\
			if (UnrealLua::IsGlueTArray(inL, Index))\
			{\
				TArray<INNER_TYPE>* Ptr = (TArray<INNER_TYPE>*) tovoid(inL, Index);\
				Result = *Ptr;\
			}\
			else\
				ReadArrFromPropertyOrTable(Result, inL, Index);\
		}\
		static void ReadPtrFromLuaStack(TArray<INNER_TYPE>*& Value, lua_State*inL, int32 Index) \
		{\
			if (UnrealLua::IsGlueTArray(inL, Index))\
			{\
				TArray<INNER_TYPE>* Ptr = (TArray<INNER_TYPE>*) tovoid(inL, Index);\
				Value = Ptr;\
			}\
			else\
			{\
				ULuaArrayHelper* ArrHelper = (ULuaArrayHelper*)UnrealLua::IsCppPtr(inL, Index);\
				if (ArrHelper)\
				{\
					Value = ArrHelper->ValuePtr<INNER_TYPE>();\
					return;\
				}\
				TArray< INNER_TYPE >& TempIns = GetTempIns<TArray< INNER_TYPE >>(); \
				Value = &TempIns;\
				if (ue_lua_istable(inL, Index))\
				{\
					TempIns = popiml<TArray<INNER_TYPE>>::pop(inL, Index);\
				}\
				else\
				{\
					ensureAlwaysMsgf(0, TEXT("not arr"));\
					UnrealLua::ReportError(inL, "not arr");\
				}\
			}\
		}\
		static int PushBackPtrToLuaStack(lua_State*inL, TArray<INNER_TYPE>* Value, int32 StackIndex) \
		{\
			if (ue_lua_isuserdata(inL, StackIndex))\
			{\
				ue_lua_pushvalue(inL, StackIndex); \
			}\
			else\
			{\
				PushTableBack(inL, *Value, StackIndex);\
			}\
			return 1;\
		}\
	};\
	template<>\
	struct PushPointerToStack<TArray<INNER_TYPE>>\
	{\
		static void Push(lua_State* inL, TArray<INNER_TYPE>* Value, int32 StackIndex)\
		{\
			CustomTypeToLua<TArray<INNER_TYPE>>::PushBackPtrToLuaStack(inL, Value, StackIndex);\
		}\
	};

#define LUA_GLUE_ARR_BODY(NAME_AFTER_FIX, INNER_TYPE) \
	namespace ArrGlue##NAME_AFTER_FIX{\
		static void JustForArrGlue() {}\
		static int32 __IndexForArr(lua_State*inL)\
		{\
			if (ue_lua_isinteger(inL, 2))\
			{\
				TArray<INNER_TYPE>* Ptr = (TArray<INNER_TYPE>*) tovoid(inL, 1);\
				int32 Index = popiml<int>::pop(inL, 2) - 1;\
				TArray<INNER_TYPE>& Value = *Ptr;\
				if (Value.IsValidIndex(Index))\
				{\
					UTableUtil::push(inL, Value[Index]);\
				}\
				else\
				{\
					ue_lua_pushnil(inL);\
				}\
				return 1;\
			}\
			else\
			{\
				const char* key = ue_lua_tostring(inL, 2);\
				ue_lua_getmetatable(inL, 1);\
				ue_lua_pushvalue(inL, 2);\
				ue_lua_rawget(inL, -2);\
				return 1;\
			}\
		}\
		static int32 __NewIndexForArr(lua_State*inL)\
		{\
			TArray<INNER_TYPE>* Ptr = (TArray<INNER_TYPE>*) tovoid(inL, 1);\
			TArray<INNER_TYPE>& Value = *Ptr;\
			int32 Index = ue_lua_tointeger(inL, 2) - 1;\
			if (Value.IsValidIndex(Index))\
			{\
				UTableUtil::read(Value[Index], inL, 3);\
			}\
			else\
			{\
				ensureAlwaysMsgf(false, TEXT("Bug"));\
			}\
			return 0;\
		}\
		static int32 travel(lua_State* inL)\
		{\
			TArray<INNER_TYPE>* Ptr = (TArray<INNER_TYPE>*) tovoid(inL, 1);\
			TArray<INNER_TYPE>& Value = *Ptr;\
			int32 Index = ue_lua_tointeger(inL, 2);\
			if (Index < Value.Num())\
			{\
				ue_lua_pushinteger(inL, Index + 1);\
				UTableUtil::push(inL, Value[Index]);\
				return 2;\
			}\
			else\
			{\
				return 0;\
			}\
		}\
		static int32 pairs(lua_State* inL)\
		{\
			lua_pushcclosure(inL, travel, 0);\
			lua_pushvalue(inL, 1);\
			lua_pushinteger(inL, 0);\
			return 3;\
		}\
		static int32 Table(lua_State* inL)\
		{\
			ue_lua_newtable(inL);\
			TArray<INNER_TYPE>* Ptr = (TArray<INNER_TYPE>*) tovoid(inL, 1); \
			TArray<INNER_TYPE>& Value = *Ptr; \
			for (int i = 0;i <Value.Num();i++)\
			{\
				UTableUtil::push(inL, i+1);\
				UTableUtil::push(inL, Value[i]);\
				ue_lua_rawset(inL, -3);\
			}\
			return 1;\
		}\
		static int32 Add(lua_State*inL)\
		{\
			TArray<INNER_TYPE>* Ptr = (TArray<INNER_TYPE>*) tovoid(inL, 1); \
			Ptr->Add(UTableUtil::pop<INNER_TYPE>(inL, 2));\
			return 0;\
		}\
		static int32 GetData(lua_State*inL)\
		{\
			TArray<INNER_TYPE>* Ptr = (TArray<INNER_TYPE>*) tovoid(inL, 1); \
			UTableUtil::push(inL, Ptr->GetData());\
			return 1;\
		}\
	}\
	LUA_GLUE_BEGIN_NAME(TArray_##NAME_AFTER_FIX, TArray<INNER_TYPE>)\
	LUA_GLUE_FUNCTION_NAME(__len, Num)\
	LUA_GLUE_FUNCTION_OUT(Add, ArrGlue##NAME_AFTER_FIX::Add)\
	LUA_GLUE_OVERLOAD(RemoveAt, void (TArray<INNER_TYPE>::*)(int32))\
	LUA_GLUE_FUNCTION_OUT(Glue, ArrGlue##NAME_AFTER_FIX::JustForArrGlue)\
	LUA_GLUE_FUNCTION_OUT(__index, ArrGlue##NAME_AFTER_FIX::__IndexForArr)\
	LUA_GLUE_FUNCTION_OUT(__newindex, ArrGlue##NAME_AFTER_FIX::__NewIndexForArr)\
	LUA_GLUE_FUNCTION_OUT(__pairs, ArrGlue##NAME_AFTER_FIX::pairs)\
	LUA_GLUE_FUNCTION_OUT(Table, ArrGlue##NAME_AFTER_FIX::Table)\
	LUA_GLUE_FUNCTION_OUT(GetData, ArrGlue##NAME_AFTER_FIX::GetData)\
	LUA_GLUE_END()


#define LUA_GLUE_ARR(NAME_AFTER_FIX, INNER_TYPE) \
	LUA_GLUE_ARR_TRAIT(NAME_AFTER_FIX, INNER_TYPE) \
	LUA_GLUE_ARR_BODY(NAME_AFTER_FIX, INNER_TYPE)

template<class T>
struct TCheckArgIsFit<TArray<T>> {
	static UnrealLua::CheckResult ToCheck(UnrealLua::ArgType _Type, lua_State*inL = nullptr, int32 Index = 0)
	{
		if (_Type == GetStructNewType<TArray<T>>::GetType())
			return 2;
		else {
			
			if (_Type == UTableUtil::GetNewType("ULuaArrayHelper"))
			{
				ue_lua_getfield(inL, Index, "GetInnerType");
				ue_lua_pushvalue(inL, Index);
				ue_lua_call(inL, 1, 1);
				int32 InnerType = ue_lua_tointeger(inL, -1);
				ue_lua_pop(inL, 1);
				return TCheckArgIsFit<typename TDecay<T>::Type>::ToCheck(InnerType, inL, Index);
			}
			else if (_Type == UnrealLua::Type::TYPE_TTABLE)
				return true;
			else
				return false;
		}
	}
};