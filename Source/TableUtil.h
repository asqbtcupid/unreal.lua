#pragma once
#include "lua_tinker.h"
#include <typeinfo>
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Luautils.h"
#include "traitweakclass.h"
#include "TableUtil.generated.h"

struct EnumItem;
DECLARE_LOG_CATEGORY_EXTERN(LuaLog, Log, All);
#define LuaDebug 1
using namespace std;
using luafunc = int( struct lua_State* );


class FLuaGcObj : FGCObject
{
public:
	TSet<UObject*> objs;
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

UCLASS(MinimalAPI)
class UTableUtil : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	static lua_State* L;

public:
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
	static int toint(int i);
	static void loadlib(const luaL_Reg funclist[], const char* classname);
	static void loadEnum(const EnumItem list[], const char* enumname);
	static void addutil(const luaL_Reg funclist[], const char* tablename);
	static UObject* FObjectFinder( UClass* Class, FString PathName );

	UFUNCTION(BlueprintCallable, Category = "TableUtil")
	static void log(FString content);

	UFUNCTION(BlueprintCallable, Category = "TableUtil")
	static void shutdown();
	static bool existdata(void * p);
	
	static void executeFunc(FString funcName, int n = 0, int nargs = 0);
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
	static UObject* Call_obj(FString funcName, int count_param);

	UFUNCTION(BlueprintCallable, Category = "TableUtil")
	static float Call_float(FString funcName, int count_param);

	UFUNCTION(BlueprintCallable, Category = "TableUtil")
	static FString Call_str(FString funcName, int count_param);

	UFUNCTION(BlueprintCallable, Category = "TableUtil")
	static bool Call_bool(FString funcName, int count_param);

	UFUNCTION(BlueprintCallable, Category = "TableUtil")
	static void CtorCpp(UObject* p, FString classpath);

	static void rmgcref(UObject* p);

	// static FLuaGcObj gcobjs;
	static int push() { return 0; }
	template<typename T>
	static int push(T value);
	template<typename T>
	static int push(T* value);

	static void pushclass(const char* classname, void* p, bool bgcrecord = false);
	template<> static int push(int value);
	template<> static int push(float value);
	template<> static int push(double value);
	template<> static int push(bool value);
	template<> static int push(FString value);
	template<> static int push(const char* value);

	template<class T> 
	static int push(TArray<T> value);

	template<class T>
	static int push(TWeakObjectPtr<T> value);

	template<class T>
	class popiml{
		public:
			static T pop(int index)
			{
// 				FString name = typeid(T).name();
// 				int FirstSpaceIndex = name.Find(TEXT(" "));
// 				name = name.Mid(FirstSpaceIndex + 1);
				return *(T*)tousertype("", index);
			}
	};
	template<class T>
	class popiml<T*> {
	public:
		static T* pop(int index)
		{
			return (T*)tousertype("", index);
		}
	};

	template<class T>
	class popiml< TArray<T> > {
	public:
		static TArray<T> pop(int index)
		{
			TArray<T> result;
#ifdef LuaDebug
			if (!lua_istable(L, -1))
			{
				log("not table poparr");
				return result;
			}
#endif
			lua_pushnil(L);
			while (lua_next(L, -2) != 0)
			{
				result.Add(UTableUtil::pop<T>(-1));
			}
			return result;
		}
	};

	
	template<> class popiml<int>{
	public:
		static int pop(int index){return (int)lua_tointeger(L, index);}
	};

	template<> class popiml<bool> {
	public:
		static bool pop(int index){return !!(lua_toboolean(L, index));}
	};

	template<> class popiml<FName> {
	public:
		static FName pop(int index) { return FName(luaL_checkstring(L, index)); }
	};
	template<> class popiml<FString> {
	public:
		static FString pop(int index) { return ANSI_TO_TCHAR(luaL_checkstring(L, index));}
	};
	template<> class popiml<float> {
	public:
		static float pop(int index) { return (float)lua_tonumber(L, index); }
	};
	template<> class popiml<double> {
	public:
		static double pop(int index) { return (double)lua_tonumber(L, index); }
	};

	template<typename T>
	static T pop(int index) 
	{
		auto result = popiml<T>::pop(index);
		lua_pop(L, 1);
		return result;
	};
	template<> static void pop<void>(int index) { lua_pop(L, 1); };

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

	static TMap<FString, TMap<FString, UProperty*>> propertyMap;
	static void InitClassMap();
	static UProperty* GetPropertyByName(FString classname, FString propertyname);
	static UProperty* GetPropertyByName(UClass *Class, FString propertyname);
};

template<>
int UTableUtil::push(int value)
{
	lua_pushinteger(L, value);
	return 1;
}
template<>
int UTableUtil::push(float value)
{
	lua_pushnumber(L, value);
	return 1;
}
template<>
int UTableUtil::push(double value)
{
	lua_pushnumber(L, value);
	return 1;
}
template<>
int UTableUtil::push(bool value)
{
	lua_pushboolean(L, value);
	return 1;
}

template<>
int UTableUtil::push(FString value)
{
	lua_pushstring(L, TCHAR_TO_ANSI(*value));
	return 1;
}

template<>
int UTableUtil::push(const char* value)
{
	lua_pushstring(L, value);
	return 1;
}

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
	// 	class T;
	FString name = typeid(T).name();
	int FirstSpaceIndex = name.Find(TEXT(" "));
	name = name.Mid(FirstSpaceIndex + 1);
	pushclass(TCHAR_TO_ANSI(*name), (void*)(new T(value)), true);
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
	traitweakclass<T>::traitType* weakObj = new traitweakclass<T>::traitType(p);
	UClass* Class = T::StaticClass();
	FString namecpp = FString::Printf(TEXT("%s%s"), Class->GetPrefixCPP(), *Class->GetName());
	namecpp = "TWeakObjectPtr_" + namecpp;
	pushclass(TCHAR_TO_ANSI(*namecpp), (void*)weakObj, true);
	return 1;
}