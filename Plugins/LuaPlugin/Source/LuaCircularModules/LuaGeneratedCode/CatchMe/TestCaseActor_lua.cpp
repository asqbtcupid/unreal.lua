#include "TableUtil.h"
#include "TestCaseActor.lua.h"
struct lua_static_load_TestCaseActor_struct
{
	static void load()
	{
		UTableUtil::loadlib(TestCaseActor_Lib, "ATestCaseActor");
	}
	lua_static_load_TestCaseActor_struct(){UTableUtil::GetInitDelegates().AddStatic(&load);}
};
static lua_static_load_TestCaseActor_struct lua_TestCaseActor_static_var;
void FixLinkFunc_CatchMe_TestCaseActor(){};
