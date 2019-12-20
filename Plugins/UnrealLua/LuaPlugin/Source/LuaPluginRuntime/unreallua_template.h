#pragma once
#include <type_traits>
#include "CoreMinimal.h"
#include "Templates/IsAbstract.h"

struct lua_State;

template<class T>
struct TJustError;

template<class T>
struct THasCtor
{
	enum { Value = !TIsAbstract<T>::Value && std::is_default_constructible<typename TDecay<T>::Type>::value };
};

namespace UnrealLua {
	const int RingBufferLen = 10;
	struct DefaultCustomEnum
	{
		enum{
			Value = false,
			PopFromLuaStackByPointer = false,
			NeedCreateTempInsWhenPushRet = false,
			HasCtor = true,
			HasDtor = true,
		};
	};
}
template<class T>
struct CustomTypeToLua:UnrealLua::DefaultCustomEnum
{
};

template<>
struct CustomTypeToLua<void>
{
	enum { Value = true };
	static void PushFromValue(lua_State*) {};
	static void PushFromStack(lua_State*) {};
	static void PushFromFunctionReturn(lua_State*) {};
	static void PushFromFunctionReferenceParam(lua_State*) {};
	static void PushFromFunctionReferenceParam(lua_State*, int32 StackIndex) {};
};

template<class T>
struct TCanGetTempIns
{
	using DecayType = typename TDecay<T>::Type;
	enum {
		IsUObject = std::is_convertible<DecayType*, UObject*>::value,
		Value = !IsUObject&&THasCtor<DecayType>::Value && CustomTypeToLua<DecayType>::HasCtor,
		CanInit = !IsUObject&&std::is_copy_assignable<DecayType>::value&&THasCtor<DecayType>::Value &&CustomTypeToLua<DecayType>::HasCtor,
		CanCopy = !IsUObject && std::is_copy_assignable<DecayType>::value,
	};
};
template<class T>
static T* NewForT()
{
	return new T[30];
}

template<class T>
static T& GetTempIns(typename TEnableIf<TCanGetTempIns<T>::Value, int>::Type* p = nullptr)
{
	const int32 len = 30;
	static T* Temp = NewForT<T>();
	static int32 i = -1;
	i = (i + 1) % len;
	return Temp[i];
}

template<class T>
static T& GetTempIns(typename TEnableIf<!TCanGetTempIns<T>::Value, int>::Type* p = nullptr)
{
	const int32 len = 30;
	static T* Temp = (T*)FMemory::Malloc(sizeof(T)*len);
	static int32 i = -1;
	i = (i + 1) % len;
	return Temp[i];
}
template<class T>
struct TempCopyRingBuffer
{
	TempCopyRingBuffer(const T& Value):Index(-1)
	{
		for (int i = 0; i < UnrealLua::RingBufferLen; i++)
		{
			Data[i] = new T(Value);
		}
	}
	T& Get()
	{
		Index = (Index + 1) % UnrealLua::RingBufferLen;
		return *Data[Index];
	}
	T* Data[UnrealLua::RingBufferLen];
	int32 Index;
};

template<class T>
static T& GetTempCopy(const T& Value, typename TEnableIf<TCanGetTempIns<T>::CanCopy, int>::Type* p = nullptr)
{
	static TempCopyRingBuffer<T>* Buffer = new TempCopyRingBuffer<T>(Value);
	T& TempValue = Buffer->Get();
	TempValue = Value;
	return TempValue;
}

template<class T>
static T& GetTempCopy(const T& Value, typename TEnableIf<!TCanGetTempIns<T>::CanCopy, int>::Type* p = nullptr)
{
	T& Temp = GetTempIns<T>();
	Temp = Value;
	return Temp;
}

template<class T>
static T& GetTempInsInit(typename TEnableIf<TCanGetTempIns<T>::CanInit, T>::Type* p = nullptr)
{
	T& Ins = GetTempIns<T>();
	Ins = T();
	return Ins;
}

template<class T>
static T& GetTempInsInit(typename TEnableIf<!TCanGetTempIns<T>::CanInit, T>::Type* p = nullptr)
{
	T& Ins = GetTempIns<T>();
	return Ins;
}

#define TEST_HAS_FUNC(FUNC_NAME, FUNC_SIGN) \
template <class T, FUNC_SIGN> struct UnrealLuaTemplateCheck##FUNC_NAME;\
template <class T> static char UnrealLuaMemberTest##FUNC_NAME(UnrealLuaTemplateCheck##FUNC_NAME<T, &CustomTypeToLua<T>::FUNC_NAME> *);\
template <class T> static int UnrealLuaMemberTest##FUNC_NAME(...);\
template <class T>\
class THas_##FUNC_NAME\
{\
public:\
	enum { Value = sizeof(UnrealLuaMemberTest##FUNC_NAME<typename TDecay<T>::Type>(nullptr)) == sizeof(char) };\
};

TEST_HAS_FUNC(ReadValueFromLuaStack, void(*)(T&, lua_State*inL, int32))
TEST_HAS_FUNC(ReadPtrFromLuaStack, void(*)(T*&, lua_State*inL, int32))
TEST_HAS_FUNC(PushPtrToLuaStack, int(*)(lua_State*inL, T*))
TEST_HAS_FUNC(PushValueToLuaStack, int(*)(lua_State*inL, const T&))
TEST_HAS_FUNC(PushBackPtrToLuaStack, int(*)(lua_State*inL, T*, int32))
TEST_HAS_FUNC(PushBackValueToLuaStack, int(*)(lua_State*inL, T&, int32))
TEST_HAS_FUNC(PushRetPtrToLuaStack, int(*)(lua_State*inL, const T*))
TEST_HAS_FUNC(PushRetValueToLuaStack, int(*)(lua_State*inL, const T&))
TEST_HAS_FUNC(PushNewPtrToLuaStack, int(*)(lua_State*inL, const T*))
TEST_HAS_FUNC(PushNewValueToLuaStack, int(*)(lua_State*inL, const T&))

template<class T>
void TSafeDtor(typename TEnableIf <std::is_destructible<T>::value && CustomTypeToLua<T>::HasDtor, T>::Type* p)
{
	delete p;
}

template<class T>
void TSafeDtor(typename TEnableIf <!(std::is_destructible<T>::value&& CustomTypeToLua<T>::HasDtor), T>::Type* p)
{
}

template<class T>
T* TSafeCopy(typename TEnableIf<!std::is_copy_constructible<T>::value, T>::Type* p = nullptr)
{
	ensureAlwaysMsgf(false, TEXT("shouldn't come here"));
	return nullptr;
}

template<class T>
T* TSafeCopy(typename TEnableIf<std::is_copy_constructible<T>::value, T>::Type* p = nullptr)
{
	return new T(*p);
}
#define TEST_HAS_OPERATOR_TWO(MetaName, OP) \
template<typename T, typename Arg> No operator OP (const T&, const Arg&);\
template<typename T, typename Arg = T>\
struct MetaName##_Exists\
{\
	enum { Value = !std::is_same<decltype(*(T*)(0) OP *(Arg*)(0)), No>::value };\
	using Type = decltype(*(T*)(0) OP *(Arg*)(0));\
};

namespace UnrealLuaCheck
{
	struct No {};
	TEST_HAS_OPERATOR_TWO(__eq, == )
	TEST_HAS_OPERATOR_TWO(__sub, -)
	TEST_HAS_OPERATOR_TWO(__add, +)
	TEST_HAS_OPERATOR_TWO(__lt, < )
	TEST_HAS_OPERATOR_TWO(__le, <= )

	template <class T, class T1> struct UnrealLuaTemplateCheck_OperatorCall;
	template <class T> static char UnrealLuaOperatorCallTest(UnrealLuaTemplateCheck_OperatorCall<T, decltype(&T::operator()) >*);
	template <class T> static int UnrealLuaOperatorCallTest(...);
	template <class T>
	class THas_OperatorCall
	{
	public:
		enum { Value = sizeof(UnrealLuaOperatorCallTest<T>(nullptr)) == sizeof(char) };
	};

	template<typename T> No operator-(const T&);
	template<typename T>
	struct THas_OperatorUnm
	{
		enum { Value = !std::is_same<decltype(-(*(T*)(0))), No>::value };
	};
}

template<class T>
bool TSafe__eq(T* a, T* b, typename TEnableIf<UnrealLuaCheck::__eq_Exists<T>::Value, int>::Type* p = nullptr)
{
	if (a == b)
		return true;
	else
		return *a == *b;
}

template<class T>
bool TSafe__eq(T* a, T* b, typename TEnableIf<!UnrealLuaCheck::__eq_Exists<T>::Value, int>::Type* p = nullptr)
{
	return a == b;
}

template<class T>
bool TSafe__le(T* a, T* b, typename TEnableIf<UnrealLuaCheck::__le_Exists<T>::Value, int>::Type* p = nullptr)
{
	return *a <= *b;
}

template<class T>
bool TSafe__le(T* a, T* b, typename TEnableIf<!UnrealLuaCheck::__le_Exists<T>::Value, int>::Type* p = nullptr)
{
	ensureAlwaysMsgf(false, TEXT("shouldn't come here"));
	return false;
}

template<class T>
bool TSafe__lt(T* a, T* b, typename TEnableIf<UnrealLuaCheck::__lt_Exists<T>::Value, int>::Type* p = nullptr)
{
	return *a < *b;
}

template<class T>
bool TSafe__lt(T* a, T* b, typename TEnableIf<!UnrealLuaCheck::__lt_Exists<T>::Value, int>::Type* p = nullptr)
{
	ensureAlwaysMsgf(false, TEXT("shouldn't come here"));
	return false;
}

template<class T>
class traitstructclass
{
public:
	enum { Value = false };
};

template <class T, UScriptStruct*(*)()> struct Check;
template <class T> static char MemberTest(Check<T, &T::StaticStruct> *);
template <class T> static int MemberTest(...);
template <class T>
class THasStaticStruct
{
public:
	enum { Value = sizeof(MemberTest<typename TDecay<T>::Type>(nullptr)) == sizeof(char) };
};
template<class T>
struct TIsStruct
{
	using DecayType = typename TDecay<T>::Type;
	enum {
		Value = THasStaticStruct<DecayType>::Value || traitstructclass<DecayType>::Value
	};
};

namespace UnrealLua
{
	template<class T>
	struct CheckNeedTempIns
	{
		using DecayType = typename TDecay<T>::Type;
		enum {
			Value = CustomTypeToLua<DecayType>::Value ? CustomTypeToLua<DecayType>::NeedCreateTempInsWhenPushRet: TIsStruct<DecayType>::Value,
		};
	};

	template<class T>
	const T& MayNeedTempIns(const T& v, typename TEnableIf<!CheckNeedTempIns<T>::Value, int>::Type* p = nullptr)
	{
		return v;
	}

	template<class T>
	const T& MayNeedTempIns(const T& v, typename TEnableIf<CheckNeedTempIns<T>::Value, int>::Type* p = nullptr)
	{
		T& TempIns = GetTempCopy<T>(v);
		return TempIns;
	}	
}

template<class T>
constexpr const char* TTraitStructTypeName(typename TEnableIf<traitstructclass<T>::Value, int>::Type* p = nullptr)
{
	return traitstructclass<T>::name();
}

template<class T>
const char* TTraitStructTypeName(typename TEnableIf<!traitstructclass<T>::Value, int>::Type* p = nullptr)
{
	static FString StructName = "F" + T::StaticStruct()->GetName();
	static auto StructNameUtf8 = FTCHARToUTF8((const TCHAR*)*StructName);
	const char* NameAnsi = (ANSICHAR*)StructNameUtf8.Get();
	return NameAnsi;
}

template<class T>
constexpr const char* TTraitStructTypeName_Nogc(typename TEnableIf<traitstructclass<T>::Value, int>::Type* p = nullptr)
{
	return traitstructclass<T>::name_nogc();
}

template<class T>
const char* TTraitStructTypeName_Nogc(typename TEnableIf<!traitstructclass<T>::Value, int>::Type* p = nullptr)
{
	static FString StructName = "F" + T::StaticStruct()->GetName() + "_nogc";
	static auto StructNameUtf8 = FTCHARToUTF8((const TCHAR*)*StructName);
	const char* NameAnsi = (ANSICHAR*)StructNameUtf8.Get();
	return NameAnsi;
}

template<class T>
struct TIsPopByPointer
{
	enum
	{
		Value = TIsStruct<T>::Value && !TIsSame<typename TDecay<T>::Type, FText>::Value
	};
};

template<class StructType, class MemberType>
bool TIsStaticMember(StructType* var, const MemberType& memaddr)
{
	int64 offset = reinterpret_cast<size_t>(&memaddr) - reinterpret_cast<size_t>(var);
	return (offset > sizeof(StructType)) || offset < 0;
}

template<class Type>
Type* TGetHeapMemoryForTest()
{
	static int* p = new int;
	return (Type*)p;
}

template<class T>
struct TUnrealLuaIsBasicType_Imp
{
	enum { Value = false };
};

#define LUA_DEFINE_BASICTYPE(Type)\
template<>\
struct TUnrealLuaIsBasicType_Imp<Type>\
{\
	enum { Value = true };\
};

LUA_DEFINE_BASICTYPE(int8)
LUA_DEFINE_BASICTYPE(uint8)
LUA_DEFINE_BASICTYPE(int16)
LUA_DEFINE_BASICTYPE(uint16)
LUA_DEFINE_BASICTYPE(int32)
LUA_DEFINE_BASICTYPE(uint32)
LUA_DEFINE_BASICTYPE(int64)
LUA_DEFINE_BASICTYPE(uint64)
LUA_DEFINE_BASICTYPE(float)
LUA_DEFINE_BASICTYPE(double)
LUA_DEFINE_BASICTYPE(bool)

template<class T>
struct TUnrealLuaIsBasicType : TUnrealLuaIsBasicType_Imp<typename TDecay<T>::Type>
{
};
