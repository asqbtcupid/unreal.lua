#pragma once
#include "TableUtil.h"

template<class Func>
struct TCreateCheckFuncs;

template<class... FuncArgs>
struct TCreateCheckFuncs_Imp
{
	template<class... DefaultArgs>
	static UnrealLuaBlueFunc::CheckFuncsType Get_Imp(DefaultArgs&&... Args)
	{
		TArray<UnrealLuaBlueFunc::CheckFuncType> CheckFuncs =
		{ (UnrealLuaBlueFunc::CheckFuncType(), &TCheckArgIsFit<typename TDecay<FuncArgs>::Type>::ToCheck)... };
		UnrealLuaBlueFunc::CheckFuncsType ParamCountToCheckFunc;
		ParamCountToCheckFunc.CheckFuncs = CheckFuncs;
		static_assert(sizeof...(FuncArgs) < 10, "to many args");
		ParamCountToCheckFunc.ValidParamCount.AddZeroed(10);
		for (int i = 0; i <= sizeof...(FuncArgs); i++)
		{
			if (i < sizeof...(FuncArgs) - sizeof...(DefaultArgs))
				ParamCountToCheckFunc.ValidParamCount[i] = false;
			else
				ParamCountToCheckFunc.ValidParamCount[i] = true;
		}
		return ParamCountToCheckFunc;
	}
};

template<class T, class Ret, class... FuncArgs>
struct TCreateCheckFuncs<Ret(T::*)(FuncArgs...)>
{
	template<class... DefaultArgs>
	static UnrealLuaBlueFunc::CheckFuncsType Get(DefaultArgs&&... Args)
	{
		UnrealLuaBlueFunc::CheckFuncsType ParamCountToCheckFunc = TCreateCheckFuncs_Imp<FuncArgs...>::template Get_Imp(Forward<DefaultArgs>(Args)...);
		ParamCountToCheckFunc.IndexOffset = 2;
		return ParamCountToCheckFunc;
	}
};
template<class T, class Ret, class... FuncArgs>
struct TCreateCheckFuncs<Ret(T::*)(FuncArgs...)const> :public TCreateCheckFuncs < Ret(T::*)(FuncArgs...)> {};

template<class Ret, class... FuncArgs>
struct TCreateCheckFuncs<Ret(*)(FuncArgs...)>
{
	template<class... DefaultArgs>
	static UnrealLuaBlueFunc::CheckFuncsType Get(DefaultArgs&&... Args)
	{
		UnrealLuaBlueFunc::CheckFuncsType ParamCountToCheckFunc = TCreateCheckFuncs_Imp<FuncArgs...>::template Get_Imp(Forward<DefaultArgs>(Args)...);
		ParamCountToCheckFunc.IndexOffset = 1;
		return ParamCountToCheckFunc;
	}
};

template<class T>
struct TIsRefType
{
	using DecayType = typename TDecay<T>::Type;
	enum { Value = TAnd<TIsLValueReferenceType<T>, TIsSame<T, DecayType&>>::Value };
};

template<int N, class... Args>
struct NthIsRefType
{
	using TheOrginalType = typename UE4Tuple_Private::TNthTypeFromParameterPack<N, Args...>::Type;
	enum { Value = TIsRefType<TheOrginalType>::Value };
};

template<int N, class... Args>
using NthType = typename UE4Tuple_Private::TNthTypeFromParameterPack<N, Args...>::Type;

template<class T>
struct PushPointerToStack
{
	static void Push(lua_State* inL, T* Value, int32 StackIndex)
	{
		ue_lua_pushvalue(inL, StackIndex); 
	}
};

template<bool>
struct PushBackOneByeOne
{
	template<class T>
	static void Call(lua_State* inL, T& Value, int32 StackIndex) {}
};

template<>
struct PushBackOneByeOne<true>
{
	template<class T>
	static void Call(lua_State* inL, T* Value, int32 StackIndex, typename TEnableIf<TIsPopByPointer<T>::Value, int>::Type* p = nullptr)
	{
		PushPointerToStack<T>::Push(inL, Value, StackIndex);
	}

	template<class T>
	static void Call(lua_State* inL, T& Value, int32 StackIndex, typename TEnableIf<!TIsPopByPointer<T>::Value, int>::Type* p = nullptr)
	{
		UTableUtil::pushback(inL, StackIndex, Value);
	}
};
template<class T, class... Args>
struct CountPushBack
{
	enum {
		Value = TIsRefType<T>::Value + CountPushBack<Args...>::Value
	};
};

template<class T>
struct CountPushBack<T>
{
	enum
	{
		Value = TIsRefType<T>::Value
	};
};

template<int N, class T, class... OriginalArgs>
struct TMakeDefaultTuple;

template<int N, class T, class... OriginalArgs>
struct TMakeDefaultTuple : public TMakeDefaultTuple<N-1, OriginalArgs...>
{
};

template<class T, class... OriginalArgs>
struct TMakeDefaultTuple<0, T, OriginalArgs...>
{
	using Type = TTuple<T, OriginalArgs...>;

	template<class... DefaultArgs>
	static Type ConstructTuple(const DefaultArgs&... Args)
	{
		return Type(Args...);
	}
};

template<class T>
void ReadFromTupleWithTrueType(T*& Value, T* DefaultValue)
{
	Value = DefaultValue;
}

template<class T>
void ReadFromTupleWithTrueType(T*& Value, T& DefaultValue)
{
	Value = &DefaultValue;
}


template<int N>
struct ReadValueFromDefault {
	template<class T, class... DefaultArgs>
	static void Read(T& Value, TTuple<DefaultArgs...>& DefaultTuple)
	{
		Value = (T)DefaultTuple.template Get<N>();
	}
};

template<int N>
struct ReadValueFromDefault_Struct {
	template<class T, class... DefaultArgs>
	static void Read(T*& Value, TTuple<DefaultArgs...>& DefaultTuple)
	{
		ReadFromTupleWithTrueType(Value, DefaultTuple.template Get<N>());
	}
};

#define Compile_ReadValueFromDefault(N)\
	template<>\
	struct ReadValueFromDefault<N>\
	{\
		template<class T, class... DefaultArgs>\
		static void Read(T& Value, TTuple<DefaultArgs...>& DefaultTuple)\
		{\
			ensureAlwaysMsgf(0, TEXT("Args doesn't enough; bug"));\
		}\
	};\
	template<>\
	struct ReadValueFromDefault_Struct<N>\
	{\
		template<class T, class... DefaultArgs>\
		static void Read(T*& Value, TTuple<DefaultArgs...>& DefaultTuple)\
		{\
			ensureAlwaysMsgf(0, TEXT("Args doesn't enough; bug"));\
		}\
	};

Compile_ReadValueFromDefault(-1)
Compile_ReadValueFromDefault(-2)
Compile_ReadValueFromDefault(-3)
Compile_ReadValueFromDefault(-4)
Compile_ReadValueFromDefault(-5)
Compile_ReadValueFromDefault(-6)
Compile_ReadValueFromDefault(-7)


template <typename Indices, class... Args>
struct TupleOperationHelper_imp;

template <uint32... Indices, class... Args>
struct TupleOperationHelper_imp< TIntegerSequence<uint32, Indices...>, Args... >
{
	template <typename TupleType>
	static void PopAllValueFromStack(lua_State*inL, TupleType&& Tuple, int32 StartIndex)
	{
		int Temp[] = { 0, (UTableUtil::read(Tuple.template Get<Indices>(), inL, Indices + StartIndex), 0)... };
		(void)Temp;
	}

	template<int N, int M, class T, class... DefaultArgs>
	static void PopValueOneByOne_WithDefault(lua_State* inL, T*& Value, uint32 Index, uint32 StackLen, TTuple<DefaultArgs...>& DefaultTuple, typename TEnableIf<TIsPopByPointer<T>::Value, int>::Type* p = nullptr)
	{
		if (Index <= StackLen)
			UTableUtil::read(Value, inL, Index);
		else
		{
			ReadValueFromDefault_Struct<N-M>::Read(Value, DefaultTuple);
		}
	}

	template<int N, int M, class T, class... DefaultArgs>
	static void PopValueOneByOne_WithDefault(lua_State* inL, T& Value, uint32 Index, uint32 StackLen, TTuple<DefaultArgs...>& DefaultTuple, typename TEnableIf<!TIsPopByPointer<T>::Value, int>::Type* p = nullptr)
	{
		if(Index <= StackLen)
			UTableUtil::read(Value, inL, Index);
		else
		{
			ReadValueFromDefault<N-M>::Read(Value, DefaultTuple);
		}
	}

	template <int N, typename TupleType, class... DefaultArgs>
	static void PopAllValueFromStack_WithDefault(lua_State*inL, TupleType&& Tuple, int32 StartIndex, TTuple<DefaultArgs...>& DefaultTuple)
	{
		uint32 StackLen = lua_gettop(inL);
		int Temp[] = { 0, (TupleOperationHelper_imp::template PopValueOneByOne_WithDefault<Indices, N>(inL, Tuple.template Get<Indices>(), Indices + StartIndex, StackLen, DefaultTuple), 0)... };
		(void)Temp;
	}

	template<class TupleType, class FuncType>
#ifdef USE_TUPLE_AUTO_RETURN_TYPES
	static decltype(auto) CallCppFunc(TupleType&& Tuple, FuncType&& Func)
#else
	static auto CallCppFunc(TupleType&& Tuple, FuncType&& Func) -> decltype(Func(Tuple.template Get<Indices>()...))
#endif
	{
		return Func(Tuple.template Get<Indices>()...);
	}

	template<class TupleType>
	static int PushBackRefValue(lua_State* inL, TupleType& Tuple, int32 StartStackIndex)
	{
		int Temp[] = { 0, (PushBackOneByeOne<NthIsRefType<Indices, Args...>::Value>::Call(inL, Tuple.template Get<Indices>(), StartStackIndex + Indices), 0)... };
		(void)Temp;
		return CountPushBack<int32, Args...>::Value;
	}
};
template <class... Args>
struct TupleOperationHelper :TupleOperationHelper_imp<TMakeIntegerSequence<uint32, sizeof...(Args)>, Args...>
{

};

template<class RetType, class T>
int32 Pushret_or_Push(lua_State*inL, const T& value, typename TEnableIf<TIsLValueReferenceType<RetType>::Value, int>::Type*ppp=nullptr )
{
	return UTableUtil::push(inL, value);
}

template<class RetType, class T>
int32 Pushret_or_Push(lua_State*inL, const T& value, typename TEnableIf<!TIsLValueReferenceType<RetType>::Value, int>::Type*ppp = nullptr)
{
	return UTableUtil::push_ret(inL, value);
}

template<bool bIsLeftRef>
struct TPushRet_or_PushPtr_imp
{
	template<class T>
	int32 Push(lua_State*inL, const T& value)
	{
		return UTableUtil::push_ret(inL, value);
	}
};

template<>
struct TPushRet_or_PushPtr_imp<true>
{
	template<class T>
	int32 Push(lua_State*inL, const T& value)
	{
		return UTableUtil::push(inL, value);
	}
};

template<class RetType>
struct TPushRet_or_PushPtr : public TPushRet_or_PushPtr_imp<TIsLValueReferenceType<RetType>::Value>
{};

template<>
struct TPushRet_or_PushPtr<void>
{
	int32 Push(lua_State*inL) { return 0; }
};

template<class T>
struct TraitTupleInerType
{
	using DecayType = typename TDecay<T>::Type;
	using Type = typename std::conditional_t<TIsPopByPointer<DecayType>::Value, DecayType*, DecayType>;
};

template<class ParamType, class T1>
ParamType&& ConvertStructPointBack(T1& Value, typename TEnableIf<!TIsPopByPointer<T1>::Value, int>::Type* p = nullptr)
{
	return (ParamType&&)(Value);
}

template<class ParamType, class T1>
T1& ConvertStructPointBack(T1* Value, typename TEnableIf<TIsPopByPointer<T1>::Value, int>::Type* p = nullptr, typename TEnableIf<!TIsPointer<ParamType>::Value, int>::Type* p1 = nullptr)
{
	if (Value)
		return *Value;
	else
	{
		ensureAlwaysMsgf(0, TEXT("bug"));
		return GetTempIns<T1>();
	}
}

template<class ParamType, class T1>
T1* ConvertStructPointBack(T1* Value, typename TEnableIf<TIsPopByPointer<T1>::Value, int>::Type* p = nullptr, typename TEnableIf<TIsPointer<ParamType>::Value, int>::Type* p1 = nullptr)
{
	return Value;
}

template<class... FuncArgs, class... DefaultArgs>
static int32 LuaHookFunc(lua_State*inL, int32(*func)(lua_State* inL, const FuncArgs&... args), DefaultArgs&&... DefaultsSave)
{
	return func(inL, Forward<DefaultArgs>(DefaultsSave)...);
}


static int32 LuaPopAndCallAndReturn(lua_State*inL, int32(*func)(lua_State* inL))
{
	return func(inL);
}

template<class T>
int32 LuaPopAndCallAndReturn(lua_State*inL, int32(T::*func)(lua_State* inL))
{
	T* ptr = (T*)tovoid(inL, 1);
	return (ptr->*func)(inL);
}

template<class T, class... Args>
int32 LuaPopAndCallAndReturn(lua_State*inL, void(T::*func)(Args...))
{
	return LuaPopAndCallAndReturn(inL, (void(T::*)(Args...) const)(func));
}

template<class T, class... Args>
int32 LuaPopAndCallAndReturn(lua_State*inL, void(T::*func)(Args...) const)
{
	const int FirstIndexExcludeSelf = 2;
	TTuple<typename TraitTupleInerType<Args>::Type...> tuple;
	TupleOperationHelper<Args...>::PopAllValueFromStack(inL, tuple, FirstIndexExcludeSelf);
	T* ptr = (T*)tovoid(inL, 1);
	auto lambda = [ptr, func](typename TraitTupleInerType<Args>::Type&... args) { (ptr->*func)(ConvertStructPointBack<Args>(args)...);};
	TupleOperationHelper<Args...>::CallCppFunc(tuple, lambda);
	return TupleOperationHelper<Args...>::PushBackRefValue(inL, tuple, FirstIndexExcludeSelf);
}

template<class T, class Lambdaf, class... Args>
int32 LuaPopAndCallAndReturn_ForVirtual(lua_State*inL, void (T::*func)(Args...), Lambdaf ActualCallFunc)
{
	return LuaPopAndCallAndReturn_ForVirtual(inL, (void (T::*)(Args...)const)(func), ActualCallFunc);
}

template<class T, class Lambdaf, class... Args>
int32 LuaPopAndCallAndReturn_ForVirtual(lua_State*inL, void (T::*func)(Args...)const, Lambdaf ActualCallFunc)
{
	const int FirstIndexExcludeSelf = 2;
	TTuple<typename TraitTupleInerType<Args>::Type...> tuple;
	TupleOperationHelper<Args...>::PopAllValueFromStack(inL, tuple, FirstIndexExcludeSelf);
	T* ptr = (T*)tovoid(inL, 1);
	auto lambda = [ptr, ActualCallFunc](typename TraitTupleInerType<Args>::Type&... args) { ActualCallFunc(ptr, ConvertStructPointBack<Args>(args)...);};
	TupleOperationHelper<Args...>::CallCppFunc(tuple, lambda);
	return TupleOperationHelper<Args...>::PushBackRefValue(inL, tuple, FirstIndexExcludeSelf);
}

template<class T, class Ret, class... Args>
int32 LuaPopAndCallAndReturn(lua_State*inL, Ret(T::*func)(Args...))
{
	return LuaPopAndCallAndReturn(inL, (Ret(T::*)(Args...)const)(func));
}

template<class T, class Ret, class... Args>
int32 LuaPopAndCallAndReturn(lua_State*inL, Ret(T::*func)(Args...) const)
{
	const int FirstIndexExcludeSelf = 2;
	TTuple<typename TraitTupleInerType<Args>::Type...> tuple;
	TupleOperationHelper<Args...>::PopAllValueFromStack(inL, tuple, FirstIndexExcludeSelf);
	T* ptr = (T*)tovoid(inL, 1);
	auto lambda = [ptr, func](typename TraitTupleInerType<Args>::Type&... args)->Ret { return (ptr->*func)(ConvertStructPointBack<Args>(args)...); };
	Pushret_or_Push<Ret>(inL, TupleOperationHelper<Args...>::CallCppFunc(tuple, lambda));
	return TupleOperationHelper<Args...>::PushBackRefValue(inL, tuple, FirstIndexExcludeSelf) + 1;
}

template<class T, class Ret, class Lambdaf, class... Args>
int32 LuaPopAndCallAndReturn_ForVirtual(lua_State*inL, Ret(T::*func)(Args...), Lambdaf ActualCallFunc)
{
	return LuaPopAndCallAndReturn_ForVirtual(inL, (Ret(T::*)(Args...) const)(func), ActualCallFunc);
}

template<class T, class Ret, class Lambdaf, class... Args>
int32 LuaPopAndCallAndReturn_ForVirtual(lua_State*inL, Ret(T::*func)(Args...)const, Lambdaf ActualCallFunc)
{
	const int FirstIndexExcludeSelf = 2;
	TTuple<typename TraitTupleInerType<Args>::Type...> tuple;
	TupleOperationHelper<Args...>::PopAllValueFromStack(inL, tuple, FirstIndexExcludeSelf);
	T* ptr = (T*)tovoid(inL, 1);
	auto lambda = [ptr, ActualCallFunc](typename TraitTupleInerType<Args>::Type&... args)->Ret { return ActualCallFunc(ptr, ConvertStructPointBack<Args>(args)...);};
	Pushret_or_Push<Ret>(inL, TupleOperationHelper<Args...>::CallCppFunc(tuple, lambda));
	return TupleOperationHelper<Args...>::PushBackRefValue(inL, tuple, FirstIndexExcludeSelf) + 1;
}


template<class... Args>
int32 LuaPopAndCallAndReturn(lua_State*inL, void(*func)(Args...))
{
	const int FirstIndexExcludeSelf = 1;
	TTuple<typename TraitTupleInerType<Args>::Type...> tuple;
	TupleOperationHelper<Args...>::PopAllValueFromStack(inL, tuple, FirstIndexExcludeSelf);
	auto lambda = [func](typename TraitTupleInerType<Args>::Type&... args) { (*func)(ConvertStructPointBack<Args>(args)...);};
	TupleOperationHelper<Args...>::CallCppFunc(tuple, lambda);
	return TupleOperationHelper<Args...>::PushBackRefValue(inL, tuple, FirstIndexExcludeSelf);
}

template<class Ret, class... Args>
int32 LuaPopAndCallAndReturn(lua_State*inL, Ret(*func)(Args...))
{
	const int FirstIndexExcludeSelf = 1;
	TTuple<typename TraitTupleInerType<Args>::Type...> tuple;
	TupleOperationHelper<Args...>::PopAllValueFromStack(inL, tuple, FirstIndexExcludeSelf);
	auto lambda = [func](typename TraitTupleInerType<Args>::Type&... args)->Ret { return (*func)(ConvertStructPointBack<Args>(args)...);};
	Pushret_or_Push<Ret>(inL, TupleOperationHelper<Args...>::CallCppFunc(tuple, lambda));
	return TupleOperationHelper<Args...>::PushBackRefValue(inL, tuple, FirstIndexExcludeSelf) + 1;
}

// with default
template<class T, class... Args, class... DefaultArgs>
int32 LuaPopAndCallAndReturn(lua_State*inL, void(T::*func)(Args...), DefaultArgs&&... Augs)
{
	return LuaPopAndCallAndReturn(inL, (void(T::*)(Args...)const)(func), Forward<DefaultArgs>(Augs)...);
}

template<class T, class... Args, class... DefaultArgs>
int32 LuaPopAndCallAndReturn(lua_State*inL, void(T::*func)(Args...) const, DefaultArgs&&... Augs)
{
	static_assert(sizeof...(Args) >= sizeof...(Augs), "too many default augs");
	const int FirstIndexExcludeSelf = 2;
	auto DefaultTuple = TMakeDefaultTuple<sizeof...(Args)-sizeof...(DefaultArgs), typename TDecay<Args>::Type...>::ConstructTuple(Forward<DefaultArgs>(Augs)...);
	TTuple<typename TraitTupleInerType<Args>::Type...> tuple;
	TupleOperationHelper<Args...>::template PopAllValueFromStack_WithDefault<sizeof...(Args) - sizeof...(DefaultArgs)>(inL, tuple, FirstIndexExcludeSelf, DefaultTuple);
	T* ptr = (T*)tovoid(inL, 1);
	auto lambda = [ptr, func](typename TraitTupleInerType<Args>::Type&... args) { (ptr->*func)(ConvertStructPointBack<Args>(args)...); };
	TupleOperationHelper<Args...>::CallCppFunc(tuple, lambda);
	return TupleOperationHelper<Args...>::PushBackRefValue(inL, tuple, FirstIndexExcludeSelf);
}

template<class T, class Lambdaf, class... Args, class... DefaultArgs>
int32 LuaPopAndCallAndReturn_ForVirtual(lua_State*inL, void (T::*func)(Args...), Lambdaf ActualCallFunc, DefaultArgs&&... Augs)
{
	return LuaPopAndCallAndReturn_ForVirtual(inL, (void(T::*)(Args...)const)(func), ActualCallFunc, Forward<DefaultArgs>(Augs)...);
}

template<class T, class Lambdaf, class... Args, class... DefaultArgs>
int32 LuaPopAndCallAndReturn_ForVirtual(lua_State*inL, void (T::*func)(Args...)const, Lambdaf ActualCallFunc, DefaultArgs&&... Augs)
{
	static_assert(sizeof...(Args) >= sizeof...(Augs), "too many default augs");
	const int FirstIndexExcludeSelf = 2;
	auto DefaultTuple = TMakeDefaultTuple<sizeof...(Args)-sizeof...(DefaultArgs), typename TDecay<Args>::Type...>::ConstructTuple(Forward<DefaultArgs>(Augs)...);
	TTuple<typename TraitTupleInerType<Args>::Type...> tuple;
	TupleOperationHelper<Args...>::template PopAllValueFromStack_WithDefault<sizeof...(Args) - sizeof...(DefaultArgs)>(inL, tuple, FirstIndexExcludeSelf, DefaultTuple);
	T* ptr = (T*)tovoid(inL, 1);
	auto lambda = [ptr, ActualCallFunc](typename TraitTupleInerType<Args>::Type&... args) { ActualCallFunc(ptr, ConvertStructPointBack<Args>(args)...); };
	TupleOperationHelper<Args...>::CallCppFunc(tuple, lambda);
	return TupleOperationHelper<Args...>::PushBackRefValue(inL, tuple, FirstIndexExcludeSelf);
}

template<class T, class Ret, class... Args, class... DefaultArgs>
int32 LuaPopAndCallAndReturn(lua_State*inL, Ret(T::*func)(Args...), DefaultArgs&&... Augs)
{
	return LuaPopAndCallAndReturn(inL, (Ret(T::*)(Args...)const)(func), Forward<DefaultArgs>(Augs)...);
}

template<class T, class Ret, class... Args, class... DefaultArgs>
int32 LuaPopAndCallAndReturn(lua_State*inL, Ret(T::*func)(Args...) const, DefaultArgs&&... Augs)
{
	static_assert(sizeof...(Args) >= sizeof...(Augs), "too many default augs");
	const int FirstIndexExcludeSelf = 2;
	auto DefaultTuple = TMakeDefaultTuple<sizeof...(Args) - sizeof...(DefaultArgs), typename TDecay<Args>::Type...>::ConstructTuple(Forward<DefaultArgs>(Augs)...);
	TTuple<typename TraitTupleInerType<Args>::Type...> tuple;
	TupleOperationHelper<Args...>::template PopAllValueFromStack_WithDefault<sizeof...(Args) - sizeof...(DefaultArgs)>(inL, tuple, FirstIndexExcludeSelf, DefaultTuple);
	T* ptr = (T*)tovoid(inL, 1);
	auto lambda = [ptr, func](typename TraitTupleInerType<Args>::Type&... args)->Ret { return (ptr->*func)(ConvertStructPointBack<Args>(args)...); };
	Pushret_or_Push<Ret>(inL, TupleOperationHelper<Args...>::CallCppFunc(tuple, lambda));
	return TupleOperationHelper<Args...>::PushBackRefValue(inL, tuple, FirstIndexExcludeSelf) + 1;
}

template<class T, class Ret, class Lambdaf, class... Args, class... DefaultArgs>
int32 LuaPopAndCallAndReturn_ForVirtual(lua_State*inL, Ret(T::*func)(Args...), Lambdaf ActualCallFunc, DefaultArgs&&... Augs)
{
	return LuaPopAndCallAndReturn_ForVirtual(inL, (Ret(T::*)(Args...)const)(func), ActualCallFunc, Forward<DefaultArgs>(Augs)...);
}

template<class T, class Ret, class Lambdaf, class... Args, class... DefaultArgs>
int32 LuaPopAndCallAndReturn_ForVirtual(lua_State*inL, Ret(T::*func)(Args...)const, Lambdaf ActualCallFunc, DefaultArgs&&... Augs)
{
	static_assert(sizeof...(Args) >= sizeof...(Augs), "too many default augs");
	const int FirstIndexExcludeSelf = 2;
	auto DefaultTuple = TMakeDefaultTuple<sizeof...(Args)-sizeof...(DefaultArgs), typename TDecay<Args>::Type...>::ConstructTuple(Forward<DefaultArgs>(Augs)...);
	TTuple<typename TraitTupleInerType<Args>::Type...> tuple;
	TupleOperationHelper<Args...>::template PopAllValueFromStack_WithDefault<sizeof...(Args) - sizeof...(DefaultArgs)>(inL, tuple, FirstIndexExcludeSelf, DefaultTuple);
	T* ptr = (T*)tovoid(inL, 1);
	auto lambda = [ptr, ActualCallFunc](typename TraitTupleInerType<Args>::Type&... args)->Ret { return ActualCallFunc(ptr, ConvertStructPointBack<Args>(args)...); };
	Pushret_or_Push<Ret>(inL, TupleOperationHelper<Args...>::CallCppFunc(tuple, lambda));
	return TupleOperationHelper<Args...>::PushBackRefValue(inL, tuple, FirstIndexExcludeSelf) + 1;
}


template<class... Args, class... DefaultArgs>
int32 LuaPopAndCallAndReturn(lua_State*inL, void(*func)(Args...), DefaultArgs&&... Augs)
{
	static_assert(sizeof...(Args) >= sizeof...(Augs), "too many default augs");
	const int FirstIndexExcludeSelf = 1;
	auto DefaultTuple = TMakeDefaultTuple<sizeof...(Args)-sizeof...(DefaultArgs), typename TDecay<Args>::Type...>::ConstructTuple(Forward<DefaultArgs>(Augs)...);
	TTuple<typename TraitTupleInerType<Args>::Type...> tuple;
	TupleOperationHelper<Args...>::template PopAllValueFromStack_WithDefault<sizeof...(Args) - sizeof...(DefaultArgs)>(inL, tuple, FirstIndexExcludeSelf, DefaultTuple);
	auto lambda = [func](typename TraitTupleInerType<Args>::Type&... args) { (*func)(ConvertStructPointBack<Args>(args)...); };
	TupleOperationHelper<Args...>::CallCppFunc(tuple, lambda);
	return TupleOperationHelper<Args...>::PushBackRefValue(inL, tuple, FirstIndexExcludeSelf);
}

template<class Ret, class... Args, class... DefaultArgs>
int32 LuaPopAndCallAndReturn(lua_State*inL, Ret(*func)(Args...), DefaultArgs&&... Augs)
{
	static_assert(sizeof...(Args) >= sizeof...(Augs), "too many default augs");
	const int FirstIndexExcludeSelf = 1;
	auto DefaultTuple = TMakeDefaultTuple<sizeof...(Args)-sizeof...(DefaultArgs), typename TDecay<Args>::Type...>::ConstructTuple(Forward<DefaultArgs>(Augs)...);
	TTuple<typename TraitTupleInerType<Args>::Type...> tuple;
	TupleOperationHelper<Args...>::template PopAllValueFromStack_WithDefault<sizeof...(Args) - sizeof...(DefaultArgs)>(inL, tuple, FirstIndexExcludeSelf, DefaultTuple);
	auto lambda = [func](typename TraitTupleInerType<Args>::Type&... args)->Ret { return (*func)(ConvertStructPointBack<Args>(args)...); };
	Pushret_or_Push<Ret>(inL, TupleOperationHelper<Args...>::CallCppFunc(tuple, lambda));
	return TupleOperationHelper<Args...>::PushBackRefValue(inL, tuple, FirstIndexExcludeSelf) + 1;
}

#define LUA_GLUE_CONCAT(a, b) a##b
#define LUA_GLUE_CONCAT1(a, b) LUA_GLUE_CONCAT(a, b)
#define LUA_GLUE_STR(str) #str
#define LUA_GLUE_STR1 LUA_GLUE_STR(str)
#define LUA_GLUE_GETTER_NAME(FIELD_NAME) LUA_GLUE_STR(LuaGet_##FIELD_NAME)
#define LUA_GLUE_SETTER_NAME(FIELD_NAME) LUA_GLUE_STR(LuaSet_##FIELD_NAME)
#define LUA_GLUE_PROPERTY_GET(FIELD_NAME) { LUA_GLUE_GETTER_NAME(FIELD_NAME), [](lua_State*inL)->int32{\
												TheClassType* Obj = (TheClassType*)tovoid(inL, 1); \
												UTableUtil::push(inL, Obj->FIELD_NAME);\
												return 1;}, ELuaFuncExportFlag::RF_GetPropertyFunc| \
												(uint32)(TIsStaticMember<TheClassType>(TGetHeapMemoryForTest<TheClassType>(),TGetHeapMemoryForTest<TheClassType>()->FIELD_NAME)?ELuaFuncExportFlag::RF_IsStaticProperty:0x00000000 ) }, 

#define LUA_GLUE_PROPERTY_SET(FIELD_NAME) { LUA_GLUE_SETTER_NAME(FIELD_NAME), [](lua_State*inL)->int32{\
																	TheClassType* Obj = (TheClassType*)tovoid(inL, 1);\
 																	Obj->FIELD_NAME = UTableUtil::pop<decltype(Obj->FIELD_NAME)&>(inL, 2);\
 																	return 1;}, ELuaFuncExportFlag::RF_SetPropertyFunc| \
												(uint32)(TIsStaticMember<TheClassType>(TGetHeapMemoryForTest<TheClassType>(),TGetHeapMemoryForTest<TheClassType>()->FIELD_NAME)?ELuaFuncExportFlag::RF_IsStaticProperty:0x00000000 ) }, 

#define LUA_GLUE_PROPERTY(FIELD_NAME) LUA_GLUE_PROPERTY_GET(FIELD_NAME)\
										LUA_GLUE_PROPERTY_SET(FIELD_NAME)

#define LUA_GLUE_STATIC_PROPERTY_GET(FIELD_NAME) { LUA_GLUE_GETTER_NAME(FIELD_NAME), [](lua_State*inL)->int32{\
 												UTableUtil::push(inL, TheClassType::FIELD_NAME);\
												return 1;}}, 

#define LUA_GLUE_STATIC_PROPERTY_SET(FIELD_NAME) { LUA_GLUE_SETTER_NAME(FIELD_NAME), [](lua_State*inL)->int32{\
 																	UTableUtil::read(TheClassType::FIELD_NAME, inL, 1);\
 																	return 1;}},

#define LUA_GLUE_STATIC_PROPERTY(FIELD_NAME) LUA_GLUE_STATIC_PROPERTY_GET(FIELD_NAME)\
										LUA_GLUE_STATIC_PROPERTY_SET(FIELD_NAME)
template<class T, bool IsUObject>
struct traitstructclass_imp;

#define LUA_GLUE_STRUCT_TRAIT_NAME(NAME, CLASS_TYPE)\
	template<>\
	struct traitstructclass_imp<CLASS_TYPE, true>\
	{enum{Value = false};};\
	template<>\
	struct traitstructclass_imp<CLASS_TYPE, false>\
	{\
		inline static const char* name() { return #NAME; }\
		inline static const char* name_nogc() { return LUA_GLUE_STR(NAME##_nogc); }\
		enum{Value = true};\
	};\
	template<>\
	class traitstructclass<CLASS_TYPE>: public traitstructclass_imp<CLASS_TYPE, std::is_convertible<CLASS_TYPE*, UObject*>::value> {};\
	template<>\
	class traitstructclass<const CLASS_TYPE>: public traitstructclass_imp<CLASS_TYPE, std::is_convertible<CLASS_TYPE*, UObject*>::value>{};

#define LUA_GLUE_STRUCT_TRAIT(CLASS_TYPE) LUA_GLUE_STRUCT_TRAIT_NAME(CLASS_TYPE, CLASS_TYPE)

template<class T>
struct TSafeLuaFuncsEnum
{
	enum {
		IsUObject = std::is_convertible<T*, UObject*>::value,
		HasCtor = !IsUObject && CustomTypeToLua<T>::HasCtor && THasCtor<T>::Value && !TIsAbstract<T>::Value,
		HasTempInit = !IsUObject&& TCanGetTempIns<T>::CanInit,
		HasTempIns = !IsUObject && !TIsAbstract<T>::Value && HasTempInit,
		HasDtor = !IsUObject && std::is_destructible<T>::value,
	};
	enum {
		NoCtorExportFlag = HasCtor ? 0x0000000 : 0x0000001,
		NoTempExportFlag = HasTempIns ? 0x00000000 : 0x0000001,
		NoDtorExportFlag = HasDtor ? 0x00000000 : 0x0000001,
		NoCopyExportFlag = IsUObject? 0x0000001: 0x00000000,
	};
};

template<class T>
T* TSafeCtor(typename TEnableIf< TSafeLuaFuncsEnum<T>::HasCtor, int>::Type* ppp = nullptr)
{
	return new T();
}

template<class T>
T* TSafeCtor(typename TEnableIf<!TSafeLuaFuncsEnum<T>::HasCtor, int>::Type* ppp = nullptr)
{
	return nullptr;
}

template<class T>
T* TSafeGetTempIns(typename TEnableIf<!TIsAbstract<T>::Value, T>::Type* p = nullptr)
{
	return &GetTempInsInit<T>();
}

template<class T>
T* TSafeGetTempIns(typename TEnableIf<TIsAbstract<T>::Value, T>::Type* p = nullptr)
{
	ensureAlwaysMsgf(false, TEXT("abstract class shouldn't come here"));
	return nullptr;
}


template<class T>
int TSafe__add(lua_State*inL, T* a, T* b, typename TEnableIf<UnrealLuaCheck::__add_Exists<T>::Value, int>::Type* p = nullptr)
{
	return UTableUtil::push_ret(inL, *a + *b);
}

template<class T>
int TSafe__add(lua_State*inL, T* a, T* b, typename TEnableIf<!UnrealLuaCheck::__add_Exists<T>::Value, int>::Type* p = nullptr)
{
	ensureAlwaysMsgf(false, TEXT("shouldn't come here"));
	return 0;
}

template<class T>
int TSafe__sub(lua_State*inL, T* a, T* b, typename TEnableIf<UnrealLuaCheck::__sub_Exists<T>::Value, int>::Type* p = nullptr)
{
	return UTableUtil::push_ret(inL, *a - *b);
}

template<class T>
int TSafe__sub(lua_State*inL, T* a, T* b, typename TEnableIf<!UnrealLuaCheck::__sub_Exists<T>::Value, int>::Type* p = nullptr)
{
	ensureAlwaysMsgf(false, TEXT("shouldn't come here"));
	return 0;
}

template<class T>
int TSafe__call(lua_State*inL, typename TEnableIf<UnrealLuaCheck::THas_OperatorCall<T>::Value, int>::Type* p = nullptr)
{
	return LuaPopAndCallAndReturn(inL, &T::operator());
}

template<class T>
int TSafe__call(lua_State*inL, typename TEnableIf<!UnrealLuaCheck::THas_OperatorCall<T>::Value, int>::Type* p = nullptr)
{
	ensureAlwaysMsgf(false, TEXT("shouldn't come here"));
	return 0;
}

template<class T>
int TSafe__unm(lua_State*inL, T* Value, typename TEnableIf<!UnrealLuaCheck::THas_OperatorUnm<T>::Value, int>::Type* p = nullptr)
{
	ensureAlwaysMsgf(false, TEXT("shouldn't come here"));
	return 0;
}

template<class T>
int TSafe__unm(lua_State*inL, T* Value, typename TEnableIf<UnrealLuaCheck::THas_OperatorUnm<T>::Value, int>::Type* p = nullptr)
{
	UTableUtil::push_new(inL, -(*Value));
	return 1;
}

#if LuaDebug
	#define LUA_GLUE_GCSWITCH(NAME, CLASS_TYPE)\
	static int32 NAME##__gc(lua_State* L)\
	{\
		FString n = #NAME;\
		UTableUtil::SubGcCount(L, n);\
		return NAME##_Destroy(L);\
	}
#else 
	#define LUA_GLUE_GCSWITCH(NAME, CLASS_TYPE)\
	static int32 NAME##__gc(lua_State* L)\
	{\
		return NAME##_Destroy(L);\
	}
#endif

#define LUA_GLUE_BASIC_FUNC_NAME(NAME, CLASS_TYPE) \
	static int32 NAME##_New(lua_State* L)\
	{\
		CLASS_TYPE* Obj = TSafeCtor<CLASS_TYPE>(); \
		if(Obj != nullptr)\
			pushstruct_gc(L, #NAME, (void*)Obj); \
		else{\
			ensureAlwaysMsgf(0, TEXT("no ctor, shouldn't call this"));\
			ue_lua_pushnil(L); \
		}\
		return 1;\
	}\
	static int32 NAME##_Destroy(lua_State* inL)\
	{\
		CLASS_TYPE* Obj = (CLASS_TYPE*)tovoid(inL, 1);\
		TSafeDtor<CLASS_TYPE>(Obj);\
		return 0;\
	}\
	LUA_GLUE_GCSWITCH(NAME, CLASS_TYPE)\
	static int32 NAME##_Temp(lua_State* L)\
	{\
		CLASS_TYPE* Ins = TSafeGetTempIns<CLASS_TYPE>();\
		if(Ins != nullptr)\
			pushstruct_nogc(L, #NAME, LUA_GLUE_STR(NAME##_nogc), (void*)(Ins));\
		else\
			ue_lua_pushnil(L);\
		return 1;\
	}\
	static int32 NAME##_Copy(lua_State* L)\
	{\
		CLASS_TYPE* Obj = (CLASS_TYPE*)tostruct(L, 1);\
		CLASS_TYPE* p = TSafeCopy<CLASS_TYPE>(Obj); \
		if (p)\
			pushstruct_gc(L, #NAME, (void*)(p)); \
		else\
			ue_lua_pushnil(L);\
		return 1;\
	}\
	static int32 NAME##_static_cast(lua_State* L)\
	{\
		CLASS_TYPE* Obj = (CLASS_TYPE*)tostruct(L, 1);\
		UTableUtil::push(L, Obj);\
		return 1;\
	}\
	static int32 NAME##_dynamic_cast(lua_State* L)\
	{\
		return 0;\
	}\
	static int32 NAME##__eq(lua_State* L)\
	{\
		CLASS_TYPE* a = (CLASS_TYPE*)tovoid(L, 1);\
		CLASS_TYPE* b = (CLASS_TYPE*)tovoid(L, 2);\
		UTableUtil::push(L, TSafe__eq(a, b));\
		return 1;\
	}\
	static int32 NAME##__le(lua_State* L)\
	{\
		CLASS_TYPE* a = (CLASS_TYPE*)tovoid(L, 1);\
		CLASS_TYPE* b = (CLASS_TYPE*)tovoid(L, 2);\
		UTableUtil::push(L, TSafe__le(a, b));\
		return 1;\
	}\
	static int32 NAME##__lt(lua_State* L)\
	{\
		CLASS_TYPE* a = (CLASS_TYPE*)tovoid(L, 1);\
		CLASS_TYPE* b = (CLASS_TYPE*)tovoid(L, 2);\
		UTableUtil::push(L, TSafe__lt(a, b));\
		return 1;\
	}\
	static int32 NAME##__add(lua_State* L)\
	{\
		CLASS_TYPE* a = (CLASS_TYPE*)tovoid(L, 1);\
		CLASS_TYPE* b = (CLASS_TYPE*)tovoid(L, 2);\
		return TSafe__add(L, a, b);\
	}\
	static int32 NAME##__sub(lua_State* L)\
	{\
		CLASS_TYPE* a = (CLASS_TYPE*)tovoid(L, 1);\
		CLASS_TYPE* b = (CLASS_TYPE*)tovoid(L, 2);\
		return TSafe__sub(L, a, b);\
	}\
	static int32 NAME##__call(lua_State* L)\
	{\
		return TSafe__call<CLASS_TYPE>(L);\
	}\
	static int32 NAME##__unm(lua_State* L)\
	{\
		CLASS_TYPE* a = (CLASS_TYPE*)tovoid(L, 1);\
		return TSafe__unm<CLASS_TYPE>(L, a);\
	}\
	static int32 NAME##__ptr(lua_State* L)\
	{\
		CLASS_TYPE* a = (CLASS_TYPE*)tovoid(L, 1);\
		ue_lua_pushlightuserdata(L, a);\
		return 1;\
	}\
	static const UnrealLuaBlueFunc* Get_##NAME##_Lib()\
	{\
		static const UnrealLuaBlueFunc NAME##_Lib[] = \
		{\
			{ "New", NAME##_New, (uint32)(TSafeLuaFuncsEnum<CLASS_TYPE>::NoCtorExportFlag|ELuaFuncExportFlag::RF_OverLoad), TCreateCheckFuncs<void(*)()>::Get()}, \
			{ "Temp", NAME##_Temp, (uint32)(TSafeLuaFuncsEnum<CLASS_TYPE>::NoTempExportFlag|ELuaFuncExportFlag::RF_OverLoad), TCreateCheckFuncs<void(*)()>::Get()}, \
			{ "Destroy", NAME##_Destroy, (uint32)TSafeLuaFuncsEnum<CLASS_TYPE>::NoDtorExportFlag }, \
			{ "Copy", NAME##_Copy,	(uint32)TSafeLuaFuncsEnum<CLASS_TYPE>::NoCopyExportFlag}, \
			{ "static_cast", NAME##_static_cast}, \
			{ "__gc", NAME##__gc, (uint32)TSafeLuaFuncsEnum<CLASS_TYPE>::NoDtorExportFlag}, \
			{ "__eq", NAME##__eq}, \
			{ "__le", NAME##__le}, \
			{ "__lt", NAME##__lt}, \
			{ "__add", NAME##__add}, \
			{ "__sub", NAME##__sub}, \
			{ "__call", NAME##__call}, \
			{ "__unm", NAME##__unm}, \
			{ "__ptr", NAME##__ptr}, \
			{ nullptr, nullptr }\
		};\
		return NAME##_Lib;\
	}

#define LUA_GLUE_BEGIN_NOTRAIT_NAME(NAME, CLASS_TYPE)\
namespace LuaGlueSpace_##NAME \
{\
	LUA_GLUE_BASIC_FUNC_NAME(NAME, CLASS_TYPE) \
	using TheClassType = CLASS_TYPE;\
	template<class T, class... BaseTypes>\
	struct lua_static_load_struct\
	{\
		static lua_static_load_struct lua_static_var;\
		lua_static_load_struct() { \
			UTableUtil::GetInitDelegates().AddStatic(&load); \
			if (UTableUtil::HasInit)\
			{\
				load();\
			}\
		}\
		static void load()\
		{\
			UTableUtil::AddAndOpenUserDefineClass(\
				#NAME, Get_##NAME##_Lib(),\
				{

#define LUA_FUNC_CONCAT( ... ) __VA_ARGS__

#define LUA_GLUE_BEGIN_NAME(NAME, ...) LUA_GLUE_STRUCT_TRAIT_NAME(NAME, LUA_FUNC_CONCAT(__VA_ARGS__))\
												LUA_GLUE_BEGIN_NOTRAIT_NAME(NAME, LUA_FUNC_CONCAT(__VA_ARGS__))

#define LUA_GLUE_BEGIN_NOTRAIT(CLASS_TYPE)  LUA_GLUE_BEGIN_NOTRAIT_NAME(CLASS_TYPE, CLASS_TYPE) 
#define LUA_GLUE_BEGIN( CLASS_TYPE ) LUA_GLUE_BEGIN_NAME( CLASS_TYPE, CLASS_TYPE )

#define LUA_GLUE_END(...) \
				}, { ("", TTraitStructTypeName<BaseTypes>())...}, \
			UnrealLuaClass(GetStructNewType<TheClassType>::GetType(), { (0, GetStructNewType<BaseTypes>::GetType())...}));\
		}\
	};\
	template<class T, class... BaseTypes>\
	lua_static_load_struct<T, BaseTypes...> lua_static_load_struct<T, BaseTypes...>::lua_static_var;\
	static void Just_Specialize(){lua_static_load_struct<int, ##__VA_ARGS__>::lua_static_var;}\
}

template<class... Args>
struct TCtorType {};

template<class T, class... Args>
struct TCtorType<T(Args...)>
{
	static int32 Ctor(lua_State*inL)
	{
		const int FirstIndexExcludeSelf = 1;
		TTuple<typename TraitTupleInerType<Args>::Type...> tuple;
		TupleOperationHelper<Args...>::PopAllValueFromStack(inL, tuple, FirstIndexExcludeSelf);
		auto lambda = [](typename TraitTupleInerType<Args>::Type&... args) { return new T(ConvertStructPointBack<Args>(args)...); };
		T* Result = TupleOperationHelper<Args...>::CallCppFunc(tuple, lambda);
		pushstruct_gc(inL, TTraitStructTypeName<T>(), Result);
		return 1;
	}
	template<class... DefaultArgs>
	static int32 Ctor(lua_State*inL, DefaultArgs&&... Augs)
	{
		const int FirstIndexExcludeSelf = 1;
		auto DefaultTuple = TMakeDefaultTuple<sizeof...(Args)-sizeof...(DefaultArgs), typename TDecay<Args>::Type...>::ConstructTuple(Forward<DefaultArgs>(Augs)...);
		TTuple<typename TraitTupleInerType<Args>::Type...> tuple;
		TupleOperationHelper<Args...>::template PopAllValueFromStack_WithDefault<sizeof...(Args) - sizeof...(DefaultArgs)>(inL, tuple, FirstIndexExcludeSelf, DefaultTuple);
		auto lambda = [](typename TraitTupleInerType<Args>::Type&... args) { return new T(ConvertStructPointBack<Args>(args)...); };
		T* Result = TupleOperationHelper<Args...>::CallCppFunc(tuple, lambda);
		pushstruct_gc(inL, TTraitStructTypeName<T>(), Result);
		return 1;
	}

	static int32 Temp(lua_State*inL)
	{
		const int FirstIndexExcludeSelf = 1;
		TTuple<typename TraitTupleInerType<Args>::Type...> tuple;
		TupleOperationHelper<Args...>::PopAllValueFromStack(inL, tuple, FirstIndexExcludeSelf);
		auto lambda = [](typename TraitTupleInerType<Args>::Type&... args) 
		{ 
			T& TempIns = GetTempIns<T>();
			TempIns = T(ConvertStructPointBack<Args>(args)...);
			return &TempIns;
		};
		T* Result = TupleOperationHelper<Args...>::CallCppFunc(tuple, lambda);
		pushstruct_nogc(inL, TTraitStructTypeName<T>(), TTraitStructTypeName_Nogc<T>(), Result);
		return 1;
	}
	template<class... DefaultArgs>
	static int32 Temp(lua_State*inL, DefaultArgs&&... Augs)
	{
		const int FirstIndexExcludeSelf = 1;
		auto DefaultTuple = TMakeDefaultTuple<sizeof...(Args)-sizeof...(DefaultArgs), typename TDecay<Args>::Type...>::ConstructTuple(Forward<DefaultArgs>(Augs)...);
		TTuple<typename TraitTupleInerType<Args>::Type...> tuple;
		TupleOperationHelper<Args...>::template PopAllValueFromStack_WithDefault<sizeof...(Args) - sizeof...(DefaultArgs)>(inL, tuple, FirstIndexExcludeSelf, DefaultTuple);
		auto lambda = [](typename TraitTupleInerType<Args>::Type&... args) {
			T& TempIns = GetTempIns<T>();
			TempIns = T(ConvertStructPointBack<Args>(args)...);
			return &TempIns;
		};
		T* Result = TupleOperationHelper<Args...>::CallCppFunc(tuple, lambda);
		pushstruct_nogc(inL, TTraitStructTypeName<T>(), TTraitStructTypeName_Nogc<T>(), Result);
		return 1;
	}
};

#define LUA_GLUE_NEW(FUNC_NAME, FUNC, ...)	{#FUNC_NAME, [](lua_State*inL)->int32{ return TCtorType<TheClassType FUNC>::Ctor(inL, ##__VA_ARGS__);}},
#define LUA_GLUE_TEMP(FUNC_NAME, FUNC, ...)	{#FUNC_NAME, [](lua_State*inL)->int32{ return TCtorType<TheClassType FUNC>::Temp(inL, ##__VA_ARGS__);}},
#define LUA_GLUE_NEW_TEMP(NEW_NAME, TEMP_NAME,FUNC, ...)	{#TEMP_NAME, [](lua_State*inL)->int32{ return TCtorType<TheClassType FUNC>::Temp(inL, ##__VA_ARGS__);}},\
												{#NEW_NAME, [](lua_State*inL)->int32{ return TCtorType<TheClassType FUNC>::Ctor(inL, ##__VA_ARGS__);}},
#define LUA_GLUE_FUNCTION(FUNC_NAME, ...) {#FUNC_NAME, [](lua_State*inL)->int32{ return LuaPopAndCallAndReturn(inL, &TheClassType::FUNC_NAME, ##__VA_ARGS__);}},
#define LUA_GLUE_FUNCTION_NAME(FUNC_NAME, FUNCTION, ...) {#FUNC_NAME, [](lua_State*inL)->int32{ return LuaPopAndCallAndReturn(inL, &TheClassType::FUNCTION, ##__VA_ARGS__);}},

#define LUA_GLUE_FUNCTION_VIRTUAL(FUNC_NAME, ...) {#FUNC_NAME, [](lua_State*inL)->int32{ return LuaPopAndCallAndReturn_ForVirtual(inL, &TheClassType::FUNC_NAME, [](TheClassType*p, auto&& ...arg) {return p->TheClassType::FUNC_NAME(arg...);}, ##__VA_ARGS__);}},
#define  LUA_GLUE_CUSTOM_FUNCTION(FUNC_NAME) {#FUNC_NAME, static_cast<int(*)(lua_State*inL)>(&TheClassType::FUNC_NAME)},
#define  LUA_GLUE_CUSTOM_FUNCTION_NAME(FUNC_NAME, FUNCTION) {#FUNC_NAME, static_cast<int(*)(lua_State*inL)>(&TheClassType::FUNCTION)},

#define LUA_GLUE_EXPAND_BEGIN(CLASS_TYPE) LUA_GLUE_BEGIN(CLASS_TYPE)


#define LUA_GLUE_EXPAND_END(CLASS_TYPE) LUA_GLUE_END()
				

#define LUA_GLUE_ENUM_BEGIN(NAME, ENUM_TYPE) \
namespace LuaGlueSpace_ENUM_##NAME \
{\
	using TheClassType = ENUM_TYPE;\
	template<class T>\
	struct lua_static_load_enum_##NAME##_struct\
	{\
		static lua_static_load_enum_##NAME##_struct lua_enum_##NAME##_static_var;\
		static void load()\
		{\
			UTableUtil::AddEnumGlue(\
			#NAME,\
			{

#define LUA_GLUE_ENUMKEY(KEY_NAME)\
				{#KEY_NAME, (int32)(TheClassType::KEY_NAME)},

#define LUA_GLUE_ENUM_SPACE_BEGIN(NAME, NAMESPACE) \
namespace LuaGlueSpace_ENUM_##NAME \
{\
	using namespace NAMESPACE;\
	template<class T>\
	struct lua_static_load_enum_##NAME##_struct\
	{\
		static lua_static_load_enum_##NAME##_struct lua_enum_##NAME##_static_var;\
		static void load()\
		{\
			UTableUtil::AddEnumGlue(\
			#NAME,\
			{

#define LUA_GLUE_ENUM_SPACE_KEY(KEY_NAME)\
				{#KEY_NAME, (int32)(KEY_NAME)},

#define LUA_GLUE_ENUM_END(NAME) \
			});\
		}\
		lua_static_load_enum_##NAME##_struct() { \
			UTableUtil::GetInitDelegates().AddStatic(&load); \
			if (UTableUtil::HasInit)\
			{\
				load();\
			}\
		}\
	};\
	template<class T>\
	lua_static_load_enum_##NAME##_struct<T> lua_static_load_enum_##NAME##_struct<T>::lua_enum_##NAME##_static_var;\
	static void Just_Specialize(){lua_static_load_enum_##NAME##_struct<int>::lua_enum_##NAME##_static_var;}\
}

#define LUA_GLUE_FUNCTION_OUT(FUNC_NAME,FUNC_DECL, ...) \
	{#FUNC_NAME, [](lua_State*inL)->int32 { return LuaPopAndCallAndReturn(inL, &FUNC_DECL,##__VA_ARGS__);}},

#define LUA_GLUE_FUNCTION_HOOK(FUNC_NAME,FUNC_DECL, ...) \
	{#FUNC_NAME, [](lua_State*inL)->int32 { return LuaHookFunc(inL, &FUNC_DECL,##__VA_ARGS__);}},


#define LUA_GLUE_CUSTOM_FUNCTION_OUT(FUNC_NAME, FUNC_DECL) \
	{#FUNC_NAME, &FUNC_DECL},

#define LUA_GLUE_OVERLOAD_NAME(FUNC_NAME, FUNCTION, SIGNATURE, ...)\
{#FUNC_NAME, [](lua_State*inL)->int32{ return LuaPopAndCallAndReturn(inL, static_cast<SIGNATURE>(&TheClassType::FUNCTION), ##__VA_ARGS__);}, \
(uint32)ELuaFuncExportFlag::RF_OverLoad,\
TCreateCheckFuncs<SIGNATURE>::Get(__VA_ARGS__)},

#define LUA_GLUE_FUNCTION_OVERLOAD(FUNC_NAME, FUNCTION, SIGNATURE, ...) LUA_GLUE_OVERLOAD_NAME(FUNC_NAME, FUNCTION, SIGNATURE, ##__VA_ARGS__)

#define LUA_GLUE_OVERLOAD(FUNC_NAME, SIGNATURE, ...)\
LUA_GLUE_FUNCTION_OVERLOAD(FUNC_NAME, FUNC_NAME, SIGNATURE, ##__VA_ARGS__)

#define LUA_GLUE_FUNCTION_OUT_OVERLOAD(FUNC_NAME, FUNCTION, SIGNATURE, ...)\
{#FUNC_NAME, [](lua_State*inL)->int32{ return LuaPopAndCallAndReturn(inL, static_cast<SIGNATURE>(&FUNCTION), ##__VA_ARGS__);}, \
(uint32)ELuaFuncExportFlag::RF_OverLoad,\
TCreateCheckFuncs<SIGNATURE>::Get(__VA_ARGS__)},

#define LUA_GLUE_CTOR(FUNC, ...)\
{"New", [](lua_State*inL)->int32{ return TCtorType<TheClassType FUNC>::Ctor(inL, ##__VA_ARGS__);},\
(uint32)ELuaFuncExportFlag::RF_OverLoad, TCreateCheckFuncs<TheClassType(*)FUNC>::Get(__VA_ARGS__)},\
{"Temp", [](lua_State*inL)->int32{ return TCtorType<TheClassType FUNC>::Temp(inL, ##__VA_ARGS__);},\
(uint32)ELuaFuncExportFlag::RF_OverLoad, TCreateCheckFuncs<TheClassType(*)FUNC>::Get(__VA_ARGS__)},

#define LUA_GLUE_INTERFACE_BODY(NAME, ...)\
namespace LuaGlueSpace_##NAME \
{\
	static const UnrealLuaBlueFunc NAME##_Lib[] = {{nullptr,nullptr}};\
	using TheClassType = LUA_FUNC_CONCAT(__VA_ARGS__);\
	template<class T,class... BaseTypes>\
	struct lua_static_load_struct\
	{\
		static lua_static_load_struct lua_static_var;\
		lua_static_load_struct() { \
			UTableUtil::GetInitDelegates().AddStatic(&load); \
			if (UTableUtil::HasInit)\
			{\
				load();\
			}\
		}\
		static void load()\
		{\
			UTableUtil::AddAndOpenUserDefineClass(\
				#NAME, NAME##_Lib,\
				{


#define LUA_GLUE_INTERFACE(NAME, ...) LUA_GLUE_STRUCT_TRAIT_NAME(NAME, LUA_FUNC_CONCAT(__VA_ARGS__))\
										LUA_GLUE_INTERFACE_BODY(NAME, ##__VA_ARGS__)

#define LUA_GLUE_ALIAS(ALIASNAME, ORIGINAL_NAME)\
	namespace LuaGlueSpaceAlias_##ALIASNAME \
	{\
		template<class T>\
		struct lua_static_load_##ALIASNAME##_struct\
		{\
			static lua_static_load_##ALIASNAME##_struct staticVar;\
			static void load()\
			{\
				UTableUtil::AddAliasName(#ALIASNAME, #ORIGINAL_NAME);\
			}\
			lua_static_load_##ALIASNAME##_struct(){ \
			UTableUtil::GetInitDelegates().AddStatic(&load); \
			if (UTableUtil::HasInit)\
			{\
				load();\
			}\
		}\
		};\
		template<class T>\
		lua_static_load_##ALIASNAME##_struct<T> lua_static_load_##ALIASNAME##_struct<T>::staticVar;\
		static void Just_Specialize(){lua_static_load_##ALIASNAME##_struct<int>::staticVar;}\
	}


template<class T>
T* ForLuaGlueGetAt(T* Start, uint32 Index)
{
	return Start + Index;
}

#define LUA_GLUE_GETAT(Name)  LUA_GLUE_FUNCTION_OUT(Name, ForLuaGlueGetAt<TheClassType>)
