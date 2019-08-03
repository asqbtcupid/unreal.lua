#pragma once
#include "TableUtil.h"

struct UnrealLuaFunctor
{
	UnrealLuaFunctor(lua_State* inL, int StackIndex)
	{
		TheBindingState = UTableUtil::GetMainThread(inL);
		if (!(ue_lua_isfunction(inL, StackIndex) || lua_iscfunction(inL, StackIndex)))
		{
			ensureAlwaysMsgf(0, TEXT("Must be a function"));
			ref = -1;
		}
		else
			ref = UTableUtil::ref_luavalue(inL, StackIndex);
	}
	UnrealLuaFunctor(UnrealLuaFunctor&& Right)
	{
		ref = Right.ref;
		Right.ref = -1;
		TheBindingState = Right.TheBindingState;
	}
	UnrealLuaFunctor& operator=(UnrealLuaFunctor&& Right)
	{
		ref = Right.ref;
		Right.ref = -1;
		TheBindingState = Right.TheBindingState;
		return *this;
	}

	bool IsValid() const
	{
		if (TheBindingState)
		{
			if (!UTableUtil::IsStateShutdown(TheBindingState))
			{
				if (ref > 0)
					return true;
			}
		}
		return false;
	}
	UnrealLuaFunctor(const UnrealLuaFunctor& Right) = delete;
	UnrealLuaFunctor& operator=(const UnrealLuaFunctor& Right) = delete;


	~UnrealLuaFunctor()
	{
		if(IsValid())
			UTableUtil::unref(TheBindingState, ref);
	}
	template<class... T>
	void Call(const T&... args) const
	{
		if(IsValid())
			UTableUtil::call_ref_func_ret<void>(TheBindingState, ref, args...);
		else
			ensureAlwaysMsgf(0, TEXT("bug"));
	}

	template<class RET, class... T>
	RET Callr(const T&... args) const
	{
		
		if (IsValid())
			return UTableUtil::call_ref_func_ret<RET>(TheBindingState, ref, args...);
		else
		{
			ensureAlwaysMsgf(0, TEXT("bug"));
			return GetTempIns<RET>();
		}
	}
	int GetRef() const
	{
		return ref;
	}
private:
	int ref;
	lua_State* TheBindingState;
};
