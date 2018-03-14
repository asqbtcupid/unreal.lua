#include "TableUtil.h"
#include "LuaScript.h"
#include "Paths.h"
#include "TextProperty.h"
#include "LuaFixLink.h"
#include "LuaMapHelper.h"
#include "../Launch/Resources/Version.h"
#include "BPAndLuaBridge.h"
#include "Engine/World.h"
#include "Engine/LevelScriptActor.h"

#if WITH_EDITOR
#include "Editor.h"
#endif
#include "GameDelegates.h"
#include "Platform.h"

static int32 LuaPanic(lua_State *L)
{
	LuaFixLink();
	UTableUtil::log(FString::Printf(TEXT("PANIC: unprotected error in call to Lua API(%s)"), ANSI_TO_TCHAR(lua_tostring(L, -1))));
	return 0;
}

FString GetLuaCodeFromPath(FString FilePath)
{
	FString FirstLine = FString("--[[") + FilePath + FString("]]");

	const static FString Prefix = "/Game/LuaSource/";
	int32 IndexStart = 0;
	FilePath.FindLastChar('.', IndexStart);
	if (IndexStart != INDEX_NONE)
	{
		FString FileName = FilePath.Mid(IndexStart + 1, FilePath.Len() - IndexStart);
		FilePath = FilePath.Replace(L".", L"/");
		FilePath = FilePath + "." + FileName;
	}
	else
	{
		FilePath = FilePath + FString(".") + FilePath;
	}
	FilePath = Prefix + FilePath;
	ULuaScript* CodeObject = LoadObject<ULuaScript>(nullptr, *FilePath);
	if (CodeObject) {
		return FirstLine + CodeObject->Code;
	}
	return "";
}

int CustomLuaLoader(lua_State* L)
{
	FString FilePath = lua_tostring(L, -1);
	FString Code = GetLuaCodeFromPath(FilePath);
	if (!Code.IsEmpty())
	{
		luaL_loadstring(L, TCHAR_TO_UTF8(*Code));
		return 1;
	}
	return 0;
}

int SetExistTable(lua_State* inL)
{
	lua_getfield(inL, LUA_REGISTRYINDEX, "_existuserdata");
	auto u = static_cast<void**>(lua_touserdata(inL, 1));
	lua_pushlightuserdata(inL, *u);
	lua_pushvalue(inL, 2);
	lua_rawset(inL, -3);
	return 0;
}

int32 indexFunc(lua_State* inL)
{
	lua_getmetatable(inL, 1);
	lua_pushvalue(inL, 2);
	lua_rawget(inL, -2);
	if (lua_isnil(inL, -1))
	{
		FString property = FString(lua_tostring(inL, 2));
		lua_pop(inL, 1);
		FString propertyKey = FString::Printf(TEXT("LuaGet_%s"), *property);
		UTableUtil::push(inL, propertyKey);
		lua_rawget(inL, -2);
		if (!lua_isnil(inL, -1))
		{
			lua_pushvalue(inL, 1);
			lua_call(inL, 1, 1);
		}
	}
	return 1;
}

int32 newindexFunc(lua_State* inL)
{
	lua_getmetatable(inL, 1);
	FString property = FString(lua_tostring(inL, 2));
	FString propertyKey = FString::Printf(TEXT("LuaSet_%s"), *property);
	UTableUtil::push(inL, propertyKey);
	lua_rawget(inL, -2);
	if (!lua_isnil(inL, -1))
	{
		lua_pushvalue(inL, 1);
		lua_pushvalue(inL, 3);
		lua_call(inL, 2, 0);
	}
	else
	{
		ensureAlwaysMsgf(0, L"Set value to not exist key: %s", *property);
	}
	return 0;
}

int32 cast(lua_State* inL)
{
	if (lua_isnil(inL, 2))
	{
		UTableUtil::log(FString("cast error, nil"));
		return 1;
	}
	lua_pushstring(inL, "classname");
	lua_rawget(inL, 1);
	luaL_getmetatable(inL, lua_tostring(inL, -1));
	lua_setmetatable(inL, 2);
	lua_pushvalue(inL, 2);
	return 1;
}

int32 uobjcet_gcfunc(lua_State *inL)
{
	auto u = (void**)lua_touserdata(inL, -1);
	UTableUtil::rmgcref(static_cast<UObject*>(*u));
	lua_getmetatable(inL, 1);
	lua_getfield(inL, -1, "Destroy");
	if (lua_isfunction(inL, -1))
	{
		lua_pushvalue(inL, 1);
		if (lua_pcall(inL, 1, 0, 0))
		{
			UTableUtil::log(FString(lua_tostring(inL, -1)));
		}
	}
#ifdef LuaDebug
	else
		lua_pop(inL, 1);
	lua_getfield(inL, -1, "classname");
	FString n = lua_tostring(inL, -1);
	UTableUtil::countforgc[n]--;
#endif
	return 0;
}
int32 struct_gcfunc(lua_State *inL)
{
	lua_getmetatable(inL, 1);
	lua_getfield(inL, -1, "Destroy");
	lua_pushvalue(inL, 1);
	lua_call(inL, 1, 0);
#ifdef LuaDebug
	lua_getmetatable(inL, 1);
	lua_getfield(inL, -1, "classname");
	FString n = lua_tostring(inL, -1);
	UTableUtil::countforgc[n]--;
#endif
	return 0;
}

int32 serialize_table(lua_State *inL)
{
	int top = lua_gettop(inL);
	if (top == 1)
	{
		lua_newtable(inL);
		lua_getmetatable(inL, 1);
		lua_pushnil(inL);
		while (lua_next(inL, -2))
		{
			FString Key = lua_tostring(inL, -2);
			if (Key.Contains("LuaGet_"))
			{
				lua_pushvalue(inL, 1);
				lua_call(inL, 1, 1);
				UTableUtil::push(inL, Key.Mid(7));
				lua_insert(inL, -2);
				lua_rawset(inL, -5);
				continue;
			}
			lua_pop(inL, 1);
		}
		lua_pop(inL, 1);
		return 1;
	}
	else
	{
		lua_getmetatable(inL, 1);
		lua_pushnil(inL);
		while (lua_next(inL, 2))
		{
			FString Key = "LuaSet_" + FString(lua_tostring(inL, -2));
			lua_getfield(inL, 3, TCHAR_TO_UTF8(*Key));
			lua_pushvalue(inL, 1);
			lua_pushvalue(inL, -3);
			lua_call(inL, 2, 0);
			lua_pop(inL, 1);
		}
		return 0;
	}
}

int32 EnsureNew(lua_State* inL)
{
	ensureAlwaysMsgf(0, L"This class not api class, so don't have new, make it become api class if you want one.");
	return 0;
}

int32 EnsureDestroy(lua_State* inL)
{
	UObject* Obj = (UObject*)touobject(inL, 1);
	if (Obj)
	{
		UTableUtil::rmgcref(Obj);
	}
	return 0;
}

int32 UObject_GetName(lua_State* inL)
{
	UObject* Obj = (UObject*)touobject(inL, 1);
	UTableUtil::push(inL, Obj->GetName());
	return 1;
}

int32 UObject_GetClass(lua_State* inL)
{
	UObject* Obj = (UObject*)touobject(inL, 1);
	pushuobject(inL, Obj->GetClass());
	return 1;
}

int32 UObject_GetOuter(lua_State* inL)
{
	UObject* Obj = (UObject*)touobject(inL, 1);
	pushuobject(inL, Obj->GetOuter());
	return 1;
}

int32 UObject_IsPendingKill(lua_State* inL)
{
	UObject* Obj = (UObject*)touobject(inL, 1);
	UTableUtil::push(inL, Obj->IsPendingKill());
	return 1;
}

int32 UObject_MarkPendingKill(lua_State* inL)
{
	UObject* Obj = (UObject*)touobject(inL, 1);
	Obj->MarkPendingKill();
	return 0;
}

int32 UObject_AddToRoot(lua_State* inL)
{
	UObject* Obj = (UObject*)touobject(inL, 1);
	Obj->AddToRoot();
	return 0;
}

int32 UObject_RemoveFromRoot(lua_State* inL)
{
	UObject* Obj = (UObject*)touobject(inL, 1);
	Obj->RemoveFromRoot();
	return 0;
}

int ErrHandleFunc(lua_State*L)
{
	lua_getfield(L, LUA_GLOBALSINDEX, "debug");
	lua_getfield(L, -1, "traceback");
	lua_pushthread(L);
	lua_pushvalue(L, 1);
	lua_pushinteger(L, 2);
	lua_call(L, 3, 1);
	lua_remove(L, -2);
	lua_concat(L, 2);
	lua_getfield(L, LUA_GLOBALSINDEX, "ErrHandleInLua");
	if (lua_isnil(L, -1))
	{
		lua_pop(L, 1);
	}
	else
	{
		lua_pushvalue(L, -2);
		lua_call(L, 1, 0);
#ifdef LuaDebug
		FString error = lua_tostring(L, -2);
		ensureAlwaysMsgf(0, *error);
#endif
	}
	return 1;
}


static int32 BpCall(lua_State* inL)
{
	void** p = (void**)lua_touserdata(inL, lua_upvalueindex(1));
	UFunction* Function = (UFunction*)(*p);
	UObject* Obj = (UObject*)touobject(inL, 1);
	uint8* Buffer = (uint8*)FMemory_Alloca(Function->ParmsSize);
	FScopedArguments scoped_arguments(Function, Buffer);

	int ArgIndex = 2;
	int ArgCount = lua_gettop(inL);

	// Iterate over input parameters
	TArray<UProperty*> PushBackParms;
	TArray<UProperty*> ReturnParms;
	TArray<int32> StackIndexs;
	for (TFieldIterator<UProperty> It(Function); It && (It->GetPropertyFlags() & (CPF_Parm)); ++It)
	{
		auto Prop = *It;

		if (Prop->GetPropertyFlags() & CPF_ReferenceParm)
		{
			PushBackParms.Add(Prop);
			StackIndexs.Add(ArgIndex);
		}
		else if ((Prop->GetPropertyFlags() & (CPF_OutParm | CPF_ReturnParm)))
		{
			ReturnParms.Insert(Prop, 0);
			continue;
		}

		if (ArgIndex <= ArgCount)
		{
			UTableUtil::popproperty(inL, ArgIndex, Prop, Buffer);
			++ArgIndex;
		}
		else if (Prop->GetPropertyFlags() & CPF_ReferenceParm)
		{
			ensureMsgf(0, L"you should pass reference, other wise there will be bug");
		}
	}
	Obj->ProcessEvent(Function, Buffer);
	for (UProperty* Prop : ReturnParms)
	{
		UTableUtil::push_ret_property(inL, Prop, Buffer);
	}
	for (int i = 0; i < PushBackParms.Num(); i++)
	{
		UProperty* Prop = PushBackParms[i];
		UTableUtil::pushback_ref_property(inL, StackIndexs[i], Prop, Buffer);
	}

	return PushBackParms.Num() + ReturnParms.Num();
}

#define BpPropertyGetterName(PropertyType) GetBpProp_##PropertyType
#define BpPropertySetterName(PropertyType) SetBpProp_##PropertyType

#define BpPropertyGetSet(PropertyType) static int32 BpPropertyGetterName(PropertyType)(lua_State* inL)\
					{\
						void** p = (void**)lua_touserdata(inL, lua_upvalueindex(1));\
						PropertyType* Property = (PropertyType*)(*p);\
						UObject* Obj = (UObject*)touobject(inL, 1);\
						UTableUtil::pushproperty_type(inL, Property, Obj);\
						return 1;\
					}\
					static int32 BpPropertySetterName(PropertyType)(lua_State* inL)\
					{\
						void** p = (void**)lua_touserdata(inL, lua_upvalueindex(1));\
						PropertyType* Property = (PropertyType*)(*p);\
						UObject* Obj = (UObject*)touobject(inL, 1);\
						UTableUtil::popproperty_type(inL, 2, Property, Obj);\
						return 0;\
					}

#define BpStructPropertyGetterName(PropertyType) BpStructGetProp_##PropertyType
#define BpStructPropertySetterName(PropertyType) BpStructSetProp_##PropertyType

#define BpStructPropertyGetSet(PropertyType) static int32 BpStructPropertyGetterName(PropertyType)(lua_State* inL)\
					{\
						void** p = (void**)lua_touserdata(inL, lua_upvalueindex(1));\
						PropertyType* Property = (PropertyType*)(*p);\
						UTableUtil::pushproperty_type(inL, Property, tostruct(inL, 1));\
						return 1;\
					}\
					static int32 BpStructPropertySetterName(PropertyType)(lua_State* inL)\
					{\
						void** p = (void**)lua_touserdata(inL, lua_upvalueindex(1));\
						PropertyType* Property = (PropertyType*)(*p);\
						UTableUtil::popproperty_type(inL, 2, Property, tostruct(inL, 1));\
						return 0;\
					}

#define BpSetterAndGetter(PropertyType) BpPropertyGetSet(PropertyType) \
										BpStructPropertyGetSet(PropertyType)

BpSetterAndGetter(UBoolProperty)
BpSetterAndGetter(UIntProperty)
BpSetterAndGetter(UUInt32Property)
BpSetterAndGetter(UInt64Property)
BpSetterAndGetter(UFloatProperty)
BpSetterAndGetter(UDoubleProperty)
BpSetterAndGetter(UObjectPropertyBase)
BpSetterAndGetter(UStrProperty)
BpSetterAndGetter(UNameProperty)
BpSetterAndGetter(UTextProperty)
BpSetterAndGetter(UByteProperty)
BpSetterAndGetter(UEnumProperty)
BpSetterAndGetter(UStructProperty)
BpSetterAndGetter(UMulticastDelegateProperty)
BpSetterAndGetter(UWeakObjectProperty)
BpSetterAndGetter(UArrayProperty)
BpSetterAndGetter(UMapProperty)
BpSetterAndGetter(USetProperty)
BpSetterAndGetter(UInterfaceProperty)

static int32 StructProperty_FirstMem_Getter(lua_State* inL)
{
	void** p = (void**)lua_touserdata(inL, lua_upvalueindex(1)); 
	UStructProperty* Property = (UStructProperty*)(*p);
	UObject* Obj = (UObject*)touobject(inL, 1); 
	UTableUtil::pushproperty_type_firstmem(inL, Property, Obj); 
	return 1; 
}

static int32 GetBpProp(lua_State* inL)
{
	void** p = (void**)lua_touserdata(inL, lua_upvalueindex(1));
	UProperty* Property = (UProperty*)(*p);
	UObject* Obj = (UObject*)touobject(inL, 1);
	UTableUtil::pushproperty(inL, Property, Obj);
	return 1;
}

static int32 SetBpProp(lua_State* inL)
{
	void** p = (void**)lua_touserdata(inL, lua_upvalueindex(1));
	UProperty* Property = (UProperty*)(*p);
	UObject* Obj = (UObject*)touobject(inL, 1);
	UTableUtil::popproperty(inL, 2, Property, Obj);
	return 0;
}

static int32 GetBpClass(lua_State* inL)
{
	void** p = (void**)lua_touserdata(inL, lua_upvalueindex(1));
	UClass* Class = (UClass*)(*p);
	pushuobject(inL, Class);
	return 1;
}

static int32 BpStructGetProp(lua_State* inL)
{
	void** p = (void**)lua_touserdata(inL, lua_upvalueindex(1));
	UProperty* Property = (UProperty*)(*p);
	UTableUtil::pushproperty(inL, Property, tostruct(inL, 1));
	return 1;
}

static int32 BpStructSetProp(lua_State* inL)
{
	void** p = (void**)lua_touserdata(inL, lua_upvalueindex(1));
	UProperty* Property = (UProperty*)(*p);
	UTableUtil::popproperty(inL, 2, Property, tostruct(inL, 1));
	return 0;
}

static int32 BpStructNew(lua_State* inL)
{
	void** p = (void**)lua_touserdata(inL, lua_upvalueindex(1));
	int Size = lua_tointeger(inL, lua_upvalueindex(2));
	const char* name = lua_tostring(inL, lua_upvalueindex(3));
	UScriptStruct* Struct = (UScriptStruct*)(*p);
	uint8* result = (uint8*)FMemory::Malloc(Size);
	Struct->InitializeStruct(result);
	pushstruct_gc(inL, name, result);
	return 1;
}

static int32 BpStructDestroy(lua_State* inL)
{
	void* ptr = tostruct(inL, 1);
	void** p = (void**)lua_touserdata(inL, lua_upvalueindex(1));
	UScriptStruct* Struct = (UScriptStruct*)(*p);

	Struct->DestroyStruct(ptr);
	FMemory::Free(ptr);
	return 0;
}

static int32 BpStructCopy(lua_State* inL)
{
	void** p = (void**)lua_touserdata(inL, lua_upvalueindex(1));
	int Size = lua_tointeger(inL, lua_upvalueindex(2));
	const char* name = lua_tostring(inL, lua_upvalueindex(3));
	UScriptStruct* Struct = (UScriptStruct*)(*p);
	uint8* result = (uint8*)FMemory::Malloc(Size);
	Struct->InitializeStruct(result);
	void* ptr = tostruct(inL, 1);
	Struct->CopyScriptStruct(result, ptr);
	pushstruct_gc(inL, name, result);
	return 1;
}



