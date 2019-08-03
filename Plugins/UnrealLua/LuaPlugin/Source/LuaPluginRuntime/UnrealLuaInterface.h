// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TableUtil.h"
#include "Kismet/GameplayStatics.h"
#if WITH_EDITOR
#include "TickableEditorObject.h"
#endif
#include "UnrealLuaInterface.generated.h"


class LuaStateTick :public FTickableGameObject
{
public:
	LuaStateTick():IsEnableTick(true){}
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override;
	void EnableTick(bool IsEnable);
	virtual TStatId GetStatId() const override;
	struct LuaContext* OwningLuaContext;
	bool IsEnableTick;
};

// class LuaStateTick_Editor {};
#if WITH_EDITOR
class LuaStateTick_Editor :public FTickableEditorObject
{
public:
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;
	struct LuaContext* OwningLuaContext;
};
#else
class LuaStateTick_Editor {};
#endif

struct LUAPLUGINRUNTIME_API LuaContext
{
	LuaContext(const UGameInstance* GameIns);
	LuaContext(const UWorld* GameIns);
	LuaContext();

	bool Init();
	void PostInit();
	~LuaContext();
	operator lua_State*()
	{
		return TheContextLuaState;
	}
	void CreateGameTick();
	void CreateEditorTick();
	void BindEngineDelegate();
	void CleanEngineDelegate();
	void OnWorldCleanup(UWorld* World, bool bSessionEnded, bool bCleanupResources);
	void OnEndPlayMap();
	void OnPreExit();
	void OnGC();
	void EnableTick(bool IsEnable);
	lua_State* TheContextLuaState;
	const void* TheOuter;
	bool IsPreExit;
	FDelegateHandle LuaPreGarbageCollectDelegateHandle;
	FDelegateHandle LuaOnWorldCleanUpDelegateHandle;
	FDelegateHandle LuaOnEndPlayDelegateHandle;
	FDelegateHandle LuaOnPreExitHandle;
#if WITH_EDITOR
	FDelegateHandle LuaOnEditorBeginPlayDelegateHandle;
#endif
	
	class LuaStateTick* GameTickObject;
	class LuaStateTick_Editor* EditorTickObject;
};

struct LUAPLUGINRUNTIME_API LuaContextMgr
{
	LuaContextMgr();
	~LuaContextMgr();
	static LuaContextMgr& Get();
	void OnPreExit();

	LuaContext& GetLuaContextByWorld(const UWorld *World);

	LuaContext& GetLuaContextByIns(const UGameInstance* GameIns);
	
	lua_State* GetLuaStateByIns(const UGameInstance* GameIns)
	{
		return GetLuaContextByIns(GameIns).TheContextLuaState;
	}

	LuaContext* GetDefaultConstructluaContext();


	LuaContext* SingleGameLuaContext;
	LuaContext* GetSingleGameluaContext()
	{
		if (SingleGameLuaContext == nullptr)
		{
			UTableUtil::log("Init SingleGameluaContext");
			SingleGameLuaContext = new LuaContext();
			SingleGameLuaContext->CreateGameTick();
		}
		return SingleGameLuaContext;
	}

	void ShutdownSingleGameLuaContext()
	{
		if (SingleGameLuaContext)
		{
			delete SingleGameLuaContext;
			SingleGameLuaContext = nullptr;
		}
	}

	void InitSingleGameLuaContext()
	{
		GetSingleGameluaContext();
	}

	void BindDelegates();

	void OnWorldCleanup(UWorld* World, bool bSessionEnded, bool bCleanupResources);
	void OnEndPlayMap();
	void ShutdownGameInstanceLuaContext();
	void ShutdownSpecifyGameInstanceContext(const UGameInstance* GameIns);

	TArray<lua_State*> GetGameinstanceLuaStates();
	TArray<lua_State*> GetAllLuaState();
	TMap<const void*, LuaContext*> GameInstanceToLuaStateMap;
	TMap<const UWorld*, LuaContext*> WorldInsToLuaStateMap;

	TMap<lua_State*, const void*> LuaStateToGameInsMap;

	LuaContext* luaContextForDefaultConstruct;
	FDelegateHandle LuaOnWorldCleanUpDelegateHandle;
	FDelegateHandle LuaOnEndPlayDelegateHandle;
	FDelegateHandle LuaOnPreExit;

	TSharedPtr<LuaContext> GetLuaStateByVoidPtr(void* UserContext);
	bool ShutdownLuaStateByVoidPtr(void* UserContext);
	TMap<void*, TSharedPtr<LuaContext>> LuaStateForUserContext;
};

// #define LuaCtor(classname, ...) {UUnrealLuaInterface::Call("Ctor", classname, this, ##__VA_ARGS__);\
// 									UUnrealLuaInterface::ReplaceUe4Function(this, classname);}

#define LuaCtor(classname, ...) UUnrealLuaInterface::Call("Ctor", classname, this, ##__VA_ARGS__);
#define LuaInject(classname) UUnrealLuaInterface::ReplaceUe4Function(this, classname);


#define LuaCtorAndInject(classname, ...) {UUnrealLuaInterface::Call("Ctor", classname, this, ##__VA_ARGS__);\
									UUnrealLuaInterface::ReplaceUe4Function(this, classname);}

#define LuaCall(functionname, ptr, ...) UUnrealLuaInterface::Call("Call", functionname, ptr, ##__VA_ARGS__);
#define LuaCallr(ret, functionname, ptr,...) UUnrealLuaInterface::Callr<ret>("Call", functionname, ptr, ##__VA_ARGS__);
#define LuaStaticCall(functionname, ...)	UUnrealLuaInterface::StaticCall(functionname, this, ##__VA_ARGS__);
#define LuaStaticCallr(ret, functionname, ...)	UUnrealLuaInterface::StaticCallr<ret>(functionname, this, ##__VA_ARGS__);

#if WITH_EDITOR

#define LuaCtorExist(classname, ...) { \
										if(UUnrealLuaInterface::IsEnableLua() && UUnrealLuaInterface::IsExistFile(this, classname)) \
										{\
											UUnrealLuaInterface::Call("Ctor", classname, this, ##__VA_ARGS__);\
										}\
									 }
#define ExistThenCallAndReturn(functionname, ptr, ...) if (UUnrealLuaInterface::IsEnableLua() && UUnrealLuaInterface::IsExistFunc(ptr, #functionname)) { UUnrealLuaInterface::Call("Call", #functionname, ptr, ##__VA_ARGS__);return; }
#define ExistThenCallAndReturn_r(Ret, functionname, ptr, ...) if (UUnrealLuaInterface::IsEnableLua() && UUnrealLuaInterface::IsExistFunc(ptr, #functionname)){ return UUnrealLuaInterface::Callr<Ret>("Call", #functionname, ptr, ##__VA_ARGS__);}

#else

#define LuaCtorExist(classname, ...) { \
										static bool __buselua__ = UUnrealLuaInterface::IsEnableLua() && UUnrealLuaInterface::IsExistFile(this, classname);\
										if(__buselua__) \
										{\
											UUnrealLuaInterface::Call("Ctor", classname, this, ##__VA_ARGS__);\
										}\
									 }

#define ExistThenCallAndReturn(functionname, ptr, ...) \
					{\
						static bool __buselua__ = UUnrealLuaInterface::IsEnableLua() && UUnrealLuaInterface::IsExistFunc(ptr, #functionname); \
						if (__buselua__)){ UUnrealLuaInterface::Call("Call", #functionname, ptr, ##__VA_ARGS__);return;} \
					}

#define ExistThenCallAndReturn_r(Ret, functionname, ptr, ...) \
					{\
						static bool __buselua__ = UUnrealLuaInterface::IsEnableLua() && UUnrealLuaInterface::IsExistFunc(ptr, #functionname); \
						if (__buselua__ ){ return UUnrealLuaInterface::Callr<Ret>("Call", #functionname, ptr, ##__VA_ARGS__);} \
					}
#endif

#define LuaCall_AllState(functionname, ...) \
TArray<lua_State*> States = LuaContextMgr::Get().GetAllLuaState();\
for (lua_State* L : States)\
{\
	UTableUtil::call_with_state(L, "Call", functionname, ##__VA_ARGS__);\
}\

#define LuaCall_State(inL, functionname, ...)\
UTableUtil::call_with_state(inL, "Call", functionname, ##__VA_ARGS__);


#define LuaCallr_State(inL, ret, functionname, ...)\
UTableUtil::callr_with_state<ret>(inL, "Call", functionname, ##__VA_ARGS__);

#define LuaStaticCallr_State(inL, ret, functionname, ...)\
UTableUtil::callr_with_state<ret>(inL, functionname, ##__VA_ARGS__);

UCLASS()
class LUAPLUGINRUNTIME_API UUnrealLuaInterface : public UObject
{
	GENERATED_BODY()
private:
	static bool IsLuaEnable;
public:
	static int32 bEnableLuaCVar;

	static lua_State* GetLuaStateByIns(const UObject* Ins);
	static LuaContext& GetLuaContextByIns(const UObject* Ins);

	static void ShutdownAllState();
	static void ShutdownSpecifyGameInsLuaState(const UGameInstance* GameIns);
	static void InitState(const UObject* Ins)
	{
		GetLuaStateByIns(Ins);
	}

	template<class... T>
	static void Call(const char* FuncName, const char* ClassName, const UObject* WorldContext, const T&... Args)
	{
		if (IsEnableLua())
		{
			lua_State* inL = GetLuaStateByIns(WorldContext);
			UTableUtil::call_with_state(inL, FuncName, ClassName, WorldContext, Args...);
		}
	}

	template<class ret, class... T>
	static ret Callr(const char* FuncName, const char* ClassName, const UObject* WorldContext, const T&... Args)
	{
		if (IsEnableLua())
		{
			lua_State* inL = GetLuaStateByIns(WorldContext);
			return UTableUtil::callr_with_state<ret>(inL, FuncName, ClassName, WorldContext, Args...);
		}
		else
		{
			return *(ret*)(1);
		}
	}

	template<class... T>
	static void StaticCall(const char* FuncName, const UObject* WorldContext, const T&... Args)
	{
		if (IsEnableLua())
		{
			lua_State* inL = GetLuaStateByIns(WorldContext);
			UTableUtil::call_with_state(inL, FuncName, Args...);
		}
	}

	template<class ret, class... T>
	static ret StaticCallr(const char* FuncName, const UObject* WorldContext, const T&... Args)
	{
		if (IsEnableLua())
		{
			lua_State* inL = GetLuaStateByIns(WorldContext);
			return UTableUtil::callr_with_state<ret>(inL, FuncName, Args...);
		}
		else
		{
			return ret();
		}
	}

	static bool IsExistFunc(const UObject* Ptr, const char* FuncName);
	static bool IsExistFile(const UObject* Ptr, const char* FileName);

	static inline bool IsEnableLua() 
	{ 
		return IsLuaEnable && bEnableLuaCVar;
	}

	static void EnableLua(bool bIsEnable)
	{
		IsLuaEnable = bIsEnable;
	}

	static void ReplaceUe4Function(const UObject* Obj, const char* LuaClassPath);
	UFUNCTION()
		static void DoString(UObject* Obj, FString Str);

	UFUNCTION()
		static void DoFile(UObject* Obj, FString Str);
};
