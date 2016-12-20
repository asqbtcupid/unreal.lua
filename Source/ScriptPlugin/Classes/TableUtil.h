#pragma once
#include "lua_tinker.h"
#include <iostream>
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TableUtil.generated.h"

using namespace std;
using luafunc = int( struct lua_State* );

// struct lua_State;
// class UBaseTinkerTable;
UCLASS()
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
	
	UFUNCTION(BlueprintCallable, Category = "TableUtil")
	static void call(FString funcName);

	template<typename T>
	static void push(T value);

	static void push(const char* classname, void* p);

	template<> static void push(int value);
	template<> static void push(FString value);
	
	// static lua_State* lua_state;
	UFUNCTION(BlueprintCallable, Category = "TableUtil")
	static int32 test();

	UFUNCTION(BlueprintCallable, Category = "TableUtil")
	static float tick(float delta);
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