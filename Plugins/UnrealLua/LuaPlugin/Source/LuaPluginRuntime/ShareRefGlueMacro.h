#pragma once

#include "TableUtil.h"
#include "UserDefineClassMacro.h"

template<class T, ESPMode Mode>
struct CustomTypeToLua<TSharedRef< T, Mode>>
{
	enum { 
		Value = true ,
		PopFromLuaStackByPointer = false,
		NeedCreateTempInsWhenPushRet = false,
		HasCtor = false,
		HasDtor = true,
// 		HasDtor = std::is_destructible<T>::value && !TIsAbstract<T>::Value,
	}; 
	static int PushRetValueToLuaStack(lua_State*inL, const TSharedRef< T, Mode>& Value)
	{
		TSharedRef< T, Mode>* NewPtr = new TSharedRef< T, Mode>(Value);
		UTableUtil::push_new(inL, NewPtr);
// 		pushstruct_gc(inL, TTraitStructTypeName<TSharedRef< T, Mode>>(), Value);
		return 1;
	}
};

#define LUA_GLUE_SHARED_REF_TRAIT(NAME_AFTER_FIX, INNER_TYPE, ...) 

#define LUA_GLUE_SHARED_REF_BODY(NAME_AFTER_FIX, INNER_TYPE, ...) \
	namespace SharedRefGlue##NAME_AFTER_FIX{\
		static int32 _IsTSharedRef(){return 0;}\
		using SharedType = TSharedRef<INNER_TYPE, ##__VA_ARGS__>;\
		static int32 Table(lua_State* inL)\
		{\
			SharedType* Ptr = (SharedType*)tovoid(inL, 1);\
			ue_lua_newtable(inL);\
			UTableUtil::push(inL, "ObjectPtr");\
			UTableUtil::push(inL, Ptr->Get());\
			ue_lua_rawset(inL, -3);\
			return 1;\
		}\
		static int32 New(lua_State* inL)\
		{\
			ensureAlwaysMsgf(ue_lua_gettop(inL) == 1, TEXT(""));\
			if (ue_lua_gettop(inL) == 1)\
			{\
				INNER_TYPE* Data = (INNER_TYPE*)tovoid(inL, 1);\
				ue_lua_getmetatable(inL, 1);\
				ue_lua_getfield(inL, -1, "_IsTSharedRef");\
				if(ue_lua_isnil(inL,-1))\
				{\
					Data = TSafeCopy<INNER_TYPE>(Data);\
					if (Data)\
					{\
						SharedType* Ptr = new SharedType(Data);\
						UTableUtil::push_new(inL, Ptr);\
						return 1; \
					}\
				}\
				else\
				{\
					SharedType* Right = (SharedType*)Data;\
					SharedType* Ptr = new SharedType(*Right);\
					UTableUtil::push_new(inL, Ptr);\
					return 1; \
				}\
			}\
			return 0;\
		}\
		static int32 Set(lua_State* inL)\
		{\
			SharedType* Ptr = (SharedType*)tovoid(inL, 1);\
			ensureAlwaysMsgf(ue_lua_gettop(inL) == 2, TEXT(""));\
			if (ue_lua_gettop(inL) == 2)\
			{\
				ue_lua_getmetatable(inL, 2);\
				ue_lua_getfield(inL, -1, "_IsTSharedRef");\
				if(ue_lua_isnil(inL,-1))\
				{\
					INNER_TYPE* Data = (INNER_TYPE*)tovoid(inL, 2);\
					if (Data == nullptr){\
						ensureAlwaysMsgf(0, TEXT("shouldn't come here")); \
					}\
					else\
					{\
						Data = TSafeCopy<INNER_TYPE>(Data); \
						if (Data)\
							*Ptr = SharedType(Data); \
					}\
				}\
				else\
				{\
					SharedType* Right = (SharedType*)tovoid(inL, 1);\
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
	LUA_GLUE_BEGIN_NAME(TSharedRef_##NAME_AFTER_FIX, SharedRefGlue##NAME_AFTER_FIX::SharedType)\
	LUA_GLUE_FUNCTION(Get)\
	LUA_GLUE_FUNCTION_OUT(_IsTSharedRef, SharedRefGlue##NAME_AFTER_FIX::_IsTSharedRef)\
	LUA_GLUE_FUNCTION_OUT(New, SharedRefGlue##NAME_AFTER_FIX::New)\
	LUA_GLUE_FUNCTION_OUT(Set, SharedRefGlue##NAME_AFTER_FIX::Set)\
	LUA_GLUE_FUNCTION_OUT(Table, SharedRefGlue##NAME_AFTER_FIX::Table)\
	LUA_GLUE_FUNCTION(GetSharedReferenceCount)\
	LUA_GLUE_FUNCTION(IsUnique)\
	{"___IndexExtend", &SharedRefGlue##NAME_AFTER_FIX::IndexExtend, ELuaFuncExportFlag::RF_IndexFuncExtend},\
	{"___NewIndexExtend", &SharedRefGlue##NAME_AFTER_FIX::NewIndexExtend, ELuaFuncExportFlag::RF_NewIndexFuncExtend},\
	LUA_GLUE_END()


#define LUA_GLUE_SHARED_REF(NAME_AFTER_FIX, INNER_TYPE, ...) \
		LUA_GLUE_SHARED_REF_TRAIT(NAME_AFTER_FIX, INNER_TYPE, ##__VA_ARGS__) \
		LUA_GLUE_SHARED_REF_BODY(NAME_AFTER_FIX, INNER_TYPE, ##__VA_ARGS__)

template<class T, ESPMode Mode>
struct GetStructNewType<TSharedRef<T, Mode>>
{
	static UnrealLua::ArgType GetType()
	{
		static int TheType = UnrealLua::ArgType(UnrealLua::TemplateFactor::TSharedRef, GetStructNewType<T>::GetType());
		return TheType;
	}
};

template<class T, ESPMode Mode>
struct TCheckArgIsFit<TSharedRef<T, Mode>> {
	static UnrealLua::CheckResult ToCheck(UnrealLua::ArgType _Type, lua_State*inL = nullptr, int32 Index = 0)
	{
		if (_Type == GetStructNewType<TSharedRef<T, Mode>>::GetType())
			return UnrealLua::CheckResult::FitType;
		else
		{
			int32 ArgStructType = _Type.InnerStructType(UnrealLua::TemplateFactor::TSharedRef);
			if (ArgStructType > 0 && UTableUtil::CheckIsChildClass(GetStructNewType<T>::GetType(), ArgStructType))
				return UnrealLua::CheckResult::ChildType;
			else
				return false;
		}
	}
};