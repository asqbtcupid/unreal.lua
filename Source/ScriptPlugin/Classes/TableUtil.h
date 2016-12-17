#pragma once
extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TableUtil.generated.h"


// struct lua_State;
// class UBaseTinkerTable;
UCLASS()
class UTableUtil : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	 UTableUtil();
	
	// static lua_State* lua_state;
	UFUNCTION(BlueprintCallable, Category = "TableUtil")
	static int32 test();
};
