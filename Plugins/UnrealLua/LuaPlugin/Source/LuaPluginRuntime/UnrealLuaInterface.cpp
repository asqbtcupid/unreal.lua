// Fill out your copyright notice in the Description page of Project Settings.

#include "UnrealLuaInterface.h"
#include "UObjectGlobals.h"
#include "Engine.h"
#include "GameDelegates.h"
#include "LuaFunctionInjectHelper.h"
static void* LuaMemAlloc(void *Ud, void *Ptr, size_t OldSize, size_t NewSize)
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

static int32 LuaStatePanic(lua_State *L)
{
	UTableUtil::log(FString::Printf(TEXT("PANIC: unprotected error in call to Lua API(%s)"), ANSI_TO_TCHAR(lua_tostring(L, -1))));
	return 0;
}


LuaContext::LuaContext(const UGameInstance* GameIns) :
	TheContextLuaState(nullptr),
	TheOuter(GameIns),
	GameTickObject(nullptr),
	EditorTickObject(nullptr)
{
	if (Init())
	{
		CreateGameTick();
		lua_State*L = TheContextLuaState;
		pushuobject(L, (void*)GameIns);
		lua_setglobal(L, "TheGameInstance");
	}
	else
	{
		ensureAlwaysMsgf(0, TEXT("Lua Init Error"));
	}
}

LuaContext::LuaContext(const UWorld* WorldIns):
TheContextLuaState(nullptr),
TheOuter(WorldIns),
GameTickObject(nullptr),
EditorTickObject(nullptr)
{
	if (Init())
	{
// 		EditorTickObject = new LuaStateTick_Editor();
// 		EditorTickObject->OwningLuaContext = this;
// 		GameTickObject = new LuaStateTick();
	}
	else
	{
		ensureAlwaysMsgf(0, TEXT("Lua Init Error"));
	}
}

LuaContext::LuaContext() :
	TheContextLuaState(nullptr),
	TheOuter(nullptr),
	GameTickObject(nullptr),
	EditorTickObject(nullptr)
{
	if (!Init())
	{
		ensureAlwaysMsgf(0, TEXT("Lua Init Error"));
	}
}

bool LuaContext::Init()
{
	IsPreExit = false;
	auto l = lua_newstate(LuaMemAlloc, nullptr);
	if (l) lua_atpanic(l, &LuaStatePanic);
	luaL_openlibs(l);

	FString gameDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
	FString luaDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / TEXT("Plugins/UnrealLua/LuaSource"));

	UTableUtil::log("try load lua main.lua");
#if USE_LUASOURCE || WITH_EDITOR
	FString mainFilePath = luaDir / TEXT("main.lua");
	if (luaL_dofile(l, TCHAR_TO_UTF8(*mainFilePath)))
	{
		UTableUtil::log(FString(lua_tostring(l, -1)));
// 		checkf(0, TEXT("Failed to Init Lua state %s"));
		return false;
	}
#else    
	UTableUtil::useCustomLoader(l);
	if (luaL_dostring(l, "require \"main\""))
	{
		UTableUtil::log(FString(lua_tostring(l, -1)));
// 		checkf(0, TEXT("Failed to Init Lua state %s"));
		return false;
	}
#endif
	else
	{
		UTableUtil::log("start init lua");
		UTableUtil::PowerTheState(l);
		UTableUtil::push(l, luaDir);
		lua_setglobal(l, "_luadir");
		UTableUtil::push(l, gameDir);
		lua_setglobal(l, "_gamedir");
		UTableUtil::call_with_state(l, "Init");
		BindEngineDelegate();

		TheContextLuaState = l;
		UTableUtil::log("sucess init lua");
		return true;
	}
}

void LuaContext::PostInit()
{
	UTableUtil::call_with_state(TheContextLuaState, "OpenDebugger");
}

LuaContext::~LuaContext()
{
	if (IsPreExit)
		return;
	lua_State* L = TheContextLuaState;
	UTableUtil::call_with_state(L, "Shutdown");
	if (GameTickObject)
		delete GameTickObject;

	if (EditorTickObject)
		delete EditorTickObject;
	
#if LuaDebug
	if (lua_gettop(L) > 10)
		ensureAlwaysMsgf(0, TEXT("you push someting into lua stack but forget to pop.%d"), lua_gettop(L));
	UTableUtil::log(FString::Printf(TEXT("Shutdown How many thing left in stack:%d"), lua_gettop(L)));
#endif
	lua_close(L);
	UTableUtil::ShutdownTheState(L);
}

void LuaContext::CreateGameTick()
{
	GameTickObject = new LuaStateTick();
	GameTickObject->OwningLuaContext = this;
}

void LuaContext::CreateEditorTick()
{
#if WITH_EDITOR
	EditorTickObject = new LuaStateTick_Editor();
	EditorTickObject->OwningLuaContext = this;
#endif
}

void LuaContext::BindEngineDelegate()
{
// #if ENGINE_MINOR_VERSION < 18
// 	LuaPreGarbageCollectDelegateHandle = FCoreUObjectDelegates::PreGarbageCollect.AddRaw(this, &LuaContext::OnGC);
// #else
// 	LuaPreGarbageCollectDelegateHandle = FCoreUObjectDelegates::GetPostGarbageCollect().AddRaw(this, &LuaContext::OnGC);
// #endif
// 	LuaOnWorldCleanUpDelegateHandle = FWorldDelegates::OnWorldCleanup.AddRaw(this, &LuaContext::OnWorldCleanup);
// 	LuaOnEndPlayDelegateHandle = FGameDelegates::Get().GetEndPlayMapDelegate().AddRaw(this, &LuaContext::OnEndPlayMap);
// 	LuaOnPreExitHandle = FCoreDelegates::OnPreExit.AddRaw(this, &LuaContext::OnPreExit);
}

void LuaContext::CleanEngineDelegate()
{
// 	LuaOnWorldCleanUpDelegateHandle
}

void LuaContext::OnWorldCleanup(UWorld* World, bool bSessionEnded, bool bCleanupResources)
{
	UTableUtil::call_with_state(TheContextLuaState, "OnWorldCleanup", World, bSessionEnded, bCleanupResources);
	UTableUtil::OnWorldCleanUp(TheContextLuaState, World);
	lua_gc(TheContextLuaState, LUA_GCCOLLECT, 0);
	lua_gc(TheContextLuaState, LUA_GCCOLLECT, 0);
}

void LuaContext::OnEndPlayMap()
{
// 	UTableUtil::call_with_state(TheContextLuaState, "OnWorldCleanup", World, bSessionEnded, bCleanupResources);
}

void LuaContext::OnPreExit()
{
	IsPreExit = true;
}

void LuaContext::OnGC()
{

}

void LuaContext::EnableTick(bool IsEnable)
{
	if (GameTickObject)
		GameTickObject->EnableTick(IsEnable);
}

void LuaStateTick::Tick(float DeltaTime)
{
	lua_State* L = OwningLuaContext->TheContextLuaState;
	lua_getglobal(L, "Tick");
	lua_pushnumber(L, DeltaTime);
	lua_call(L, 1, 0);
}

bool LuaStateTick::IsTickable() const
{
	return IsEnableTick;
}

void LuaStateTick::EnableTick(bool IsEnable)
{
	IsEnableTick = IsEnable;
}

TStatId LuaStateTick::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(LuaStateTick, STATGROUP_Tickables);
}

LuaContextMgr::LuaContextMgr():
	SingleGameLuaContext(nullptr),
	luaContextForDefaultConstruct(nullptr)
{
	UTableUtil::MapPropertyToPushPopFunction();
	UTableUtil::Init();
	BindDelegates();
}

LuaContextMgr::~LuaContextMgr()
{
	FWorldDelegates::OnWorldCleanup.Remove(LuaOnWorldCleanUpDelegateHandle);
	FGameDelegates::Get().GetEndPlayMapDelegate().Remove(LuaOnEndPlayDelegateHandle);
}

LuaContextMgr& LuaContextMgr::Get()
{
	static LuaContextMgr TheMgr;
	return TheMgr;
}

void LuaContextMgr::OnPreExit()
{
#if WITH_EDITOR
	for (auto& Pairs : GameInstanceToLuaStateMap)
	{
		Pairs.Value->OnEndPlayMap();
		delete Pairs.Value;
	}
	GameInstanceToLuaStateMap.Reset();
	for (auto& Pairs : WorldInsToLuaStateMap)
	{
		Pairs.Value->OnEndPlayMap();
		delete Pairs.Value;
	}
	WorldInsToLuaStateMap.Reset();

	if (luaContextForDefaultConstruct)
	{
		delete luaContextForDefaultConstruct;
		luaContextForDefaultConstruct = nullptr;
	}
#else
	if (SingleGameLuaContext)
	{
		delete SingleGameLuaContext;
		SingleGameLuaContext = nullptr;
	}
#endif
}

LuaContext& LuaContextMgr::GetLuaContextByWorld(const UWorld *World)
{
	LuaContext** LuaContextIns = WorldInsToLuaStateMap.Find(World);
	if (LuaContextIns == nullptr)
	{
		LuaContext* NewLuaContextIns = new LuaContext(World);
		WorldInsToLuaStateMap.Add(World, NewLuaContextIns);
		return *NewLuaContextIns;
	}
	else
		return **LuaContextIns;
}

LuaContext& LuaContextMgr::GetLuaContextByIns(const UGameInstance* GameIns)
{
	LuaContext** LuaContextIns = GameInstanceToLuaStateMap.Find(GameIns);
	if (LuaContextIns == nullptr)
	{
		LuaContext* NewLuaContextIns = new LuaContext(GameIns);
		GameInstanceToLuaStateMap.Add(GameIns, NewLuaContextIns);
// 		LuaStateToGameInsMap.Add(NewLuaContextIns->TheContextLuaState, GameIns);
		NewLuaContextIns->PostInit();
		return *NewLuaContextIns;
	}
	else
	{
		if (GameIns == nullptr)
		{
			delete *LuaContextIns;
			GameInstanceToLuaStateMap.Remove(nullptr);
			return GetLuaContextByIns(nullptr);
		}
		else
			return **LuaContextIns;
	}
}


LuaContext* LuaContextMgr::GetDefaultConstructluaContext()
{
	if (luaContextForDefaultConstruct == nullptr)
		luaContextForDefaultConstruct = new LuaContext();
	return luaContextForDefaultConstruct;
}

void LuaContextMgr::BindDelegates()
{
	LuaOnWorldCleanUpDelegateHandle = FWorldDelegates::OnWorldCleanup.AddRaw(this, &LuaContextMgr::OnWorldCleanup);
	LuaOnEndPlayDelegateHandle = FGameDelegates::Get().GetEndPlayMapDelegate().AddRaw(this, &LuaContextMgr::OnEndPlayMap);
// 	LuaOnPreExit = FCoreDelegates::OnPreExit.AddRaw(this, &LuaContextMgr::OnPreExit);
}

void LuaContextMgr::OnWorldCleanup(UWorld* World, bool bSessionEnded, bool bCleanupResources)
{
#if WITH_EDITOR
	UGameInstance* GameIns = World->GetGameInstance();
	if (GameIns)
	{
		LuaContext** Context = GameInstanceToLuaStateMap.Find(GameIns);
		if (Context)
			(*Context)->OnWorldCleanup(World, bSessionEnded, bCleanupResources);
	}
	else if (LuaContext** Context = WorldInsToLuaStateMap.Find(World))
	{
		(*Context)->OnWorldCleanup(World, bSessionEnded, bCleanupResources);
		delete (*Context);
		WorldInsToLuaStateMap.Remove(World);
	}
	else
	{
		LuaContext** Context1 = GameInstanceToLuaStateMap.Find(nullptr);
		if(Context1)
			(*Context1)->OnWorldCleanup(World, bSessionEnded, bCleanupResources);
	}
#else
	if (SingleGameLuaContext)
	{
		SingleGameLuaContext->OnWorldCleanup(World, bSessionEnded, bCleanupResources);
	}
#endif
}

void LuaContextMgr::OnEndPlayMap()
{
#if WITH_EDITOR
	ShutdownGameInstanceLuaContext();
#else
	if (SingleGameLuaContext)
	{
		delete SingleGameLuaContext;
		SingleGameLuaContext = nullptr;
	}
#endif
	LuaFunctionInjectHelper::Destroy();
}

void LuaContextMgr::ShutdownGameInstanceLuaContext()
{
#if WITH_EDITOR
	TArray<const void*> KeysToRemove;
	for (auto& Pairs : GameInstanceToLuaStateMap)
	{
		if (Pairs.Key != nullptr)
		{
			Pairs.Value->OnEndPlayMap();
			delete Pairs.Value;
			KeysToRemove.Add(Pairs.Key);
// 			LuaStateToGameInsMap.Remove(*(Pairs.Value));
		}
	}
	for (const void* Ins : KeysToRemove)
	{
		GameInstanceToLuaStateMap.Remove(Ins);
	}
#else
	ShutdownSingleGameLuaContext();
#endif
}

void LuaContextMgr::ShutdownSpecifyGameInstanceContext(const UGameInstance* GameIns)
{
#if WITH_EDITOR
	if (LuaContext** ContextPtr = GameInstanceToLuaStateMap.Find(GameIns))
	{
		delete *ContextPtr;
		GameInstanceToLuaStateMap.Remove(GameIns);
	}
#else
	ShutdownSingleGameLuaContext();
#endif
}

TArray<lua_State*> LuaContextMgr::GetGameinstanceLuaStates()
{
	TArray<lua_State*> Result;
	for (auto& Paris : GameInstanceToLuaStateMap)
	{
		Result.Add(Paris.Value->TheContextLuaState);
	}
	return Result;
};

TArray<lua_State*> LuaContextMgr::GetAllLuaState()
{
	TArray<lua_State*> AllStates = GetGameinstanceLuaStates();
	for (auto& Pair : LuaStateForUserContext)
	{
		AllStates.Add(Pair.Value->TheContextLuaState);
	}
	return AllStates;
}

TSharedPtr<LuaContext> LuaContextMgr::GetLuaStateByVoidPtr(void* UserContext)
{
	if (TSharedPtr<LuaContext>* p = LuaStateForUserContext.Find(UserContext))
		return (*p);
	else
	{
		TSharedPtr<LuaContext> NewLuaContextIns = MakeShareable(new LuaContext());
		LuaStateForUserContext.Add(UserContext, NewLuaContextIns);
		NewLuaContextIns->PostInit();
		return NewLuaContextIns;
	}
}

bool LuaContextMgr::ShutdownLuaStateByVoidPtr(void* UserContext)
{
	if (auto p = LuaStateForUserContext.Find(UserContext))
	{
		LuaStateForUserContext.Remove(UserContext);
		return true;
	}
	return false;
}

lua_State* UUnrealLuaInterface::GetLuaStateByIns(const UObject* Ins)
{
	return GetLuaContextByIns(Ins).TheContextLuaState;
}

LuaContext& UUnrealLuaInterface::GetLuaContextByIns(const UObject* Ins)
{
#if WITH_EDITOR

	if (Ins->HasAnyFlags(RF_ClassDefaultObject))
	{
		LuaContext* LuaStateContext = LuaContextMgr::Get().GetDefaultConstructluaContext();
		return *LuaStateContext;
	}

	const UGameInstance* GameIns = Cast<UGameInstance>(Ins);
	if (GameIns == nullptr)
	{
		GameIns = Ins->GetTypedOuter<UGameInstance>();
		if (GameIns == nullptr)
		{
			UWorld* WorldBelong = GEngine->GetWorldFromContextObject(Ins, EGetWorldErrorMode::ReturnNull);
			if (WorldBelong == nullptr)
			{
				return LuaContextMgr::Get().GetLuaContextByIns(nullptr);
			}
			else
			{
				GameIns = WorldBelong->GetGameInstance();
				if (!GameIns)
				{
// 					if (WorldBelong->WorldType == EWorldType::EditorPreview || WorldBelong->WorldType == EWorldType::Editor)
						return LuaContextMgr::Get().GetLuaContextByWorld(WorldBelong);
// 					else
// 					{
// 						ensureAlwaysMsgf(0, TEXT(""));
// 					}
				}
			}
		}
	}
	return LuaContextMgr::Get().GetLuaContextByIns(GameIns);
#else
	return *(LuaContextMgr::Get().GetSingleGameluaContext());
#endif
}

void UUnrealLuaInterface::ShutdownAllState()
{
	LuaContextMgr::Get().ShutdownGameInstanceLuaContext();
}

void UUnrealLuaInterface::ShutdownSpecifyGameInsLuaState(const UGameInstance* GameIns)
{
	LuaContextMgr::Get().ShutdownSpecifyGameInstanceContext(GameIns);
}

void UUnrealLuaInterface::DoString(UObject* Obj, FString Str)
{
	lua_State* inL = GetLuaStateByIns(Obj);
	UTableUtil::DoString(inL, Str);
}

void UUnrealLuaInterface::DoFile(UObject* Obj, FString Str)
{
	lua_State* inL = GetLuaStateByIns(Obj);
	UTableUtil::DoFile(inL, Str);
}

bool UUnrealLuaInterface::IsExistFunc(const UObject* Ptr, const char* FuncName)
{
	lua_State* inL = GetLuaStateByIns(Ptr);
	lua_getfield(inL, LUA_REGISTRYINDEX, "_existuserdata");
	lua_pushlightuserdata(inL, (void*)Ptr);
	lua_rawget(inL, -2);
	if (!lua_istable(inL, -1))
	{
		lua_pop(inL, 2);
		return false;
	}
	else
	{
// to opmization
		bool IsExist = LuaStaticCallr_State(inL, bool, "IsExistLuaFunc", Ptr, FuncName);
		lua_pop(inL, 2);
		return IsExist;
	}
}

bool UUnrealLuaInterface::IsExistFile(const UObject* Ptr, const char* FileName)
{
	lua_State* inL = GetLuaStateByIns(Ptr);
	lua_getglobal(inL, "require");
	lua_pushstring(inL, FileName);
	if (lua_pcall(inL, 1, 1, 0))
	{
		return false;
	}
	if (lua_istable(inL, -1))
	{
		lua_pop(inL, 1);
		return true;
	}
	else
		return false;
}

bool UUnrealLuaInterface::IsLuaEnable = true;
int32 UUnrealLuaInterface::bEnableLuaCVar = 1;

FAutoConsoleVariableRef CVarEnableLua(
	TEXT("p.EnableLua"),
	UUnrealLuaInterface::bEnableLuaCVar,
	TEXT("Whether to enable enable lua\n")
	TEXT("0: Disable, 1: Enable"),
	ECVF_Default);

void UUnrealLuaInterface::ReplaceUe4Function(const UObject* Obj, const char* LuaClassPath)
{
// todo
	return;
	if (IsEnableLua())
	{
		if (Obj->HasAnyFlags(RF_ClassDefaultObject))
			return;
		lua_State* inL = GetLuaStateByIns(Obj);
		LuaFunctionInjectHelper::Get()->ReplaceUClassFunction(inL, Obj->GetClass(), LuaClassPath);
	}
}
#if WITH_EDITOR

void LuaStateTick_Editor::Tick(float DeltaTime)
{
	lua_State* L = OwningLuaContext->TheContextLuaState;
	lua_getglobal(L, "Tick");
	lua_pushnumber(L, DeltaTime);
	lua_call(L, 1, 0);
}

bool LuaStateTick_Editor::IsTickable() const
{
	return true;
}

TStatId LuaStateTick_Editor::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(LuaStateTick, STATGROUP_Tickables);
}
#endif