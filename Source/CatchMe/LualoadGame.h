#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Lualoadgame.generated.h"
struct lua_State;
UCLASS()
class ULuaLoadGame : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public: 
	 
	static void LoadAll(lua_State* L);
};
  