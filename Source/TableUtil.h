#pragma once
#include "lua_tinker.h"
#include "allheader.inl"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Luautils.h"
#include "traitweakclass.h"
#include "traitstructclass.h"
#include "TableUtil.generated.h"

struct EnumItem;
DECLARE_LOG_CATEGORY_EXTERN(LuaLog, Log, All);
#define LuaDebug 1 
using namespace std;
using luafunc = int( struct lua_State* );

class FLuaGcObj : public FGCObject
{
public:
	TSet<UObject*> objs;
	static FLuaGcObj* Get()
	{
		static FLuaGcObj* Singleton = nullptr;
		if(Singleton == nullptr)
			Singleton = new FLuaGcObj;
		return Singleton;
	}

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override
	{
		Collector.AllowEliminatingReferences(false);
		for (auto Object : objs)
		{
			Collector.AddReferencedObject(Object);
		}
		Collector.AllowEliminatingReferences(true);
	}

};

void* tousertype(lua_State* L, const char* classname, int i);

template<class T>
class popiml{
	public:
		static T pop(lua_State *L, int index)
		{
			return *(T*)tousertype(L, "", index);
		}
};
template<class T>
class popiml<T*> {
public:
	static T* pop(lua_State *L, int index)
	{
		return (T*)tousertype(L, "", index);
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

	
template<> class popiml<int>{
public:
	static int pop(lua_State *L, int index){return (int)lua_tointeger(L, index);}
};

template<> class popiml<bool> {
public:
	static bool pop(lua_State *L, int index){return !!(lua_toboolean(L, index));}
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
	static FString pop(lua_State *L, int index) { return ANSI_TO_TCHAR(luaL_checkstring(L, index));}
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

UCLASS()
class UTableUtil : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static lua_State* L;
#ifdef LuaDebug
	static TMap<FString, int> countforgc;
#endif
	 UTableUtil();
	static void addmodule(const char* classname);

	UFUNCTION(BlueprintCallable, Category = "TableUtil", meta = (DisplayName = "Initlua"))
	static void init();

	static void openmodule(const char* classname);
	static void closemodule();
	static void addfunc(const char* classname, luafunc f);
	static void initmeta();
	static void setmeta(const char* classname, int index);

	static void* tousertype(const char* classname, int i);
	static void loadlib(const luaL_Reg funclist[], const char* classname);
	static void loadEnum(const EnumItem list[], const char* enumname);
	static void addutil(const luaL_Reg funclist[], const char* tablename);
	static int pushluafunc(int index = -1);
	static int popluafunc(int index = -1);
	static void unref(int r);

	static UObject* FObjectFinder( UClass* Class, FString PathName );

	UFUNCTION(BlueprintCallable, Category = "TableUtil")
	static void log(FString content);

	UFUNCTION(BlueprintCallable, Category = "TableUtil")
	static void shutdown();
	static bool existdata(void * p);
	
	static void executeFunc(FString funcName, int n = 0, int nargs = 0);
	static void executeFuncid(int32 funcid, int n = 0, int nargs = 0);
	UFUNCTION(BlueprintCallable, Category = "TableUtil")
	static void Call_void(FString funcName, int count_param = -1);

	UFUNCTION(BlueprintCallable, Category = "TableUtil")
	static void Push_obj(UObject *p);

	UFUNCTION(BlueprintCallable, Category = "TableUtil")
	static void Push_float(float value);

	UFUNCTION(BlueprintCallable, Category = "TableUtil")
	static void Push_str(FString value);

	UFUNCTION(BlueprintCallable, Category = "TableUtil")
	static void Push_bool(bool isTrue);

	UFUNCTION(BlueprintCallable, Category = "TableUtil")
	static UObject* Call_obj(FString funcName, int count_param = -1);

	UFUNCTION(BlueprintCallable, Category = "TableUtil")
	static float Call_float(FString funcName, int count_param = -1);

	UFUNCTION(BlueprintCallable, Category = "TableUtil")
	static FString Call_str(FString funcName, int count_param = -1);

	UFUNCTION(BlueprintCallable, Category = "TableUtil")
	static bool Call_bool(FString funcName, int count_param = -1);

	UFUNCTION(BlueprintCallable, Category = "TableUtil")
	static void CtorCpp(UObject* p, FString classpath);

	static void rmgcref(UObject* p);
	static void addgcref(UObject* p);

	// static FLuaGcObj gcobjs;
	static int push() { return 0; }
	template<typename T>
	static int push(T value);
	template<typename T>
	static int push(T* value);

	static void pushclass(const char* classname, void* p, bool bgcrecord = false);
	static int push(int value);
	static int push(uint8 value);
	static int push(float value);
	static int push(double value);
	static int push(bool value);
	static int push(FString value);
	static int push(FText value);
	static int push(FName value);
	static int push(const char* value);
	static void testtemplate();
	template<class T> 
	static int push(TArray<T> value);

	template<class T>
	static int push(TWeakObjectPtr<T> value);

	

	template<typename T>
	static T pop(int index) 
	{
		auto result = popiml<T>::pop(L, index);
		lua_pop(L, 1);
		return result;
	};

	template<class T1, class... T>
	static int push(T1 value, T... args)
	{
		push(value); 
		return 1 + push(args...);
	}

	template<class returnType, class... T>
	static returnType callr(FString funcname, T... args)
	{
		if (L == nullptr)
			init();
		executeFunc(funcname, 1, push(args...));
		return pop<returnType>(-1);
	}

	template<class... T>
	static void call(FString funcname, T... args)
	{
		if (L == nullptr)
			init();
		executeFunc(funcname, 0, push(args...));
	}

	template<class... T>
	static void call(int funcid, T... args)
	{
		if (L == nullptr)
			init();
		executeFuncid(funcid, 0, push(args...));
	}

	static TMap<FString, TMap<FString, UProperty*>> propertyMap;
	static TMap<FString, TMap<FString, UFunction*>> functionMap;

	static void InitClassMap();
	static UProperty* GetPropertyByName(FString classname, FString propertyname);
	static UProperty* GetPropertyByName(UClass *Class, FString propertyname);
	static UFunction* GetFunctionByName(FString classname, FString funcname);

};

template<typename T>
int UTableUtil::push(T* value)
{
// 	class T;
	UClass* Class = T::StaticClass();
	FString namecpp = FString::Printf(TEXT("%s%s"), Class->GetPrefixCPP(), *Class->GetName());
	pushclass(TCHAR_TO_ANSI(*namecpp), (void*)value);
	return 1;
}

template<typename T>
int UTableUtil::push(T value)
{
	// 	struct name trait, because xcode project GCC_ENABLE_CPP_RTTI = No, can't use typeid
	pushclass(traitstructclass<T>::name(), (void*)(new T(value)), true);
	return 1;
}

template<class T> 
int UTableUtil::push(TArray<T> value)
{
	lua_newtable(L);
	for (int i = 0; i < value.Num(); i++)
	{
		push(i+1);
		push(value[i]);
		lua_rawset(L, -3);
	}
	return 1;
}

template<class T>
int UTableUtil::push(TWeakObjectPtr<T> value)
{
	T *p = (T *)(value.Get());
	typename traitweakclass<T>::traitType* weakObj = new typename traitweakclass<T>::traitType(p);
	UClass* Class = T::StaticClass();
	FString namecpp = FString::Printf(TEXT("%s%s"), Class->GetPrefixCPP(), *Class->GetName());
	namecpp = "TWeakObjectPtr_" + namecpp;
	pushclass(TCHAR_TO_ANSI(*namecpp), (void*)weakObj, true);
	return 1;
}