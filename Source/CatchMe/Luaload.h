#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Luaload.generated.h"
struct lua_State;
UCLASS()
class ULuaLoad : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	 
	static void LoadAll(lua_State* L);
};
