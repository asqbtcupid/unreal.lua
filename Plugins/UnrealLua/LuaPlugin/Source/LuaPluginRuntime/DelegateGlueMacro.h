#pragma once

#include "TableUtil.h"
#include "UserDefineClassMacro.h"
#include "Delegate.h"
#include "UnrealLuaFunctor.h"


template<class T>
struct DelegateGlueHelper;

template<class Ret, class... Args>
struct DelegateGlueHelper<TBaseDelegate<Ret, Args...>>
{
	static Ret Call(Args... args, TSharedPtr<UnrealLuaFunctor> LuaCallBack)
	{
		return LuaCallBack->Callr<Ret>(args...);
	}

	static void ExecuteIfBound()
	{
		ensureAlwaysMsgf(0, TEXT("shouldn't come here"));
	};
};

template<class... Args>
struct DelegateGlueHelper<TBaseDelegate<void, Args...>>
{
	static void Call(Args... args, TSharedPtr<UnrealLuaFunctor> LuaCallBack)
	{
		LuaCallBack->Call(args...);
	}

	static void ExecuteIfBound(TBaseDelegate<void, Args...>& Delegate, const Args&... args)
	{
		Delegate.ExecuteIfBound(args...);
	}
};

#define LUA_GLUE_DELEGATE_TRAIT(NAME_AFTER_FIX, INNER_TYPE) 
	template<class Ret, class... Args>
	struct CustomTypeToLua<TBaseDelegate<Ret, Args...>>:UnrealLua::DefaultCustomEnum
	{
		enum{
			Value = true,
			PopFromLuaStackByPointer = true,
			NeedCreateTempInsWhenPushRet = true,
		};
		static void ReadPtrFromLuaStack(TBaseDelegate<Ret, Args...>*& Value, lua_State*inL, int32 Index)
		{
			static TBaseDelegate<Ret, Args...> Temp;
			if (ue_lua_isfunction(inL, Index))
			{
				TSharedPtr<UnrealLuaFunctor> LuaCallBack = MakeShared<UnrealLuaFunctor>(inL, Index);
				Temp.BindStatic(DelegateGlueHelper<TBaseDelegate<Ret, Args...>>::Call, LuaCallBack);
				Value = &Temp;
			}
			else 
			{
				TBaseDelegate<Ret, Args...>* ptr = (TBaseDelegate<Ret, Args...>*)tovoid(inL, Index);
				if (ptr == nullptr)
				{
					Temp = nullptr;
					Value = &Temp;
				}
				else
					Value = ptr;
			}
		}
		static void ReadValueFromLuaStack(TBaseDelegate<Ret, Args...>& Value, lua_State*inL, int32 Index)
		{
			TBaseDelegate<Ret, Args...>* ptr;
			ReadPtrFromLuaStack(ptr, inL, Index);
			Value = *ptr;
		}
	};

#define LUA_GLUE_DELEGATE_BODY(NAME, DLG_TYPE) \
	namespace DelegateGlue##NAME{\
		using DlgType = DLG_TYPE;\
		static int32 New(lua_State*inL)\
		{\
			DlgType* Ptr = new DlgType();\
			if (ue_lua_gettop(inL) >= 1)\
			{\
				TSharedPtr<UnrealLuaFunctor> LuaCallBack = MakeShared<UnrealLuaFunctor>(inL, 1); \
				Ptr->BindStatic(DelegateGlueHelper<DlgType>::Call, LuaCallBack); \
			}\
			pushstruct_gc(inL, #NAME, Ptr);\
			return 1;\
		}\
		static int32 Bind(lua_State*inL)\
		{\
			DlgType* Ptr = (DlgType*)tovoid(inL, 1);\
			TSharedPtr<UnrealLuaFunctor> LuaCallBack = MakeShared<UnrealLuaFunctor>(inL, 2);\
			Ptr->BindStatic(DelegateGlueHelper<DlgType>::Call, LuaCallBack);\
			return 0;\
		}\
	}\
	LUA_GLUE_BEGIN_NAME(NAME, DLG_TYPE)\
	LUA_GLUE_FUNCTION_OUT(New, DelegateGlue##NAME::New)\
	LUA_GLUE_FUNCTION_OUT(Bind, DelegateGlue##NAME::Bind)\
	LUA_GLUE_FUNCTION_NAME(Clear, Unbind)\
	LUA_GLUE_FUNCTION(Unbind)\
	LUA_GLUE_FUNCTION(Execute)\
	LUA_GLUE_FUNCTION_OUT(ExecuteIfBound, DelegateGlueHelper<DLG_TYPE>::ExecuteIfBound)\
	LUA_GLUE_FUNCTION(IsBound)\
	LUA_GLUE_END()

#define LUA_GLUE_DELEGATE(NAME, DLG_TYPE) \
		LUA_GLUE_DELEGATE_TRAIT(NAME, DLG_TYPE) \
		LUA_GLUE_DELEGATE_BODY(NAME, DLG_TYPE)

	template<class Ret, class... Args>
	struct TCheckArgIsFit<TBaseDelegate<Ret, Args...>> {
		static UnrealLua::CheckResult ToCheck(UnrealLua::ArgType _Type, lua_State*inL = nullptr, int32 Index = 0)
		{
			if (_Type == GetStructNewType<TBaseDelegate<Ret, Args...>>::GetType())
				return true;
			else
				return _Type == UnrealLua::Type::TYPE_TFUNCTION;
		}
	};