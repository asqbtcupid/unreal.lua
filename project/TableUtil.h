#pragma once
#include "lua_tinker.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TableUtil.generated.h"

using namespace std;
using luafunc = int( struct lua_State* );

struct EnumItem
{
	const char* key;
	const int32 value;
};

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
	 UTableUtil();
	static void addmodule(const char* classname);

	UFUNCTION(BlueprintCallable, Category = "TableUtil")
	static void init();
	static void openmodule(const char* classname);
	static void closemodule();
	static void addfunc(const char* classname, luafunc f);
	static void initmeta();
	static void* tousertype(const char* classname, int i);
	static int toint(int i);
	static void loadlib(const luaL_Reg funclist[], const char* classname);
	static void loadEnum(const EnumItem list[], const char* enumname);

	UFUNCTION(BlueprintCallable, Category = "TableUtil")
	static void log(FString content);

	UFUNCTION(BlueprintCallable, Category = "TableUtil")
	static void shutdown();
	static void clearStack();
	static bool existdata(void * p);
	
	static void executeFunc(FString funcName);
	UFUNCTION(BlueprintCallable, Category = "TableUtil")
	static void Call_void(FString funcName);

	UFUNCTION(BlueprintCallable, Category = "TableUtil")
	static void Push_obj(UObject *p);

	UFUNCTION(BlueprintCallable, Category = "TableUtil")
	static void Push_float(float value);

	UFUNCTION(BlueprintCallable, Category = "TableUtil")
	static void Push_str(FString value);

	UFUNCTION(BlueprintCallable, Category = "TableUtil")
	static UObject* Call_obj(FString funcName);

	UFUNCTION(BlueprintCallable, Category = "TableUtil")
	static float Call_float(FString funcName);

	UFUNCTION(BlueprintCallable, Category = "TableUtil")
	static FString Call_str(FString funcName);

	UFUNCTION(BlueprintCallable, Category = "TableUtil")
	static void setpawn(ADefaultPawn *p);

	static void addgcref(UObject* p);
	static void rmgcref(UObject* p);


	static FLuaGcObj gcobjs;
// 	UPROPERTY()
// 	static TArray<UObject*> s;
	template<typename T>
	static void push(T value);

	static void push(const char* classname, void* p, bool bgcrecord = false);

	template<> static void push(int value);
	template<> static void push(FString value);
	
	static TMap<FString, TMap<FString, UProperty*>> propertyMap;
	static void InitClassMap();
	static UProperty* GetPropertyByName(FString classname, FString propertyname);
};

template<>
void UTableUtil::push(int value)
{
	lua_pushinteger(L, value);
}

template<>
void UTableUtil::push(FString value)
{
	lua_pushstring(L, TCHAR_TO_ANSI(*value));
}



template<typename T>
void UTableUtil::push(T value)
{

}