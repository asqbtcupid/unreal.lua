#pragma once

#include "TableUtil.h"
#include "UserDefineClassMacro.h"
#include "ShareRefGlueMacro.h"

#define LUA_GLUE_SHARED_PTR_TRAIT(NAME_AFTER_FIX, INNER_TYPE, ...) \
	template<>\
	struct CustomTypeToLua< TSharedPtr<INNER_TYPE, ##__VA_ARGS__> >:UnrealLua::DefaultCustomEnum\
	{\
		enum{\
			Value = true,\
			NeedCreateTempInsWhenPushRet = false,\
			PopFromLuaStackByPointer = false,\
			HasCtor = THasCtor<INNER_TYPE>::Value && !TIsAbstract<INNER_TYPE>::Value,\
			HasDtor = true,\
		};\
		static int PushRetValueToLuaStack(lua_State*inL, const TSharedPtr<INNER_TYPE, ##__VA_ARGS__>& Value)\
		{\
			TSharedPtr<INNER_TYPE, ##__VA_ARGS__>* NewPtr = new TSharedPtr<INNER_TYPE, ##__VA_ARGS__>(Value);\
			pushstruct_gc(inL, LUA_GLUE_STR(TSharedPtr_##NAME_AFTER_FIX), NewPtr);\
			return 1;\
		}\
	};

#define LUA_GLUE_SHARED_PTR_BODY(NAME_AFTER_FIX, INNER_TYPE, ...) \
	namespace SharedPtrGlue##NAME_AFTER_FIX{\
		static int32 _IsTSharedPtr(){return 0;}\
		using SharedType = TSharedPtr<INNER_TYPE, ##__VA_ARGS__>;\
		using SharedRefType = TSharedRef<INNER_TYPE, ##__VA_ARGS__>;\
		static int32 Table(lua_State* inL)\
		{\
			SharedType* Ptr = (SharedType*)tovoid(inL, 1);\
			ue_lua_newtable(inL);\
			UTableUtil::push(inL, "ObjectPtr");\
			UTableUtil::push(inL, *(Ptr->Get()));\
			ue_lua_rawset(inL, -3);\
			return 1;\
		}\
		static int32 New(lua_State* inL)\
		{\
			SharedType* Ptr = TSafeCtor<SharedType>();\
			ensureAlwaysMsgf(ue_lua_gettop(inL) <= 1, TEXT(""));\
			if (ue_lua_gettop(inL) == 1)\
			{\
				INNER_TYPE* Data = (INNER_TYPE*)tovoid(inL, 1);\
				ue_lua_getmetatable(inL, 1);\
				ue_lua_getfield(inL, -1, "_IsTSharedPtr");\
				if(ue_lua_isnil(inL,-1))\
				{\
					Data = TSafeCopy<INNER_TYPE>(Data);\
					if (Data)\
						*Ptr = MakeShareable(Data);\
				}\
				else\
				{\
					SharedRefType* Right = (SharedRefType*)Data;\
					*Ptr = *Right;\
				}\
			}\
			pushstruct_gc(inL, LUA_GLUE_STR(TSharedPtr_##NAME_AFTER_FIX), Ptr);\
			return 1;\
		}\
		static int32 Set(lua_State* inL)\
		{\
			SharedType* Ptr = (SharedType*)tovoid(inL, 1);\
			ensureAlwaysMsgf(ue_lua_gettop(inL) == 2, TEXT(""));\
			if (ue_lua_gettop(inL) == 2)\
			{\
				INNER_TYPE* Data = (INNER_TYPE*)tovoid(inL, 2);\
				if (Data == nullptr){\
					*Ptr = nullptr; \
					return 0;\
				}\
				ue_lua_getmetatable(inL, 2);\
				ue_lua_getfield(inL, -1, "_IsTSharedPtr");\
				if(ue_lua_isnil(inL,-1))\
				{\
					ue_lua_pop(inL, 1);\
					ue_lua_getfield(inL, -1, "_IsTSharedRef");\
					if(ue_lua_isnil(inL,-1))\
					{\
						Data = TSafeCopy<INNER_TYPE>(Data); \
						if (Data)\
						*Ptr = MakeShareable(Data); \
					}\
					else\
					{\
						SharedType* Right = (SharedType*)Data;\
						*Ptr = *Right;\
					}\
				}\
				else\
				{\
					SharedType* Right = (SharedType*)Data;\
					*Ptr = *Right;\
				}\
			}\
			return 0;\
		}\
		static int32 ProxyFunc(lua_State* inL)\
		{\
			ue_lua_pushvalue(inL, lua_upvalueindex(2));\
			ue_lua_replace(inL, 1);\
			ue_lua_pushvalue(inL, lua_upvalueindex(1));\
			ue_lua_insert(inL, 1);\
			ue_lua_call(inL, ue_lua_gettop(inL) - 1, 1);\
			return 1;\
		}\
		static int32 IndexExtend(lua_State* inL)\
		{\
			SharedType* Ptr = (SharedType*)tovoid(inL, 1);\
			UTableUtil::push(inL, Ptr->Get());\
			ue_lua_pushvalue(inL, 2);\
			ue_lua_gettable(inL, -2);\
			if(ue_lua_isfunction(inL, -1))\
			{\
				UTableUtil::push(inL, Ptr->Get());\
				ue_lua_pushcclosure(inL, ProxyFunc, 2);\
			}\
			return 1;\
		}\
		static int32 NewIndexExtend(lua_State* inL)\
		{\
			SharedType* Ptr = (SharedType*)tovoid(inL, 1);\
			UTableUtil::push(inL, Ptr->Get());\
			ue_lua_pushvalue(inL, 2);\
			ue_lua_pushvalue(inL, 3);\
			ue_lua_settable(inL, -3);\
			return 0;\
		}\
	}\
	LUA_GLUE_BEGIN_NAME(TSharedPtr_##NAME_AFTER_FIX, SharedPtrGlue##NAME_AFTER_FIX::SharedType)\
	LUA_GLUE_FUNCTION(Get)\
	LUA_GLUE_FUNCTION_OUT(Table, SharedPtrGlue##NAME_AFTER_FIX::Table)\
	LUA_GLUE_FUNCTION_OUT(_IsTSharedPtr, SharedPtrGlue##NAME_AFTER_FIX::_IsTSharedPtr)\
	LUA_GLUE_FUNCTION_OUT(New, SharedPtrGlue##NAME_AFTER_FIX::New)\
	LUA_GLUE_FUNCTION_OUT(Set, SharedPtrGlue##NAME_AFTER_FIX::Set)\
	LUA_GLUE_FUNCTION(IsValid)\
	LUA_GLUE_FUNCTION(Reset)\
	LUA_GLUE_FUNCTION(GetSharedReferenceCount)\
	LUA_GLUE_FUNCTION(IsUnique)\
	{"___IndexExtend", &SharedPtrGlue##NAME_AFTER_FIX::IndexExtend, ELuaFuncExportFlag::RF_IndexFuncExtend},\
	{"___NewIndexExtend", &SharedPtrGlue##NAME_AFTER_FIX::NewIndexExtend, ELuaFuncExportFlag::RF_NewIndexFuncExtend},\
	LUA_GLUE_END()


#define LUA_GLUE_SHARED_PTR(NAME_AFTER_FIX, INNER_TYPE, ...) \
		LUA_GLUE_SHARED_PTR_TRAIT(NAME_AFTER_FIX, INNER_TYPE, ##__VA_ARGS__) \
		LUA_GLUE_SHARED_PTR_BODY(NAME_AFTER_FIX, INNER_TYPE, ##__VA_ARGS__)

#define LUA_GLUE_SHARED(NAME_AFTER_FIX, INNER_TYPE, ...)\
		LUA_GLUE_SHARED_REF(NAME_AFTER_FIX, INNER_TYPE, ##__VA_ARGS__)\
		LUA_GLUE_SHARED_PTR(NAME_AFTER_FIX, INNER_TYPE, ##__VA_ARGS__)

template<class T, ESPMode Mode>
struct GetStructNewType<TSharedPtr<T, Mode>>
{
	static UnrealLua::ArgType GetType()
	{
		static int TheType = UnrealLua::ArgType(UnrealLua::TemplateFactor::TSharedPtr, GetStructNewType<T>::GetType());
		return TheType;
	}
};

template<class T, ESPMode Mode>
struct TCheckArgIsFit<TSharedPtr<T, Mode>> {
	static UnrealLua::CheckResult ToCheck(UnrealLua::ArgType _Type, lua_State*inL = nullptr, int32 Index = 0)
	{
		if (_Type == GetStructNewType<TSharedPtr<T, Mode>>::GetType())
			return UnrealLua::CheckResult::FitType;
		else
		{
			int32 ArgStructType = _Type.InnerStructType(UnrealLua::TemplateFactor::TSharedPtr);
			if (ArgStructType > 0 && UTableUtil::CheckIsChildClass(GetStructNewType<T>::GetType(), ArgStructType))
				return UnrealLua::CheckResult::ChildType;
			else
				return UnrealLua::CheckResult::NotFitType;
		}
	}
};