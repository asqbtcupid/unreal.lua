#pragma once
#include "LuaApi.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Tickable.h"
#include "GCObject.h"
#include "ConstructorHelpers.h"
#include "LuaDelegate.h"
#include "SubclassOf.h"
#include "Class.h"
#include "LuaArrayHelper.h"
#include "LuaMapHelper.h"
#include "LuaSetHelper.h"
#include "AndOrNot.h"
#include "Engine/UserDefinedStruct.h"
#include "TableUtil.generated.h"

#define LuaCtor(classname, ...) UTableUtil::call("Ctor", classname, this, ##__VA_ARGS__);
#define LuaCall(functionname, ptr, ...) UTableUtil::inscall("Call", functionname, ptr, ##__VA_ARGS__);

#define LuaCallr(ret, functionname, ptr,...) UTableUtil::inscallr<ret>("Call", functionname, ptr, ##__VA_ARGS__);

#define LuaStaticCall(functionname, ...)	UTableUtil::call(functionname, ##__VA_ARGS__);
#define LuaStaticCallr(ret, functionname, ...)	UTableUtil::callr<ret>(functionname, ##__VA_ARGS__);

#ifndef USE_LUASOURCE
	#define USE_LUASOURCE	0
#endif

#ifndef STRONG_CHECK_GC_REF
	#define STRONG_CHECK_GC_REF	0
#endif

struct EnumItem
{
	const char* key;
	const int32 value;
};
struct NeedTempInsType {};
struct NotNeedTempInsType {};
template<class T>
class traitstructclass
{
public:
	using NotStructType = NotNeedTempInsType;
};
typedef int luavalue_ref;
DECLARE_LOG_CATEGORY_EXTERN(LuaLog, Log, All);

using luafunc = int(struct lua_State*);

bool existdata(lua_State*inL, void * p);
LUAPLUGINRUNTIME_API void pushuobject(lua_State *inL, void* p, bool bgcrecord = false);
LUAPLUGINRUNTIME_API void pushstruct_gc(lua_State *inL, const char* structname, void* p);
LUAPLUGINRUNTIME_API void pushstruct_nogc(lua_State *inL, const char* structname, void* p);
LUAPLUGINRUNTIME_API void pushstruct_stack(lua_State *inL, const char* structname, void* p);
LUAPLUGINRUNTIME_API void* touobject(lua_State* L, int i);
LUAPLUGINRUNTIME_API void* tostruct(lua_State* L, int i);
LUAPLUGINRUNTIME_API int ErrHandleFunc(lua_State*L);
LUAPLUGINRUNTIME_API void PrintLuaStack(lua_State*L = nullptr);


template<class T>
static T popinternal(lua_State *L, int index, typename TEnableIf< TIsSame<typename traitstructclass<T>::NotStructType, NotNeedTempInsType>::Value,T>::Type* pp = nullptr )
{
	return (T)ue_lua_tointeger(L, index);
};

template<class T>
static T popinternal(lua_State *L, int index, typename TEnableIf< TIsSame<typename traitstructclass<T>::NotStructType, NeedTempInsType>::Value, T>::Type* pp = nullptr)
{
	T* p = (T*)tostruct(L, index);
	if (p == nullptr)
	{
		return T();
	}
	return *p;
};

template<class T>
class popiml {
public:
	static T pop(lua_State *L, int index)
	{
		return popinternal<T>(L, index);
	}
};
template<class T>
class popiml<T*> {
public:
	static T* pop(lua_State *L, int index, typename TEnableIf<TIsDerivedFrom<T, UObject>::IsDerived, T>::Type* MakeSureDerivedFromUObject = nullptr)
	{
		return (T*)touobject(L, index);
	}
};

template<class T>
class popiml< TArray<T> > {
public:
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

template<class T>
class popiml< TSet<T> > {
public:
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

template<class K, class V>
class popiml< TMap<K, V> > {
public:
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

template<> class popiml<uint8> {
public:
	static uint8 pop(lua_State *L, int index) { return (uint8)ue_lua_tointeger(L, index); }
};

template<> class popiml<uint32> {
public:
	static uint32 pop(lua_State *L, int index) { return (uint32)ue_lua_tointeger(L, index); }
};

template<> class popiml<int32> {
public:
	static int pop(lua_State *L, int index) { return (int32)ue_lua_tointeger(L, index); }
};

template<class T> class popiml<TWeakObjectPtr<T>> {
public:
	static T* pop(lua_State *L, int index) { return (T*)touobject(L, index); }
};

template<class T> class popiml<TEnumAsByte<T>> {
public:
	static T pop(lua_State *L, int index) { return (T)ue_lua_tointeger(L, index); }
};

template<class T> class popiml<TSubclassOf<T>> {
public:
	static UClass* pop(lua_State *L, int index) { return (UClass*)touobject(L, index); }
};

template<> class popiml<int64> {
public:
	static int64 pop(lua_State *L, int index) { return (int64)ue_lua_tointeger(L, index); }
};

template<> class popiml<bool> {
public:
	static bool pop(lua_State *L, int index) { return !!(ue_lua_toboolean(L, index)); }
};

template<> class popiml<FName> {
public:
	static FName pop(lua_State *L, int index) { return FName(UTF8_TO_TCHAR(ue_lua_tolstring(L, index, NULL))); }
};
template<> class popiml<FText> {
public:
	static FText pop(lua_State *L, int index) { return FText::FromString(ue_lua_tolstring(L, index, NULL)); }
};
template<> class popiml<FString> {
public:
	static FString pop(lua_State *L, int index) { return UTF8_TO_TCHAR(ue_lua_tolstring(L, index, NULL)); }
};
template<> class popiml<float> {
public:
	static float pop(lua_State *L, int index) { return (float)ue_lua_tonumber(L, index); }
};
template<> class popiml<double> {
public:
	static double pop(lua_State *L, int index) { return (double)ue_lua_tonumber(L, index); }
};
template<> class popiml<void> {
public:
	static void pop(lua_State *L, int index) { ue_lua_pop(L, 1); }
};

class LuaTickObject :public FTickableGameObject
{
public:
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;
};

DECLARE_MULTICAST_DELEGATE(FLuaInitDelegates)

UCLASS()
class LUAPLUGINRUNTIME_API UTableUtil : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

		static LuaTickObject* PtrTickObject;
	static lua_State* TheOnlyLuaState;
	// temporarily for multi gameinstance, later I will add Multi Lua_State
	static int32 ManualInitCount;
	static bool HasManualInit;
	static TSet<FString> SupportedNativeStruct;
public:
	static FLuaInitDelegates& GetInitDelegates();
	static lua_State* GetTheOnlyLuaState();
	static TSet<FString> NeedGcBpClassName;
#ifdef LuaDebug
	static TMap<FString, int> countforgc;
	static TMap<FString, UClass*> bpname2bpclass;
#endif
	static TMap<FString, UUserDefinedStruct*> bpname2bpstruct;
	static void addmodule(const char* classname, bool bIsStruct, bool bNeedGc);
	static void useCustomLoader();
	UFUNCTION(BlueprintCallable, Category = "Lua")
		static void init(bool IsManual = false);
	static void GC();
	static void openmodule(const char* classname);
	static void closemodule();
	static void addfunc(const char* classname, luafunc f);
	static void initmeta(bool bIsStruct, bool bNeedGc = true);
	static void setmeta(lua_State *inL, const char* classname, int index, bool bIsStruct = false, bool bNeedGc = true);
	static void set_uobject_meta(lua_State *inL, UObject* Obj, int index);
	static void MayAddNewStructType(UUserDefinedStruct* BpStruct);

	static void loadlib(const luaL_Reg funclist[], const char* classname, bool bIsStruct = false, bool bNeedGc = true);
	static void loadstruct(const luaL_Reg funclist[], const char* classname);
	static void loadEnum(const EnumItem list[], const char* enumname);
	static luavalue_ref ref_luavalue(lua_State*inL, int index);
	static void unref(lua_State*inL, luavalue_ref r);


	static void log(const FString& content);

	static void shutdown();
	static void shutdown_internal();

	static bool existluains(lua_State*inL, void * p);

	static void rmgcref(UObject* p);
	static void addgcref(UObject* p);

	static int push(lua_State *inL) { return 0; }

	template<typename T>
	static int push(lua_State *inL, const T& value, const typename traitstructclass<T>::value* p = nullptr)
	{
		pushstruct_nogc(inL, traitstructclass<T>::name(), (void*)(&value));
		return 1;
	}

// 	template<typename T>
// 	static int push(lua_State *inL, const T* value, typename TEnableIf<TIsDerivedFrom<T, UObject>::IsDerived, T>::Type* p = nullptr)
// 	{
// 		pushuobject(inL, (void*)value);
// 		return 1;
// 	}

	template<typename T>
	static int push(lua_State *inL, T* value)
	{
		pushuobject(inL, (void*)value);
		return 1;
	}

	template<typename T>
	static int push(lua_State *inL, const T* value, const typename traitstructclass<T>::value* p = nullptr)
	{
		pushstruct_nogc(inL, traitstructclass<T>::name(), (void*)value);
		return 1;
	}

	template<class T>
	static int push(lua_State*inL, const TSubclassOf<T>& value)
	{
		pushuobject(inL, value.Get());
		return 1;
	}
	template<class T>
	static int push(lua_State*inL, const TScriptInterface<T>& value)
	{
		pushuobject(inL, (void*)value.GetObject());
		return 1;
	}

	inline static int push(lua_State *inL, uint8 value)
	{
		ue_lua_pushinteger(inL, value);
		return 1;
	}
	inline static int push(lua_State *inL, int value)
	{
		ue_lua_pushinteger(inL, value);
		return 1;
	}
	inline static int push(lua_State *inL, int64 value)
	{
		ue_lua_pushinteger(inL, value);
		return 1;
	}
	inline static int push(lua_State *inL, uint64 value)
	{
		ue_lua_pushinteger(inL, value);
		return 1;
	}
	inline static int push(lua_State *inL, unsigned int value)
	{
		ue_lua_pushinteger(inL, value);
		return 1;
	}
	inline static int push(lua_State *inL, float value)
	{
		ue_lua_pushnumber(inL, value);
		return 1;
	}

	inline static int push(lua_State *inL, double value)
	{
		ue_lua_pushnumber(inL, value);
		return 1;
	}
	inline static int push(lua_State *inL, bool value)
	{
		ue_lua_pushboolean(inL, value);
		return 1;
	}
	inline static int push(lua_State *inL, const FString& value)
	{
		ue_lua_pushstring(inL, TCHAR_TO_UTF8(*value));
		return 1;
	}
	 
	inline static int push(lua_State *inL, const FText& value)
	{
		ue_lua_pushstring(inL, TCHAR_TO_UTF8(*value.ToString()));
		return 1;
	}

	inline static int push(lua_State *inL, const FName& value)
	{
		ue_lua_pushstring(inL, TCHAR_TO_UTF8(*value.ToString()));
		return 1;
	}

	inline static int push(lua_State *inL, const char* value)
	{
		ue_lua_pushstring(inL, value);
		return 1;
	}
	inline static int push(lua_State *inL, const UClass* value)
	{
		pushuobject(inL, (void*)value);
		return 1;
	}
	static void pushproperty(lua_State*inL, UProperty* property, const void* ptr);
	static void push_ret_property(lua_State*inL, UProperty* property, const void* ptr);
	static void pushback_ref_property(lua_State*inL, int32 LuaStackIndex, UProperty* property, const void* ptr);
	static void pushproperty_type(lua_State*inL, UBoolProperty* p, const void*ptr);
	static void pushproperty_type(lua_State*inL, UIntProperty* p, const void*ptr);
	static void pushproperty_type(lua_State*inL, UUInt32Property* p, const void*ptr);
	static void pushproperty_type(lua_State*inL, UInt64Property* p, const void*ptr);
	static void pushproperty_type(lua_State*inL, UFloatProperty* p, const void*ptr);
	static void pushproperty_type(lua_State*inL, UDoubleProperty* p, const void*ptr);
	static void pushproperty_type(lua_State*inL, UObjectPropertyBase* p, const void*ptr);
	static void pushproperty_type(lua_State*inL, UStrProperty* p, const void*ptr);
	static void pushproperty_type(lua_State*inL, UNameProperty* p, const void*ptr);
	static void pushproperty_type(lua_State*inL, UTextProperty* p, const void*ptr);
	static void pushproperty_type(lua_State*inL, UByteProperty* p, const void*ptr);
	static void pushproperty_type(lua_State*inL, UEnumProperty* p, const void*ptr);
	static void pushproperty_type(lua_State*inL, UStructProperty* p, const void*ptr);
	static void pushproperty_type(lua_State*inL, UMulticastDelegateProperty* p, const void*ptr);
	static void pushproperty_type(lua_State*inL, UWeakObjectProperty* p, const void*ptr);
	static void pushproperty_type(lua_State*inL, UArrayProperty* property, const void* ptr);
	static void pushproperty_type(lua_State*inL, UMapProperty* p, const void*ptr);
	static void pushproperty_type(lua_State*inL, USetProperty* p, const void*ptr);
	static void pushproperty_type(lua_State*inL, UInterfaceProperty* p, const void*ptr);
	
	static void pushproperty_valueptr(lua_State*inL, UProperty* property, const void* ptr);
	static void pushproperty_type_valueptr(lua_State*inL, UBoolProperty* p, const void*ptr);
	static void pushproperty_type_valueptr(lua_State*inL, UIntProperty* p, const void*ptr);
	static void pushproperty_type_valueptr(lua_State*inL, UUInt32Property* p, const void*ptr);
	static void pushproperty_type_valueptr(lua_State*inL, UInt64Property* p, const void*ptr);
	static void pushproperty_type_valueptr(lua_State*inL, UFloatProperty* p, const void*ptr);
	static void pushproperty_type_valueptr(lua_State*inL, UDoubleProperty* p, const void*ptr);
	static void pushproperty_type_valueptr(lua_State*inL, UObjectPropertyBase* p, const void*ptr);
	static void pushproperty_type_valueptr(lua_State*inL, UStrProperty* p, const void*ptr);
	static void pushproperty_type_valueptr(lua_State*inL, UNameProperty* p, const void*ptr);
	static void pushproperty_type_valueptr(lua_State*inL, UTextProperty* p, const void*ptr);
	static void pushproperty_type_valueptr(lua_State*inL, UByteProperty* p, const void*ptr);
	static void pushproperty_type_valueptr(lua_State*inL, UEnumProperty* p, const void*ptr);
	static void pushproperty_type_valueptr(lua_State*inL, UStructProperty* p, const void*ptr);
	static void pushproperty_type_valueptr(lua_State*inL, UMulticastDelegateProperty* p, const void*ptr);
	static void pushproperty_type_valueptr(lua_State*inL, UWeakObjectProperty* p, const void*ptr);
	static void pushproperty_type_valueptr(lua_State*inL, UArrayProperty* property, const void* ptr);
	static void pushproperty_type_valueptr(lua_State*inL, UMapProperty* p, const void*ptr);
	static void pushproperty_type_valueptr(lua_State*inL, USetProperty* p, const void*ptr);
	static void pushproperty_type_valueptr(lua_State*inL, UInterfaceProperty* p, const void*ptr);


	template<class T>
	static int push(lua_State *inL, const TArray<T>& value);

	template<class T>
	static int push(lua_State *inL, const TSet<T>& value);

	template<class K, class V>
	static int push(lua_State *inL, const TMap<K, V>& value);

	template<class T>
	static int push(lua_State *inL, const TWeakObjectPtr<T>& value);

	template<class T, int N>
	static int pushcarr(lua_State *inL, const T(&value)[N])
	{
		ue_lua_newtable(inL);
		for (int i = 0; i < N; i++)
		{
			push(inL, i + 1);
			push(inL, value[i]);
			ue_lua_rawset(inL, -3);
		}
		return 1;
	}

	template<class T, int N>
	static int popcarr(lua_State *inL, const T(&value)[N], int index)
	{
		ue_lua_pushvalue(inL, index);
		ue_lua_pushnil(inL);
		int i = 0;
		while (ue_lua_next(inL, -2) != 0)
		{
			if (i < N) 
			{
				value[i] = popiml<T>::pop(inL, -1);
				ue_lua_pop(inL, 1);
			}
			++i;
		}
		ue_lua_pop(inL, 1);
	}

	static int pushall(lua_State *inL)
	{
		return 0;
	}

	template<class T>
	static int pushall(lua_State *inL, const T& value, typename traitstructclass<T>::value* t = nullptr)
	{
		pushstruct_stack(inL, traitstructclass<T>::name(), (void*)(&value));
		return 1;
	}

	template<class T>
	static int pushall(lua_State *inL, T&& value)
	{
		push(inL, Forward<T>(value));
		return 1;
	}
	template<class T1, class... T2>
	static int pushall(lua_State *inL, T1&& value, T2&&... args)
	{
		push(inL, Forward<T1>(value));
		return 1 + pushall(inL, Forward<T2>(args)...);
	}

	template<typename T>
	inline static T pop(lua_State*inL, int index)
	{
		T result;
		read(result, inL, index);
		ue_lua_pop(inL, 1);
		return result;
	};

	template<typename T>
	inline static void read(T& result, lua_State*inL, int index)
	{
		result = popiml<T>::pop(inL, index);
	};

	template<class T>
	inline static void read(TArray<T>& Result, lua_State*inL, int index)
	{
		ULuaArrayHelper* ArrHelper = (ULuaArrayHelper*)touobject(inL, index);
		if (ArrHelper)
		{
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
			ensureAlwaysMsgf(0, L"not arr");
		}
	}

	template<class T>
	inline static void read(TArray<T>** ArrPtr, lua_State*inL, int index)
	{
		ULuaArrayHelper* ArrHelper = (ULuaArrayHelper*)touobject(inL, index);
		if (ArrHelper)
		{
			*ArrPtr = ArrHelper->ValuePtr<T>();
		}
		else if (ue_lua_istable(inL, index))
		{
			**ArrPtr = popiml<TArray<T>>::pop(inL, index);
		}
		else
		{
			ensureAlwaysMsgf(0, L"not arr");
		}
	}

	template<class K, class V>
	inline static void read(TMap<K, V>& Result, lua_State*inL, int index)
	{
		ULuaMapHelper* Helper = (ULuaMapHelper*)touobject(inL, index);
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
			ensureAlwaysMsgf(0, L"not map");
		}
	}

	template<class K, class V>
	inline static void read(TMap<K, V>** MapPtr, lua_State*inL, int index)
	{
		ULuaMapHelper* Helper = (ULuaMapHelper*)touobject(inL, index);
		if (Helper)
		{
			*MapPtr = Helper->ValuePtr<K, V>();
		}
		else if (ue_lua_istable(inL, index))
		{
			**MapPtr = popiml<TMap<K, V>>::pop(inL, index);
		}
		else
		{
			ensureAlwaysMsgf(0, L"not map");
		}
	}

	template<class T>
	inline static void read(TSet<T>& Result, lua_State*inL, int index)
	{
		ULuaSetHelper* Helper = (ULuaSetHelper*)touobject(inL, index);
		if (Helper)
		{
			TSet<T>*Right = Helper->ValuePtr<T>();
			if (Right != &Result)
				Result = *(TSet<T>*)Right;
		}
		else if (ue_lua_istable(inL, index))
		{
			Result = popiml<TSet<T>>::pop(inL, index);
		}
		else
		{
			ensureAlwaysMsgf(0, L"not set");
		}
	}

	template<class T>
	inline static void read(TSet<T>** SetPtr, lua_State*inL, int index)
	{
		ULuaSetHelper* Helper = (ULuaSetHelper*)touobject(inL, index);
		if (Helper)
		{
			*SetPtr = Helper->ValuePtr<T>();
		}
		else if (ue_lua_istable(inL, index))
		{
			**SetPtr = popiml<TSet<T>>::pop(inL, index);
		}
		else
		{
			ensureAlwaysMsgf(0, L"not set");
		}
	}


	template<class T>
	static void pushback_table(lua_State*inL, int index, const TArray<T>& Arr)
	{
		ue_lua_pushvalue(inL, index);
		int table_len = ue_lua_objlen(inL, -1);
		for (int i = 1; i <= FMath::Max(table_len, Arr.Num()); i++)
		{
			ue_lua_pushinteger(inL, i);
			if (i <= Arr.Num())
				push(inL, Arr[i - 1]);
			else
				ue_lua_pushnil(inL);
			ue_lua_rawset(inL, -3);
		}
	}
	template<class K, class V>
	static void pushback_table(lua_State*inL, int index, const TMap<K, V>& Map)
	{
		ue_lua_newtable(inL);
		ue_lua_pushvalue(inL, index);
		ue_lua_pushnil(inL);
		int i = 1;
		while (ue_lua_next(inL, -2) !=0)
		{
			ue_lua_pop(inL, 1);
			K key = popiml<K>::pop(inL, -1);
			if (!Map.Contains(key))
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
			push(inL, Pair.Key);
			push(inL, Pair.Value);
			ue_lua_rawset(inL, -3);
		}
	}

	template<class T>
	static void pushback_table(lua_State*inL, int index, const TSet<T>& Set)
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
			push(inL, Key);
			push(inL, true);
			ue_lua_rawset(inL, -3);
		}
	}

	template<class T, class T1>
	static T1& MayNeedTempIns(T1& v, typename TEnableIf<TIsSame<typename traitstructclass<T>::NotStructType, NotNeedTempInsType>::Value, T>::Type* p = nullptr)
	{
		return v;
	}

	template<class T, class T1>
	static T1& MayNeedTempIns(T1& v, typename TEnableIf<TIsSame<typename traitstructclass<T>::NotStructType, NeedTempInsType>::Value, T>::Type* p = nullptr)
	{
		T1& TempIns = GetTempIns<T1>();
		TempIns = v;
		return TempIns;
	}

	template<class K, class V>
	static TMap<K, V>& MayNeedTempInsForMap(TMap<K, V>& v, typename TEnableIf< TOr<TIsSame<typename traitstructclass<K>::NotStructType, NeedTempInsType>, TIsSame<typename traitstructclass<V>::NotStructType, NeedTempInsType>>::Value, K>::Type* p = nullptr)
	{
		TMap<K, V>& TempIns = GetTempIns<TMap<K, V>>();
		TempIns = v;
		return TempIns;
	}

	template<class K, class V>
	static TMap<K, V>& MayNeedTempInsForMap(TMap<K, V>& v, typename TEnableIf< TAnd<TIsSame<typename traitstructclass<K>::NotStructType, NotNeedTempInsType>, TIsSame<typename traitstructclass<V>::NotStructType, NotNeedTempInsType>>::Value, K>::Type* p = nullptr)
	{
		return v;
	}

	template<class T>
	static void pushback(lua_State*inL, int index, TArray<T>& Arr)
	{
// is arrhelper
		if (touobject(inL, index))
		{
			ue_lua_pushvalue(inL, index);
		}
// is lua table
		else if (ue_lua_istable(inL, index))
		{
// 			TArray<T>& Temp = MayNeedTempIns<T>(Arr);
			pushback_table(inL, index, Arr);
		}
		else
		{
			ensureAlwaysMsgf(0, L"not arr");
		}
	}

	template<class K, class V>
	static void pushback(lua_State*inL, int index, TMap<K, V>& Map)
	{
		// is maphelper
		if (touobject(inL, index))
		{
			ue_lua_pushvalue(inL, index);
		}
		// is lua table
		else if (ue_lua_istable(inL, index))
		{
// 			TMap<K, V>& Temp = MayNeedTempInsForMap<K, V>(Map);
			pushback_table(inL, index, Map);
		}
		else
		{
			ensureAlwaysMsgf(0, L"not map");
		}
	}

	template<class T>
	static void pushback(lua_State*inL, int index, TSet<T>& Set)
	{
		// is arrhelper
		if (touobject(inL, index))
		{
			ue_lua_pushvalue(inL, index);
		}
		// is lua table
		else if (ue_lua_istable(inL, index))
		{
// 			TSet<T>& Temp = MayNeedTempIns<T>(Set);
			pushback_table(inL, index, Set);
		}
		else
		{
			ensureAlwaysMsgf(0, L"not set");
		}
	}

	template<class T>
	static void pushback(lua_State*inL, int index, const T& Struct, const typename traitstructclass<T>::value* p = nullptr)
	{
		ue_lua_pushvalue(inL, index);
	}

	template<class T>
	static void pushback_private(lua_State*inL, int index, TArray<T>& Arr)
	{
		// is arrhelper
		ULuaArrayHelper* ArrHelper = (ULuaArrayHelper*)touobject(inL, index);
		if (ArrHelper)
		{
			TArray<T>*Right = ArrHelper->ValuePtr<T>();
			*Right = Arr;
			ue_lua_pushvalue(inL, index);
		}
		// is lua table
		else if (ue_lua_istable(inL, index))
		{
// 			TArray<T>& Temp = MayNeedTempIns<T>(Arr);
			pushback_table(inL, index, Arr);
		}
		else
		{
			ensureAlwaysMsgf(0, L"not arr");
		}
	}

	template<class K, class V>
	static void pushback_private(lua_State*inL, int index, TMap<K, V>& Map)
	{
		// is maphelper
		ULuaMapHelper* Helper = (ULuaMapHelper*)touobject(inL, index);
		if (Helper)
		{
			TMap<K, V>* MapPtr = Helper->ValuePtr<K, V>();
			*MapPtr = Map;
			ue_lua_pushvalue(inL, index);
		}
		// is lua table
		else if (ue_lua_istable(inL, index))
		{
// 			TMap<K, V>& Temp = MayNeedTempInsForMap<K, V>(Map);
			pushback_table(inL, index, Map);
		}
		else
		{
			ensureAlwaysMsgf(0, L"not map");
		}
	}

	template<class T>
	static void pushback_private(lua_State*inL, int index, TSet<T>& Set)
	{
		// is maphelper
		ULuaSetHelper* Helper = (ULuaSetHelper*)touobject(inL, index);
		if (Helper)
		{
			TSet<T>* SetPtr = Helper->ValuePtr<T>();
			*SetPtr = Set;
			ue_lua_pushvalue(inL, index);
		}
		// is lua table
		else if (ue_lua_istable(inL, index))
		{
// 			TSet<T>& Temp = MayNeedTempIns<T>(Set);
			pushback_table(inL, index, Set);
		}
		else
		{
			ensureAlwaysMsgf(0, L"not set");
		}
	}

	template<class T>
	static void pushback_private(lua_State*inL, int index, const T& Struct, const typename traitstructclass<T>::value* p = nullptr)
	{
		*(T*)tostruct(inL, index) = Struct;
		ue_lua_pushvalue(inL, index);
	}

	static void popproperty(lua_State* inL, int index, UProperty* property, void* ptr);
	static void popproperty_type(lua_State*inL, int index, UBoolProperty* p, void*ptr);
	static void popproperty_type(lua_State*inL, int index, UIntProperty* p, void*ptr);
	static void popproperty_type(lua_State*inL, int index, UUInt32Property* p, void*ptr);
	static void popproperty_type(lua_State*inL, int index, UInt64Property* p, void*ptr);
	static void popproperty_type(lua_State*inL, int index, UFloatProperty* p, void*ptr);
	static void popproperty_type(lua_State*inL, int index, UDoubleProperty* p, void*ptr);
	static void popproperty_type(lua_State*inL, int index, UObjectPropertyBase* p, void*ptr);
	static void popproperty_type(lua_State*inL, int index, UStrProperty* p, void*ptr);
	static void popproperty_type(lua_State*inL, int index, UNameProperty* p, void*ptr);
	static void popproperty_type(lua_State*inL, int index, UTextProperty* p, void*ptr);
	static void popproperty_type(lua_State*inL, int index, UByteProperty* p, void*ptr);
	static void popproperty_type(lua_State*inL, int index, UEnumProperty* p, void*ptr);
	static void popproperty_type(lua_State*inL, int index, UStructProperty* p, void*ptr);
	static void popproperty_type(lua_State*inL, int index, UArrayProperty* p, void* ptr);
	static void popproperty_type(lua_State*inL, int index, UMapProperty* p, void*ptr);
	static void popproperty_type(lua_State*inL, int index, USetProperty* p, void*ptr);
	static void popproperty_type(lua_State*inL, int index, UInterfaceProperty* p, void*ptr);
	static void popproperty_type(lua_State*inL, int index, UMulticastDelegateProperty* p, void*ptr);

	template<class returnType, class... T>
	static returnType callr(const FString& funcname, T&&... args)
	{
		return callr<returnType>(TCHAR_TO_UTF8(*funcname), Forward<T>(args)...);
	}

	template<class returnType, class... T>
	static returnType callr(const char* funcname, T&&... args)
	{
		if (TheOnlyLuaState == nullptr)
			init();
		ue_lua_pushcfunction(TheOnlyLuaState, ErrHandleFunc);
		ue_lua_getfield(TheOnlyLuaState, LUA_GLOBALSINDEX, funcname);
		int32 ParamCount = pushall(TheOnlyLuaState, Forward<T>(args)...);
		if (ue_lua_pcall(TheOnlyLuaState, ParamCount, 1, -(ParamCount + 2)))
		{
			log(ue_lua_tostring(TheOnlyLuaState, -1));
		}
		ue_lua_remove(TheOnlyLuaState, -2);
		return pop<returnType>(TheOnlyLuaState, -1);
	}

	template<class... T>
	static void call(const char* funcname, T&&... args)
	{
		if (TheOnlyLuaState == nullptr)
			init();
		ue_lua_pushcfunction(TheOnlyLuaState, ErrHandleFunc);
		ue_lua_getfield(TheOnlyLuaState, LUA_GLOBALSINDEX, funcname);
		int32 ParamCount = pushall(TheOnlyLuaState, Forward<T>(args)...);
		if (ue_lua_pcall(TheOnlyLuaState, ParamCount, 0, -(ParamCount + 2)))
		{
			log(ue_lua_tostring(TheOnlyLuaState, -1));
		}
		ue_lua_pop(TheOnlyLuaState, 1);
	}

	template<class... T>
	static void call(const FString& funcname, T&&... args)
	{
		call(TCHAR_TO_UTF8(*funcname), Forward<T>(args)...);
	}

	template<class... T>
	static void call(int funcid, T&&... args)
	{
		if (TheOnlyLuaState == nullptr)
			init();
		ue_lua_pushcfunction(TheOnlyLuaState, ErrHandleFunc);
		ue_lua_rawgeti(TheOnlyLuaState, LUA_REGISTRYINDEX, funcid);
		int32 ParamCount = pushall(TheOnlyLuaState, Forward<T>(args)...);
		if (ue_lua_pcall(TheOnlyLuaState, ParamCount, 0, -(ParamCount + 2)))
		{
			log(ue_lua_tostring(TheOnlyLuaState, -1));
		}
		ue_lua_pop(TheOnlyLuaState, 1);
	}

	template<class ReturnType, class UObjectPtrType, class... T>
	static ReturnType inscallr(const char* staticfuncname, const char* memberfuncname, const UObjectPtrType* ptr, T&&... args)
	{
		if (TheOnlyLuaState == nullptr)
			init();
#ifndef LUA_CALL_WITHOUTCHECK
		if (ptr == nullptr || !existluains(TheOnlyLuaState, (void*)(ptr)))
		{
			ensureAlwaysMsgf(0, L"cpp ins doesn't ctor or has been release");
			return ReturnType();
		}
#endif
		return callr<ReturnType>(staticfuncname, memberfuncname, ptr, Forward<T>(args)...);
	}

	template<class UObjectPtrType, class... T>
	static void inscall(const char* staticfuncname, const char* memberfuncname, const UObjectPtrType* ptr, T&&... args)
	{
		if (TheOnlyLuaState == nullptr)
			init();
#ifndef LUA_CALL_WITHOUTCHECK
		if (ptr == nullptr || !existluains(TheOnlyLuaState, (void*)(ptr)))
		{
			ensureAlwaysMsgf(0, L"cpp ins doesn't ctor or has been release");
			return;
		}
#endif
		call(staticfuncname, memberfuncname, ptr, Forward<T>(args)...);
	}

	static void call(lua_State* inL, int funcid, UFunction* funcsig, void* ptr);
	static void bpcall(const char* funname, UFunction* funcsig, void* ptr);

	static void pushcontainer(lua_State* inL, void *Obj, UArrayProperty* Property);
	static void pushcontainer(lua_State* inL, void *Obj, UMapProperty* Property);
	static void pushcontainer(lua_State* inL, void *Obj, USetProperty* Property);

	template<class T>
	static T& GetTempIns()
	{
		const int32 len = 10;
		static T* Temp = new T[len];
		static int32 i = -1;
		i = (i + 1) % len;
		return Temp[i];
	}

	template<class T>
	static T& GetTempInsInit(typename TEnableIf<TStructOpsTypeTraits<T>::WithCopy, T>::Type* p = nullptr)
	{
		T& Ins = GetTempIns<T>();
		Ins = T();
		return Ins;
	}

	template<class T>
	static T& GetTempInsInit(typename TEnableIf<!TStructOpsTypeTraits<T>::WithCopy, T>::Type* p = nullptr)
	{
		T& Ins = GetTempIns<T>();
		return Ins;
	}
};

template<class T>
int UTableUtil::push(lua_State*inL, const TArray<T>& value)
{
	ue_lua_newtable(inL);
	for (int i = 0; i < value.Num(); i++)
	{
		push(inL, i + 1);
		push(inL, value[i]);
		ue_lua_rawset(inL, -3);
	}
	return 1;
}

template<class T>
int UTableUtil::push(lua_State*inL, const TSet<T>& value)
{
	ue_lua_newtable(inL);
	for (auto& ele : value)
	{
		push(inL, ele);
		push(inL, true);
		ue_lua_rawset(inL, -3);
	}
	return 1;
}

template<class K, class V>
int UTableUtil::push(lua_State*inL, const TMap<K, V>& value)
{
	ue_lua_newtable(inL);
	for (auto& ele : value)
	{
		push(inL, ele.Key);
		push(inL, ele.Value);
		ue_lua_rawset(inL, -3);
	}
	return 1;
}

template<class T>
int UTableUtil::push(lua_State*inL, const TWeakObjectPtr<T>& value)
{
	pushuobject(inL, (void*)value.Get());
	return 1;
}


class FLuaGcObj : public FGCObject
{
public:
	TSet<UObject*> objs;
	static FLuaGcObj* Get()
	{
		static FLuaGcObj* Singleton = nullptr;
		if (Singleton == nullptr)
			Singleton = new FLuaGcObj;
		return Singleton;
	}

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	

};
