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
#include "TableUtil.generated.h"

#define LuaCtor(classname, ...) UTableUtil::call("Ctor", classname, this, ##__VA_ARGS__);
#define LuaCall(functionname, ptr, ...) UTableUtil::inscall("Call", functionname, ptr, ##__VA_ARGS__);

#define LuaCallr(ret, functionname, ptr,...) UTableUtil::inscallr<ret>("Call", functionname, ptr, ##__VA_ARGS__);

#define LuaStaticCall(functionname, ...)	UTableUtil::call(functionname, ##__VA_ARGS__);
#define LuaStaticCallr(ret, functionname, ...)	UTableUtil::callr<ret>(functionname, ##__VA_ARGS__);

struct EnumItem
{
	const char* key;
	const int32 value;
};

template<class T>
class traitstructclass
{
};
typedef int luavalue_ref;
DECLARE_LOG_CATEGORY_EXTERN(LuaLog, Log, All);

using luafunc = int(struct lua_State*);

bool existdata(lua_State*inL, void * p);
LUAPLUGINRUNTIME_API void pushuobject(lua_State *inL, void* p, bool bgcrecord = false);
LUAPLUGINRUNTIME_API void pushstruct(lua_State *inL, const char* structname, void* p, bool bgcrecord = false);
LUAPLUGINRUNTIME_API void* tousertype(lua_State* L, const char* classname, int i);
LUAPLUGINRUNTIME_API void* touobject(lua_State* L, int i);
LUAPLUGINRUNTIME_API void* tostruct(lua_State* L, int i);
LUAPLUGINRUNTIME_API int ErrHandleFunc(lua_State*L);

template<class T>
class popiml {
public:
	static T pop(lua_State *L, int index, typename traitstructclass<T>::value* t = nullptr)
	{
		T* p = (T*)tostruct(L, index);
		if (p == nullptr)
		{
			return T();
		}
		return *p;
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
	static FName pop(lua_State *L, int index) { return FName(ue_lua_tolstring(L, index, NULL)); }
};
template<> class popiml<FText> {
public:
	static FText pop(lua_State *L, int index) { return FText::FromString(ue_lua_tolstring(L, index, NULL)); }
};
template<> class popiml<FString> {
public:
	static FString pop(lua_State *L, int index) { return ANSI_TO_TCHAR(ue_lua_tolstring(L, index, NULL)); }
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
#ifdef LuaDebug
	static TMap<FString, int> countforgc;
#endif
	static void addmodule(const char* classname);
	static void useCustomLoader();
	UFUNCTION(BlueprintCallable, Category = "Lua")
		static void init(bool IsManual = false);
	static void GC();
	static void openmodule(const char* classname);
	static void closemodule();
	static void addfunc(const char* classname, luafunc f);
	static void initmeta();
	static void setmeta(lua_State *inL, const char* classname, int index);

	static void* tousertype(lua_State* InL, const char* classname, int i);
	static void loadlib(const luaL_Reg funclist[], const char* classname);
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
		pushstruct(inL, traitstructclass<T>::name(), (void*)(&value));
		return 1;
	}

	template<typename T>
	static int push(lua_State *inL, const T* value, typename TEnableIf<TIsDerivedFrom<T, UObject>::IsDerived, T>::Type* p = nullptr)
	{
		pushuobject(inL, (void*)value);
		return 1;
	}

	template<typename T>
	static int push(lua_State *inL, const T* value, const typename traitstructclass<T>::value* p = nullptr)
	{
		pushstruct(inL, traitstructclass<T>::name(), (void*)value);
		return 1;
	}

	template<class T>
	int UTableUtil::push(lua_State*inL, const TSubclassOf<T>& value)
	{
		pushuobject(inL, value.Get());
		return 1;
	}


	inline static int push(lua_State *inL, uint8 value)
	{
		ue_lua_pushboolean(inL, !!value);
		return 1;
	}
	inline static int push(lua_State *inL, int value)
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
	static void pushproperty_type(lua_State*inL, UBoolProperty* p, const void*ptr);
	static void pushproperty_type(lua_State*inL, UIntProperty* p, const void*ptr);
	static void pushproperty_type(lua_State*inL, UUInt32Property* p, const void*ptr);
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
		ue_lua_pushvalue(L, index);
		ue_lua_pushnil(L);
		int i = 0;
		while (ue_lua_next(L, -2) != 0)
		{
			if (i < N) 
			{
				value[i] = popiml<T>::pop(L, -1);
				ue_lua_pop(L, 1);
			}
			++i;
		}
		ue_lua_pop(L, 1);
	}

	static int pushall(lua_State *inL)
	{
		return 0;
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
	static T pop(lua_State*inL, int index)
	{
		auto result = popiml<T>::pop(inL, index);
		ue_lua_pop(inL, 1);
		return result;
	};

	template<class T>
	static void pop(TArray<T>& Result, lua_State*inL, int index)
	{
		ULuaArrayHelper* ArrHelper = Cast<ULuaArrayHelper>((UObject*)touobject(inL, index));
		if (ArrHelper)
		{
			// 			FScriptArrayHelper_InContainer result(ArrHelper->Property, ArrHelper->Obj);
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
			ensureMsgf(0, L"not arr");
		}
	}

	static void popproperty(lua_State* inL, int index, UProperty* property, void* ptr);
	static void popproperty_type(lua_State*inL, int index, UBoolProperty* p, void*ptr);
	static void popproperty_type(lua_State*inL, int index, UIntProperty* p, void*ptr);
	static void popproperty_type(lua_State*inL, int index, UUInt32Property* p, void*ptr);
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
#ifndef LUA_CALL_WITHOUTCHECK
		if (TheOnlyLuaState == nullptr)
			init();

		if (ptr == nullptr)
			return ReturnType();

		if (!existluains(TheOnlyLuaState, (void*)(ptr)))
			return ReturnType();
		lua_pop(TheOnlyLuaState, 1);
#endif
		return callr<ReturnType>(staticfuncname, memberfuncname, ptr, Forward<T>(args)...);
	}

	template<class UObjectPtrType, class... T>
	static void inscall(const char* staticfuncname, const char* memberfuncname, const UObjectPtrType* ptr, T&&... args)
	{
#ifndef LUA_CALL_WITHOUTCHECK
		if (TheOnlyLuaState == nullptr)
			init();

		if (ptr == nullptr)
			return;

		if (!existluains(TheOnlyLuaState, (void*)(ptr)))
			return;
		lua_pop(TheOnlyLuaState, 1);
#endif
		call(staticfuncname, memberfuncname, ptr, Forward<T>(args)...);
	}

	static void call(lua_State* inL, int funcid, UFunction* funcsig, void* ptr);
	static void bpcall(const char* funname, UFunction* funcsig, void* ptr);

	static void pusharr(lua_State* inL, void *Obj, UArrayProperty* Property);
};

// template<typename T, typename T1>
// int UTableUtil::push(lua_State*inL, T* value)
// {
// // 	pushuobject(inL, (void*)value);
// 	return 1;
// }

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

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override
	{
		UTableUtil::GC();
		Collector.AllowEliminatingReferences(false);
		for (auto Object : objs)
		{
			Collector.AddReferencedObject(Object);
		}
		Collector.AllowEliminatingReferences(true);
	}

};
