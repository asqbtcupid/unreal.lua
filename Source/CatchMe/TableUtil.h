#pragma once
#include "lua_tinker.h"
// #include "allheader.inl"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "traitweakclass.h"
#include "traitstructclass.h"
#include "TableUtil.generated.h"

#define LuaCtor(classname, ...) UTableUtil::call("Ctor", classname, this, ##__VA_ARGS__);
#define LuaCall(functionname, ptr, ...) UTableUtil::inscall("Call", functionname, ptr, ##__VA_ARGS__);

#define LuaCallr(ret, functionname, ptr,...) UTableUtil::inscallr<ret>("Call", functionname, ptr, ##__VA_ARGS__);

#define LuaStaticCall(functionname, ...)	UTableUtil::call(functionname, ##__VA_ARGS__);
#define LuaStaticCallr(ret, functionname, ...)	UTableUtil::callr<ret>(functionname, ##__VA_ARGS__);

struct EnumItem;
DECLARE_LOG_CATEGORY_EXTERN(LuaLog, Log, All);
#define LuaDebug 0 

using luafunc = int(struct lua_State*);

void* tousertype(lua_State* L, const char* classname, int i);
void* touobject(lua_State* L, int i);
void* tostruct(lua_State* L, int i);
int ErrHandleFunc(lua_State*L);
template<class T>
class popiml {
public:
	static T pop(lua_State *L, int index)
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
	static T* pop(lua_State *L, int index)
	{
		return (T*)touobject(L, index);
	}
};

template<class T>
class popiml< TArray<T> > {
public:
	static TArray<T> pop(lua_State *L, int index)
	{
		TArray<T> result;
#ifdef LuaDebug
		if (!lua_istable(L, -1))
		{
			//UTableUtil::log("not table poparr");
			return result;
		}
#endif
		lua_pushnil(L);
		while (lua_next(L, -2) != 0)
		{
			result.Add(popiml<T>::pop(L, -1));
			lua_pop(L, 1);
		}
		return result;
	}
};


template<> class popiml<int> {
public:
	static int pop(lua_State *L, int index) { return (int)lua_tointeger(L, index); }
};

template<> class popiml<bool> {
public:
	static bool pop(lua_State *L, int index) { return !!(lua_toboolean(L, index)); }
};

template<> class popiml<FName> {
public:
	static FName pop(lua_State *L, int index) { return FName(luaL_checkstring(L, index)); }
};
template<> class popiml<FText> {
public:
	static FText pop(lua_State *L, int index) { return FText::FromString(luaL_checkstring(L, index)); }
};
template<> class popiml<FString> {
public:
	static FString pop(lua_State *L, int index) { return ANSI_TO_TCHAR(luaL_checkstring(L, index)); }
};
template<> class popiml<float> {
public:
	static float pop(lua_State *L, int index) { return (float)lua_tonumber(L, index); }
};
template<> class popiml<double> {
public:
	static double pop(lua_State *L, int index) { return (double)lua_tonumber(L, index); }
};
template<> class popiml<void> {
public:
	static void pop(lua_State *L, int index) { lua_pop(L, 1); }
};

class LuaTickObject :public FTickableGameObject
{
public:
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;
};

UCLASS()
class UTableUtil : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static LuaTickObject* PtrTickObject;
	static lua_State* L;
	// temporarily for multi gameinstance, later I will add Multi Lua_State
	static int32 ManualInitCount;
	static bool HasManualInit;
	// static bool bIsInsCall;
#ifdef LuaDebug
	static TMap<FString, int> countforgc;
#endif
	UTableUtil();
	static void addmodule(const char* classname);
	static void useCustomLoader();
	static void init(bool IsManual = false);
	static void GC();
	static void openmodule(const char* classname);
	static void closemodule();
	static void addfunc(const char* classname, luafunc f);
	static void initmeta();
	static void setmeta(const char* classname, int index);

	static void* tousertype(lua_State* InL, const char* classname, int i);
	static void loadlib(const luaL_Reg funclist[], const char* classname);
	static void loadEnum(const EnumItem list[], const char* enumname);
	static void addutil(const luaL_Reg funclist[], const char* tablename);
	static int pushluafunc(int index = -1);
	static int popluafunc(int index = -1);
	static void unref(int r);

	static UObject* FObjectFinder(UClass* Class, FString PathName);

	static void log(const FString& content);

	static void shutdown();
	static void shutdown_internal();
	static bool existdata(void * p);
	static bool existluains(void * p);

	static void rmgcref(UObject* p);
	static void addgcref(UObject* p);

	// static FLuaGcObj gcobjs;
	static int push() { return 0; }
	template<typename T>
	static int push(const T& value);
	template<typename T>
	static int push(T* value);

	static void pushuobject(lua_State *inL, void* p, bool bgcrecord = false);
	static void pushstruct(lua_State *inL, const char* structname, void* p, bool bgcrecord = false);
	static int push(int value);
	static int push(uint8 value);
	static int push(float value);
	static int push(double value);
	static int push(bool value);
	static int push(const FString& value);
	static int push(const FText& value);
	static int push(const FName& value);
	static int push(const char* value);
#ifdef LuaDebug
	static void testtemplate();
#endif
	template<class T>
	static int push(const TArray<T>& value);

	template<class T>
	static int push(const TSet<T>& value);

	template<class K, class V>
	static int push(const TMap<K, V>& value);

	template<class T>
	static int push(const TWeakObjectPtr<T>& value);

	template<typename T>
	static T pop(int index)
	{
		auto result = popiml<T>::pop(L, index);
		lua_pop(L, 1);
		return result;
	};

	template<class T1, class... T>
	static int push(const T1& value, T&&... args)
	{
		push(value);
		return 1 + push(Forward<T>(args)...);
	}

	template<class returnType, class... T>
	static returnType callr(const FString& funcname, T&&... args)
	{
		return callr<returnType>(TCHAR_TO_ANSI(*funcname), Forward<T>(args)...);
	}

	template<class returnType, class... T>
	static returnType callr(const char* funcname, T&&... args)
	{
		if (L == nullptr)
			init();
		lua_pushcfunction(L, ErrHandleFunc);
		lua_getfield(L, LUA_GLOBALSINDEX, funcname);
		int32 ParamCount = push(Forward<T>(args)...);
		// 		bIsInsCall = false;
		if (lua_pcall(L, ParamCount, 1, -(ParamCount + 2)))
		{
			log(lua_tostring(L, -1));
		}
		lua_remove(L, -2);
		return pop<returnType>(-1);
	}

	template<class... T>
	static void call(const char* funcname, T&&... args)
	{
		if (L == nullptr)
			init();
		lua_pushcfunction(L, ErrHandleFunc);
		lua_getfield(L, LUA_GLOBALSINDEX, funcname);
		int32 ParamCount = push(Forward<T>(args)...);
		// 		bIsInsCall = false;
		if (lua_pcall(L, ParamCount, 0, -(ParamCount + 2)))
		{
			log(lua_tostring(L, -1));
		}
		lua_pop(L, 1);
	}

	template<class... T>
	static void call(const FString& funcname, T&&... args)
	{
		call(TCHAR_TO_ANSI(*funcname), Forward<T>(args)...);
	}

	template<class... T>
	static void call(int funcid, T&&... args)
	{
		if (L == nullptr)
			init();
		lua_pushcfunction(L, ErrHandleFunc);
		lua_rawgeti(L, LUA_REGISTRYINDEX, funcid);
		int32 ParamCount = push(Forward<T>(args)...);
		if (lua_pcall(L, ParamCount, 0, -(ParamCount + 2)))
		{
			log(lua_tostring(L, -1));
		}
		lua_pop(L, 1);
	}

	template<class ReturnType, class UObjectPtrType, class... T>
	static ReturnType inscallr(const char* staticfuncname, const char* memberfuncname, const UObjectPtrType* ptr, T&&... args)
	{
		if (L == nullptr)
			init();

		if (ptr == nullptr)
			return ReturnType();

		if (!existluains((void*)(ptr)))
			return ReturnType();
		return callr<ReturnType>(staticfuncname, memberfuncname, ptr, Forward<T>(args)...);
	}

	template<class UObjectPtrType, class... T>
	static void inscall(const char* staticfuncname, const char* memberfuncname, const UObjectPtrType* ptr, T&&... args)
	{
		if (L == nullptr)
			init();

		if (ptr == nullptr)
			return;

		if (!existluains((void*)(ptr)))
			return;
		call(staticfuncname, memberfuncname, ptr, Forward<T>(args)...);
	}
};

//only for UObject
template<typename T>
int UTableUtil::push(T* value)
{
	// 	UClass* Class = T::StaticClass();
	// 	FString namecpp = FString::Printf(TEXT("%s%s"), Class->GetPrefixCPP(), *Class->GetName());
	pushuobject(L, (void*)value);
	return 1;
}

template<typename T>
int UTableUtil::push(const T& value)
{
	pushstruct(L, traitstructclass<T>::name(), (void*)(&value));
	return 1;
}

template<class T>
int UTableUtil::push(const TArray<T>& value)
{
	lua_newtable(L);
	for (int i = 0; i < value.Num(); i++)
	{
		push(i + 1);
		push(value[i]);
		lua_rawset(L, -3);
	}
	return 1;
}

template<class T>
int UTableUtil::push(const TSet<T>& value)
{
	lua_newtable(L);
	for (auto& ele : value)
	{
		push(ele);
		push(true);
		lua_rawset(L, -3);
	}
	return 1;
}

template<class K, class V>
int UTableUtil::push(const TMap<K, V>& value)
{
	lua_newtable(L);
	for (auto& ele : value)
	{
		push(ele.Key);
		push(ele.Value);
		lua_rawset(L, -3);
	}
	return 1;
}

template<class T>
int UTableUtil::push(const TWeakObjectPtr<T>& value)
{
	T *p = (T *)(value.Get());
	typename traitweakclass<T>::traitType* weakObj = new typename traitweakclass<T>::traitType(p);
	UClass* Class = T::StaticClass();
	FString namecpp = FString::Printf(TEXT("%s%s"), Class->GetPrefixCPP(), *Class->GetName());
	namecpp = "TWeakObjectPtr_" + namecpp;
	pushstruct(L, TCHAR_TO_ANSI(*namecpp), (void*)weakObj, true);
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
