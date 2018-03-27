
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
#include "NativeLuaFunc.h"

DEFINE_LOG_CATEGORY(LuaLog);
#define SetTableFunc(inL, index, FuncName, Func) lua_pushstring(inL, FuncName);\
											lua_pushcfunction(inL, Func);\
											lua_rawset(inL, index)

FLuaInitDelegates& UTableUtil::GetInitDelegates()
{
	static FLuaInitDelegates Delegates;
	return Delegates;
}


lua_State* UTableUtil::GetTheOnlyLuaState()
{
	if (TheOnlyLuaState == nullptr)
		init();
	return TheOnlyLuaState;
}

lua_State* UTableUtil::GetRunningState()
{
	if (RunningState == nullptr)
		RunningState = GetTheOnlyLuaState();
    return RunningState;
}

void UTableUtil::SetRunningState(Flua_State State, Flua_Index Index)
{
	RunningState = lua_tothread(State, Index);
}

TSet<FString> UTableUtil::NeedGcBpClassName;
LuaTickObject* UTableUtil::PtrTickObject = nullptr;
lua_State* UTableUtil::TheOnlyLuaState = nullptr;
lua_State* UTableUtil::RunningState = nullptr;
int32 UTableUtil::ManualInitCount = 0;
bool UTableUtil::HasManualInit = false;
TSet<FString> UTableUtil::SupportedNativeStruct;
FDelegateHandle LuaPreGarbageCollectDelegateHandle;
FDelegateHandle LuaOnWorldCleanUpDelegateHandle;
FDelegateHandle LuaOnEndPlayDelegateHandle;
#if WITH_EDITOR
FDelegateHandle LuaOnEditorBeginPlayDelegateHandle;
#endif
#ifdef LuaDebug
TMap<FString, int> UTableUtil::countforgc;
TMap<FString, UClass*> UTableUtil::bpname2bpclass;
#endif
TMap<FString, UUserDefinedStruct*> UTableUtil::bpname2bpstruct;

static void* LuaAlloc(void *Ud, void *Ptr, size_t OldSize, size_t NewSize)
{
	if (NewSize != 0)
	{
		return FMemory::Realloc(Ptr, NewSize);
	}
	else
	{
		FMemory::Free(Ptr);
		return NULL;
	}
}

void UTableUtil::useCustomLoader()
{
	lua_getglobal(TheOnlyLuaState, "package");
	lua_getglobal(TheOnlyLuaState, "table");
	lua_getfield(TheOnlyLuaState, -1, "insert");//table.insert
	lua_getfield(TheOnlyLuaState, -3, "loaders");//package.loaders  
	lua_pushinteger(TheOnlyLuaState, 2);
	lua_pushcfunction(TheOnlyLuaState, CustomLuaLoader);
	lua_call(TheOnlyLuaState, 3, 0);
	lua_pop(TheOnlyLuaState, lua_gettop(TheOnlyLuaState));
}

void UTableUtil::init(bool IsManual)
{
	if (TheOnlyLuaState != nullptr)
		return;
#ifdef LuaDebug
	bpname2bpclass.Reset();
#endif
	bpname2bpstruct.Reset();

	SupportedNativeStruct.Reset();
	auto l = lua_newstate(LuaAlloc, nullptr);
	if (l) lua_atpanic(l, &LuaPanic);
	luaL_openlibs(l);
	TheOnlyLuaState = l;
	FString gameDir = FPaths::ConvertRelativePathToFull(FPaths::GameDir());
	FString luaDir = FPaths::ConvertRelativePathToFull(FPaths::GameDir() / TEXT("LuaSource"));

#if USE_LUASOURCE || WITH_EDITOR
	FString mainFilePath = luaDir / TEXT("main.lua");
	if (luaL_dofile(l, TCHAR_TO_UTF8(*mainFilePath)))
	{
		UTableUtil::log(FString(lua_tostring(TheOnlyLuaState, -1)));
		checkf(0, L"Failed to Init Lua state %s");
	}
#else    
	useCustomLoader();
	FString MainCode = GetLuaCodeFromPath("main");
	checkf(!MainCode.IsEmpty(), L"MainCode Is Empty");
	if (luaL_dostring(TheOnlyLuaState, TCHAR_TO_UTF8(*MainCode)))
	{
		UTableUtil::log(FString(lua_tostring(TheOnlyLuaState, -1)));
		checkf(0, L"Failed to Init Lua state %s");
	}
#endif
	else
	{
		//set table for index exist userdata
		lua_newtable(TheOnlyLuaState);
		lua_newtable(TheOnlyLuaState);
		lua_pushstring(TheOnlyLuaState, "v");
		lua_setfield(TheOnlyLuaState, -2, "__mode");
		lua_setmetatable(TheOnlyLuaState, -2);
		lua_setfield(TheOnlyLuaState, LUA_REGISTRYINDEX, "_existuserdata");

		lua_newtable(TheOnlyLuaState);
		lua_setfield(TheOnlyLuaState, LUA_REGISTRYINDEX, "_existfirststruct");

		//when lua has correspond table of the ins, push the table
		lua_pushcfunction(TheOnlyLuaState, SetExistTable);
		lua_setglobal(TheOnlyLuaState, "_setexisttable");

		lua_newtable(TheOnlyLuaState);
		lua_setglobal(TheOnlyLuaState, "NeedGcBpClassName");

		push(TheOnlyLuaState, luaDir);
		lua_setfield(TheOnlyLuaState, LUA_GLOBALSINDEX, "_luadir");
		push(TheOnlyLuaState, gameDir);
		lua_setfield(TheOnlyLuaState, LUA_GLOBALSINDEX, "_gamedir");
#if PLATFORM_WINDOWS
		push(TheOnlyLuaState, "PLATFORM_WINDOWS");
		lua_setfield(TheOnlyLuaState, LUA_GLOBALSINDEX, "_platform");
#endif // PLATFORM_WINDOWS
#if  WITH_EDITOR
		push(TheOnlyLuaState, true);
		lua_setfield(TheOnlyLuaState, LUA_GLOBALSINDEX, "_WITH_EDITOR");
#endif

		GetInitDelegates().Broadcast();
		if (PtrTickObject == nullptr)
			PtrTickObject = new LuaTickObject();
		call("Init", IsManual);
#if ENGINE_MINOR_VERSION < 18
		LuaPreGarbageCollectDelegateHandle = FCoreUObjectDelegates::PreGarbageCollect.AddStatic(&UTableUtil::GC);
#else
		LuaPreGarbageCollectDelegateHandle = FCoreUObjectDelegates::GetPostGarbageCollect().AddStatic(&UTableUtil::GC);
#endif
		LuaOnWorldCleanUpDelegateHandle = FWorldDelegates::OnWorldCleanup.AddLambda
		(
			[](UWorld* World, bool bSessionEnded, bool bCleanupResources)
		{LuaStaticCall("OnWorldCleanup", World, bSessionEnded, bCleanupResources); }
		);
		LuaOnEndPlayDelegateHandle = FGameDelegates::Get().GetEndPlayMapDelegate().AddLambda
		(
			[&]()
		{
			FGameDelegates::Get().GetEndPlayMapDelegate().Remove(LuaOnEndPlayDelegateHandle);
			UTableUtil::shutdown();
		}
		);
#if WITH_EDITOR
		LuaOnEditorBeginPlayDelegateHandle = FEditorDelegates::PreBeginPIE.AddLambda
		(
			[&](const bool IsSim)
		{
			FEditorDelegates::PreBeginPIE.Remove(LuaOnEditorBeginPlayDelegateHandle);
			UTableUtil::shutdown();
		}
		);
#endif
	}
	// 	HasManualInit = IsManual;
}

void UTableUtil::GC()
{
	if (TheOnlyLuaState != nullptr)
	{
		call("GC");
	}
}

void UTableUtil::shutdown()
{
	if (TheOnlyLuaState != nullptr)
	{
		shutdown_internal();
	}
}


void UTableUtil::shutdown_internal()
{
	call("Shutdown");
	if (PtrTickObject != nullptr)
	{
		delete PtrTickObject;
		PtrTickObject = nullptr;
	}
#ifdef LuaDebug
	if (ue_lua_gettop(TheOnlyLuaState) > 10)
		ensureAlwaysMsgf(0, L"you push someting into lua stack but forget to pop.");
#endif
	lua_close(TheOnlyLuaState);
#ifdef LuaDebug
	for (auto& count : countforgc)
	{
		if (count.Value != 0)
		{
			checkf(0, L"gc error:%s %d", *count.Key, count.Value);
		}
	}
#endif
#if ENGINE_MINOR_VERSION < 18
	FCoreUObjectDelegates::PreGarbageCollect.Remove(LuaPreGarbageCollectDelegateHandle);
#else
	FCoreUObjectDelegates::GetPostGarbageCollect().Remove(LuaPreGarbageCollectDelegateHandle);
#endif
	FWorldDelegates::OnWorldCleanup.Remove(LuaOnWorldCleanUpDelegateHandle);
	TheOnlyLuaState = nullptr;
	RunningState = nullptr;
}

void UTableUtil::initmeta(bool bIsStruct, bool bNeedGc)
{
	SetTableFunc(TheOnlyLuaState, -3, "__index", indexFunc);
	SetTableFunc(TheOnlyLuaState, -3, "cast", cast);
	SetTableFunc(TheOnlyLuaState, -3, "__newindex", newindexFunc);

	if (bNeedGc)
	{
		lua_pushstring(TheOnlyLuaState, "__gc");
		if (!bIsStruct)
			lua_pushcfunction(TheOnlyLuaState, uobjcet_gcfunc);
		else
			lua_pushcfunction(TheOnlyLuaState, struct_gcfunc);
		lua_rawset(TheOnlyLuaState, -3);
	}
	if (!bIsStruct)
	{
		SetTableFunc(TheOnlyLuaState, -3, "New", EnsureNew);
		SetTableFunc(TheOnlyLuaState, -3, "NewObject", EnsureNew);
		SetTableFunc(TheOnlyLuaState, -3, "Destroy", EnsureDestroy);
		SetTableFunc(TheOnlyLuaState, -3, "GetClass", UObject_GetClass);
		SetTableFunc(TheOnlyLuaState, -3, "GetName", UObject_GetName);
		SetTableFunc(TheOnlyLuaState, -3, "GetOuter", UObject_GetOuter);
		SetTableFunc(TheOnlyLuaState, -3, "LuaGet_ClassPrivate", UObject_GetClass);
		SetTableFunc(TheOnlyLuaState, -3, "LuaGet_NamePrivate", UObject_GetName);
		SetTableFunc(TheOnlyLuaState, -3, "LuaGet_OuterPrivate", UObject_GetOuter);
		
		SetTableFunc(TheOnlyLuaState, -3, "IsPendingKill", UObject_IsPendingKill);
		SetTableFunc(TheOnlyLuaState, -3, "MarkPendingKill", UObject_MarkPendingKill);
		SetTableFunc(TheOnlyLuaState, -3, "AddToRoot", UObject_AddToRoot);
		SetTableFunc(TheOnlyLuaState, -3, "RemoveFromRoot", UObject_RemoveFromRoot);
	}
	lua_pushstring(TheOnlyLuaState, "__iscppclass");
	lua_pushboolean(TheOnlyLuaState, true);
	lua_rawset(TheOnlyLuaState, -3);

	SetTableFunc(TheOnlyLuaState, -3, "Table", serialize_table);
}

void UTableUtil::addmodule(const char* name, bool bIsStruct, bool bNeedGc)
{
	lua_getglobal(TheOnlyLuaState, name);
	if (lua_istable(TheOnlyLuaState, -1))
	{
		lua_pop(TheOnlyLuaState, 1);
		return;
	}
	lua_pushvalue(TheOnlyLuaState, LUA_GLOBALSINDEX);
	lua_pushstring(TheOnlyLuaState, name);
	luaL_newmetatable(TheOnlyLuaState, name);
	initmeta(bIsStruct, bNeedGc);
	lua_pushstring(TheOnlyLuaState, "classname");
	lua_pushstring(TheOnlyLuaState, name);
	lua_rawset(TheOnlyLuaState, -3);
	lua_rawset(TheOnlyLuaState, -3);
	lua_pop(TheOnlyLuaState, 2);
}

void UTableUtil::openmodule(const char* name)
{
	lua_pushvalue(TheOnlyLuaState, LUA_GLOBALSINDEX);
	lua_pushstring(TheOnlyLuaState, name);
	lua_rawget(TheOnlyLuaState, -2);
}


void UTableUtil::addfunc(const char* name, luafunc f)
{
	lua_pushstring(TheOnlyLuaState, name);
	lua_pushcfunction(TheOnlyLuaState, f);
	lua_rawset(TheOnlyLuaState, -3);
}

void UTableUtil::closemodule()
{
	lua_pop(TheOnlyLuaState, 2);
}

void* tovoid(lua_State* L, int i)
{
	if (lua_istable(L, i))
	{
		if (i < 0)
			i = lua_gettop(L) + i + 1;
		lua_pushstring(L, "_cppinstance_");
		lua_rawget(L, i);
		if (lua_isnil(L, -1))
		{
			lua_pop(L, 1);
			return nullptr;
		}
		else
		{
			auto u = static_cast<void**>(lua_touserdata(L, -1));
			lua_pop(L, 1);
			return *u;
		}
	}
	else if (lua_isuserdata(L, i))
	{
		auto u = static_cast<void**>(lua_touserdata(L, i));
		return *u;
	}
	else
		return nullptr;
}

void* touobject(lua_State* L, int i)
{
	if (lua_isnil(L, i))
		return nullptr;
	return tovoid(L, i);
}

void* tostruct(lua_State* L, int i)
{
#ifdef LuaDebug
	if (lua_isnil(L, i))
	{
		ensureAlwaysMsgf(0, L"struct can't be nil");
		return nullptr;
	}
#endif // LuaDebug
	auto u = static_cast<void**>(lua_touserdata(L, i));
	return *u;
}

void PrintLuaStack(lua_State*L /*= nullptr*/)
{
	if (L == nullptr)
		L = UTableUtil::GetTheOnlyLuaState();
	lua_getfield(L, LUA_GLOBALSINDEX, "debug");
	lua_getfield(L, -1, "traceback");
	lua_pushthread(L);
	lua_call(L, 1, 1);
	FString stackstr = lua_tostring(L, -1);
	UE_LOG(LogOutputDevice, Warning, TEXT("%s"), *stackstr);
	lua_pop(L, 2);
}

void UTableUtil::setmeta(lua_State *inL, const char* classname, int index, bool bIsStruct, bool bNeedGc)
{
	luaL_getmetatable(inL, classname);
	if (lua_istable(inL, -1))
	{
		lua_setmetatable(inL, index - 1);
	}
	else
	{
		lua_pop(inL, 1);
		UTableUtil::addmodule(classname, bIsStruct, bNeedGc);
		luaL_getmetatable(inL, classname);
		lua_setmetatable(inL, index - 1);
	}
}

void UTableUtil::set_uobject_meta(lua_State *inL, UObject* Obj, int index)
{
	UClass* Class = Obj->GetClass();

	if (Class->HasAnyClassFlags(CLASS_CompiledFromBlueprint))
	{
// remove "_C"
		auto temp1 = FTCHARToUTF8((const TCHAR*)*Class->GetName().LeftChop(2));
		const char* classname = (ANSICHAR*)temp1.Get();
		lua_getglobal(inL, classname);
		if (lua_isnil(inL, -1))
		{
			UClass* NativeClass = Class;

			while (!NativeClass->HasAnyClassFlags(CLASS_Native))
				NativeClass = NativeClass->GetSuperClass();
			
			FString NativeClassNameStr = FString::Printf(TEXT("%s%s"), NativeClass->GetPrefixCPP(), *NativeClass->GetName());
			auto temp2 = FTCHARToUTF8((const TCHAR*)*NativeClassNameStr);
			const char* nativeclassname = (ANSICHAR*)temp2.Get();

			lua_pop(inL, 1);
			UClass* BpClass = Class;

			while (BpClass && BpClass->HasAnyClassFlags(CLASS_CompiledFromBlueprint))
			{
				FString NewClassNameStr = BpClass->GetName().LeftChop(2);
				auto temp3 = FTCHARToUTF8((const TCHAR*)*NewClassNameStr);
				const char* newclassname = (ANSICHAR*)temp3.Get();

				lua_newtable(inL);
				lua_getglobal(inL, nativeclassname);
				if (lua_isnil(inL, -1))
				{
					lua_pop(inL, 1);
					addmodule(nativeclassname, false, true);
					lua_getglobal(inL, nativeclassname);
				}

				lua_pushnil(inL);
				while (lua_next(inL, -2) != 0)
				{
					lua_pushvalue(inL, -2);
					lua_pushvalue(inL, -2);
					lua_rawset(inL, -6);
					lua_pop(inL, 1);
				}
				lua_pop(inL, 1);

				lua_pushstring(inL, "classname");
				lua_pushstring(inL, newclassname);
				lua_rawset(inL, -3);

				lua_pushstring(inL, "Class");
				pushuobject(inL, BpClass);
				lua_pushcclosure(inL, GetBpClass, 1);
				lua_rawset(inL, -3);

				if (NativeClass == ALevelScriptActor::StaticClass())
				{
					lua_getglobal(inL, "NeedGcBpClassName");
					lua_pushstring(inL, newclassname);
					lua_pushboolean(inL, true);
					lua_rawset(inL, -3);
					lua_pop(inL, 1);
				}
				TSet<FString> HasAddFuncName;
				for (TFieldIterator<UFunction> FuncIt(BpClass); FuncIt; ++FuncIt)
				{
					UFunction* Function = *FuncIt;
					if (!Function->HasAnyFunctionFlags(FUNC_Native))
					{
						FString FuncName = Function->GetName();
						if (!HasAddFuncName.Contains(FuncName) || Function->GetOwnerClass() == BpClass)
						{
							HasAddFuncName.Add(FuncName);
							push(inL, Function->GetName());
							pushuobject(inL, Function);
							lua_pushcclosure(inL, BpCall, 1);
							lua_rawset(inL, -3);
						}
					}
				}

				for (TFieldIterator<UProperty> PropertyIt(BpClass); PropertyIt; ++PropertyIt)
				{
					UProperty* Property = *PropertyIt;
					if (UClass* OuterClass = Cast<UClass>(Property->GetOuter()))
					{
						if (OuterClass->HasAnyClassFlags(CLASS_CompiledFromBlueprint))
						{
							push(inL, "LuaGet_" + Property->GetName());
							pushuobject(inL, Property);
							if(Property->IsA(UBoolProperty::StaticClass()))
								lua_pushcclosure(inL, BpPropertyGetterName(UBoolProperty), 1);
							else if(Property->IsA(UIntProperty::StaticClass()))
								lua_pushcclosure(inL, BpPropertyGetterName(UIntProperty), 1);
							else if(Property->IsA(UUInt32Property::StaticClass()))
								lua_pushcclosure(inL, BpPropertyGetterName(UUInt32Property), 1);
							else if(Property->IsA(UInt64Property::StaticClass()))
								lua_pushcclosure(inL, BpPropertyGetterName(UInt64Property), 1);
							else if(Property->IsA(UFloatProperty::StaticClass()))
								lua_pushcclosure(inL, BpPropertyGetterName(UFloatProperty), 1);
							else if(Property->IsA(UDoubleProperty::StaticClass()))
								lua_pushcclosure(inL, BpPropertyGetterName(UDoubleProperty), 1);
							else if(Property->IsA(UObjectPropertyBase::StaticClass()))
								lua_pushcclosure(inL, BpPropertyGetterName(UObjectPropertyBase), 1);
							else if(Property->IsA(UStrProperty::StaticClass()))
								lua_pushcclosure(inL, BpPropertyGetterName(UStrProperty), 1);
							else if(Property->IsA(UNameProperty::StaticClass()))
								lua_pushcclosure(inL, BpPropertyGetterName(UNameProperty), 1);
							else if(Property->IsA(UTextProperty::StaticClass()))
								lua_pushcclosure(inL, BpPropertyGetterName(UTextProperty), 1);
							else if(Property->IsA(UByteProperty::StaticClass()))
								lua_pushcclosure(inL, BpPropertyGetterName(UByteProperty), 1);
							else if(Property->IsA(UEnumProperty::StaticClass()))
								lua_pushcclosure(inL, BpPropertyGetterName(UEnumProperty), 1);
							else if(Property->IsA(UStructProperty::StaticClass()))
								lua_pushcclosure(inL, BpPropertyGetterName(UStructProperty), 1);
							else if(Property->IsA(UMulticastDelegateProperty::StaticClass()))
								lua_pushcclosure(inL, BpPropertyGetterName(UMulticastDelegateProperty), 1);
							else if(Property->IsA(UWeakObjectProperty::StaticClass()))
								lua_pushcclosure(inL, BpPropertyGetterName(UWeakObjectProperty), 1);
							else if(Property->IsA(UArrayProperty::StaticClass()))
								lua_pushcclosure(inL, BpPropertyGetterName(UArrayProperty), 1);
							else if(Property->IsA(UMapProperty::StaticClass()))
								lua_pushcclosure(inL, BpPropertyGetterName(UMapProperty), 1);
							else if(Property->IsA(USetProperty::StaticClass()))
								lua_pushcclosure(inL, BpPropertyGetterName(USetProperty), 1);
							else if(Property->IsA(UInterfaceProperty::StaticClass()))
								lua_pushcclosure(inL, BpPropertyGetterName(UInterfaceProperty), 1);
							else
							{
								ensureAlwaysMsgf(0, L"");
								lua_pushcclosure(inL, GetBpProp, 1);
							}

							lua_rawset(inL, -3);
							push(inL, "LuaSet_" + Property->GetName());
							pushuobject(inL, Property);

							if(Property->IsA(UBoolProperty::StaticClass()))
								lua_pushcclosure(inL, BpPropertySetterName(UBoolProperty), 1);
							else if(Property->IsA(UIntProperty::StaticClass()))
								lua_pushcclosure(inL, BpPropertySetterName(UIntProperty), 1);
							else if(Property->IsA(UUInt32Property::StaticClass()))
								lua_pushcclosure(inL, BpPropertySetterName(UUInt32Property), 1);
							else if(Property->IsA(UInt64Property::StaticClass()))
								lua_pushcclosure(inL, BpPropertySetterName(UInt64Property), 1);
							else if(Property->IsA(UFloatProperty::StaticClass()))
								lua_pushcclosure(inL, BpPropertySetterName(UFloatProperty), 1);
							else if(Property->IsA(UDoubleProperty::StaticClass()))
								lua_pushcclosure(inL, BpPropertySetterName(UDoubleProperty), 1);
							else if(Property->IsA(UObjectPropertyBase::StaticClass()))
								lua_pushcclosure(inL, BpPropertySetterName(UObjectPropertyBase), 1);
							else if(Property->IsA(UStrProperty::StaticClass()))
								lua_pushcclosure(inL, BpPropertySetterName(UStrProperty), 1);
							else if(Property->IsA(UNameProperty::StaticClass()))
								lua_pushcclosure(inL, BpPropertySetterName(UNameProperty), 1);
							else if(Property->IsA(UTextProperty::StaticClass()))
								lua_pushcclosure(inL, BpPropertySetterName(UTextProperty), 1);
							else if(Property->IsA(UByteProperty::StaticClass()))
								lua_pushcclosure(inL, BpPropertySetterName(UByteProperty), 1);
							else if(Property->IsA(UEnumProperty::StaticClass()))
								lua_pushcclosure(inL, BpPropertySetterName(UEnumProperty), 1);
							else if(Property->IsA(UStructProperty::StaticClass()))
								lua_pushcclosure(inL, BpPropertySetterName(UStructProperty), 1);
							else if(Property->IsA(UMulticastDelegateProperty::StaticClass()))
								lua_pushcclosure(inL, BpPropertySetterName(UMulticastDelegateProperty), 1);
							else if(Property->IsA(UWeakObjectProperty::StaticClass()))
								lua_pushcclosure(inL, BpPropertySetterName(UWeakObjectProperty), 1);
							else if(Property->IsA(UArrayProperty::StaticClass()))
								lua_pushcclosure(inL, BpPropertySetterName(UArrayProperty), 1);
							else if(Property->IsA(UMapProperty::StaticClass()))
								lua_pushcclosure(inL, BpPropertySetterName(UMapProperty), 1);
							else if(Property->IsA(USetProperty::StaticClass()))
								lua_pushcclosure(inL, BpPropertySetterName(USetProperty), 1);
							else if(Property->IsA(UInterfaceProperty::StaticClass()))
								lua_pushcclosure(inL, BpPropertySetterName(UInterfaceProperty), 1);
							else
							{
								ensureAlwaysMsgf(0, L"");
								lua_pushcclosure(inL, SetBpProp, 1);
							}
							lua_rawset(inL, -3);
						}

					}
				}
				lua_setglobal(inL, newclassname);
#ifdef LuaDebug
				bpname2bpclass.Add(FString(newclassname), BpClass);
#endif
				BpClass = BpClass->GetSuperClass();
			}
			lua_getglobal(inL, classname);
			lua_setmetatable(inL, index - 1);
		}
		else
			lua_setmetatable(inL, index - 1);
#ifdef LuaDebug
		if (bpname2bpclass[FString(classname)] != Class)
			ensureAlwaysMsgf(0, L"diffrent BlueprintClass have same name, will cause bug %s", ANSI_TO_TCHAR(classname));

		if (UTableUtil::countforgc.Contains(classname))
			UTableUtil::countforgc[classname]++;
		else
			UTableUtil::countforgc.Add(classname, 1);
#endif
	}
	else
	{
		UClass* NativeClass = Class;

		while (!NativeClass->HasAnyClassFlags(CLASS_Native))
			NativeClass = NativeClass->GetSuperClass();

		FString NativeClassStr = FString::Printf(TEXT("%s%s"), NativeClass->GetPrefixCPP(), *NativeClass->GetName());
		auto temp4 = FTCHARToUTF8((const TCHAR*)*NativeClassStr);
		const char* nativeclassname = (ANSICHAR*)temp4.Get();

		setmeta(inL, nativeclassname, index);
#ifdef LuaDebug
		if (UTableUtil::countforgc.Contains(nativeclassname))
			UTableUtil::countforgc[nativeclassname]++;
		else
			UTableUtil::countforgc.Add(nativeclassname, 1);
#endif
	}
}

void UTableUtil::call(lua_State* inL, int funcid, UFunction* funcsig, void* ptr)
{
	lua_rawgeti(inL, LUA_REGISTRYINDEX, funcid);
	checkf(lua_isfunction(inL, -1), L"");
	int32 ParamCount = 0;
	for (TFieldIterator<UProperty> ParamIt(funcsig); ParamIt; ++ParamIt)
	{
		UProperty* Param = *ParamIt;
		if (Param->GetName() != "ReturnValue")
		{
			pushproperty(inL, Param, ptr);
			++ParamCount;
		}
	}
	if (lua_pcall(inL, ParamCount, 0, 0))
	{
#ifdef LuaDebug
		FString error = lua_tostring(inL, -1);
		ensureAlwaysMsgf(0, *error);
#endif
		log(lua_tostring(inL, -1));
	}
}

void UTableUtil::pushcontainer(lua_State* inL, void *Obj, UArrayProperty* Property)
{
	push(inL, ULuaArrayHelper::GetHelperCPP(Obj, Property));
}

void UTableUtil::pushcontainer(lua_State* inL, void *Obj, UMapProperty* Property)
{
	push(inL, ULuaMapHelper::GetHelperCPP(Obj, Property));
}

void UTableUtil::pushcontainer(lua_State* inL, void *Obj, USetProperty* Property)
{
	push(inL, ULuaSetHelper::GetHelperCPP(Obj, Property));
}


void UTableUtil::pushproperty(lua_State* inL, UProperty* property, const void* ptr)
{
	if (property == nullptr)
	{
		lua_pushnil(inL);
	}
	else if (UIntProperty* p = Cast<UIntProperty>(property))
	{
		pushproperty_type(inL, p, ptr);
	}
	else if (UUInt32Property* p = Cast<UUInt32Property>(property))
	{
		pushproperty_type(inL, p, ptr);
	}
	else if (UInt64Property* p = Cast<UInt64Property>(property))
	{
		pushproperty_type(inL, p, ptr);
	}
	else if (UFloatProperty* p = Cast<UFloatProperty>(property))
	{
		pushproperty_type(inL, p, ptr);
	}
	else if (UDoubleProperty* p = Cast<UDoubleProperty>(property))
	{
		pushproperty_type(inL, p, ptr);
	}
	else if (UBoolProperty * p = Cast<UBoolProperty>(property))
	{
		pushproperty_type(inL, p, ptr);
	}
	else if (UByteProperty* p = Cast<UByteProperty>(property))
	{
		pushproperty_type(inL, p, ptr);
	}
	else if (UEnumProperty* p = Cast<UEnumProperty>(property))
	{
		pushproperty_type(inL, p, ptr);
	}

	else if (UStrProperty* p = Cast<UStrProperty>(property))
	{
		pushproperty_type(inL, p, ptr);
	}
	else if (UNameProperty* p = Cast<UNameProperty>(property))
	{
		pushproperty_type(inL, p, ptr);
	}
	else if (UTextProperty* p = Cast<UTextProperty>(property))
	{
		pushproperty_type(inL, p, ptr);
	}
	else if (UStructProperty* p = Cast<UStructProperty>(property))
	{
		pushproperty_type(inL, p, ptr);
	}
	else if (UArrayProperty* p = Cast<UArrayProperty>(property))
	{
		pushproperty_type(inL, p, ptr);
	}
	else if (UMapProperty* p = Cast<UMapProperty>(property))
	{
		pushproperty_type(inL, p, ptr);
	}
	else if (USetProperty* p = Cast<USetProperty>(property))
	{
		pushproperty_type(inL, p, ptr);
	}
	else if (UObjectPropertyBase* p = Cast<UObjectPropertyBase>(property))
	{
		pushproperty_type(inL, p, ptr);
	}
	else if (UInterfaceProperty* p = Cast<UInterfaceProperty>(property))
	{
		pushproperty_type(inL, p, ptr);
	}
	else
	{
		ensureAlwaysMsgf(0, L"Some type didn't process");
	}
}

uint8* GetBpStructTempIns(const FString& StructTypeName, UStructProperty* Property)
{
	const int32 size = 10;
	struct InsConfig
	{
		InsConfig(UStructProperty* Property) :index(-1), StructSize(Property->GetSize())
		{
			for (int i = 0; i < size; i++)
			{
				p[i] = (uint8*)FMemory::Malloc(StructSize);
			}
		}
		int32 StructSize;
		int32 index;
		uint8* p[size];
		uint8* GetTempIns()
		{
			index = (index + 1) % size;
			return p[index];
		}
	};
	static TMap<FString, InsConfig> AllStructMemPool;
	InsConfig* ConfigPtr = AllStructMemPool.Find(StructTypeName);
	if (!ConfigPtr)
	{
		InsConfig NewConfig(Property);
		AllStructMemPool.Add(StructTypeName, NewConfig);
		ConfigPtr = AllStructMemPool.Find(StructTypeName);
	}
	return ConfigPtr->GetTempIns();
}

void UTableUtil::push_ret_property(lua_State*inL, UProperty* property, const void* ptr)
{
	if (UStructProperty* p = Cast<UStructProperty>(property))
	{
		UScriptStruct* Struct = p->Struct;
		FString TypeName;
		if (UUserDefinedStruct* BpStruct = Cast<UUserDefinedStruct>(p->Struct))
		{
			MayAddNewStructType(BpStruct);
			TypeName = BpStruct->GetName();
		}
		else
			TypeName = p->Struct->GetStructCPPName();
		uint8* result = (uint8*)FMemory::Malloc(p->GetSize());
		// 		uint8* result = GetBpStructTempIns(CppTypeName, p);
		ptr = (void*)p->ContainerPtrToValuePtr<uint8>(ptr);
		p->CopyCompleteValueFromScriptVM(result, ptr);
		pushstruct_gc(inL, TCHAR_TO_UTF8(*TypeName), result);
		// 		pushstruct_nogc(inL, TCHAR_TO_UTF8(*CppTypeName), result);
	}
	else if (UArrayProperty* p = Cast<UArrayProperty>(property))
	{
		FScriptArrayHelper_InContainer result(p, ptr);
		lua_newtable(inL);
		for (int32 i = 0; i < result.Num(); ++i)
		{
			lua_pushinteger(inL, i + 1);
			UTableUtil::push_ret_property(inL, p->Inner, result.GetRawPtr(i));
			lua_rawset(inL, -3);
		}
	}
	else if (UMapProperty* p = Cast<UMapProperty>(property))
	{
		FScriptMapHelper_InContainer result(p, ptr);
		lua_newtable(inL);
		for (int32 i = 0; i < result.Num(); ++i)
		{
			uint8* PairPtr = result.GetPairPtr(i);
			push_ret_property(inL, p->KeyProp, PairPtr + p->MapLayout.KeyOffset);
			push_ret_property(inL, p->ValueProp, PairPtr);
			lua_rawset(inL, -3);
		}
	}
	else if (USetProperty* p = Cast<USetProperty>(property))
	{
		FScriptSetHelper_InContainer result(p, ptr);
		lua_newtable(inL);
		for (int32 i = 0; i < result.Num(); ++i)
		{
			push_ret_property(inL, p->ElementProp, result.GetElementPtr(i));
			lua_pushboolean(inL, true);
			lua_rawset(inL, -3);
		}
	}
	else
		pushproperty(inL, property, ptr);
}

void UTableUtil::pushback_ref_property(lua_State*inL, int32 LuaStackIndex, UProperty* property, const void* ptr)
{
	if (UStructProperty* p = Cast<UStructProperty>(property))
	{
		void* DestPtr = tovoid(inL, LuaStackIndex);
		p->CopyCompleteValueFromScriptVM(DestPtr, ptr);
		lua_pushvalue(inL, LuaStackIndex);
	}
	else if (UArrayProperty *p = Cast<UArrayProperty>(property))
	{
		if (ULuaArrayHelper* ArrHelper = (ULuaArrayHelper*)touobject(inL, LuaStackIndex))
		{
			ArrHelper->CopyFrom(p, p->ContainerPtrToValuePtr<void>((void*)(ptr)));
			lua_pushvalue(inL, LuaStackIndex);
		}
		else if (lua_istable(inL, LuaStackIndex))
		{
			FScriptArrayHelper_InContainer Arr(p, ptr);
			lua_pushvalue(inL, LuaStackIndex);
			int table_len = lua_objlen(inL, -1);
			for (int i = 1; i <= FMath::Max(table_len, Arr.Num()); i++)
			{
				lua_pushinteger(inL, i);
				if (i <= Arr.Num())
					push_ret_property(inL, p->Inner, Arr.GetRawPtr(i - 1));
				else
					lua_pushnil(inL);
				lua_rawset(inL, -3);
			}
		}
		else
		{
			ensureAlwaysMsgf(0, L"not arr");
		}
	}
	else if (UMapProperty *p = Cast<UMapProperty>(property))
	{
		if (ULuaMapHelper* Helper = (ULuaMapHelper*)touobject(inL, LuaStackIndex))
		{
			Helper->CopyFrom(p, p->ContainerPtrToValuePtr<void>((void*)(ptr)));
			lua_pushvalue(inL, LuaStackIndex);
		}
		else if (lua_istable(inL, LuaStackIndex))
		{
			FScriptMapHelper_InContainer result(p, ptr);
			UProperty* CurrKeyProp = p->KeyProp;
			const int32 KeyPropertySize = CurrKeyProp->ElementSize * CurrKeyProp->ArrayDim;
			void* KeyStorageSpace = FMemory_Alloca(KeyPropertySize);
			CurrKeyProp->InitializeValue(KeyStorageSpace);

			lua_newtable(inL);
			lua_pushvalue(inL, LuaStackIndex);
			lua_pushnil(inL);
			int i = 1;
			while (lua_next(inL, -2) != 0)
			{
				lua_pop(inL, 1);
				popproperty(inL, -1, CurrKeyProp, KeyStorageSpace);
				uint8* Result = result.FindValueFromHash(KeyStorageSpace);
				if (Result == nullptr)
				{
					lua_pushvalue(inL, -1);
					lua_rawseti(inL, -4, i);
					i++;
				}
			}
			CurrKeyProp->DestroyValue(KeyStorageSpace);

			lua_pushnil(inL);
			while (lua_next(inL, -3) != 0)
			{
				lua_pushnil(inL);
				lua_rawset(inL, -4);
			}
			lua_remove(inL, -2);
			for (int32 i = 0; i < result.Num(); ++i)
			{
				uint8* PairPtr = result.GetPairPtr(i);
				push_ret_property(inL, p->KeyProp, PairPtr + p->MapLayout.KeyOffset);
				push_ret_property(inL, p->ValueProp, PairPtr);
				lua_rawset(inL, -3);
			}
		}
		else
		{
			ensureAlwaysMsgf(0, L"not map");
		}
	}
	else if (USetProperty *p = Cast<USetProperty>(property))
	{
		if (ULuaSetHelper* Helper = (ULuaSetHelper*)touobject(inL, LuaStackIndex))
		{
			Helper->CopyFrom(p, p->ContainerPtrToValuePtr<void>((void*)(ptr)));
			lua_pushvalue(inL, LuaStackIndex);
		}
		else if (lua_istable(inL, LuaStackIndex))
		{
			lua_newtable(inL);
			lua_pushvalue(inL, LuaStackIndex);
			lua_pushnil(inL);
			int i = 1;
			FScriptSetHelper_InContainer result(p, ptr);
			UProperty* CurrKeyProp = p->ElementProp;
			const int32 KeyPropertySize = CurrKeyProp->ElementSize * CurrKeyProp->ArrayDim;
			void* KeyStorageSpace = FMemory_Alloca(KeyPropertySize);
			CurrKeyProp->InitializeValue(KeyStorageSpace);
#if ENGINE_MINOR_VERSION < 18
			uint8* keyptr = result.FindElementFromHash(KeyStorageSpace);
#else
			uint8* keyptr = nullptr;
			int32 Index = result.FindElementIndexFromHash(KeyStorageSpace);
			if (Index != INDEX_NONE)
			{
				keyptr = result.GetElementPtr(Index);
			}
#endif
			while (lua_next(inL, -2) != 0)
			{
				lua_pop(inL, 1);
				popproperty(inL, -1, CurrKeyProp, KeyStorageSpace);
#if ENGINE_MINOR_VERSION < 18
				uint8* keyptr = result.FindElementFromHash(KeyStorageSpace);
#else
				uint8* keyptr = nullptr;
				int32 Index = result.FindElementIndexFromHash(KeyStorageSpace);
				if (Index != INDEX_NONE)
				{
					keyptr = result.GetElementPtr(Index);
				}
#endif
				if (keyptr == nullptr)
				{
					lua_pushvalue(inL, -1);
					lua_rawseti(inL, -4, i);
					i++;
				}
			}
			lua_pushnil(inL);
			while (lua_next(inL, -3) != 0)
			{
				lua_pushnil(inL);
				lua_rawset(inL, -4);
			}
			lua_remove(inL, -2);

			for (int32 i = 0; i < result.Num(); ++i)
			{
				push_ret_property(inL, p->ElementProp, result.GetElementPtr(i));
				lua_pushboolean(inL, true);
				lua_rawset(inL, -3);
			}
		}
		else
		{
			ensureAlwaysMsgf(0, L"not set");
		}
	}
	else
	{
		pushproperty(inL, property, ptr);
	}
}

void UTableUtil::pushproperty_type(lua_State*inL, UBoolProperty* p, const void*ptr)
{
	lua_pushboolean(inL, (bool)p->GetPropertyValue_InContainer(ptr));
}

void UTableUtil::pushproperty_type(lua_State*inL, UIntProperty* p, const void*ptr)
{
	lua_pushinteger(inL, (int32)p->GetPropertyValue_InContainer(ptr));
}

void UTableUtil::pushproperty_type(lua_State*inL, UInt64Property* p, const void*ptr)
{
	lua_pushnumber(inL, (int64)p->GetPropertyValue_InContainer(ptr));
}

void UTableUtil::pushproperty_type(lua_State*inL, UFloatProperty* p, const void*ptr)
{
	lua_pushnumber(inL, (float)p->GetPropertyValue_InContainer(ptr));
}

void UTableUtil::pushproperty_type(lua_State*inL, UDoubleProperty* p, const void*ptr)
{
	lua_pushnumber(inL, (double)p->GetPropertyValue_InContainer(ptr));
}

void UTableUtil::pushproperty_type(lua_State*inL, UObjectPropertyBase* p, const void*ptr)
{
	pushuobject(inL, p->GetObjectPropertyValue_InContainer(ptr));
}

void UTableUtil::pushproperty_type(lua_State*inL, UStrProperty* p, const void*ptr)
{
	lua_pushstring(inL, TCHAR_TO_UTF8(*(p->GetPropertyValue_InContainer(ptr))));
}

void UTableUtil::pushproperty_type(lua_State*inL, UNameProperty* p, const void*ptr)
{
	FName result = (FName)p->GetPropertyValue_InContainer(ptr);
	lua_pushstring(inL, TCHAR_TO_UTF8(*result.ToString()));
}

void UTableUtil::pushproperty_type(lua_State*inL, UTextProperty* p, const void*ptr)
{
	FText result = (FText)p->GetPropertyValue_InContainer(ptr);
	lua_pushstring(inL, TCHAR_TO_UTF8(*result.ToString()));
}

void UTableUtil::pushproperty_type(lua_State*inL, UByteProperty* p, const void*ptr)
{
	lua_pushinteger(inL, (int)p->GetPropertyValue_InContainer(ptr));
}

void UTableUtil::pushproperty_type(lua_State*inL, UEnumProperty* p, const void*ptr)
{
	pushproperty(inL, p->GetUnderlyingProperty(), p->ContainerPtrToValuePtr<void>(ptr));
}

void UTableUtil::pushproperty_type(lua_State*inL, UWeakObjectProperty* p, const void*ptr)
{
	pushuobject(inL, p->GetObjectPropertyValue_InContainer(ptr));
}

void UTableUtil::pushproperty_type(lua_State*inL, UUInt32Property* p, const void*ptr)
{
	lua_pushinteger(inL, (int32)p->GetPropertyValue_InContainer(ptr));
}

void UTableUtil::pushproperty_type(lua_State*inL, UMulticastDelegateProperty* p, const void*ptr)
{
	UFunction* FunSig = p->SignatureFunction;
	void* result = (void*)p->ContainerPtrToValuePtr<uint8>(ptr);
	auto delegateproxy = NewObject<ULuaDelegate>();
	delegateproxy->Init(result, FunSig);
	pushuobject(inL, (void*)delegateproxy, true);
}

void UTableUtil::pushproperty_type(lua_State*inL, UInterfaceProperty* p, const void*ptr)
{
	FScriptInterface* result = (FScriptInterface*)p->GetPropertyValuePtr_InContainer(ptr);
	pushuobject(inL, (void*)result->GetObject());
}

void UTableUtil::pushproperty_valueptr(lua_State*inL, UProperty* property, const void* ptr)
{
	if (property == nullptr)
	{
		ensureAlwaysMsgf(0, L"Some Bug?");
		lua_pushnil(inL);
	}
	else if (UIntProperty* p = Cast<UIntProperty>(property))
	{
		pushproperty_type_valueptr(inL, p, ptr);
	}
	else if (UUInt32Property* p = Cast<UUInt32Property>(property))
	{
		pushproperty_type_valueptr(inL, p, ptr);
	}
	else if (UInt64Property* p = Cast<UInt64Property>(property))
	{
		pushproperty_type_valueptr(inL, p, ptr);
	}
	else if (UFloatProperty* p = Cast<UFloatProperty>(property))
	{
		pushproperty_type_valueptr(inL, p, ptr);
	}
	else if (UDoubleProperty* p = Cast<UDoubleProperty>(property))
	{
		pushproperty_type_valueptr(inL, p, ptr);
	}
	else if (UBoolProperty * p = Cast<UBoolProperty>(property))
	{
		pushproperty_type_valueptr(inL, p, ptr);
	}
	else if (UByteProperty* p = Cast<UByteProperty>(property))
	{
		pushproperty_type_valueptr(inL, p, ptr);
	}
	else if (UEnumProperty* p = Cast<UEnumProperty>(property))
	{
		pushproperty_type_valueptr(inL, p, ptr);
	}

	else if (UStrProperty* p = Cast<UStrProperty>(property))
	{
		pushproperty_type_valueptr(inL, p, ptr);
	}
	else if (UNameProperty* p = Cast<UNameProperty>(property))
	{
		pushproperty_type_valueptr(inL, p, ptr);
	}
	else if (UTextProperty* p = Cast<UTextProperty>(property))
	{
		pushproperty_type_valueptr(inL, p, ptr);
	}
	else if (UStructProperty* p = Cast<UStructProperty>(property))
	{
		pushproperty_type_valueptr(inL, p, ptr);
	}
	else if (UArrayProperty* p = Cast<UArrayProperty>(property))
	{
		pushproperty_type_valueptr(inL, p, ptr);
	}
	else if (UMapProperty* p = Cast<UMapProperty>(property))
	{
		pushproperty_type_valueptr(inL, p, ptr);
	}
	else if (USetProperty* p = Cast<USetProperty>(property))
	{
		pushproperty_type_valueptr(inL, p, ptr);
	}
	else if (UObjectPropertyBase* p = Cast<UObjectPropertyBase>(property))
	{
		pushproperty_type_valueptr(inL, p, ptr);
	}
	else if (UInterfaceProperty* p = Cast<UInterfaceProperty>(property))
	{
		pushproperty_type_valueptr(inL, p, ptr);
	}
	else
	{
		ensureAlwaysMsgf(0, L"Some type didn't process");
	}
}


void UTableUtil::pushproperty_type_valueptr(lua_State*inL, UBoolProperty* p, const void*ptr)
{
	lua_pushboolean(inL, (bool)p->GetPropertyValue(ptr));
}

void UTableUtil::pushproperty_type_valueptr(lua_State*inL, UIntProperty* p, const void*ptr)
{
	lua_pushinteger(inL, (int32)p->GetPropertyValue(ptr));
}

void UTableUtil::pushproperty_type_valueptr(lua_State*inL, UInt64Property* p, const void*ptr)
{
	lua_pushnumber(inL, (int64)p->GetPropertyValue(ptr));
}

void UTableUtil::pushproperty_type_valueptr(lua_State*inL, UFloatProperty* p, const void*ptr)
{
	lua_pushnumber(inL, (float)p->GetPropertyValue(ptr));
}

void UTableUtil::pushproperty_type_valueptr(lua_State*inL, UDoubleProperty* p, const void*ptr)
{
	lua_pushnumber(inL, (double)p->GetPropertyValue(ptr));
}

void UTableUtil::pushproperty_type_valueptr(lua_State*inL, UObjectPropertyBase* p, const void*ptr)
{
	pushuobject(inL, p->GetObjectPropertyValue(ptr));
}

void UTableUtil::pushproperty_type_valueptr(lua_State*inL, UStrProperty* p, const void*ptr)
{
	lua_pushstring(inL, TCHAR_TO_UTF8(*(p->GetPropertyValue(ptr))));
}

void UTableUtil::pushproperty_type_valueptr(lua_State*inL, UNameProperty* p, const void*ptr)
{
	FName result = (FName)p->GetPropertyValue(ptr);
	lua_pushstring(inL, TCHAR_TO_UTF8(*result.ToString()));
}

void UTableUtil::pushproperty_type_valueptr(lua_State*inL, UTextProperty* p, const void*ptr)
{
	FText result = (FText)p->GetPropertyValue(ptr);
	lua_pushstring(inL, TCHAR_TO_UTF8(*result.ToString()));
}

void UTableUtil::pushproperty_type_valueptr(lua_State*inL, UByteProperty* p, const void*ptr)
{
	lua_pushinteger(inL, (int)p->GetPropertyValue(ptr));
}

void UTableUtil::pushproperty_type_valueptr(lua_State*inL, UEnumProperty* p, const void*ptr)
{
	pushproperty_valueptr(inL, p->GetUnderlyingProperty(), ptr);
}

void UTableUtil::pushproperty_type_valueptr(lua_State*inL, UWeakObjectProperty* p, const void*ptr)
{
	pushuobject(inL, p->GetObjectPropertyValue(ptr));
}

void UTableUtil::pushproperty_type_valueptr(lua_State*inL, UUInt32Property* p, const void*ptr)
{
	lua_pushinteger(inL, (int32)p->GetPropertyValue(ptr));
}

void UTableUtil::pushproperty_type_valueptr(lua_State*inL, UMulticastDelegateProperty* p, const void*ptr)
{

}

void UTableUtil::pushproperty_type_valueptr(lua_State*inL, UStructProperty* p, const void*ptr)
{
	FString TypeName;
	if (UUserDefinedStruct* BpStruct = Cast<UUserDefinedStruct>(p->Struct))
	{
		MayAddNewStructType(BpStruct);
		TypeName = BpStruct->GetName();
	}
	else
		TypeName = p->Struct->GetStructCPPName();

	pushstruct_nogc(inL, TCHAR_TO_UTF8(*TypeName), (void*)ptr);
}

void UTableUtil::pushproperty_type_valueptr(lua_State*inL, UArrayProperty* property, const void* ptr)
{
	push(inL, ULuaArrayHelper::GetHelperCPP_ValuePtr((void*)ptr, property));
}

void UTableUtil::pushproperty_type_valueptr(lua_State*inL, UMapProperty* property, const void* ptr)
{
	push(inL, ULuaMapHelper::GetHelperCPP_ValuePtr((void*)ptr, property));
}

void UTableUtil::pushproperty_type_valueptr(lua_State*inL, USetProperty* property, const void*ptr)
{
	push(inL, ULuaSetHelper::GetHelperCPP_ValuePtr((void*)ptr, property));
}

void UTableUtil::pushproperty_type_valueptr(lua_State*inL, UInterfaceProperty* p, const void*ptr)
{
	FScriptInterface* result = (FScriptInterface*)p->GetPropertyValuePtr(ptr);
	pushuobject(inL, (void*)result->GetObject());
}

void UTableUtil::MayAddNewStructType(UUserDefinedStruct* BpStruct)
{
	FString TypeName = BpStruct->GetName();
	if (UUserDefinedStruct** ExistOnePtr = bpname2bpstruct.Find(TypeName))
	{
#ifdef LuaDebug
		if (*ExistOnePtr != BpStruct)
		{
			ensureAlwaysMsgf(0, L"shouldn't same name");
		}
#endif 
		return;
	}
	bpname2bpstruct.Add(TypeName, BpStruct);
	auto temp1 = FTCHARToUTF8((const TCHAR*)*TypeName);
	const char* name = (ANSICHAR*)temp1.Get();
	auto temp2 = FTCHARToUTF8((const TCHAR*)*FString::Printf(L"%s_nogc", *TypeName));
	const char* name_nogc = (ANSICHAR*)temp2.Get();
	addmodule(name_nogc, true, false);
	lua_getglobal(TheOnlyLuaState, name_nogc);
	for (TFieldIterator<UProperty> PropertyIt(BpStruct); PropertyIt; ++PropertyIt)
	{
		UProperty* Property = *PropertyIt;
		FString PropertyName = Property->GetName().LeftChop(33);
		int32 Index;
		PropertyName.FindLastChar('_', Index);
		PropertyName = PropertyName.LeftChop(PropertyName.Len() - Index);
		push(TheOnlyLuaState, "LuaGet_" + PropertyName);
		pushuobject(TheOnlyLuaState, Property);
		if(Property->IsA(UBoolProperty::StaticClass()))
			lua_pushcclosure(TheOnlyLuaState, BpStructPropertyGetterName(UBoolProperty), 1);
		else if(Property->IsA(UIntProperty::StaticClass()))
			lua_pushcclosure(TheOnlyLuaState, BpStructPropertyGetterName(UIntProperty), 1);
		else if(Property->IsA(UUInt32Property::StaticClass()))
			lua_pushcclosure(TheOnlyLuaState, BpStructPropertyGetterName(UUInt32Property), 1);
		else if(Property->IsA(UInt64Property::StaticClass()))
			lua_pushcclosure(TheOnlyLuaState, BpStructPropertyGetterName(UInt64Property), 1);
		else if(Property->IsA(UFloatProperty::StaticClass()))
			lua_pushcclosure(TheOnlyLuaState, BpStructPropertyGetterName(UFloatProperty), 1);
		else if(Property->IsA(UDoubleProperty::StaticClass()))
			lua_pushcclosure(TheOnlyLuaState, BpStructPropertyGetterName(UDoubleProperty), 1);
		else if(Property->IsA(UObjectPropertyBase::StaticClass()))
			lua_pushcclosure(TheOnlyLuaState, BpStructPropertyGetterName(UObjectPropertyBase), 1);
		else if(Property->IsA(UStrProperty::StaticClass()))
			lua_pushcclosure(TheOnlyLuaState, BpStructPropertyGetterName(UStrProperty), 1);
		else if(Property->IsA(UNameProperty::StaticClass()))
			lua_pushcclosure(TheOnlyLuaState, BpStructPropertyGetterName(UNameProperty), 1);
		else if(Property->IsA(UTextProperty::StaticClass()))
			lua_pushcclosure(TheOnlyLuaState, BpStructPropertyGetterName(UTextProperty), 1);
		else if(Property->IsA(UByteProperty::StaticClass()))
			lua_pushcclosure(TheOnlyLuaState, BpStructPropertyGetterName(UByteProperty), 1);
		else if(Property->IsA(UEnumProperty::StaticClass()))
			lua_pushcclosure(TheOnlyLuaState, BpStructPropertyGetterName(UEnumProperty), 1);
		else if (Property->IsA(UStructProperty::StaticClass()))
		{
			if(Property->GetOuter()->IsA(UScriptStruct::StaticClass()) && Property->GetOffset_ForInternal() == 0)
				lua_pushcclosure(TheOnlyLuaState, StructProperty_FirstMem_Getter, 1);
			else
				lua_pushcclosure(TheOnlyLuaState, BpStructPropertyGetterName(UStructProperty), 1);
		}
		else if(Property->IsA(UMulticastDelegateProperty::StaticClass()))
			lua_pushcclosure(TheOnlyLuaState, BpStructPropertyGetterName(UMulticastDelegateProperty), 1);
		else if(Property->IsA(UWeakObjectProperty::StaticClass()))
			lua_pushcclosure(TheOnlyLuaState, BpStructPropertyGetterName(UWeakObjectProperty), 1);
		else if(Property->IsA(UArrayProperty::StaticClass()))
			lua_pushcclosure(TheOnlyLuaState, BpStructPropertyGetterName(UArrayProperty), 1);
		else if(Property->IsA(UMapProperty::StaticClass()))
			lua_pushcclosure(TheOnlyLuaState, BpStructPropertyGetterName(UMapProperty), 1);
		else if(Property->IsA(USetProperty::StaticClass()))
			lua_pushcclosure(TheOnlyLuaState, BpStructPropertyGetterName(USetProperty), 1);
		else if(Property->IsA(UInterfaceProperty::StaticClass()))
			lua_pushcclosure(TheOnlyLuaState, BpStructPropertyGetterName(UInterfaceProperty), 1);
		else
			lua_pushcclosure(TheOnlyLuaState, BpStructGetProp, 1);
		lua_rawset(TheOnlyLuaState, -3);
		push(TheOnlyLuaState, "LuaSet_" + PropertyName);
		pushuobject(TheOnlyLuaState, Property);
		if(Property->IsA(UBoolProperty::StaticClass()))
			lua_pushcclosure(TheOnlyLuaState, BpStructPropertySetterName(UBoolProperty), 1);
		else if(Property->IsA(UIntProperty::StaticClass()))
			lua_pushcclosure(TheOnlyLuaState, BpStructPropertySetterName(UIntProperty), 1);
		else if(Property->IsA(UUInt32Property::StaticClass()))
			lua_pushcclosure(TheOnlyLuaState, BpStructPropertySetterName(UUInt32Property), 1);
		else if(Property->IsA(UInt64Property::StaticClass()))
			lua_pushcclosure(TheOnlyLuaState, BpStructPropertySetterName(UInt64Property), 1);
		else if(Property->IsA(UFloatProperty::StaticClass()))
			lua_pushcclosure(TheOnlyLuaState, BpStructPropertySetterName(UFloatProperty), 1);
		else if(Property->IsA(UDoubleProperty::StaticClass()))
			lua_pushcclosure(TheOnlyLuaState, BpStructPropertySetterName(UDoubleProperty), 1);
		else if(Property->IsA(UObjectPropertyBase::StaticClass()))
			lua_pushcclosure(TheOnlyLuaState, BpStructPropertySetterName(UObjectPropertyBase), 1);
		else if(Property->IsA(UStrProperty::StaticClass()))
			lua_pushcclosure(TheOnlyLuaState, BpStructPropertySetterName(UStrProperty), 1);
		else if(Property->IsA(UNameProperty::StaticClass()))
			lua_pushcclosure(TheOnlyLuaState, BpStructPropertySetterName(UNameProperty), 1);
		else if(Property->IsA(UTextProperty::StaticClass()))
			lua_pushcclosure(TheOnlyLuaState, BpStructPropertySetterName(UTextProperty), 1);
		else if(Property->IsA(UByteProperty::StaticClass()))
			lua_pushcclosure(TheOnlyLuaState, BpStructPropertySetterName(UByteProperty), 1);
		else if(Property->IsA(UEnumProperty::StaticClass()))
			lua_pushcclosure(TheOnlyLuaState, BpStructPropertySetterName(UEnumProperty), 1);
		else if(Property->IsA(UStructProperty::StaticClass()))
			lua_pushcclosure(TheOnlyLuaState, BpStructPropertySetterName(UStructProperty), 1);
		else if(Property->IsA(UMulticastDelegateProperty::StaticClass()))
			lua_pushcclosure(TheOnlyLuaState, BpStructPropertySetterName(UMulticastDelegateProperty), 1);
		else if(Property->IsA(UWeakObjectProperty::StaticClass()))
			lua_pushcclosure(TheOnlyLuaState, BpStructPropertySetterName(UWeakObjectProperty), 1);
		else if(Property->IsA(UArrayProperty::StaticClass()))
			lua_pushcclosure(TheOnlyLuaState, BpStructPropertySetterName(UArrayProperty), 1);
		else if(Property->IsA(UMapProperty::StaticClass()))
			lua_pushcclosure(TheOnlyLuaState, BpStructPropertySetterName(UMapProperty), 1);
		else if(Property->IsA(USetProperty::StaticClass()))
			lua_pushcclosure(TheOnlyLuaState, BpStructPropertySetterName(USetProperty), 1);
		else if(Property->IsA(UInterfaceProperty::StaticClass()))
			lua_pushcclosure(TheOnlyLuaState, BpStructPropertySetterName(UInterfaceProperty), 1);
		else
			lua_pushcclosure(TheOnlyLuaState, BpStructSetProp, 1);
		lua_rawset(TheOnlyLuaState, -3);
	}
	lua_pushstring(TheOnlyLuaState, "Copy");
	pushuobject(TheOnlyLuaState, BpStruct);
	lua_pushinteger(TheOnlyLuaState, BpStruct->GetStructureSize());
	lua_pushstring(TheOnlyLuaState, name);
	lua_pushcclosure(TheOnlyLuaState, BpStructCopy, 3);
	lua_rawset(TheOnlyLuaState, -3);
	addmodule(name, true, false);
	lua_getglobal(TheOnlyLuaState, name);
	lua_pushnil(TheOnlyLuaState);
	while (lua_next(TheOnlyLuaState, -3) != 0)
	{
		lua_pushvalue(TheOnlyLuaState, -2);
		lua_pushvalue(TheOnlyLuaState, -2);
		lua_rawset(TheOnlyLuaState, -5);
		lua_pop(TheOnlyLuaState, 1);
	}

	lua_pushstring(TheOnlyLuaState, "New");
	pushuobject(TheOnlyLuaState, BpStruct);
	lua_pushinteger(TheOnlyLuaState, BpStruct->GetStructureSize());
	lua_pushstring(TheOnlyLuaState, name);
	lua_pushcclosure(TheOnlyLuaState, BpStructNew, 3);
	lua_rawset(TheOnlyLuaState, -3);

	lua_pushstring(TheOnlyLuaState, "Destroy");
	pushuobject(TheOnlyLuaState, BpStruct);
	lua_pushcclosure(TheOnlyLuaState, BpStructDestroy, 1);
	lua_rawset(TheOnlyLuaState, -3);

	lua_pushstring(TheOnlyLuaState, "__gc");
	lua_pushcfunction(TheOnlyLuaState, struct_gcfunc);
	lua_rawset(TheOnlyLuaState, -3);

	lua_pushstring(TheOnlyLuaState, "classname");
	push(TheOnlyLuaState, name);
	lua_rawset(TheOnlyLuaState, -3);

	lua_pop(TheOnlyLuaState, 2);
}

void UTableUtil::pushproperty_type(lua_State*inL, UStructProperty* p, const void*ptr)
{
	FString TypeName;
	if (UUserDefinedStruct* BpStruct = Cast<UUserDefinedStruct>(p->Struct))
	{
		MayAddNewStructType(BpStruct);
		TypeName = BpStruct->GetName();
	}
	else
		TypeName = p->Struct->GetStructCPPName();

	void* result = (void*)p->ContainerPtrToValuePtr<uint8>(ptr);
	pushstruct_nogc(inL, TCHAR_TO_UTF8(*TypeName), result);
}



void UTableUtil::pushproperty_type_firstmem(lua_State*inL, UStructProperty* p, const void*ptr)
{
	FString TypeName;
	if (UUserDefinedStruct* BpStruct = Cast<UUserDefinedStruct>(p->Struct))
	{
		MayAddNewStructType(BpStruct);
		TypeName = BpStruct->GetName();
	}
	else
		TypeName = p->Struct->GetStructCPPName();

	void* result = (void*)p->ContainerPtrToValuePtr<uint8>(ptr);
	pushstruct_nogc_firstmem(inL, TCHAR_TO_UTF8(*TypeName), result);
}

void UTableUtil::pushproperty_type(lua_State*inL, UArrayProperty* property, const void* ptr)
{
	pushcontainer(inL, (void*)ptr, property);
}

void UTableUtil::pushproperty_type(lua_State*inL, UMapProperty* property, const void* ptr)
{
	pushcontainer(inL, (void*)ptr, property);
}

void UTableUtil::pushproperty_type(lua_State*inL, USetProperty* property, const void*ptr)
{
	pushcontainer(inL, (void*)ptr, property);
}

void UTableUtil::popproperty(lua_State* inL, int index, UProperty* property, void* ptr)
{
#if LuaDebug
	if (lua_gettop(inL) < FMath::Abs(index))
		ensureAlwaysMsgf(0, L"some bug?");
#endif
	if (UIntProperty* p = Cast<UIntProperty>(property))
	{
		popproperty_type(inL, index, p, ptr);
	}
	else if (UUInt32Property* p = Cast<UUInt32Property>(property))
	{
		popproperty_type(inL, index, p, ptr);
	}
	else if (UInt64Property* p = Cast<UInt64Property>(property))
	{
		popproperty_type(inL, index, p, ptr);
	}
	else if (UFloatProperty* p = Cast<UFloatProperty>(property))
	{
		popproperty_type(inL, index, p, ptr);
	}
	else if (UDoubleProperty* p = Cast<UDoubleProperty>(property))
	{
		popproperty_type(inL, index, p, ptr);
	}
	else if (UBoolProperty * p = Cast<UBoolProperty>(property))
	{
		popproperty_type(inL, index, p, ptr);
	}
	else if (UByteProperty* p = Cast<UByteProperty>(property))
	{
		popproperty_type(inL, index, p, ptr);
	}
	else if (UEnumProperty* p = Cast<UEnumProperty>(property))
	{
		popproperty_type(inL, index, p, ptr);
	}

	else if (UStrProperty* p = Cast<UStrProperty>(property))
	{
		popproperty_type(inL, index, p, ptr);
	}
	else if (UNameProperty* p = Cast<UNameProperty>(property))
	{
		popproperty_type(inL, index, p, ptr);
	}
	else if (UTextProperty* p = Cast<UTextProperty>(property))
	{
		popproperty_type(inL, index, p, ptr);
	}
	else if (UStructProperty* p = Cast<UStructProperty>(property))
	{
		popproperty_type(inL, index, p, ptr);
	}
	else if (UArrayProperty* p = Cast<UArrayProperty>(property))
	{
		popproperty_type(inL, index, p, ptr);
	}
	else if (UMapProperty* p = Cast<UMapProperty>(property))
	{
		popproperty_type(inL, index, p, ptr);
	}
	else if (USetProperty* p = Cast<USetProperty>(property))
	{
		popproperty_type(inL, index, p, ptr);
	}
	else if (UObjectPropertyBase* p = Cast<UObjectPropertyBase>(property))
	{
		popproperty_type(inL, index, p, ptr);
	}
	else if (UInterfaceProperty* p = Cast<UInterfaceProperty>(property))
	{
		popproperty_type(inL, index, p, ptr);
	}
	else
	{
		ensureAlwaysMsgf(0, L"Some Type didn't process");
	}
}

void UTableUtil::popproperty_type(lua_State*inL, int index, UBoolProperty* p, void*ptr)
{
	bool value = popiml<bool>::pop(inL, index);
	p->SetPropertyValue_InContainer(ptr, value);
}

void UTableUtil::popproperty_type(lua_State*inL, int index, UIntProperty* p, void*ptr)
{
	int32 value = popiml<int>::pop(inL, index);
	p->SetPropertyValue_InContainer(ptr, value);
}

void UTableUtil::popproperty_type(lua_State*inL, int index, UInt64Property* p, void*ptr)
{
	int64 value = popiml<int64>::pop(inL, index);
	p->SetPropertyValue_InContainer(ptr, value);
}

void UTableUtil::popproperty_type(lua_State*inL, int index, UUInt32Property* p, void*ptr)
{
	uint32 value = popiml<uint32>::pop(inL, index);
	p->SetPropertyValue_InContainer(ptr, value);
}

void UTableUtil::popproperty_type(lua_State*inL, int index, UFloatProperty* p, void*ptr)
{
	float value = popiml<float>::pop(inL, index);
	p->SetPropertyValue_InContainer(ptr, value);
}

void UTableUtil::popproperty_type(lua_State*inL, int index, UDoubleProperty* p, void*ptr)
{
	double value = popiml<double>::pop(inL, index);
	p->SetPropertyValue_InContainer(ptr, value);
}

void UTableUtil::popproperty_type(lua_State*inL, int index, UObjectPropertyBase* p, void*ptr)
{
	UObject* value = popiml<UObject*>::pop(inL, index);
	p->SetObjectPropertyValue_InContainer(ptr, value);
}

void UTableUtil::popproperty_type(lua_State*inL, int index, UStrProperty* p, void*ptr)
{
	FString value = popiml<FString>::pop(inL, index);
	p->SetPropertyValue_InContainer(ptr, value);
}

void UTableUtil::popproperty_type(lua_State*inL, int index, UNameProperty* p, void*ptr)
{
	FName value = popiml<FName>::pop(inL, index);
	p->SetPropertyValue_InContainer(ptr, value);
}

void UTableUtil::popproperty_type(lua_State*inL, int index, UTextProperty* p, void*ptr)
{
	FText value = popiml<FText>::pop(inL, index);
	p->SetPropertyValue_InContainer(ptr, value);
}

void UTableUtil::popproperty_type(lua_State*inL, int index, UByteProperty* p, void*ptr)
{
	int value = popiml<int>::pop(inL, index);
	p->SetPropertyValue_InContainer(ptr, value);
}

void UTableUtil::popproperty_type(lua_State*inL, int index, UEnumProperty* p, void*ptr)
{
	popproperty(inL, index, p->GetUnderlyingProperty(), p->ContainerPtrToValuePtr<void>(ptr));
}

void UTableUtil::popproperty_type(lua_State*inL, int index, UStructProperty* p, void*ptr)
{
	void* value = tostruct(inL, index);
	p->CopyCompleteValue(p->ContainerPtrToValuePtr<void>(ptr), value);
}

void UTableUtil::popproperty_type(lua_State*inL, int index, UArrayProperty* p, void* ptr)
{
	ULuaArrayHelper* ArrHelper = (ULuaArrayHelper*)touobject(inL, index);
	if (ArrHelper)
	{
		ArrHelper->CopyTo(p, p->ContainerPtrToValuePtr<void>(ptr));
	}
	else
	{
		lua_pushvalue(inL, index);
		int32 len = lua_objlen(inL, -1);
		FScriptArrayHelper_InContainer result(p, ptr);
		result.Resize(len);
		lua_pushnil(inL);
		while (lua_next(inL, -2))
		{
			int32 i = lua_tointeger(inL, -2) - 1;
			popproperty(inL, -1, p->Inner, result.GetRawPtr(i));
			lua_pop(inL, 1);
		}
		lua_pop(inL, 1);
	}
}

void UTableUtil::popproperty_type(lua_State*inL, int index, UMapProperty* p, void*ptr)
{
	ULuaMapHelper* Helper = (ULuaMapHelper*)touobject(inL, index);
	if (Helper)
	{
		Helper->CopyTo(p, p->ContainerPtrToValuePtr<void>(ptr));
	}
	else if (lua_istable(inL, index))
	{
		lua_pushvalue(inL, index);
		FScriptMapHelper_InContainer result(p, ptr);
		result.EmptyValues();
		lua_pushnil(inL);
		while (lua_next(inL, -2))
		{
			int32 i = result.AddDefaultValue_Invalid_NeedsRehash();
			uint8* PairPtr = result.GetPairPtr(i);
			popproperty(inL, -2, p->KeyProp, PairPtr + p->MapLayout.KeyOffset);
			popproperty(inL, -1, p->ValueProp, PairPtr);
			lua_pop(inL, 1);
		}
		result.Rehash();
		lua_pop(inL, 1);
	}
	else
	{
		ensureAlwaysMsgf(0, L"not map");
	}
}

void UTableUtil::popproperty_type(lua_State*inL, int index, USetProperty* p, void*ptr)
{
	ULuaSetHelper* Helper = (ULuaSetHelper*)touobject(inL, index);
	if (Helper)
	{
		Helper->CopyTo(p, p->ContainerPtrToValuePtr<void>(ptr));
	}
	else if (lua_istable(inL, index))
	{
		lua_pushvalue(inL, index);
		FScriptSetHelper_InContainer result(p, ptr);
		result.EmptyElements();
		lua_pushnil(inL);
		while (lua_next(inL, -2))
		{
			int32 i = result.AddDefaultValue_Invalid_NeedsRehash();
			uint8* ElementPtr = result.GetElementPtr(i);
			popproperty(inL, -2, p->ElementProp, ElementPtr);
			lua_pop(inL, 1);
		}
		result.Rehash();
		lua_pop(inL, 1);
	}
	else
	{
		ensureAlwaysMsgf(0, L"not set");
	}

}

void UTableUtil::popproperty_type(lua_State*inL, int index, UMulticastDelegateProperty* p, void*ptr)
{

}

void UTableUtil::popproperty_type(lua_State*inL, int index, UInterfaceProperty* p, void*ptr)
{
	FScriptInterface* result = (FScriptInterface*)p->GetPropertyValuePtr_InContainer(ptr);
	UObject* value = (UObject*)touobject(inL, index);
	result->SetObject(value);
}

void pushuobject(lua_State *inL, void* p, bool bgcrecord)
{
	if (p == nullptr)
	{
		lua_pushnil(inL);
		return;
	}
	if (!existdata(inL, p))
	{
		*(void**)lua_newuserdata(inL, sizeof(void *)) = p;

		lua_getfield(inL, LUA_REGISTRYINDEX, "_existuserdata");
		lua_pushlightuserdata(inL, p);
		lua_pushvalue(inL, -3);
		lua_rawset(inL, -3);
		lua_pop(inL, 1);

		UTableUtil::set_uobject_meta(inL, (UObject*)p, -1);
		UTableUtil::addgcref((UObject*)p);
	}
}

void pushstruct_gc(lua_State *inL, const char* structname, void* p)
{
	if (p == nullptr)
	{
		lua_pushnil(inL);
		return;
	}
	*(void**)lua_newuserdata(inL, sizeof(void *)) = p;
	UTableUtil::setmeta(inL, structname, -1, true);
#ifdef LuaDebug
	if (UTableUtil::countforgc.Contains(structname))
		UTableUtil::countforgc[structname]++;
	else
		UTableUtil::countforgc.Add(structname, 1);
#endif
}

void pushstruct_nogc(lua_State *inL, const char* structname, void* p)
{
	if (p == nullptr)
	{
		lua_pushnil(inL);
		return;
	}
	if (!existdata(inL, p))
	{
		*(void**)lua_newuserdata(inL, sizeof(void *)) = p;

		lua_getfield(inL, LUA_REGISTRYINDEX, "_existuserdata");
		lua_pushlightuserdata(inL, p);
		lua_pushvalue(inL, -3);
		lua_rawset(inL, -3);
		lua_pop(inL, 1);
		UTableUtil::setmeta(inL, TCHAR_TO_ANSI(*FString::Printf(L"%s_nogc", ANSI_TO_TCHAR(structname))), -1, true, false);
	}
}

void pushstruct_nogc_firstmem(lua_State *inL, const char* structname, void* p)
{
	lua_getfield(inL, LUA_REGISTRYINDEX, "_existfirststruct");
	lua_getfield(inL, -1, structname);
	if (lua_isnil(inL, -1))
	{
// _existfirststruct[structname] = setmetatable({}, weak)
		lua_pop(inL, 1);
		lua_newtable(inL);

		lua_newtable(inL);
		lua_pushstring(inL, "v");
		lua_setfield(inL, -2, "__mode");
		lua_setmetatable(inL, -2);

		lua_pushstring(inL, structname);
		lua_pushvalue(inL, -2);
		lua_rawset(inL, -4);
	}
	lua_remove(inL, -2);

	lua_pushlightuserdata(inL, p);
	lua_rawget(inL, -2);

	if (lua_isnil(inL, -1))
	{
		lua_pop(inL, 1);
		*(void**)lua_newuserdata(inL, sizeof(void *)) = p;
		lua_pushlightuserdata(inL, p);
		lua_pushvalue(inL, -2);
		lua_rawset(inL, -4);
		UTableUtil::setmeta(inL, TCHAR_TO_ANSI(*FString::Printf(L"%s_nogc", ANSI_TO_TCHAR(structname))), -1, true);
	}

	lua_replace(inL, -2);
}

void pushstruct_stack(lua_State *inL, const char* structname, void* p)
{
	*(void**)lua_newuserdata(inL, sizeof(void *)) = p;
	UTableUtil::setmeta(inL, TCHAR_TO_ANSI(*FString::Printf(L"%s_nogc", ANSI_TO_TCHAR(structname))), -1, true);
}

void UTableUtil::loadlib(const luaL_Reg funclist[], const char* classname, bool bIsStruct, bool bNeedGc)
{
	int i = 0;
	UTableUtil::addmodule(classname, bIsStruct, bNeedGc);
	UTableUtil::openmodule(classname);
	while (true)
	{
		luaL_Reg temp = funclist[i];
		if (temp.name == nullptr)
		{
			break;
		}
		else if (temp.func != nullptr)
		{
			UTableUtil::addfunc(temp.name, temp.func);
		}
		i++;
	}
	UTableUtil::closemodule();
}


void UTableUtil::loadstruct(const luaL_Reg funclist[], const char* classname)
{
	SupportedNativeStruct.Add(classname);
	loadlib(funclist, classname, true);
	loadlib(funclist, TCHAR_TO_ANSI(*FString::Printf(L"%s_nogc", ANSI_TO_TCHAR(classname))), true, false);
}

void UTableUtil::loadEnum(const EnumItem list[], const char* enumname)
{
	lua_newtable(TheOnlyLuaState);
	int i = 0;
	while (true)
	{
		EnumItem temp = list[i];
		if (temp.key == nullptr)
			break;
		else
		{
			lua_pushstring(TheOnlyLuaState, temp.key);
			lua_pushinteger(TheOnlyLuaState, temp.value);
			lua_rawset(TheOnlyLuaState, -3);
		}
		++i;
	}
	lua_setfield(TheOnlyLuaState, LUA_GLOBALSINDEX, enumname);
}

bool existdata(lua_State*inL, void * p)
{
	lua_getfield(inL, LUA_REGISTRYINDEX, "_existuserdata");
	lua_pushlightuserdata(inL, p);
	lua_rawget(inL, -2);
	if (lua_isnil(inL, -1))
	{
		lua_pop(inL, 2);
		return false;
	}
	else
	{
		lua_replace(inL, -2);
		return true;
	}
}


bool UTableUtil::existluains(lua_State*inL, void * p)
{
	lua_getfield(inL, LUA_REGISTRYINDEX, "_existuserdata");
	lua_pushlightuserdata(inL, p);
	lua_rawget(inL, -2);
	bool bDoesExist = false;
	if (lua_istable(inL, -1))
		bDoesExist = true;
	lua_pop(inL, 2);
	return bDoesExist;
}

void UTableUtil::log(const FString& content)
{
	UE_LOG(LuaLog, Display, TEXT("[lua error] %s"), *content);
}

luavalue_ref UTableUtil::ref_luavalue(lua_State*inL, int index)
{
	if (index < 0)
		index = lua_gettop(inL) + index + 1;

	lua_pushvalue(inL, index);
	luavalue_ref r = luaL_ref(inL, LUA_REGISTRYINDEX);

	return r;
}

void UTableUtil::unref(lua_State*inL, luavalue_ref r)
{
	int ref = r;

	luaL_unref(inL, LUA_REGISTRYINDEX, ref);
}

bool UTableUtil::bisRun()
{
	return TheOnlyLuaState != nullptr;
}

void UTableUtil::addgcref(UObject *p)
{
	FLuaGcObj::Get()->objs.Add(p);
}

void UTableUtil::rmgcref(UObject *p)
{
	FLuaGcObj::Get()->objs.Remove(p);
}

void LuaTickObject::Tick(float DeltaTime)
{
	LuaStaticCall("Tick", DeltaTime);
}

bool LuaTickObject::IsTickable() const
{
	return true;
}

TStatId LuaTickObject::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(LuaTickObject, STATGROUP_Tickables);
}

void FLuaGcObj::AddReferencedObjects(FReferenceCollector& Collector)
{
#if STRONG_CHECK_GC_REF
	Collector.AllowEliminatingReferences(false);
#endif
	Collector.AddReferencedObjects(objs);
#if STRONG_CHECK_GC_REF
	Collector.AllowEliminatingReferences(true);
#endif 
}

