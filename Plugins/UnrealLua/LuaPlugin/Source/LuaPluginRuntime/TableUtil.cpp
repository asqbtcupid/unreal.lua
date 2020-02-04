
#include "TableUtil.h"
#include "LuaScript.h"
#include "Paths.h"
#include "TextProperty.h"
#include "LuaMapHelper.h"
#include "../Launch/Resources/Version.h"
#include "BPAndLuaBridge.h"
#include "Engine/World.h"
#include "Engine/LevelScriptActor.h"
#include "VoidPtrStruct.h"
#include "UnrealLua.h"

#if WITH_EDITOR
#include "Editor.h"
#include "SNotificationList.h"
#include "NotificationManager.h"
#endif
#include "GameDelegates.h"
#include "Platform.h"
#include "NativeLuaFunc.h"
#include "LuaDelegateSingle.h"
#include "CoreDelegates.h"

DEFINE_LOG_CATEGORY(LuaLog);
#define SetTableFunc(inL, index, FuncName, Func) lua_pushstring(inL, FuncName);\
											lua_pushcfunction(inL, Func);\
											lua_rawset(inL, index)

#define SetTableClosureOneUpvalue(inL, index, FuncName, Upvalue1, Func) lua_pushstring(inL, FuncName);\
											pushuobject(inL, Upvalue1);\
											lua_pushcclosure(inL, Func, 1);\
											lua_rawset(inL, index)

FORCEINLINE uint64 GetPropertyFlag(UProperty* Property)
{
	UClass* PropertyClass = Property->GetClass();
	uint64 CastFlag = uint64(PropertyClass->ClassCastFlags);
	CastFlag = CastFlag & (CASTCLASS_UByteProperty | CASTCLASS_UIntProperty | CASTCLASS_UInt8Property
		| CASTCLASS_UUInt64Property | CASTCLASS_UUInt32Property | CASTCLASS_UUInt16Property
		| CASTCLASS_UInt64Property | CASTCLASS_UInt16Property | CASTCLASS_UBoolProperty
		| CASTCLASS_UNameProperty | CASTCLASS_UStrProperty | CASTCLASS_UTextProperty
		| CASTCLASS_UDoubleProperty | CASTCLASS_UFloatProperty | CASTCLASS_UObjectProperty | CASTCLASS_UStructProperty);
	return CastFlag;
}

FORCEINLINE void CopyTableForLua(lua_State*inL)
{
	lua_pushnil(inL);
	while (lua_next(inL, -3))
	{
		lua_pushvalue(inL, -2);
		lua_pushvalue(inL, -2);
		lua_rawset(inL, -5);
		lua_pop(inL, 1);
	}
}

template<class KeyType, class ValueType>
void LuaRawSet(lua_State* inL, int TableIndex, const KeyType& Key, const ValueType& Value)
{
	UTableUtil::pushall(inL, Key, Value);
	ue_lua_rawset(inL, TableIndex - 2);
}
const int ChildMaxCount = 100000;
TMap<lua_State*, TMap<UObject*, TMap<FString, UClass*>>> UTableUtil::NeedGcBpClassName;
TMap<lua_State*, TMap<UClass*, TSharedPtr<FTCHARToUTF8>>> UTableUtil::HasAddUClass;
TMap<lua_State*, TSet<FString>> UTableUtil::HasRequire;
FLuaBugReport UTableUtil::LuaBugReportDelegate;
TMap<UObject*, TSet<lua_State*>> UTableUtil::ObjectReferencedLuaState;
TSet<lua_State*> UTableUtil::HasShutdownLuaState;
TMap<lua_State*, TArray<struct LuaBaseBpInterface*>> UTableUtil::ExistBpInterfaceForState;
TMap<UFunction*, struct MuldelegateBpInterface*> UTableUtil::MultiDlgInterfaces;
TMap<FString, TMap<FString, TArray<UnrealLuaBlueFunc>>> UTableUtil::ClassOverloadFuncs;
TMap<FString, int32> UTableUtil::ClassDefineTypeInLua;
TMap<int32, TArray<int32>> UTableUtil::ChildsParentTypesInLua;
TMap<int32, bool> UTableUtil::ClassRelationShip;
TMap<FString, UserDefinedClassConfig> UTableUtil::HasAddedUserDefinedClass;
TMap<FString, int32> UTableUtil::HasInitClassType;
TMap<lua_State*, TArray<TArray<UnrealLuaBlueFunc>*>> UTableUtil::OverloadFuncsCandidate;

#if LuaDebug
TMap<lua_State*, TMap<FString, int>> UTableUtil::countforgc;
void UTableUtil::AddGcCount(lua_State*inL, const FString& classname)
{
	lua_geti(inL, LUA_REGISTRYINDEX, LUA_RIDX_MAINTHREAD);
	lua_State* MainState = lua_tothread(inL, -1);

	if (countforgc[MainState].Contains(classname))
		countforgc[MainState][classname]++;
	else
		countforgc[MainState].Add(classname, 1);
	lua_pop(inL, 1);
}


void UTableUtil::SubGcCount(lua_State*inL, const FString& classname)
{
	lua_geti(inL, LUA_REGISTRYINDEX, LUA_RIDX_MAINTHREAD);
	lua_State* MainState = lua_tothread(inL, -1);

	countforgc[MainState][classname]--;
	
	lua_pop(inL, 1);
}
#endif

static int32 GcCheckActorRef = 1;
FAutoConsoleVariableRef CVarLuaStrongCheckActorRef(
	TEXT("r.Lua.CheckActorRef"),
	GcCheckActorRef,
	TEXT("0: no check ")
	TEXT("1: check \n"),
	ECVF_Default);

bool UTableUtil::HasInit = false;
TMap<FString, UUserDefinedStruct*> UTableUtil::bpname2bpstruct;
TMap<FString, FString> UTableUtil::GlueClassAlias;
TMap<FString, TArray<FString>> UTableUtil::ClassBaseClass;
TMap<FString, TMap<FString, UnrealLuaBlueFunc>>  UTableUtil::UserDefineGlue;
TMap<FString, TMap<FString, UnrealLuaBlueFunc>> UTableUtil::ExpandClassGlue;
TMap<FString, TArray<EnumGlueStruct> > UTableUtil::ManualEnumGlue;
TMap<UClass*, TFunction<void(lua_State*, UProperty*, const void*)> > UTableUtil::PropertyClassToPushFuncMap;
TMap<UClass*, TFunction<void(lua_State*, int, UProperty*, void*)> > UTableUtil::PropertyClassToPopFuncMap;


#define MAP_PROPERTY_PUSH_AND_POP_FUNC(PropertyType) PropertyClassToPushFuncMap.Add(PropertyType::StaticClass(), pushproperty_##PropertyType);\
													PropertyClassToPopFuncMap.Add(PropertyType::StaticClass(), popproperty_##PropertyType);

FLuaInitDelegates& UTableUtil::GetInitDelegates()
{
	static FLuaInitDelegates Delegates;
	return Delegates;
}


void UTableUtil::Init()
{
	GetInitDelegates().Broadcast();
	HasInit = true;
}

FLuaOnPowerStateDelegate& UTableUtil::GetOnPowerStateDelegate()
{
	static FLuaOnPowerStateDelegate TheDelegate;
	return TheDelegate;
}

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

void UTableUtil::useCustomLoader(lua_State *inL)
{
	lua_getglobal(inL, "package");
	lua_getglobal(inL, "table");
	lua_getfield(inL, -1, "insert");//table.insert
#ifdef USE_LUA53 
	lua_getfield(inL, -3, "searchers");//package.loaders  
#else
	lua_getfield(inL, -3, "loaders");//package.loaders  
#endif

	lua_pushinteger(inL, 2);
	lua_pushcfunction(inL, CustomLuaLoader_SearchSaved);
	lua_call(inL, 3, 0);

	lua_getfield(inL, -1, "insert");//table.insert
#ifdef USE_LUA53 
	lua_getfield(inL, -3, "searchers");//package.loaders  
#else
	lua_getfield(inL, -3, "loaders");//package.loaders  
#endif
	lua_pushinteger(inL, 4);
	lua_pushcfunction(inL, CustomLuaLoader);
	lua_call(inL, 3, 0);

	lua_pop(inL, lua_gettop(inL));
}

void UTableUtil::useCustomLoader(UObject *inL)
{
	useCustomLoader((lua_State*)inL);
}

void UTableUtil::MapPropertyToPushPopFunction()
{
	MAP_PROPERTY_PUSH_AND_POP_FUNC(UBoolProperty)
	MAP_PROPERTY_PUSH_AND_POP_FUNC(UIntProperty)
	MAP_PROPERTY_PUSH_AND_POP_FUNC(UInt8Property)
		MAP_PROPERTY_PUSH_AND_POP_FUNC(UUInt16Property)
		MAP_PROPERTY_PUSH_AND_POP_FUNC(UInt16Property)
		MAP_PROPERTY_PUSH_AND_POP_FUNC(UUInt32Property)
	MAP_PROPERTY_PUSH_AND_POP_FUNC(UInt64Property)
	MAP_PROPERTY_PUSH_AND_POP_FUNC(UUInt64Property)
	MAP_PROPERTY_PUSH_AND_POP_FUNC(UFloatProperty)
	MAP_PROPERTY_PUSH_AND_POP_FUNC(UDoubleProperty)
		MAP_PROPERTY_PUSH_AND_POP_FUNC(UObjectPropertyBase)
		MAP_PROPERTY_PUSH_AND_POP_FUNC(UObjectProperty)
		MAP_PROPERTY_PUSH_AND_POP_FUNC(UClassProperty)
	MAP_PROPERTY_PUSH_AND_POP_FUNC(UStrProperty)
	MAP_PROPERTY_PUSH_AND_POP_FUNC(UNameProperty)
	MAP_PROPERTY_PUSH_AND_POP_FUNC(UTextProperty)
	MAP_PROPERTY_PUSH_AND_POP_FUNC(UByteProperty)
	MAP_PROPERTY_PUSH_AND_POP_FUNC(UEnumProperty)
	MAP_PROPERTY_PUSH_AND_POP_FUNC(UStructProperty)
#if ENGINE_MINOR_VERSION >= 23
	MAP_PROPERTY_PUSH_AND_POP_FUNC(UMulticastInlineDelegateProperty)
	MAP_PROPERTY_PUSH_AND_POP_FUNC(UMulticastSparseDelegateProperty)
#else
	MAP_PROPERTY_PUSH_AND_POP_FUNC(UMulticastDelegateProperty)
#endif
	MAP_PROPERTY_PUSH_AND_POP_FUNC(UDelegateProperty)
	MAP_PROPERTY_PUSH_AND_POP_FUNC(UWeakObjectProperty)
	MAP_PROPERTY_PUSH_AND_POP_FUNC(UArrayProperty)
	MAP_PROPERTY_PUSH_AND_POP_FUNC(UMapProperty)
	MAP_PROPERTY_PUSH_AND_POP_FUNC(USetProperty)
	MAP_PROPERTY_PUSH_AND_POP_FUNC(UInterfaceProperty)
}

void UTableUtil::PowerTheState(lua_State* inL)
{
	InitClassInheritRelationship();
	HasShutdownLuaState.Remove(inL);
	TSet<FString>& HasAdded = HasRequire.FindOrAdd(inL);
	HasAddUClass.FindOrAdd(inL);
	HasAdded.Reset();
#if LuaDebug
	countforgc.FindOrAdd(inL);
#endif
	//set table for index exist userdata
	lua_newtable(inL);
	lua_newtable(inL);
	lua_pushstring(inL, "v");
	lua_setfield(inL, -2, "__mode");
	lua_setmetatable(inL, -2);
	lua_seti(inL, LUA_REGISTRYINDEX, ExistTableIndex);
	lua_pushinteger(inL, 0);
	lua_seti(inL, LUA_REGISTRYINDEX, ExistTableIndex+1);
	lua_pushinteger(inL, 0);
	lua_seti(inL, LUA_REGISTRYINDEX, ExistTableIndex+2);
	lua_pushinteger(inL, 0);
	lua_seti(inL, LUA_REGISTRYINDEX, ExistTableIndex+3);

	lua_newtable(inL);
	lua_setfield(inL, LUA_REGISTRYINDEX, "_existfirststruct");

	//when lua has correspond table of the ins, push the table
	lua_pushcfunction(inL, SetExistTable);
	lua_setglobal(inL, "_setexisttable");

	lua_newtable(inL);
	lua_setglobal(inL, "NeedGcBpClassName");

#if PLATFORM_WINDOWS
	push(inL, "PLATFORM_WINDOWS");
	lua_setglobal(inL, "_platform");
#endif // PLATFORM_WINDOWS
#if  WITH_EDITOR
	push(inL, true);
	lua_setglobal(inL, "_WITH_EDITOR");
#endif
	lua_pushcfunction(inL, GlobalLoadObject);
	lua_setglobal(inL, "GlobalLoadObject");
	lua_pushcfunction(inL, GlobalLoadClass);
	lua_setglobal(inL, "GlobalLoadClass");

	requirecpp(inL, "UTableUtil");
	GetOnPowerStateDelegate().Broadcast(inL);
}

void UTableUtil::PowerTheState(UObject* inL)
{
	PowerTheState((lua_State*)inL);
}

void UTableUtil::SetAIsBParents(const TArray<int32>& A, int32 B)
{
	for (int Parent : A)
	{
		if (Parent != B)
		{
			ClassRelationShip.Add(Parent*ChildMaxCount + B, true);
			TArray<int> GrandParents = ChildsParentTypesInLua.FindOrAdd(Parent);
			SetAIsBParents(GrandParents, B);
		}
	}
}

void UTableUtil::InitClassInheritRelationship()
{
	for (auto& Pairs : ChildsParentTypesInLua)
	{
		SetAIsBParents(Pairs.Value, Pairs.Key);
	}
}

void UTableUtil::ShutdownTheState(lua_State* inL)
{
	HasShutdownLuaState.Add(inL);
	TArray<struct LuaBaseBpInterface*>& BpInterfaces = ExistBpInterfaceForState.FindOrAdd(inL);
	for (auto ThePair : BpInterfaces)
	{
		delete ThePair;
	}
	ExistBpInterfaceForState.Remove(inL);

	for (TArray<UnrealLuaBlueFunc>* OverloadCandidatesOfState : OverloadFuncsCandidate.FindOrAdd(inL))
	{
		delete OverloadCandidatesOfState;
	}
	OverloadFuncsCandidate.Remove(inL);
#if LuaDebug
	for (auto& count : countforgc[inL])
	{
		if (count.Value != 0)
		{
			ensureAlwaysMsgf(0, TEXT("gc error:%s %d"), *count.Key, count.Value);
		}
	}
	countforgc.Remove(inL);
#endif
}

struct LuaBaseBpInterface* UTableUtil::GetBpPropertyInterface(lua_State*inL, UProperty* BpField)
{
	lua_State* MainState = GetMainThread(inL);
	TArray<LuaBaseBpInterface*>& StatesInterface = ExistBpInterfaceForState.FindOrAdd(MainState);
	LuaBaseBpInterface* BpInterface = CreatePropertyInterfaceRaw(MainState, BpField);
	StatesInterface.Add(BpInterface);
	return BpInterface;
}

struct LuaBaseBpInterface* UTableUtil::GetBpFuncInterface(lua_State*inL, UFunction* BpFunction)
{
	lua_State* MainState = GetMainThread(inL);
	TArray<LuaBaseBpInterface*>& StatesInterface = ExistBpInterfaceForState.FindOrAdd(MainState);
	LuaBaseBpInterface* BpInterface = new LuaUFunctionInterface(MainState, BpFunction);
	StatesInterface.Add(BpInterface);
	return BpInterface;
}

struct MuldelegateBpInterface* UTableUtil::GetMultiDlgInterface(UFunction* SigFunction)
{
	if (MuldelegateBpInterface** Interface= MultiDlgInterfaces.Find(SigFunction))
	{
		return *Interface;
;	}
	else
	{
		MuldelegateBpInterface* NewInterface = new MuldelegateBpInterface(SigFunction);
		MultiDlgInterfaces.Add(SigFunction, NewInterface);
		return NewInterface;
	}
}

bool UTableUtil::IsStateShutdown(lua_State*inL)
{
	return HasShutdownLuaState.Contains(inL);
}


void UTableUtil::initmeta(lua_State *inL, const char* classname, bool bIsStruct, bool bNeedGc /*= true*/, const char* luaclassname/*=nullptr*/)
{
	if (bIsStruct)
	{
		UScriptStruct* StructClass = FindObject<UScriptStruct>(ANY_PACKAGE, *(FString(classname).RightChop(1)));
		lua_newtable(inL);
		if (StructClass)
			AddFuncToTable(inL, -2, "__index", index_struct_func_with_class_with_glue<false>, LuaSpace::StackValue(-2), LuaSpace::StackValue(-2), StructClass);
		else
			AddFuncToTable(inL, -2, "__index", index_struct_func, LuaSpace::StackValue(-2), LuaSpace::StackValue(-2));
		lua_pop(inL, 1);
		lua_newtable(inL);
		if (StructClass)
			AddFuncToTable(inL, -2, "__newindex", newindex_struct_Func_with_class_with_glue<false>, LuaSpace::StackValue(-2), LuaSpace::StackValue(-2), StructClass);
		else
			AddFuncToTable(inL, -2, "__newindex", newindex_struct_func, LuaSpace::StackValue(-2), LuaSpace::StackValue(-2));
		lua_pop(inL, 1);
		if (bNeedGc)
		{
			AddFuncToTable(inL, -1, "__gc", struct_gcfunc);
		}
	}
	else
	{
		AddFuncToTable(inL, -1, "__gc", uobjcet_gcfunc);
	}
	AddFuncToTable(inL, -1, "Table", serialize_table);
}

void UTableUtil::init_refelction_native_uclass_meta(lua_State* inL, const char* classname, UClass* TheClass)
{
 	lua_newtable(inL);
	UClass* Class = TheClass;

	AddFuncToTable(inL, -1, "Cast", GeneralCast, Class);
	AddFuncToTable(inL, -1, "LoadClass", GeneralLoadClass, Class);
	AddFuncToTable(inL, -1, "LoadObject", GeneralLoadObject, Class);
	AddFuncToTable(inL, -1, "Class", GeneralGetClass, Class);
	AddFuncToTable(inL, -1, "StaticClass", GeneralGetClass, Class);
	AddFuncToTable(inL, -1, "FClassFinder", GeneralFClassFinder, Class);
	AddFuncToTable(inL, -1, "__gc", uobjcet_gcfunc);

	AddFuncToTable(inL, -1, "New", GeneralNewObject, Class);
	AddFuncToTable(inL, -1, "NewObject", GeneralNewObject, Class);
	AddFuncToTable(inL, -1, "GetDefaultObject", GeneralGetDefaultObject, Class);
	AddFuncToTable(inL, -1, "Destroy", EnsureDestroy);
	AddFuncToTable(inL, -1, "GetClass", UObject_GetClass);
	AddFuncToTable(inL, -1, "GetName", UObject_GetName);
	AddFuncToTable(inL, -1, "GetOuter", UObject_GetOuter);
	AddFuncToTable(inL, -1, "LuaGet_ClassPrivate", UObject_GetClass);
	AddFuncToTable(inL, -1, "LuaGet_NamePrivate", UObject_GetName);
	AddFuncToTable(inL, -1, "LuaGet_OuterPrivate", UObject_GetOuter);
	AddFuncToTable(inL, -1, "IsPendingKill", UObject_IsPendingKill);
	AddFuncToTable(inL, -1, "MarkPendingKill", UObject_MarkPendingKill);
	AddFuncToTable(inL, -1, "AddToRoot", UObject_AddToRoot);
	AddFuncToTable(inL, -1, "RemoveFromRoot", UObject_RemoveFromRoot);
	// 	AddFuncToTable(inL, -1, "GetAllProperty", GetUObjectAllProperty);
	AddFuncToTable(inL, -1, "ReloadConfig", GeneralReloadConfig);
	AddFuncToTable(inL, -1, "LoadConfig", GeneralLoadConfig);
	AddFuncToTable(inL, -1, "SaveConfig", GeneralSaveConfig);
	LuaRawSet(inL, -1, "classname", classname);
	LuaRawSet(inL, -1, "IsObject", true);
	UClass* MeOrParentClass = TheClass;
	TSet<FString> HasAddFunc;
	bool HasGlueFunctionForIndex = false;
	bool HasGlueFunctionForNewIndex = false;

	TMap<FString, UnrealLuaBlueFunc> StaticPropertyFuncMap;
	while (MeOrParentClass)
	{
		if (MeOrParentClass->HasAnyClassFlags(CLASS_Native))
		{
			FString NameOfExpandClassToCheck = FString::Printf(TEXT("%s%s"), MeOrParentClass->GetPrefixCPP(), *MeOrParentClass->GetName());
			if (auto* ExpandFunc = ExpandClassGlue.Find(NameOfExpandClassToCheck))
			{
				for (auto& Pairs : *ExpandFunc)
				{
					if (!HasAddFunc.Contains(Pairs.Key))
					{
						if (Pairs.Value.ExportFlag & RF_IsStaticProperty)
							StaticPropertyFuncMap.Add(Pairs.Key, Pairs.Value);
						AddFuncToTable(inL, -1, TCHAR_TO_UTF8(*Pairs.Key), Pairs.Value.func);
						HasAddFunc.Add(Pairs.Key);
						HasGlueFunctionForIndex = true;
					}
				}
			}
			TMap<FString, TArray<UnrealLuaBlueFunc>>& OverloadFuncs = ClassOverloadFuncs.FindOrAdd(NameOfExpandClassToCheck);
			BuildOverLoadFuncTree(inL, OverloadFuncs);
		}
		MeOrParentClass = MeOrParentClass->GetSuperClass();
	}
	AddStaticMetaToTable(inL, StaticPropertyFuncMap, Class, true);

	lua_newtable(inL);
	CopyTableForLua(inL);
	auto AddGetBpProperty = [&]() {
		for (TFieldIterator<UProperty> PropertyIt(Class); PropertyIt; ++PropertyIt)
		{
			UProperty* Property = *PropertyIt;
			FString PropertyName = Property->GetName();
			void* LuaProperty = GetBpPropertyInterface(inL, Property);
			auto SetFunc = [=](const FString& Name, int TableIndex)
			{
				bool bPushFunction = false;
				push(inL, Name);
				lua_pushlightuserdata(inL, LuaProperty);
				if (Property->IsA(UBoolProperty::StaticClass()))
					lua_pushcclosure(inL, BpPropertyGetterName(UBoolProperty), 1);
				else if (Property->IsA(UIntProperty::StaticClass()))
					lua_pushcclosure(inL, BpPropertyGetterName(UIntProperty), 1);
				else if (Property->IsA(UInt8Property::StaticClass()))
					lua_pushcclosure(inL, BpPropertyGetterName(UInt8Property), 1);
				else if (Property->IsA(UUInt16Property::StaticClass()))
					lua_pushcclosure(inL, BpPropertyGetterName(UUInt16Property), 1);
				else if (Property->IsA(UInt16Property::StaticClass()))
					lua_pushcclosure(inL, BpPropertyGetterName(UInt16Property), 1);
				else if (Property->IsA(UUInt32Property::StaticClass()))
					lua_pushcclosure(inL, BpPropertyGetterName(UUInt32Property), 1);
				else if (Property->IsA(UInt64Property::StaticClass()))
					lua_pushcclosure(inL, BpPropertyGetterName(UInt64Property), 1);
				else if (Property->IsA(UUInt64Property::StaticClass()))
					lua_pushcclosure(inL, BpPropertyGetterName(UUInt64Property), 1);
				else if (Property->IsA(UFloatProperty::StaticClass()))
					lua_pushcclosure(inL, BpPropertyGetterName(UFloatProperty), 1);
				else if (Property->IsA(UDoubleProperty::StaticClass()))
					lua_pushcclosure(inL, BpPropertyGetterName(UDoubleProperty), 1);
				else if (Property->IsA(UObjectPropertyBase::StaticClass()))
					lua_pushcclosure(inL, BpPropertyGetterName(UObjectPropertyBase), 1);
				else if (Property->IsA(UObjectProperty::StaticClass()))
					lua_pushcclosure(inL, BpPropertyGetterName(UObjectProperty), 1);
				else if (Property->IsA(UClassProperty::StaticClass()))
					lua_pushcclosure(inL, BpPropertyGetterName(UClassProperty), 1);
				else if (Property->IsA(UStrProperty::StaticClass()))
					lua_pushcclosure(inL, BpPropertyGetterName(UStrProperty), 1);
				else if (Property->IsA(UNameProperty::StaticClass()))
					lua_pushcclosure(inL, BpPropertyGetterName(UNameProperty), 1);
				else if (Property->IsA(UTextProperty::StaticClass()))
					lua_pushcclosure(inL, BpPropertyGetterName(UTextProperty), 1);
				else if (Property->IsA(UByteProperty::StaticClass()))
					lua_pushcclosure(inL, BpPropertyGetterName(UByteProperty), 1);
				else if (Property->IsA(UEnumProperty::StaticClass()))
					lua_pushcclosure(inL, BpPropertyGetterName(UEnumProperty), 1);
				else if (Property->IsA(UStructProperty::StaticClass()))
				{
					lua_newtable(inL);
					lua_newtable(inL);
					lua_pushstring(inL, "k");
					lua_setfield(inL, -2, "__mode");
					lua_setmetatable(inL, -2);
					lua_pushcclosure(inL, BpPropertyGetterName(UStructProperty), 2);
					bPushFunction = true;
				}
				else if (Property->IsA(UMulticastDelegateProperty::StaticClass())) 
				{
					lua_newtable(inL);
					lua_newtable(inL);
					lua_pushstring(inL, "k");
					lua_setfield(inL, -2, "__mode");
					lua_setmetatable(inL, -2);
#if ENGINE_MINOR_VERSION >= 23
					if(Property->IsA(UMulticastInlineDelegateProperty::StaticClass()))
						lua_pushcclosure(inL, BpPropertyGetterName(UMulticastInlineDelegateProperty), 2);
					else
						lua_pushcclosure(inL, BpPropertyGetterName(UMulticastSparseDelegateProperty), 2);
#else
					lua_pushcclosure(inL, BpPropertyGetterName(UMulticastDelegateProperty), 2);
#endif
					bPushFunction = true;
				}
				else if (Property->IsA(UDelegateProperty::StaticClass()))
				{
					lua_newtable(inL);
					lua_newtable(inL);
					lua_pushstring(inL, "k");
					lua_setfield(inL, -2, "__mode");
					lua_setmetatable(inL, -2);
					lua_pushcclosure(inL, BpPropertyGetterName(UDelegateProperty), 2);
					bPushFunction = true;
				}
				else if (Property->IsA(UWeakObjectProperty::StaticClass()))
					lua_pushcclosure(inL, BpPropertyGetterName(UWeakObjectProperty), 1);
				else if (Property->IsA(UArrayProperty::StaticClass()))
					lua_pushcclosure(inL, BpPropertyGetterName(UArrayProperty), 1);
				else if (Property->IsA(UMapProperty::StaticClass()))
					lua_pushcclosure(inL, BpPropertyGetterName(UMapProperty), 1);
				else if (Property->IsA(USetProperty::StaticClass()))
					lua_pushcclosure(inL, BpPropertyGetterName(USetProperty), 1);
				else if (Property->IsA(UInterfaceProperty::StaticClass()))
					lua_pushcclosure(inL, BpPropertyGetterName(UInterfaceProperty), 1);
				else
				{
					ensureAlwaysMsgf(0, TEXT("Bug"));
					lua_pushcclosure(inL, BpStructGetProp, 1);
				}
				push(inL, "LuaGet_" + Name);
				lua_pushvalue(inL, -2);
				lua_rawset(inL, TableIndex - 3);
				if (bPushFunction)
				{
					lua_createtable(inL, 2, 0);

					lua_createtable(inL, 0, 10);
					lua_createtable(inL, 0, 1);
					lua_pushstring(inL, "k");
					lua_setfield(inL, -2, "__mode");
					lua_setmetatable(inL, -2);
					lua_rawseti(inL, -2, 2);

					lua_pushvalue(inL, -2);
					lua_rawseti(inL, -2, 1);
					lua_remove(inL, -2);
					lua_rawset(inL, TableIndex);
				}
				else
				{
					lua_pop(inL, 1);
					*(void**)lua_newuserdata(inL, sizeof(void *)) = LuaProperty;
					lua_rawset(inL, TableIndex);
				}
			};
			if(LuaProperty)
				SetFunc(PropertyName, -3);
		}
		TSet<FString> HasAddFunc;
		UClass* MeOrParentClass = TheClass;
		while (MeOrParentClass)
		{
			if (MeOrParentClass->HasAnyClassFlags(CLASS_Native))
			{
				FString NameOfExpandClassToCheck = FString::Printf(TEXT("%s%s"), MeOrParentClass->GetPrefixCPP(), *MeOrParentClass->GetName());
				if (auto* ExpandFunc = ExpandClassGlue.Find(NameOfExpandClassToCheck))
				{
					for (auto& Pairs : *ExpandFunc)
					{
						FString FuncName = Pairs.Key;
						if (!HasAddFunc.Contains(FuncName))
						{
							HasGlueFunctionForIndex = true;
							HasAddFunc.Add(FuncName);
							if ((Pairs.Value.ExportFlag & ELuaFuncExportFlag::RF_GetPropertyFunc)!=0)
							{
								if ((Pairs.Value.ExportFlag & ELuaFuncExportFlag::RF_IsStructProperty) != 0)
								{
									FString FuncName = Pairs.Key.RightChop(7);
									lua_pushstring(inL, TCHAR_TO_UTF8(*FuncName));

									lua_createtable(inL, 2, 0);

									lua_createtable(inL, 0, 10);
									lua_createtable(inL, 0, 1);
									lua_pushstring(inL, "k");
									lua_setfield(inL, -2, "__mode");
									lua_setmetatable(inL, -2);
									lua_rawseti(inL, -2, 2);

									lua_pushcfunction(inL, Pairs.Value.func);
									lua_rawseti(inL, -2, 1);
									lua_rawset(inL, -3);
								}
								else
								{
									FString FuncName = Pairs.Key.RightChop(7);
									lua_pushstring(inL, TCHAR_TO_UTF8(*FuncName));
									lua_pushlightuserdata(inL, (void*)Pairs.Value.func);
									lua_rawset(inL, -3);
								}
							}
						}
					}
				}
			}
			MeOrParentClass = MeOrParentClass->GetSuperClass();
		}
	};
	AddGetBpProperty();
	if (HasGlueFunctionForIndex)
		AddFuncToTable(inL, -2, "__index", index_reflection_uobject_func_withexpand<true>, LuaSpace::StackValue(-2), LuaSpace::StackValue(-2), Class);
	else
		AddFuncToTable(inL, -2, "__index", index_reflection_uobject_func_withexpand<false>, LuaSpace::StackValue(-2), LuaSpace::StackValue(-2), Class);

	lua_pop(inL, 1);

	lua_newtable(inL);
	auto AddSetBpProperty = [&]() {
		for (TFieldIterator<UProperty> PropertyIt(Class); PropertyIt; ++PropertyIt)
		{
			UProperty* Property = *PropertyIt;
			FString PropertyName = Property->GetName();
			void* LuaProperty = GetBpPropertyInterface(inL, Property);
			auto SetFunc = [=](const FString& Name, int TableIndex)
			{
				bool bPushFunction = false;
				push(inL, Name);
				lua_pushlightuserdata(inL, LuaProperty);
				if (Property->IsA(UBoolProperty::StaticClass()))
					lua_pushcclosure(inL, BpPropertySetterName(UBoolProperty), 1);
				else if (Property->IsA(UIntProperty::StaticClass()))
					lua_pushcclosure(inL, BpPropertySetterName(UIntProperty), 1);
				else if (Property->IsA(UInt8Property::StaticClass()))
					lua_pushcclosure(inL, BpPropertySetterName(UInt8Property), 1);
				else if (Property->IsA(UUInt16Property::StaticClass()))
					lua_pushcclosure(inL, BpPropertySetterName(UUInt16Property), 1);
				else if (Property->IsA(UInt16Property::StaticClass()))
					lua_pushcclosure(inL, BpPropertySetterName(UInt16Property), 1);
				else if (Property->IsA(UUInt32Property::StaticClass()))
					lua_pushcclosure(inL, BpPropertySetterName(UUInt32Property), 1);
				else if (Property->IsA(UInt64Property::StaticClass()))
					lua_pushcclosure(inL, BpPropertySetterName(UInt64Property), 1);
				else if (Property->IsA(UUInt64Property::StaticClass()))
					lua_pushcclosure(inL, BpPropertySetterName(UUInt64Property), 1);
				else if (Property->IsA(UFloatProperty::StaticClass()))
					lua_pushcclosure(inL, BpPropertySetterName(UFloatProperty), 1);
				else if (Property->IsA(UDoubleProperty::StaticClass()))
					lua_pushcclosure(inL, BpPropertySetterName(UDoubleProperty), 1);
				else if (Property->IsA(UObjectPropertyBase::StaticClass()))
					lua_pushcclosure(inL, BpPropertySetterName(UObjectPropertyBase), 1);
				else if (Property->IsA(UObjectProperty::StaticClass()))
					lua_pushcclosure(inL, BpPropertySetterName(UObjectProperty), 1);
				else if (Property->IsA(UClassProperty::StaticClass()))
					lua_pushcclosure(inL, BpPropertySetterName(UClassProperty), 1);
				else if (Property->IsA(UStrProperty::StaticClass()))
					lua_pushcclosure(inL, BpPropertySetterName(UStrProperty), 1);
				else if (Property->IsA(UNameProperty::StaticClass()))
					lua_pushcclosure(inL, BpPropertySetterName(UNameProperty), 1);
				else if (Property->IsA(UTextProperty::StaticClass()))
					lua_pushcclosure(inL, BpPropertySetterName(UTextProperty), 1);
				else if (Property->IsA(UByteProperty::StaticClass()))
					lua_pushcclosure(inL, BpPropertySetterName(UByteProperty), 1);
				else if (Property->IsA(UEnumProperty::StaticClass()))
					lua_pushcclosure(inL, BpPropertySetterName(UEnumProperty), 1);
				else if (Property->IsA(UStructProperty::StaticClass())) 
					lua_pushcclosure(inL, BpPropertySetterName(UStructProperty), 1);
				else if (Property->IsA(UMulticastDelegateProperty::StaticClass()))
				{
#if ENGINE_MINOR_VERSION >= 23
					if (Property->IsA(UMulticastInlineDelegateProperty::StaticClass()))
						lua_pushcclosure(inL, BpPropertySetterName(UMulticastInlineDelegateProperty), 1);
					else
						lua_pushcclosure(inL, BpPropertySetterName(UMulticastSparseDelegateProperty), 1);
#else
					lua_pushcclosure(inL, BpPropertySetterName(UMulticastDelegateProperty), 1);
#endif
				}
				else if (Property->IsA(UDelegateProperty::StaticClass())) 
					lua_pushcclosure(inL, BpPropertySetterName(UDelegateProperty), 1);
				else if (Property->IsA(UWeakObjectProperty::StaticClass()))
					lua_pushcclosure(inL, BpPropertySetterName(UWeakObjectProperty), 1);
				else if (Property->IsA(UArrayProperty::StaticClass()))
					lua_pushcclosure(inL, BpPropertySetterName(UArrayProperty), 1);
				else if (Property->IsA(UMapProperty::StaticClass()))
					lua_pushcclosure(inL, BpPropertySetterName(UMapProperty), 1);
				else if (Property->IsA(USetProperty::StaticClass()))
					lua_pushcclosure(inL, BpPropertySetterName(USetProperty), 1);
				else if (Property->IsA(UInterfaceProperty::StaticClass()))
					lua_pushcclosure(inL, BpPropertySetterName(UInterfaceProperty), 1);
				else
				{
					ensureAlwaysMsgf(0, TEXT("Bug"));
					lua_pushcclosure(inL, BpStructGetProp, 1);
				}
				push(inL, "LuaSet_" + Name);
				lua_pushvalue(inL, -2);
				lua_rawset(inL, TableIndex - 3);
				if (bPushFunction)
					lua_rawset(inL, TableIndex);
				else
				{
					lua_pop(inL, 1);
					*(void**)lua_newuserdata(inL, sizeof(void *)) = LuaProperty;
					lua_rawset(inL, TableIndex);
				}
			};
			if(LuaProperty)
				SetFunc(PropertyName, -3);
		}
		TSet<FString> HasAddFunc;
		UClass* MeOrParentClass = TheClass;
		while (MeOrParentClass)
		{
			if (MeOrParentClass->HasAnyClassFlags(CLASS_Native))
			{
				FString NameOfExpandClassToCheck = FString::Printf(TEXT("%s%s"), MeOrParentClass->GetPrefixCPP(), *MeOrParentClass->GetName());
				if (auto* ExpandFunc = ExpandClassGlue.Find(NameOfExpandClassToCheck))
				{
					for (auto& Pairs : *ExpandFunc)
					{
						FString FuncName = Pairs.Key;
						if (!HasAddFunc.Contains(FuncName))
						{
							HasGlueFunctionForNewIndex = true;
							HasAddFunc.Add(FuncName);
							if ((Pairs.Value.ExportFlag & ELuaFuncExportFlag::RF_SetPropertyFunc)!=0)
							{
								FString FuncName = Pairs.Key.RightChop(7);
								lua_pushstring(inL, TCHAR_TO_UTF8(*FuncName));
								lua_pushlightuserdata(inL, (void*)Pairs.Value.func);
								lua_rawset(inL, -3);
							}
						}
					}
				}
			}
			MeOrParentClass = MeOrParentClass->GetSuperClass();
		}
	};
	AddSetBpProperty();
	if (HasGlueFunctionForNewIndex)
	{
		AddFuncToTable(inL, -2, "__newindex", newindex_reflection_uobject_func_withexpand<true>, LuaSpace::StackValue(-2), LuaSpace::StackValue(-2), Class);
		AddFuncToTable(inL, -2, "__trynewindex", try_newindex_reflection_uobject_func_withexpand<true>, LuaSpace::StackValue(-1), Class);
	}
	else
	{
		AddFuncToTable(inL, -2, "__newindex", newindex_reflection_uobject_func_withexpand<false>, LuaSpace::StackValue(-2), LuaSpace::StackValue(-2), Class);
		AddFuncToTable(inL, -2, "__trynewindex", try_newindex_reflection_uobject_func_withexpand<false>, LuaSpace::StackValue(-1), Class);
	}
	lua_pop(inL, 1);

	lua_setglobal(inL, classname);
}

void UTableUtil::init_reflection_struct_meta(lua_State* inL, const char* structname, UScriptStruct* StructClass, bool bIsNeedGc)
{	
	UScriptStruct* Class = StructClass;
	bool IsBpStruct = StructClass->IsA(UUserDefinedStruct::StaticClass());
	auto* ExpandFunc = ExpandClassGlue.Find(structname);
	bool bHasGlue = ExpandFunc != nullptr;
	if(bHasGlue)
		AddBaseClassFuncList(ExpandFunc, structname);

	lua_newtable(inL);
	LuaRawSet(inL, -1, "classname", structname);

	FString NameNoGc = structname;
	NameNoGc += "_nogc";
	auto temp1 = FTCHARToUTF8((const TCHAR*)*NameNoGc);
	const char* structname_nogc = (ANSICHAR*)temp1.Get();
	AddFuncToTable(inL, -1, "Copy", BpStructCopy, Class, Class->GetStructureSize(), structname);
	AddFuncToTable(inL, -1, "New", BpStructNew, Class, Class->GetStructureSize(), structname);
	AddFuncToTable(inL, -1, "Temp", BpStructTemp, Class, Class->GetStructureSize(), structname, structname_nogc);
	AddFuncToTable(inL, -1, "Destroy", BpStructDestroy, Class);
	AddFuncToTable(inL, -1, "__eq", BpStruct__eq, Class);

	if (bIsNeedGc)
	{
		AddFuncToTable(inL, -1, "__gc", struct_gcfunc);
	}

	auto GetPropertyName = [=](UProperty* Property)
	{
		FString PropertyName = Property->GetName();
		if (IsBpStruct)
		{
			PropertyName = PropertyName.LeftChop(33);
			int32 Index;
			PropertyName.FindLastChar('_', Index);
			PropertyName = PropertyName.LeftChop(PropertyName.Len() - Index);
		}
		return PropertyName;
	};
	bool HasGlueFunctionForIndex = false;
	bool HasGlueFunctionForNewIndex = false;
	lua_newtable(inL);
	auto AddSetBpProperty = [&]() {
		for (TFieldIterator<UProperty> PropertyIt(Class); PropertyIt; ++PropertyIt)
		{
			bool bPushFunction = false;
			UProperty* Property = *PropertyIt;
			FString PropertyName = GetPropertyName(Property);
			void* LuaProperty = GetBpPropertyInterface(inL, Property);
			auto SetFunc = [=](const FString& Name, int TableIndex)
			{
				push(inL, Name);
				lua_pushlightuserdata(inL, LuaProperty);
				if (Property->IsA(UBoolProperty::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertySetterName(UBoolProperty), 1);
				else if (Property->IsA(UIntProperty::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertySetterName(UIntProperty), 1);
				else if (Property->IsA(UInt8Property::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertySetterName(UInt8Property), 1);
				else if (Property->IsA(UUInt16Property::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertySetterName(UUInt16Property), 1);
				else if (Property->IsA(UInt16Property::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertySetterName(UInt16Property), 1);
				else if (Property->IsA(UUInt32Property::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertySetterName(UUInt32Property), 1);
				else if (Property->IsA(UInt64Property::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertySetterName(UInt64Property), 1);
				else if (Property->IsA(UUInt64Property::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertySetterName(UUInt64Property), 1);
				else if (Property->IsA(UFloatProperty::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertySetterName(UFloatProperty), 1);
				else if (Property->IsA(UDoubleProperty::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertySetterName(UDoubleProperty), 1);
				else if (Property->IsA(UObjectPropertyBase::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertySetterName(UObjectPropertyBase), 1);
				else if (Property->IsA(UObjectProperty::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertySetterName(UObjectProperty), 1);
				else if (Property->IsA(UClassProperty::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertySetterName(UClassProperty), 1);
				else if (Property->IsA(UStrProperty::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertySetterName(UStrProperty), 1);
				else if (Property->IsA(UNameProperty::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertySetterName(UNameProperty), 1);
				else if (Property->IsA(UTextProperty::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertySetterName(UTextProperty), 1);
				else if (Property->IsA(UByteProperty::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertySetterName(UByteProperty), 1);
				else if (Property->IsA(UEnumProperty::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertySetterName(UEnumProperty), 1);
				else if (Property->IsA(UStructProperty::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertySetterName(UStructProperty), 1);
				else if (Property->IsA(UMulticastDelegateProperty::StaticClass()))
				{
#if ENGINE_MINOR_VERSION >= 23
					if (Property->IsA(UMulticastInlineDelegateProperty::StaticClass()))
						lua_pushcclosure(inL, BpStructPropertySetterName(UMulticastInlineDelegateProperty), 1);
					else
						lua_pushcclosure(inL, BpStructPropertySetterName(UMulticastSparseDelegateProperty), 1);
#else
					lua_pushcclosure(inL, BpStructPropertySetterName(UMulticastDelegateProperty), 1);
#endif
				}
				else if (Property->IsA(UDelegateProperty::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertySetterName(UDelegateProperty), 1);
				else if (Property->IsA(UWeakObjectProperty::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertySetterName(UWeakObjectProperty), 1);
				else if (Property->IsA(UArrayProperty::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertySetterName(UArrayProperty), 1);
				else if (Property->IsA(UMapProperty::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertySetterName(UMapProperty), 1);
				else if (Property->IsA(USetProperty::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertySetterName(USetProperty), 1);
				else if (Property->IsA(UInterfaceProperty::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertySetterName(UInterfaceProperty), 1);
				else
				{
					ensureAlwaysMsgf(0, TEXT("Bug"));
					lua_pushcclosure(inL, BpStructGetProp, 1);
				}
				push(inL, "LuaSet_" + Name);
				lua_pushvalue(inL, -2);
				lua_rawset(inL, TableIndex - 3);
				if (bPushFunction)
					lua_rawset(inL, TableIndex);
				else
				{
					lua_pop(inL, 1);
					*(void**)lua_newuserdata(inL, sizeof(void *)) = LuaProperty;
					lua_rawset(inL, TableIndex);
				}
			};
			SetFunc(PropertyName, -3);
		}
		if (ExpandFunc)
		{
			for (auto& Pairs : *ExpandFunc)
			{
				if ((Pairs.Value.ExportFlag & ELuaFuncExportFlag::RF_SetPropertyFunc)!=0)
				{
					HasGlueFunctionForNewIndex = true;
					FString FuncName = Pairs.Key.RightChop(7);
					lua_pushstring(inL, TCHAR_TO_UTF8(*FuncName));
					lua_pushlightuserdata(inL, (void*)Pairs.Value.func);
					lua_rawset(inL, -3);
				}
			}
		}
	};
	AddSetBpProperty();
	if(HasGlueFunctionForNewIndex)
		AddFuncToTable(inL, -2, "__newindex", newindex_struct_Func_with_class_with_glue<true>, LuaSpace::StackValue(-1), Class);
	else
		AddFuncToTable(inL, -2, "__newindex", newindex_struct_Func_with_class_with_glue<false>, LuaSpace::StackValue(-1), Class);
	lua_pop(inL, 1);

	if (ExpandFunc)
	{
		bool HasStaticProperty = false;
		for (auto& Pairs : *ExpandFunc)
		{
			bool isStaticProperty = (Pairs.Value.ExportFlag & ELuaFuncExportFlag::RF_IsStaticProperty) != 0;
			HasStaticProperty = HasStaticProperty || isStaticProperty;
			if (!bIsNeedGc && Pairs.Key == "__gc")
				continue;
			HasGlueFunctionForIndex = true;
			AddFuncToTable(inL, -1, TCHAR_TO_UTF8(*Pairs.Key), Pairs.Value.func);
		}
		if (HasStaticProperty)
		{
			HasGlueFunctionForIndex = true;
			AddStaticMetaToTable(inL, *ExpandFunc);
		}
		TMap<FString, TArray<UnrealLuaBlueFunc>>& OverloadFuncs = ClassOverloadFuncs.FindOrAdd(structname);
		BuildOverLoadFuncTree(inL, OverloadFuncs);
	}
	if (int32* ClassType = ClassDefineTypeInLua.Find(structname))
	{
		LuaRawSet(inL, -1, "_type_", *ClassType);
	}
	else
	{
		int32 NewType = GetNewType(structname);
		ClassDefineTypeInLua.Add(structname, NewType);
		LuaRawSet(inL, -1, "_type_", NewType);
	}

	lua_newtable(inL);
	CopyTableForLua(inL);
	auto AddGetBpProperty = [&]() {
		for (TFieldIterator<UProperty> PropertyIt(Class); PropertyIt; ++PropertyIt)
		{
			UProperty* Property = *PropertyIt;
			FString PropertyName = GetPropertyName(Property);
			void* LuaProperty = GetBpPropertyInterface(inL, Property);
			auto SetFunc = [=](const FString& Name, int TableIndex)
			{
				bool bPushFunction = false;
				push(inL, Name);
				lua_pushlightuserdata(inL, LuaProperty);
				if (Property->IsA(UBoolProperty::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertyGetterName(UBoolProperty), 1);
				else if (Property->IsA(UIntProperty::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertyGetterName(UIntProperty), 1);
				else if (Property->IsA(UInt8Property::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertyGetterName(UInt8Property), 1);
				else if (Property->IsA(UUInt16Property::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertyGetterName(UUInt16Property), 1);
				else if (Property->IsA(UInt16Property::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertyGetterName(UInt16Property), 1);
				else if (Property->IsA(UUInt32Property::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertyGetterName(UUInt32Property), 1);
				else if (Property->IsA(UInt64Property::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertyGetterName(UInt64Property), 1);
				else if (Property->IsA(UUInt64Property::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertyGetterName(UUInt64Property), 1);
				else if (Property->IsA(UFloatProperty::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertyGetterName(UFloatProperty), 1);
				else if (Property->IsA(UDoubleProperty::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertyGetterName(UDoubleProperty), 1);
				else if (Property->IsA(UObjectPropertyBase::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertyGetterName(UObjectPropertyBase), 1);
				else if (Property->IsA(UObjectProperty::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertyGetterName(UObjectProperty), 1);
				else if (Property->IsA(UClassProperty::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertyGetterName(UClassProperty), 1);
				else if (Property->IsA(UStrProperty::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertyGetterName(UStrProperty), 1);
				else if (Property->IsA(UNameProperty::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertyGetterName(UNameProperty), 1);
				else if (Property->IsA(UTextProperty::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertyGetterName(UTextProperty), 1);
				else if (Property->IsA(UByteProperty::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertyGetterName(UByteProperty), 1);
				else if (Property->IsA(UEnumProperty::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertyGetterName(UEnumProperty), 1);
				else if (Property->IsA(UStructProperty::StaticClass()))
				{
					lua_newtable(inL);
					lua_newtable(inL);
					lua_pushstring(inL, "k");
					lua_setfield(inL, -2, "__mode");
					lua_setmetatable(inL, -2);
					lua_pushcclosure(inL, BpStructPropertyGetterName(UStructProperty), 2);
					bPushFunction = true;
				}
				else if (Property->IsA(UMulticastDelegateProperty::StaticClass()))
				{
					lua_newtable(inL);
					lua_newtable(inL);
					lua_pushstring(inL, "k");
					lua_setfield(inL, -2, "__mode");
					lua_setmetatable(inL, -2);
#if ENGINE_MINOR_VERSION >= 23
					if (Property->IsA(UMulticastInlineDelegateProperty::StaticClass()))
						lua_pushcclosure(inL, BpStructPropertyGetterName(UMulticastInlineDelegateProperty), 2);
					else
						lua_pushcclosure(inL, BpStructPropertyGetterName(UMulticastSparseDelegateProperty), 2);
#else
					lua_pushcclosure(inL, BpStructPropertyGetterName(UMulticastDelegateProperty), 2);
#endif
					bPushFunction = true;
				}
				else if (Property->IsA(UDelegateProperty::StaticClass()))
				{
					lua_newtable(inL);
					lua_newtable(inL);
					lua_pushstring(inL, "k");
					lua_setfield(inL, -2, "__mode");
					lua_setmetatable(inL, -2);
					lua_pushcclosure(inL, BpStructPropertyGetterName(UDelegateProperty), 2);
					bPushFunction = true;
				}
				else if (Property->IsA(UWeakObjectProperty::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertyGetterName(UWeakObjectProperty), 1);
				else if (Property->IsA(UArrayProperty::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertyGetterName(UArrayProperty), 1);
				else if (Property->IsA(UMapProperty::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertyGetterName(UMapProperty), 1);
				else if (Property->IsA(USetProperty::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertyGetterName(USetProperty), 1);
				else if (Property->IsA(UInterfaceProperty::StaticClass()))
					lua_pushcclosure(inL, BpStructPropertyGetterName(UInterfaceProperty), 1);
				else
				{
					ensureAlwaysMsgf(0, TEXT("Bug"));
					lua_pushcclosure(inL, BpStructGetProp, 1);
				}
				push(inL, "LuaGet_" + Name);
				lua_pushvalue(inL, -2);
				lua_rawset(inL, TableIndex - 3);
				if (bPushFunction)
				{
					lua_createtable(inL, 2, 0);

					lua_createtable(inL, 0, 10);
					lua_createtable(inL, 0, 1);
					lua_pushstring(inL, "k");
					lua_setfield(inL, -2, "__mode");
					lua_setmetatable(inL, -2);
					lua_rawseti(inL, -2, 2);

					lua_pushvalue(inL, -2);
					lua_rawseti(inL, -2, 1);
					lua_remove(inL, -2);
					lua_rawset(inL, TableIndex);
				}
				else
				{
					lua_pop(inL, 1);
					*(void**)lua_newuserdata(inL, sizeof(void *)) = LuaProperty;
					lua_rawset(inL, TableIndex);
				}
			};
			SetFunc(PropertyName, -3);
		}
		if (ExpandFunc)
		{
			for (auto& Pairs : *ExpandFunc)
			{
				if ( ((Pairs.Value.ExportFlag & ELuaFuncExportFlag::RF_GetPropertyFunc) != 0) )					
				{
					HasGlueFunctionForIndex = true;
					FString FuncName = Pairs.Key.RightChop(7);
					if ((Pairs.Value.ExportFlag & ELuaFuncExportFlag::RF_IsStructProperty) != 0)
					{
						lua_pushstring(inL, TCHAR_TO_UTF8(*FuncName));

						lua_createtable(inL, 2, 0);

						lua_createtable(inL, 0, 10);
						lua_createtable(inL, 0, 1);
						lua_pushstring(inL, "k");
						lua_setfield(inL, -2, "__mode");
						lua_setmetatable(inL, -2);
						lua_rawseti(inL, -2, 2);

						lua_pushcfunction(inL, Pairs.Value.func);
						lua_rawseti(inL, -2, 1);
						lua_rawset(inL, -3);
					}
					else
					{
						lua_pushstring(inL, TCHAR_TO_UTF8(*FuncName));
						lua_pushlightuserdata(inL, (void*)Pairs.Value.func);
						lua_rawset(inL, -3);
					}
				}
				else if (Pairs.Key != "__gc" && Pairs.Key != "__index" && Pairs.Key != "__newindex")
				{
					HasGlueFunctionForIndex = true;
					AddFuncToTable(inL, -1, TCHAR_TO_UTF8(*Pairs.Key), Pairs.Value.func);
				}
			}
		}
	};
	AddGetBpProperty();
	if(HasGlueFunctionForIndex)
		AddFuncToTable(inL, -2, "__index", index_struct_func_with_class_with_glue<true>, LuaSpace::StackValue(-2), LuaSpace::StackValue(-2), Class);
	else
		AddFuncToTable(inL, -2, "__index", index_struct_func_with_class_with_glue<false>, LuaSpace::StackValue(-2), LuaSpace::StackValue(-2), Class);

	lua_pop(inL, 1);

	
	FString GlobalKey = structname;
	if (!bIsNeedGc)
		GlobalKey += "_nogc";
	lua_setglobal(inL, TCHAR_TO_UTF8(*GlobalKey));
}

void UTableUtil::addmodule(lua_State *inL, const char* name, bool bIsStruct, bool bNeedGc, const char* luaclassname)
{
	const char* GlobalKey = luaclassname ? luaclassname : name;
	lua_getglobal(inL, GlobalKey);
	if (lua_istable(inL, -1))
	{
		lua_pop(inL, 1);
		return;
	}
	lua_pop(inL, 1);
	lua_newtable(inL);
	LuaRawSet(inL, -1, "IsObject", false);
	initmeta(inL, name, bIsStruct, bNeedGc, luaclassname);
	LuaRawSet(inL, -1, "classname", name);
	if (int32* ClassType = ClassDefineTypeInLua.Find(name))
	{
		LuaRawSet(inL, -1, "_type_", *ClassType);
	}
	lua_setglobal(inL, GlobalKey);
}

void UTableUtil::openmodule(lua_State *inL, const char* name)
{
	lua_getglobal(inL, name);
}


void UTableUtil::addfunc(lua_State *inL, const char* name, luafunc f)
{
	lua_pushstring(inL, name);
	lua_pushcfunction(inL, f);
	lua_rawset(inL, -3);
}

void UTableUtil::closemodule(lua_State *inL)
{
	lua_pop(inL, 1);
}

void* touobject(lua_State* L, int i)
{
	if (lua_isnil(L, i))
		return nullptr;
	UObject* Obj = (UObject*)tovoid(L, i);
#if LuaDebug

	if (!Obj->IsValidLowLevel())
	{
		if (GcCheckActorRef)
		{
			lua_getmetatable(L, i);
			lua_getfield(L, -1, "classname");
			FString Name = lua_tostring(L, -1);
			ensureAlwaysMsgf(0, TEXT("Bug"));
			UnrealLua::ReportError(L, "touobject Bug" + Name);
		}
		return nullptr;
	}
#endif
	return Obj;
}

#if LuaDebug
void* tostruct(lua_State* L, int i)
{
	if (lua_isnil(L, i))
	{
		ensureAlwaysMsgf(0, TEXT("struct can't be nil"));
		UnrealLua::ReportError(L,"struct can't be nil");
		return nullptr;
	}
	else if (!lua_isuserdata(L, i))
	{
		ensureAlwaysMsgf(0, TEXT("bug"));
		UnrealLua::ReportError(L,"tostruct bug");
		return nullptr;
	}
	auto u = static_cast<void**>(lua_touserdata(L, i));
	return *u;
}
#endif // LuaDebug

FString PrintLuaStackOfL(lua_State* inL)
{
	lua_State* L = inL;
	lua_getglobal(L, "debug");
	lua_getfield(L, -1, "traceback");
	lua_pushthread(L);
	lua_call(L, 1, 1);
	FString stackstr = lua_tostring(L, -1);
	UE_LOG(LogOutputDevice, Warning, TEXT("%s"), *stackstr);
	lua_pop(L, 2);
	return stackstr;
}

void UTableUtil::setmeta(lua_State *inL, const char* classname, int index, bool bIsStruct, bool bNeedGc)
{
	int32 Type = lua_getglobal(inL, classname);
	if (Type == LUA_TTABLE)
	{
		lua_setmetatable(inL, index - 1);
	}
	else
	{
		lua_pop(inL, 1);
		UTableUtil::addmodule(inL, classname, bIsStruct, bNeedGc);
		lua_getglobal(inL, classname);
		lua_setmetatable(inL, index - 1);
	}
}

bool UTableUtil::ExistClassInGlobal(lua_State* inL, const char* classname)
{
	lua_getglobal(inL, classname);
	bool IsExist = !lua_isnil(inL, -1);
	lua_pop(inL, 1);
	return IsExist;
}

void UTableUtil::set_uobject_meta(lua_State *inL, UObject* Obj, int index)
{
	UClass* Class = Obj->GetClass();
// 	while (!Class->HasAnyClassFlags(CLASS_Native))
// 		Class = Class->GetSuperClass();
	lua_State* MainThread = GetMainThread(inL);
	auto& HasAddClasses = HasAddUClass.FindOrAdd(MainThread);
	auto ClassNamePtr = HasAddClasses.Find(Class);
	const char* ClassName = nullptr;
	if (ClassNamePtr)
	{
		ClassName = (*(ClassNamePtr))->Get();
	}
	else
	{
		UClass* NativeClass = Class;
		if (NativeClass->HasAnyClassFlags(CLASS_Native))
		{
			FString NativeClassStr = FString::Printf(TEXT("%s%s"), NativeClass->GetPrefixCPP(), *NativeClass->GetName());
			TSharedPtr<FTCHARToUTF8> NewClassNameUTF8 = MakeShareable(new FTCHARToUTF8((const TCHAR*)*NativeClassStr));
			HasAddClasses.Add(Class, NewClassNameUTF8);
			ClassName = NewClassNameUTF8->Get();
			requirecpp(inL, ClassName);
		}
		else
		{
			FString BpClassStr = Class->GetName();
			TSharedPtr<FTCHARToUTF8> NewClassNameUTF8 = MakeShareable(new FTCHARToUTF8((const TCHAR*)*BpClassStr));
			HasAddClasses.Add(Class, NewClassNameUTF8);
			ClassName = NewClassNameUTF8->Get();
			if (ExistClassInGlobal(inL, ClassName))
				ensureAlwaysMsgf(0, TEXT("Shouldn't be this"));
			init_refelction_native_uclass_meta(MainThread, ClassName, Class);

			UPackage* Package = Class->GetTypedOuter<UPackage>();
			auto& WorldBpSet = NeedGcBpClassName.FindOrAdd(MainThread);
			if (Package)
			{
				auto& BpSet = WorldBpSet.FindOrAdd(Package);
				BpSet.Add(FString(ClassName), Class);
			}
		}
	}

	setmeta(inL, ClassName, index);

#if LuaDebug
	AddGcCount(inL, FString(ClassName));
#endif
}

void UTableUtil::OnWorldCleanUp(lua_State*inL, UWorld* World)
{
	if (World)
	{
		lua_State* MainThread = GetMainThread(inL);
		auto& WorldBpSet = NeedGcBpClassName.FindOrAdd(MainThread);
		auto CleanFunc = [MainThread](TMap<FString, UClass*>& BpSet) 
		{
			auto& HasAddClassForState = HasAddUClass.FindOrAdd(MainThread);
			for (auto&Pairs: BpSet)
			{
				auto temp = FTCHARToUTF8((const TCHAR*)*Pairs.Key);
				const char* classnameptr = (ANSICHAR*)temp.Get();
				lua_pushnil(MainThread);
				lua_setglobal(MainThread, classnameptr);
				HasAddClassForState.Remove(Pairs.Value);
			}
		};
		auto& BpSet = WorldBpSet.FindOrAdd(World->GetTypedOuter<UPackage>());
		CleanFunc(BpSet);
	}
}

void UTableUtil::call(lua_State* inL, int funcid, UFunction* funcsig, void* ptr)
{
	lua_rawgeti(inL, LUA_REGISTRYINDEX, funcid);
	checkf(lua_isfunction(inL, -1), TEXT(""));
	MuldelegateBpInterface* MultiDlgInterface = GetMultiDlgInterface(funcsig);
	MultiDlgInterface->Call(inL, ptr);
}

void UTableUtil::call(lua_State*inL, FFrame& Stack, RESULT_DECL)
{
	FString FuncName;
	UFunction* FuncToCall = nullptr;
	if (Stack.CurrentNativeFunction)
	{
		P_FINISH;
		FuncToCall = Stack.CurrentNativeFunction;
	}
	else
	{
		FuncToCall = Stack.Node;
	}
	lua_getglobal(inL, "Call");
	push(inL, FuncToCall->GetName());
	push(inL, Stack.Object);
	TArray<UProperty*> PushBackParms;
	TArray<UProperty*> ReturnParms;
	
	TArray<int32> StackIndexs;
	int ArgIndex = 0;
	int ParamCount = 2;
	int OutParamCount = 0;
	for (TFieldIterator<UProperty> It(FuncToCall); It && (It->GetPropertyFlags() & (CPF_Parm)); ++It)
	{
		auto Prop = *It;
		if ((Prop->GetPropertyFlags() & (CPF_ReturnParm)))
		{
			ReturnParms.Insert(Prop, 0);
			continue;
		}

		if ((Prop->GetPropertyFlags() & (CPF_OutParm) && !(Prop->GetPropertyFlags()&CPF_ConstParm)))
		{
			OutParamCount++;
			continue;
		}

		if (Prop->GetPropertyFlags() & CPF_ReferenceParm && !(Prop->GetPropertyFlags()&CPF_ConstParm))
		{
			PushBackParms.Add(Prop);
			StackIndexs.Add(ArgIndex);
		}
		pushproperty(inL, Prop, Stack.Locals);
		ParamCount++;

		++ArgIndex;
	}

	TArray<FOutParmRec*> OutParmsArr;
	FOutParmRec* OutParam = Stack.OutParms;
	while (OutParamCount>0)
	{
		OutParmsArr.Add(OutParam);
		OutParam = OutParam->NextOutParm;
		OutParamCount--;
	}


	int32 AllReturnCount = ReturnParms.Num() + PushBackParms.Num() + OutParmsArr.Num();
	if (lua_pcall(inL, ParamCount, AllReturnCount, 0))
	{
#if LuaDebug
		FString error = lua_tostring(inL, -1);
		ensureAlwaysMsgf(0, *error);
		UnrealLua::ReportError(inL, error);
#endif
		log(lua_tostring(inL, -1));
	}
	else
	{
		int32 LuaStackIndex = -(AllReturnCount);
		for (auto ReturnProp : ReturnParms)
		{
// todo
			uint8* SrcP = (uint8*)(RESULT_PARAM);
			uint8* P = ReturnProp->ContainerPtrToValuePtr<uint8>(SrcP);
			UTableUtil::popproperty(inL, LuaStackIndex, ReturnProp, SrcP - (P - SrcP));
			++LuaStackIndex;
		}

		for (auto OutParmRec : OutParmsArr)
		{
// todo
			uint8* SrcP = (uint8*)(OutParmRec->PropAddr);
			uint8* P = OutParmRec->Property->ContainerPtrToValuePtr<uint8>(SrcP);
			UTableUtil::popproperty(inL, LuaStackIndex, OutParmRec->Property, SrcP - (P - SrcP));
			++LuaStackIndex;
		}
		
		for (auto ReturnProp : PushBackParms)
		{
			UTableUtil::popproperty(inL, LuaStackIndex, ReturnProp, Stack.Locals);
			++LuaStackIndex;
		}
		lua_pop(inL, AllReturnCount);
	}
}

UnrealLua::ArgType UTableUtil::GetTypeOfProperty(UProperty* Property)
{
	uint64 CastFlag = GetPropertyFlag(Property);
	switch (CastFlag)
	{
	case CASTCLASS_UByteProperty:
		return UnrealLua::Type::TYPE_Byte;
	case CASTCLASS_UIntProperty:
		return UnrealLua::Type::TYPE_INTERGER;
	case CASTCLASS_UInt8Property:
		return UnrealLua::Type::TYPE_INTERGER8;
	case CASTCLASS_UUInt64Property:
		return UnrealLua::Type::TYPE_INTERGERu64;
	case CASTCLASS_UUInt32Property:
		return UnrealLua::Type::TYPE_INTERGERu32;
	case CASTCLASS_UUInt16Property:
		return UnrealLua::Type::TYPE_INTERGERu16;
	case CASTCLASS_UInt64Property:
		return UnrealLua::Type::TYPE_INTERGER64;
	case CASTCLASS_UInt16Property:
		return UnrealLua::Type::TYPE_INTERGER16;
	case CASTCLASS_UBoolProperty:
		return UnrealLua::Type::TYPE_TBOOLEAN;
	case CASTCLASS_UNameProperty:
		return UnrealLua::Type::TYPE_Name;
	case CASTCLASS_UStrProperty:
		return UnrealLua::Type::TYPE_String;
	case CASTCLASS_UTextProperty:
		return UnrealLua::Type::TYPE_Text;
	case CASTCLASS_UDoubleProperty:
		return UnrealLua::Type::TYPE_TNUMBERdouble;
	case CASTCLASS_UFloatProperty:
		return UnrealLua::Type::TYPE_TNUMBERfloat;
	case CASTCLASS_UObjectProperty:
		return UnrealLua::Type::TYPE_UOBJECT;
	case CASTCLASS_UStructProperty:
		return GetNewType("F" + ((UStructProperty*)Property)->Struct->GetName());
	}
	return -1;
}

#define CasePush(PropertyType) case CASTCLASS_##PropertyType:pushproperty_##PropertyType(inL, Property, ptr);break;
void UTableUtil::pushproperty(lua_State* inL, UProperty* Property, const void* ptr)
{
// need to optimize
	uint64 CastFlag = GetPropertyFlag(Property);
	switch (CastFlag)
	{
		CasePush(UBoolProperty);
		CasePush(UIntProperty);
		CasePush(UInt8Property);
		CasePush(UUInt16Property);
		CasePush(UInt16Property);
		CasePush(UUInt32Property);
		CasePush(UInt64Property);
		CasePush(UUInt64Property);
		CasePush(UFloatProperty);
		CasePush(UDoubleProperty);
		CasePush(UObjectPropertyBase);
		CasePush(UObjectProperty);
		CasePush(UClassProperty);
		CasePush(UStrProperty);
		CasePush(UNameProperty);
		CasePush(UTextProperty);
		CasePush(UByteProperty);
		CasePush(UEnumProperty);
		CasePush(UStructProperty);
#if ENGINE_MINOR_VERSION >= 23
		CasePush(UMulticastInlineDelegateProperty);
		CasePush(UMulticastSparseDelegateProperty);
#else
		CasePush(UMulticastDelegateProperty);
#endif
		CasePush(UDelegateProperty);
		CasePush(UWeakObjectProperty);
		CasePush(UArrayProperty);
		CasePush(UMapProperty);
		CasePush(USetProperty);
		CasePush(UInterfaceProperty);
	default:
		if (auto* ProcessFunc = PropertyClassToPushFuncMap.Find(Property->GetClass()))
		{
			(*ProcessFunc)(inL, Property, ptr);
		}
		else
		{
			if (UStructProperty* p = Cast<UStructProperty>(Property))
			{
				pushproperty_type(inL, p, ptr);
			}

			else if (UObjectPropertyBase* p = Cast<UObjectPropertyBase>(Property))
			{
				pushproperty_type(inL, p, ptr);
			}

			else
			{
				ensureAlwaysMsgf(0, TEXT("Some type didn't process"));
				lua_pushnil(inL);
			}
		}
	}
}

void UTableUtil::push_ret_property(lua_State*inL, UProperty* property, const void* ptr)
{
	if (UTextProperty* p = Cast<UTextProperty>(property))
	{
		FText* Text_ptr = (FText*)p->ContainerPtrToValuePtr<uint8>(ptr);
		push_ret(inL, *Text_ptr);
	}
	else if (UStructProperty* p = Cast<UStructProperty>(property))
	{
		UScriptStruct* Struct = p->Struct;
		FString TypeName;
		if (UUserDefinedStruct* BpStruct = Cast<UUserDefinedStruct>(p->Struct))
		{
			MayAddNewStructType(inL, BpStruct);
			TypeName = BpStruct->GetName();
		}
		else
			TypeName = p->Struct->GetStructCPPName();
// 		uint8* result = (uint8*)FMemory::Malloc(p->GetSize());
		uint8* result = GetBpStructTempIns(TypeName, p->GetSize());
		p->InitializeValue(result);
		ptr = (void*)p->ContainerPtrToValuePtr<uint8>(ptr);
		p->CopyCompleteValueFromScriptVM(result, ptr);
// 		pushstruct_gc(inL, TCHAR_TO_UTF8(*TypeName), result);
		FString nogc_name = TypeName;
		nogc_name += "_nogc";
		pushstruct_nogc(inL, TCHAR_TO_UTF8(*TypeName), TCHAR_TO_UTF8(*nogc_name), result);
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
			push_ret_property(inL, p->KeyProp, PairPtr);
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
	if (UTextProperty* p = Cast<UTextProperty>(property))
	{
		if (!lua_isuserdata(inL, LuaStackIndex))
		{
			FText* Text_ptr = (FText*)p->ContainerPtrToValuePtr<uint8>(ptr);
			push(inL, Text_ptr->ToString());
		}
		else
		{
			void* DestPtr = tovoid(inL, LuaStackIndex);
			ptr = (void*)p->ContainerPtrToValuePtr<uint8>(ptr);
			p->CopyCompleteValueFromScriptVM(DestPtr, ptr);
			lua_pushvalue(inL, LuaStackIndex);
		}
	}
	else if (UStructProperty* p = Cast<UStructProperty>(property))
	{
		void* DestPtr = tovoid(inL, LuaStackIndex);
		ptr = (void*)p->ContainerPtrToValuePtr<uint8>(ptr);
		p->CopyCompleteValueFromScriptVM(DestPtr, ptr);
		lua_pushvalue(inL, LuaStackIndex);
	}
	else if (UArrayProperty *p = Cast<UArrayProperty>(property))
	{
		if (UnrealLua::IsGlueTArray(inL, LuaStackIndex))
		{
			void* Ptr = tovoid(inL, LuaStackIndex);
			ULuaArrayHelper::GlueArrCopyTo(p, p->ContainerPtrToValuePtr<void>(ptr), Ptr);
			lua_pushvalue(inL, LuaStackIndex);
		}
		else if (ULuaArrayHelper* ArrHelper = (ULuaArrayHelper*)UnrealLua::IsCppPtr(inL, LuaStackIndex))
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
			ensureAlwaysMsgf(0, TEXT("not arr"));
			UnrealLua::ReportError(inL,"not arr");
		}
	}
	else if (UMapProperty *p = Cast<UMapProperty>(property))
	{
		if (UnrealLua::IsGlueTMap(inL, LuaStackIndex))
		{
			void* ArrPtr = (void*)tovoid(inL, LuaStackIndex);
			ULuaMapHelper::GlueMapCopyTo(p, p->ContainerPtrToValuePtr<void>(ptr), ArrPtr);
			lua_pushvalue(inL, LuaStackIndex);
		}
		else if (ULuaMapHelper* Helper = (ULuaMapHelper*)UnrealLua::IsCppPtr(inL, LuaStackIndex))
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
				push_ret_property(inL, p->KeyProp, PairPtr);
				push_ret_property(inL, p->ValueProp, PairPtr);
				lua_rawset(inL, -3);
			}
		}
		else
		{
			ensureAlwaysMsgf(0, TEXT("not map"));
			UnrealLua::ReportError(inL,"not map");
		}
	}
	else if (USetProperty *p = Cast<USetProperty>(property))
	{
		if (UnrealLua::IsGlueTSet(inL, LuaStackIndex))
		{
			void* ArrPtr = (void*)tovoid(inL, LuaStackIndex);
			ULuaSetHelper::GlueSetCopyTo(p, p->ContainerPtrToValuePtr<void>(ptr), ArrPtr);
			lua_pushvalue(inL, LuaStackIndex);
		}
		else if (ULuaSetHelper* Helper = (ULuaSetHelper*)UnrealLua::IsCppPtr(inL, LuaStackIndex))
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
			ensureAlwaysMsgf(0, TEXT("not set"));
			UnrealLua::ReportError(inL,"not set");
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

void UTableUtil::pushproperty_type(lua_State*inL, UInt8Property* p, const void*ptr)
{
	lua_pushinteger(inL, (int8)p->GetPropertyValue_InContainer(ptr));
}

void UTableUtil::pushproperty_type(lua_State*inL, UInt64Property* p, const void*ptr)
{
#ifdef USE_LUA53 
	lua_pushinteger(inL, (int64)p->GetPropertyValue_InContainer(ptr));
#else 
	lua_pushnumber(inL, (int64)p->GetPropertyValue_InContainer(ptr));
#endif
}

void UTableUtil::pushproperty_type(lua_State*inL, UUInt64Property* p, const void*ptr)
{
#ifdef USE_LUA53 
	lua_pushinteger(inL, (uint64)p->GetPropertyValue_InContainer(ptr));
#else 
	lua_pushnumber(inL, (uint64)p->GetPropertyValue_InContainer(ptr));
#endif
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

void UTableUtil::pushproperty_type(lua_State*inL, UObjectProperty* p, const void*ptr)
{
	pushuobject(inL, p->GetObjectPropertyValue_InContainer(ptr));
}

void UTableUtil::pushproperty_type(lua_State*inL, UClassProperty* p, const void*ptr)
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
	ptr = (void*)p->ContainerPtrToValuePtr<uint8>(ptr);
	pushstruct_nogc(inL, "FText", "FText_nogc", (void*)ptr);
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
void UTableUtil::pushproperty_type(lua_State*inL, UUInt16Property* p, const void*ptr)
{
	lua_pushinteger(inL, (int32)p->GetPropertyValue_InContainer(ptr));
}
void UTableUtil::pushproperty_type(lua_State*inL, UInt16Property* p, const void*ptr)
{
	lua_pushinteger(inL, (int32)p->GetPropertyValue_InContainer(ptr));
}

void UTableUtil::pushproperty_type(lua_State*inL, UMulticastDelegateProperty* p, const void*ptr)
{
	UFunction* FunSig = p->SignatureFunction;
	void* result = (void*)p->ContainerPtrToValuePtr<uint8>(ptr);
	auto delegateproxy = NewObject<ULuaDelegateMulti>();
	delegateproxy->Init(result, FunSig);
	pushuobject(inL, (void*)delegateproxy, true);
}

#if ENGINE_MINOR_VERSION >= 23
void UTableUtil::pushproperty_type(lua_State*inL, UMulticastInlineDelegateProperty* p, const void*ptr)
{
	check(0)
}
void UTableUtil::pushproperty_type(lua_State*inL, UMulticastSparseDelegateProperty* p, const void*ptr)
{
	check(0)
}
#endif

void UTableUtil::pushproperty_type(lua_State*inL, UInterfaceProperty* p, const void*ptr)
{
	FScriptInterface* result = (FScriptInterface*)p->GetPropertyValuePtr_InContainer(ptr);
	pushuobject(inL, (void*)result->GetObject());
}

void UTableUtil::pushproperty_type(lua_State*inL, UDelegateProperty* p, const void*ptr)
{
	FScriptDelegate* DelegatePtr = (FScriptDelegate*)p->GetPropertyValuePtr_InContainer(ptr);
	ULuaDelegateSingle* NewOne = ULuaDelegateSingle::CreateInCppRef(DelegatePtr, p->SignatureFunction);
	push(inL, NewOne);
}

void UTableUtil::pushproperty_valueptr(lua_State*inL, UProperty* property, const void* ptr)
{
	if (property == nullptr)
	{
		ensureAlwaysMsgf(0, TEXT("Some Bug?"));
		lua_pushnil(inL);
	}
	else if (UIntProperty* p = Cast<UIntProperty>(property))
	{
		pushproperty_type_valueptr(inL, p, ptr);
	}
	else if (UInt8Property* p = Cast<UInt8Property>(property))
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
	else if (UDelegateProperty* p = Cast<UDelegateProperty>(property))
	{
		pushproperty_type_valueptr(inL, p, ptr);
	}
	else
	{
		ensureAlwaysMsgf(0, TEXT("Some type didn't process"));
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

void UTableUtil::pushproperty_type_valueptr(lua_State*inL, UInt8Property* p, const void*ptr)
{
	lua_pushinteger(inL, (int8)p->GetPropertyValue(ptr));
}

void UTableUtil::pushproperty_type_valueptr(lua_State*inL, UInt64Property* p, const void*ptr)
{
#ifdef USE_LUA53 
	lua_pushinteger(inL, (int64)p->GetPropertyValue(ptr));
#else 
	lua_pushnumber(inL, (int64)p->GetPropertyValue(ptr));
#endif
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

void UTableUtil::pushproperty_type_valueptr(lua_State*inL, UObjectProperty* p, const void*ptr)
{
	pushuobject(inL, p->GetObjectPropertyValue(ptr));
}

void UTableUtil::pushproperty_type_valueptr(lua_State*inL, UClassProperty* p, const void*ptr)
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
	pushstruct_nogc(inL, "FText", "FText_nogc",(void*)ptr);
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
#if ENGINE_MINOR_VERSION >= 23
void UTableUtil::pushproperty_type_valueptr(lua_State*inL, UMulticastInlineDelegateProperty* p, const void*ptr)
{
}
void UTableUtil::pushproperty_type_valueptr(lua_State*inL, UMulticastSparseDelegateProperty* p, const void*ptr)
{
}
#endif
void UTableUtil::pushproperty_type_valueptr(lua_State*inL, UStructProperty* p, const void*ptr)
{
	FString TypeName;
	if (UUserDefinedStruct* BpStruct = Cast<UUserDefinedStruct>(p->Struct))
	{
		MayAddNewStructType(inL, BpStruct);
		TypeName = BpStruct->GetName();
	}
	else
		TypeName = p->Struct->GetStructCPPName();

	FString TypeName_nogc = TypeName+"_nogc";

	pushstruct_nogc(inL, TCHAR_TO_UTF8(*TypeName), TCHAR_TO_UTF8(*TypeName_nogc),(void*)ptr);
}

void UTableUtil::pushproperty_type_valueptr(lua_State*inL, UArrayProperty* property, const void* ptr)
{
	pushstruct_gc(inL, "ULuaArrayHelper", ULuaArrayHelper::GetHelperCPP_ValuePtr((void*)ptr, property));
}

void UTableUtil::pushproperty_type_valueptr(lua_State*inL, UMapProperty* property, const void* ptr)
{
	pushstruct_gc(inL, "ULuaMapHelper", ULuaMapHelper::GetHelperCPP_ValuePtr((void*)ptr, property));
}

void UTableUtil::pushproperty_type_valueptr(lua_State*inL, USetProperty* property, const void*ptr)
{
	pushstruct_gc(inL, "ULuaSetHelper", ULuaSetHelper::GetHelperCPP_ValuePtr((void*)ptr, property));
}

void UTableUtil::pushproperty_type_valueptr(lua_State*inL, UInterfaceProperty* p, const void*ptr)
{
	FScriptInterface* result = (FScriptInterface*)p->GetPropertyValuePtr(ptr);
	pushuobject(inL, (void*)result->GetObject());
}
// only for bp call lua
void UTableUtil::pushproperty_type_valueptr(lua_State*inL, UDelegateProperty* p, const void*ptr)
{
	FScriptDelegate* DelegatePtr = (FScriptDelegate*)p->GetPropertyValuePtr(ptr);
	ULuaDelegateSingle* NewOne = ULuaDelegateSingle::CreateInCppCopy(DelegatePtr, p->SignatureFunction);
	push(inL, NewOne);
}

void UTableUtil::MayAddNewStructType(lua_State *inL, UUserDefinedStruct* BpStruct)
{
	FString TypeName = BpStruct->GetName();
	lua_getglobal(inL, TCHAR_TO_UTF8(*TypeName));
	if (!lua_isnil(inL, -1))
	{
		lua_pop(inL, 1);
		return;
	}
	lua_pop(inL, 1);
	auto temp1 = FTCHARToUTF8((const TCHAR*)*TypeName);
	const char* name = (ANSICHAR*)temp1.Get();
	
	init_reflection_struct_meta(inL, name, BpStruct, false);
	init_reflection_struct_meta(inL, name, BpStruct, true);
}

void UTableUtil::AddAndOpenUserDefineClass(const char* ClassName, const UnrealLuaBlueFunc BasicFuncList[], TArray<UnrealLuaBlueFunc> funclist, TArray<FString> BaseClass, UnrealLuaClass ClassConfig)
{
	if (HasAddedUserDefinedClass.Find(ClassName))
		return;
	UserDefinedClassConfig NewConfig(FString(ClassName), ClassConfig.MyClassType);
	HasAddedUserDefinedClass.Add(ClassName, NewConfig);
	TMap<FString, UnrealLuaBlueFunc>& NameToFunction = ExpandClassGlue.FindOrAdd(ClassName);
	TMap<FString, TArray<UnrealLuaBlueFunc>>& OverloadFuncs = ClassOverloadFuncs.FindOrAdd(ClassName);
	if (BasicFuncList != nullptr)
	{
		for (int i = 0; i < 10000;i++)
		{
			if (BasicFuncList[i].name == nullptr)
				break;
			if ((BasicFuncList[i].ExportFlag & ELuaFuncExportFlag::RF_NoExport)!=0)
				continue;
			if ((BasicFuncList[i].ExportFlag & ELuaFuncExportFlag::RF_OverLoad) != 0)
			{
				TArray<UnrealLuaBlueFunc>& Funcs = OverloadFuncs.FindOrAdd(BasicFuncList[i].name);
				Funcs.Add(BasicFuncList[i]);
				continue;
			}
			NameToFunction.Add(BasicFuncList[i].name, BasicFuncList[i]);
		}
	}
	
	for (auto& V : funclist)
	{
		if ((V.ExportFlag & ELuaFuncExportFlag::RF_NoExport)!=0)
			continue;
		if ((V.ExportFlag & ELuaFuncExportFlag::RF_OverLoad) != 0)
		{
			TArray<UnrealLuaBlueFunc>& Funcs = OverloadFuncs.FindOrAdd(V.name);
			Funcs.Add(V);
			continue;
		}
		NameToFunction.Add(V.name, V);
	}

	for (auto&V : NameToFunction)
	{
// just overwrite
		OverloadFuncs.Remove(V.Key);
	}

	TSet<FString> RemoveNotOverLoadFuncNames;
	for (auto& V : OverloadFuncs)
	{
		if (V.Value.Num() == 1)
		{
			UnrealLuaBlueFunc& FuncConfig = V.Value[0];
			NameToFunction.Add(FuncConfig.name , FuncConfig);
			RemoveNotOverLoadFuncNames.Add(V.Key);
		}
		else
		{
			for (int i = 0; i < V.Value.Num(); i++)
			{
				UnrealLuaBlueFunc&FuncConfig = V.Value[i];
				FString NameWithAfterFix = "";
				NameWithAfterFix += FuncConfig.name;
				NameWithAfterFix = FString::Printf(TEXT("%s%d"), *NameWithAfterFix, i + 1);
				NameToFunction.Add(NameWithAfterFix, FuncConfig);
			}
		}
	}
	for (FString& Name : RemoveNotOverLoadFuncNames)
	{
		OverloadFuncs.Remove(Name);
	}

	UserDefineGlue.Add(ClassName, NameToFunction);
	ClassBaseClass.Add(ClassName, BaseClass);

	ClassDefineTypeInLua.Add(ClassName, ClassConfig.MyClassType);
	TArray<int> Parents;
	for (auto& v : ClassConfig.BaseClassType)
	{
		if (v != ClassConfig.MyClassType)
			Parents.Add(v);
	}
	ChildsParentTypesInLua.Add(ClassConfig.MyClassType, Parents);
}

void UTableUtil::AddExpandClassGlue(const char* ClassName, TArray<UnrealLuaBlueFunc> meta_funclist, TArray<UnrealLuaBlueFunc> funclist, TArray<FString> BaseClass)
{
	TMap<FString, UnrealLuaBlueFunc>& Funcs = ExpandClassGlue.FindOrAdd(ClassName);
	for (auto& Value : funclist)
	{
		Funcs.Add(Value.name, Value);
	}
	if( BaseClass.Num()>0 )
		ClassBaseClass.Add(ClassName, BaseClass);
	UserDefineGlue.Add(ClassName, Funcs);
}

void UTableUtil::AddEnumGlue(const char* EnumName, TArray<EnumGlueStruct> ArrGlue)
{
	ManualEnumGlue.Add(EnumName, ArrGlue);
}

void UTableUtil::AddBaseClassFuncList(TMap<FString, UnrealLuaBlueFunc>* MyFuncList, FString MyClassName)
{
	if (auto* BaseClassArr = ClassBaseClass.Find(MyClassName))
	{
		for (int i = 0;i < BaseClassArr->Num(); i++)
		{
			FString BaseClassName = (*BaseClassArr)[i];
			if (BaseClassName != MyClassName)
			{
				auto FindBaseClassGlue = [=](TMap<FString, TMap<FString, UnrealLuaBlueFunc>>& GlueMap)
				{
					if (auto* BaseClassFuncMapPtr = GlueMap.Find((*BaseClassArr)[i]))
					{
						for (auto& Pairs : *BaseClassFuncMapPtr)
						{
							const FString& FuncName = Pairs.Key;
							if (MyFuncList->Find(FuncName) == nullptr)
							{
								MyFuncList->Add(FuncName, Pairs.Value);
							}
						}
					}
				};
				FindBaseClassGlue(UserDefineGlue);
				AddBaseClassFuncList(MyFuncList, BaseClassName);
			}
		}
	}
}

void UTableUtil::AddAliasName(const FString& AliasName, const FString& OriginalName)
{
	GlueClassAlias.Add(AliasName, OriginalName);
}

bool UTableUtil::requirecpp(lua_State* inL, const FString& classname)
{
	lua_State* MainState = GetMainThread(inL);
	TSet<FString>& HasRequired = HasRequire.FindOrAdd(MainState);
	if (HasRequired.Contains(classname))
		return false;

	HasRequired.Add(classname);
	UClass* Class = FindObject<UClass>(ANY_PACKAGE, *classname.RightChop(1));
	if (Class)
	{
		FString TheFoundClassName = Class->GetPrefixCPP() + Class->GetName();
		if (TheFoundClassName == classname)
		{
			init_refelction_native_uclass_meta(MainState, TCHAR_TO_UTF8(*classname), Class);
			return true;
		}
	}
	UScriptStruct* Struct = FindObject<UScriptStruct>(ANY_PACKAGE, *classname.RightChop(1));
	if (Struct)
	{
		init_reflection_struct_meta(MainState, TCHAR_TO_UTF8(*classname), Struct, false);
		init_reflection_struct_meta(MainState, TCHAR_TO_UTF8(*classname), Struct, true);
		return true;
	}
	else if (UEnum* EnumClass = FindObject<UEnum>(ANY_PACKAGE, *classname))
	{
		lua_newtable(MainState);
		for (int32 i = 0; i <= EnumClass->NumEnums(); ++i)
		{
			FString ValueName = EnumClass->GetNameStringByIndex(i);
			if (ValueName.IsEmpty())
				continue;
			int64 value = EnumClass->GetValueByIndex(i);
			LuaRawSet(MainState, -1, ValueName, value);
		}
		lua_setglobal(MainState, TCHAR_TO_UTF8(*EnumClass->GetName()));
		return true;
	}
	else if (auto *Name2FuncMap = UserDefineGlue.Find(classname))
	{
		AddBaseClassFuncList(Name2FuncMap, classname);
		loadstruct(MainState, *Name2FuncMap, TCHAR_TO_UTF8(*classname));
		return true;
	}
	else if (auto* ManualArrGlue = ManualEnumGlue.Find(classname))
	{
		lua_newtable(MainState);
		for (auto& KeyAndValue : *ManualArrGlue)
		{
			LuaRawSet(MainState, -1, KeyAndValue.Name, KeyAndValue.Value);
		}
		lua_setglobal(MainState, TCHAR_TO_UTF8(*classname));
		return true;
	}
	else if (FString* OriginName = GlueClassAlias.Find(classname))
	{
		requirecpp(MainState, *OriginName);
		lua_getglobal(MainState, TCHAR_TO_UTF8(**OriginName) );
		lua_setglobal(MainState, TCHAR_TO_UTF8(*classname));
		return true;
	}
	else
	{
		// 		ensureAlwaysMsgf(0, TEXT("notthing add?"));
		return false;
	}
}

bool UTableUtil::requirecpp(lua_State* inL, const char* classname)
{
	lua_geti(inL, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
	lua_pushstring(inL, classname);
	int32 Type = lua_rawget(inL, -2);
	if (Type == LUA_TNIL)
	{
		lua_pop(inL, 2);
		return requirecpp(inL, FString(classname));
	}
	else
	{
		lua_pop(inL, 2);
		return false;
	}
}

int UTableUtil::require_lua(lua_State* inL)
{
	FString classname = lua_tostring(inL, 1);
	UTableUtil::push(inL, requirecpp(inL, classname));
	return 1;
}


void UTableUtil::pushproperty_type(lua_State*inL, UStructProperty* p, const void*ptr)
{
	FString TypeName;
	if (UUserDefinedStruct* BpStruct = Cast<UUserDefinedStruct>(p->Struct))
	{
		MayAddNewStructType(inL, BpStruct);
		TypeName = BpStruct->GetName();
	}
	else
		TypeName = p->Struct->GetStructCPPName();

	void* result = (void*)p->ContainerPtrToValuePtr<uint8>(ptr);
	FString TypeName_nogc = TypeName+"_nogc";
	pushstruct_nogc(inL, TCHAR_TO_UTF8(*TypeName), TCHAR_TO_UTF8(*TypeName_nogc), result);
}

void UTableUtil::pushproperty_type(lua_State*inL, UArrayProperty* Property, const void* ptr)
{
	pushstruct_gc(inL, "ULuaArrayHelper", ULuaArrayHelper::GetHelperCPP((void*)ptr, Property));
}

void UTableUtil::pushproperty_type(lua_State*inL, UMapProperty* Property, const void* ptr)
{
	pushstruct_gc(inL, "ULuaMapHelper", ULuaMapHelper::GetHelperCPP((void*)ptr, Property));
}

void UTableUtil::pushproperty_type(lua_State*inL, USetProperty* Property, const void*ptr)
{
	pushstruct_gc(inL, "ULuaSetHelper", ULuaSetHelper::GetHelperCPP((void*)ptr, Property));
}

void UTableUtil::popproperty(lua_State* inL, int index, UProperty* property, void* ptr)
{
#if LuaDebug
	if (lua_gettop(inL) < FMath::Abs(index))
		ensureAlwaysMsgf(0, TEXT("some bug?"));
#endif
	if (auto* ProcessFunc = PropertyClassToPopFuncMap.Find(property->GetClass()))
	{
		(*ProcessFunc)(inL, index, property, ptr);
	}
	else
	{
		
		if (UStructProperty* p = Cast<UStructProperty>(property))
		{
			popproperty_type(inL, index, p, ptr);
		}
		
		else if (UObjectPropertyBase* p = Cast<UObjectPropertyBase>(property))
		{
			popproperty_type(inL, index, p, ptr);
		}
		
		else
		{
			ensureAlwaysMsgf(0, TEXT("Some Type didn't process"));
		}
	}
}

int UTableUtil::push(lua_State *inL, const void* VoidPtr)
{
	VoidPtrStruct *StructPtr = new VoidPtrStruct(VoidPtr);
	pushstruct_gc(inL, "VoidPtrStruct", StructPtr);
	return 1;
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

void UTableUtil::popproperty_type(lua_State*inL, int index, UInt8Property* p, void*ptr)
{
	int8 value = popiml<int8>::pop(inL, index);
	p->SetPropertyValue_InContainer(ptr, value);
}

void UTableUtil::popproperty_type(lua_State*inL, int index, UInt16Property* p, void*ptr)
{
	int16 value = popiml<int>::pop(inL, index);
	p->SetPropertyValue_InContainer(ptr, value);
}

void UTableUtil::popproperty_type(lua_State*inL, int index, UUInt16Property* p, void*ptr)
{
	uint16 value = popiml<int>::pop(inL, index);
	p->SetPropertyValue_InContainer(ptr, value);
}

void UTableUtil::popproperty_type(lua_State*inL, int index, UInt64Property* p, void*ptr)
{
	int64 value = popiml<int64>::pop(inL, index);
	p->SetPropertyValue_InContainer(ptr, value);
}

void UTableUtil::popproperty_type(lua_State*inL, int index, UUInt64Property* p, void*ptr)
{
	uint64 value = popiml<uint64>::pop(inL, index);
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

void UTableUtil::popproperty_type(lua_State*inL, int index, UObjectProperty* p, void*ptr)
{
	UObject* value = popiml<UObject*>::pop(inL, index);
	p->SetObjectPropertyValue_InContainer(ptr, value);
}

void UTableUtil::popproperty_type(lua_State*inL, int index, UClassProperty* p, void*ptr)
{
	UObject* value = popiml<UObject*>::pop(inL, index);
	p->SetObjectPropertyValue_InContainer(ptr, value);
}

void UTableUtil::popproperty_type(lua_State*inL, int index, UWeakObjectProperty* p, void*ptr)
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
	if (UnrealLua::IsGlueTArray(inL, index))
	{
		void* ArrPtr = (void*)tovoid(inL, index);
		ULuaArrayHelper::GlueArrCopyTo(p, ArrPtr, p->ContainerPtrToValuePtr<void>(ptr));
	}
	else
	{
		ULuaArrayHelper* ArrHelper = (ULuaArrayHelper*)UnrealLua::IsCppPtr(inL, index);
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
}

void UTableUtil::popproperty_type(lua_State*inL, int index, UMapProperty* p, void*ptr)
{
	if (UnrealLua::IsGlueTMap(inL, index))
	{
		void* Ptr = (void*)tovoid(inL, index);
		ULuaMapHelper::GlueMapCopyTo(p, Ptr, p->ContainerPtrToValuePtr<void>(ptr));
	}
	else
	{
		ULuaMapHelper* Helper = (ULuaMapHelper*)UnrealLua::IsCppPtr(inL, index);
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
				popproperty(inL, -2, p->KeyProp, PairPtr);
				popproperty(inL, -1, p->ValueProp, PairPtr);
				lua_pop(inL, 1);
			}
			result.Rehash();
			lua_pop(inL, 1);
		}
		else
		{
			ensureAlwaysMsgf(0, TEXT("not map"));
		}
	}
}

void UTableUtil::popproperty_type(lua_State*inL, int index, USetProperty* p, void*ptr)
{
	if (UnrealLua::IsGlueTSet(inL, index))
	{
		void* ArrPtr = (void*)tovoid(inL, index);
		ULuaSetHelper::GlueSetCopyTo(p, ArrPtr, p->ContainerPtrToValuePtr<void>(ptr));
	}
	else {
		ULuaSetHelper* Helper = (ULuaSetHelper*)UnrealLua::IsCppPtr(inL, index);
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
			ensureAlwaysMsgf(0, TEXT("not set"));
		}
	}
}

void UTableUtil::popproperty_type(lua_State*inL, int index, UMulticastDelegateProperty* p, void*ptr)
{
	ensureAlwaysMsgf(0, TEXT("shouldn't come here"));
}

#if ENGINE_MINOR_VERSION >= 23
void UTableUtil::popproperty_type(lua_State*inL, int index, UMulticastSparseDelegateProperty* p, void*ptr)
{
	ensureAlwaysMsgf(0, TEXT("shouldn't come here"));
}
void UTableUtil::popproperty_type(lua_State*inL, int index, UMulticastInlineDelegateProperty* p, void*ptr)
{
	ensureAlwaysMsgf(0, TEXT("shouldn't come here"));
}
#endif

void UTableUtil::popproperty_type(lua_State*inL, int index, UInterfaceProperty* p, void*ptr)
{
	FScriptInterface* result = (FScriptInterface*)p->GetPropertyValuePtr_InContainer(ptr);
	UObject* value = (UObject*)touobject(inL, index);
	result->SetObject(value);
}

void UTableUtil::popproperty_type(lua_State*inL, int index, UDelegateProperty* p, void*ptr)
{
	ensureAlwaysMsgf(0, TEXT("shouldn't come here"));
}

void pushuobject(lua_State *inL, void* p, bool bgcrecord)
{
	if (p == nullptr)
	{
		lua_pushnil(inL);
		return;
	}
	else 
	{
		UObject* UObject_ptr = (UObject*)p;
		if (!UObject_ptr->IsValidLowLevel())
		{
			lua_pushnil(inL);
			return;
		}
		if (!existdata(inL, p))
		{
			*(void**)lua_newuserdata(inL, sizeof(void *)) = p;

			lua_geti(inL, LUA_REGISTRYINDEX, ExistTableIndex);
			lua_pushlightuserdata(inL, p);
			lua_pushvalue(inL, -3);
			lua_rawset(inL, -3);
			lua_pop(inL, 1);

			UTableUtil::set_uobject_meta(inL, (UObject*)p, -1);
			UTableUtil::addgcref(inL, (UObject*)p);
		}
	}
}

void pushstruct_gc(lua_State *inL, const char* structname, void* p)
{
#if LuaDebug
	if (p == nullptr)
	{
		ensureAlwaysMsgf(0, TEXT("bug"));
		lua_pushnil(inL);
		return;
	}
#endif
		*(void**)lua_newuserdata(inL, sizeof(void *)) = p;
		UTableUtil::requirecpp(inL, structname);
		UTableUtil::setmeta(inL, structname, -1, true);
#if LuaDebug
		UTableUtil::AddGcCount(inL, structname);
#endif
}

void pushstruct_nogc(lua_State *inL, const char* structname, const char* structname_nogc, void* p)
{
	if (p == nullptr)
	{
		lua_pushnil(inL);
		return;
	}
	*(void**)lua_newuserdata(inL, sizeof(void *)) = p;

	UTableUtil::requirecpp(inL, structname);
	UTableUtil::setmeta(inL, structname_nogc, -1, true);
}

void pushstruct_temp(lua_State *inL, const char* structname, const char* structname_nogc,void* p)
{
#if LuaDebug
	if (p == nullptr)
	{
		lua_pushnil(inL);
		return;
	}
#endif

	lua_geti(inL, LUA_REGISTRYINDEX, ExistTableIndex);
	int32 Type = lua_rawgetp(inL, -1, p);
	if (Type == LUA_TNIL)
	{
		lua_pop(inL, 1);
		*(void**)lua_newuserdata(inL, sizeof(void *)) = p;
		UTableUtil::requirecpp(inL, structname);
		UTableUtil::setmeta(inL, structname_nogc, -1, true, false);
		lua_pushvalue(inL, -1);
		lua_rawsetp(inL, -3, p);
		lua_remove(inL, -2);
	}
	else
	{
		lua_remove(inL, -2);
		lua_remove(inL, -2);
	}
}

void UTableUtil::loadlib(lua_State *inL, TMap<FString, UnrealLuaBlueFunc>& funclist, const char* classname, bool bIsStruct /*= false*/, bool bNeedGc /*= true*/, const char* luaclassname /*= nullptr*/)
{
	int i = 0;
	UTableUtil::addmodule(inL, classname, bIsStruct, bNeedGc, luaclassname);
	UTableUtil::openmodule(inL, luaclassname ? luaclassname : classname);
	bool HasStaticProperty = false;
	if (bIsStruct)
	{
		lua_CFunction IndexExtendFunc = nullptr;
		lua_CFunction OverrideIndex = nullptr;
		lua_CFunction OverrideNewIndex = nullptr;
		lua_CFunction NewIndexExtendFunc = nullptr;

		for (auto& paris : funclist)
		{
			bool isStaticProperty = (paris.Value.ExportFlag & ELuaFuncExportFlag::RF_IsStaticProperty) != 0;
			HasStaticProperty = HasStaticProperty || isStaticProperty;
			if (!bNeedGc && paris.Key == "__gc")
				continue;
			else if (paris.Value.ExportFlag & ELuaFuncExportFlag::RF_NewIndexFuncExtend)
			{
				NewIndexExtendFunc = paris.Value.func;
				continue;
			}
			else if (paris.Value.ExportFlag & ELuaFuncExportFlag::RF_IndexFuncExtend)
			{
				IndexExtendFunc = paris.Value.func;
				continue;
			}
			else if (paris.Key == "__index")
			{
				OverrideIndex = paris.Value.func;
				continue;
			}
			else if (paris.Key == "__newindex")
			{
				OverrideNewIndex = paris.Value.func;
				continue;
			}
			else if ((paris.Value.ExportFlag & ELuaFuncExportFlag::RF_GetPropertyFunc) != 0)
			{
				if (!isStaticProperty)
				{
					FString FuncName = paris.Key.RightChop(7);
					if ((paris.Value.ExportFlag & ELuaFuncExportFlag::RF_IsStructProperty) != 0)
					{
						lua_pushstring(inL, TCHAR_TO_UTF8(*FuncName));

						lua_createtable(inL, 2, 0);

						lua_createtable(inL, 0, 10);
						lua_createtable(inL, 0, 1);
						lua_pushstring(inL, "k");
						lua_setfield(inL, -2, "__mode");
						lua_setmetatable(inL, -2);
						lua_rawseti(inL, -2, 2);

						lua_pushlightuserdata(inL, (void*)paris.Value.func);
						lua_rawseti(inL, -2, 1);

						lua_rawset(inL, -3);
					}
					else
					{
						lua_pushstring(inL, TCHAR_TO_UTF8(*FuncName));
						lua_pushlightuserdata(inL, (void*)paris.Value.func);
						lua_rawset(inL, -3);
					}
				}
				AddFuncToTable(inL, -1, TCHAR_TO_UTF8(*paris.Key), paris.Value.func);
			}
			else
			{
				UTableUtil::addfunc(inL, TCHAR_TO_UTF8(*paris.Key), paris.Value.func);
			}
		}

		TMap<FString, TArray<UnrealLuaBlueFunc>>& OverloadFuncs = ClassOverloadFuncs.FindOrAdd(classname);
		BuildOverLoadFuncTree(inL, OverloadFuncs);

		lua_newtable(inL);
		CopyTableForLua(inL);
		for (auto& paris : funclist)
		{
			bool isStaticProperty = (paris.Value.ExportFlag & ELuaFuncExportFlag::RF_IsStaticProperty) != 0;
			if (isStaticProperty && (paris.Value.ExportFlag & ELuaFuncExportFlag::RF_GetPropertyFunc) != 0)
			{
				FString FuncName = paris.Key.RightChop(7);
				if ((paris.Value.ExportFlag & ELuaFuncExportFlag::RF_IsStructProperty) != 0)
				{
					lua_pushstring(inL, TCHAR_TO_UTF8(*FuncName));
					lua_pushcfunction(inL, paris.Value.func);
					lua_pushnil(inL);
					lua_call(inL, 1, 1);
					lua_rawset(inL, -3);
				}
				else
				{
					lua_pushstring(inL, TCHAR_TO_UTF8(*FuncName));
					lua_pushlightuserdata(inL, (void*)paris.Value.func);
					lua_rawset(inL, -3);
				}
			}
		}

		if(OverrideIndex)
			AddFuncToTable(inL, -2, "__index", OverrideIndex, LuaSpace::StackValue(-2));
		else if (IndexExtendFunc)
			AddFuncToTable(inL, -2, "__index", index_struct_func_with_extend, LuaSpace::StackValue(-1), IndexExtendFunc);
		else
			AddFuncToTable(inL, -2, "__index", index_struct_func, LuaSpace::StackValue(-1));
		lua_pop(inL, 1);

		lua_newtable(inL);
		for (auto& paris : funclist)
		{
			if ((paris.Value.ExportFlag & ELuaFuncExportFlag::RF_SetPropertyFunc)!=0) {
				FString FuncName = paris.Key.RightChop(7);
				UTableUtil::addfunc(inL, TCHAR_TO_UTF8(*FuncName), paris.Value.func);
			}
		}

		if(OverrideNewIndex)
			AddFuncToTable(inL, -2, "__newindex", OverrideNewIndex, LuaSpace::StackValue(-2));
		else if (NewIndexExtendFunc)
			AddFuncToTable(inL, -2, "__newindex", newindex_struct_func_with_extend, LuaSpace::StackValue(-1), NewIndexExtendFunc);
		else
			AddFuncToTable(inL, -2, "__newindex", newindex_struct_func, LuaSpace::StackValue(-1));
		lua_pop(inL, 1);
	}
	else
	{
		for (auto& paris : funclist)
		{
			bool isStaticProperty = (paris.Value.ExportFlag & ELuaFuncExportFlag::RF_IsStaticProperty) != 0;
			HasStaticProperty = HasStaticProperty || isStaticProperty;
			if (!bNeedGc && paris.Key == "__gc")
				continue;
			if (paris.Value.ExportFlag & ELuaFuncExportFlag::RF_NewIndexFuncExtend || paris.Value.ExportFlag & ELuaFuncExportFlag::RF_IndexFuncExtend)
				continue;

			UTableUtil::addfunc(inL, TCHAR_TO_UTF8(*paris.Key), paris.Value.func);
		}
		TMap<FString, TArray<UnrealLuaBlueFunc>>& OverloadFuncs = ClassOverloadFuncs.FindOrAdd(classname);
		BuildOverLoadFuncTree(inL, OverloadFuncs);
	}


// not exactly right,because bit field
	if (HasStaticProperty)
	{
		AddStaticMetaToTable(inL, funclist);
	}

	UTableUtil::closemodule(inL);
}

void UTableUtil::AddStaticMetaToTable(lua_State*inL, TMap<FString, UnrealLuaBlueFunc>& funclist, UObject*Class, bool IsObject)
{
	lua_newtable(inL);
	lua_newtable(inL);
	for (auto& paris : funclist)
	{
		bool isGetProperty = (paris.Value.ExportFlag & ELuaFuncExportFlag::RF_GetPropertyFunc)!=0;
		bool isStaticProperty = (paris.Value.ExportFlag & ELuaFuncExportFlag::RF_IsStaticProperty)!=0;
		bool IsStructProperty = (paris.Value.ExportFlag & ELuaFuncExportFlag::RF_IsStructProperty) != 0;
		if (isGetProperty && isStaticProperty)
		{
			FString FuncName = paris.Key.RightChop(7);
			if (IsStructProperty)
			{
				lua_pushstring(inL, TCHAR_TO_UTF8(*FuncName));
				lua_pushcfunction(inL, paris.Value.func);
				lua_pushnil(inL);
				lua_call(inL, 1, 1);
				lua_rawset(inL, -3);
			}
			else
				UTableUtil::addfunc(inL, TCHAR_TO_UTF8(*FuncName), paris.Value.func);
		}
	}
	if (IsObject)
	{
		lua_newtable(inL);
		AddFuncToTable(inL, -3, "__index", ObjectIndexStaticProperty, LuaSpace::StackValue(-1), LuaSpace::StackValue(-3), Class);
		lua_pop(inL, 1);
	}
	else
		AddFuncToTable(inL, -2, "__index", IndexStaticProperty, LuaSpace::StackValue(-1));

	lua_pop(inL, 1);
	lua_newtable(inL);
	for (auto& paris : funclist)
	{
		bool isSetProperty = (paris.Value.ExportFlag & ELuaFuncExportFlag::RF_SetPropertyFunc)!=0;
		bool isStaticProperty = (paris.Value.ExportFlag & ELuaFuncExportFlag::RF_IsStaticProperty)!=0;
		if (isSetProperty && isStaticProperty)
		{
			FString FuncName = paris.Key.RightChop(7);
			UTableUtil::addfunc(inL, TCHAR_TO_UTF8(*FuncName), paris.Value.func);
		}
	}
	if (IsObject)
	{
		AddFuncToTable(inL, -2, "__newindex", ObjectNewindexStaticProperty, LuaSpace::StackValue(-1));
	}
	else
	{
		AddFuncToTable(inL, -2, "__newindex", NewindexStaticProperty, LuaSpace::StackValue(-1));
	}
	lua_pop(inL, 1);
	lua_setmetatable(inL, -2);
}

void UTableUtil::loadstruct(lua_State *inL, TMap<FString, UnrealLuaBlueFunc>& funclist, const char* classname)
{
	FString nogc_name = classname;
	nogc_name += "_nogc";
	loadlib(inL, funclist, classname, true);
	loadlib(inL, funclist, classname, true, false, TCHAR_TO_UTF8(*nogc_name));
}

bool existdata(lua_State*inL, void * p)
{
	lua_geti(inL, LUA_REGISTRYINDEX, ExistTableIndex);
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
	lua_geti(inL, LUA_REGISTRYINDEX, ExistTableIndex);
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
	UE_LOG(LuaLog, Display, TEXT("[lua log] %s"), *content);
}
namespace UnrealLua {
	void* IsCppPtr(lua_State* L, int32 i)
	{
		int LuaType = ue_lua_type(L, i);
		void **u = nullptr;
		switch (LuaType)
		{
			case LUA_TUSERDATA:
				u = static_cast<void**>(lua_touserdata(L, i));
				return *u;
			default:
				return nullptr;
		}
	}

	void ReportError(lua_State*inL, FString Error)
	{
		Error += "\n" + PrintLuaStackOfL(inL);
		UTableUtil::LuaBugReportDelegate.Broadcast(Error);
#if WITH_EDITOR
		FNotificationInfo NotificationInfo(FText::FromString(TEXT("Lua ERROR !!!")));
		const TSharedPtr<SNotificationItem> Notification = FSlateNotificationManager::Get().AddNotification(NotificationInfo);
		//Notification->SetCompletionState(SNotificationItem::ECompletionState::CS_Fail);
#endif
	}


	bool IsGlueTMap(lua_State*inL, int32 Index)
	{
		bool result = false;
		if (lua_isuserdata(inL, Index) != 0)
		{
			lua_getmetatable(inL, Index);
			lua_getfield(inL, -1, "Glue");
			if (lua_isnil(inL, -1))
			{
				result = false;
			}
			else
			{
				result = true;
			}
			lua_pop(inL, 2);
		}
		return result;
	}

	bool IsGlueTSet(lua_State*inL, int32 Index)
	{
		return IsGlueTMap(inL, Index);
	}

	bool IsGlueTArray(lua_State*inL, int32 Index)
	{
		return IsGlueTMap(inL, Index);
	}
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

void UTableUtil::addgcref(lua_State*inL, UObject* p)
{
#if WITH_EDITOR
	lua_State* MainThread = GetMainThread(inL);
	TSet<lua_State*>& States = ObjectReferencedLuaState.FindOrAdd(p);
	if (States.Contains(MainThread))
		return;
	else
	{
		States.Add(MainThread);
		if(States.Num() == 1)
			FLuaGcObj::Get()->objs.Add(p);
	}
#else
	FLuaGcObj::Get()->objs.Add(p);
#endif
}

void UTableUtil::push_totable(lua_State*inL, UScriptStruct* StructType, const void* p)
{
	lua_newtable(inL);
	bool IsBpStruct = StructType->IsA(UUserDefinedStruct::StaticClass());
	for (TFieldIterator<UProperty> PropertyIt(StructType); PropertyIt; ++PropertyIt)
	{
		UProperty* Property = *PropertyIt;
		FString PropertyName = Property->GetName();
		if (IsBpStruct)
		{
			PropertyName = PropertyName.LeftChop(33);
			int32 Index;
			PropertyName.FindLastChar('_', Index);
			PropertyName = PropertyName.LeftChop(PropertyName.Len() - Index);
		}
		push(inL, PropertyName);
		push_totable(inL, Property, p);
		lua_rawset(inL, -3);
	}
}

void UTableUtil::push_totable(lua_State*inL, UProperty* Property, const void* p)
{
	if (UStructProperty* StructProperty = Cast<UStructProperty>(Property))
	{
		push_totable(inL, StructProperty->Struct, StructProperty->ContainerPtrToValuePtr<uint8>(p));
	}
	else if (UTextProperty* TextProperty = Cast<UTextProperty>(Property))
	{
		FText* Text_ptr = (FText*)TextProperty->ContainerPtrToValuePtr<uint8>(p);
		FText* Ret = new FText(*Text_ptr);
		pushstruct_gc(inL, "FText", Ret);
	}
	else if (UArrayProperty* ArrProperty = Cast<UArrayProperty>(Property))
	{
		push_totable(inL, ArrProperty, ArrProperty->ContainerPtrToValuePtr<uint8>(p));
	}
	else if (UMapProperty* MapProperty = Cast<UMapProperty>(Property))
	{
		push_totable(inL, MapProperty, MapProperty->ContainerPtrToValuePtr<uint8>(p));
	}
	else if (USetProperty* SetProperty = Cast<USetProperty>(Property))
	{
		push_totable(inL, SetProperty, SetProperty->ContainerPtrToValuePtr<uint8>(p));
	}
	else
	{
		push_ret_property(inL, Property, p);
	}
}


void UTableUtil::push_totable(lua_State*inL, UArrayProperty* Property, const void* p)
{
	FScriptArrayHelper result(Property, p);
	lua_newtable(inL);
	for (int32 i = 0; i < result.Num(); ++i)
	{
		lua_pushinteger(inL, i + 1);
		push_totable(inL, Property->Inner, result.GetRawPtr(i));
		lua_rawset(inL, -3);
	}
}

void UTableUtil::push_totable(lua_State*inL, UMapProperty* Property, const void* p)
{
	FScriptMapHelper result(Property, p);
	lua_newtable(inL);
	for (int32 i = 0; i < result.Num(); ++i)
	{
		uint8* PairPtr = result.GetPairPtr(i);
		UTableUtil::push_totable(inL, Property->KeyProp, PairPtr);
		UTableUtil::push_totable(inL, Property->ValueProp, PairPtr);
		lua_rawset(inL, -3);
	}

}

void UTableUtil::push_totable(lua_State*inL, USetProperty* Property, const void* p)
{
	FScriptSetHelper result(Property, p);
	lua_newtable(inL);
	for (int32 i = 0; i < result.Num(); ++i)
	{
		UTableUtil::push_totable(inL, Property->ElementProp, result.GetElementPtr(i));
		lua_pushboolean(inL, true);
		lua_rawset(inL, -3);
	}
}

void UTableUtil::rmgcref(lua_State*inL, UObject* p)
{
#if WITH_EDITOR
	lua_State* MainThread = GetMainThread(inL);
	TSet<lua_State*>& States = ObjectReferencedLuaState.FindOrAdd(p);
	States.Remove(MainThread);
	if (States.Num() == 0)
		FLuaGcObj::Get()->objs.Remove(p);
#else
	FLuaGcObj::Get()->objs.Remove(p);
#endif
}

void FLuaGcObj::AddReferencedObjects(FReferenceCollector& Collector)
{
#if STRONG_CHECK_GC_REF
	bool bCheckActorRef = GcCheckActorRef == 1;
	if (bCheckActorRef)
		Collector.AllowEliminatingReferences(false);
#endif
	Collector.AddReferencedObjects(objs);
#if STRONG_CHECK_GC_REF
	if (bCheckActorRef)
		Collector.AllowEliminatingReferences(true);
#endif 
}

void UTableUtil::DoString(lua_State* inL, FString Str)
{
	if (luaL_dostring(inL, TCHAR_TO_UTF8(*Str)))
	{
		UTableUtil::log(FString(lua_tostring(inL, -1)));
		ensureAlwaysMsgf(0, TEXT("Failed to dostring"));
	}
}

void UTableUtil::DoFile(lua_State* inL, FString Str)
{
	Str = Str.Replace(TEXT("."), TEXT("/"));
#if USE_LUASOURCE || WITH_EDITOR
	FString luaDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / TEXT("Plugins/UnrealLua/LuaSource"));
	FString FilePath = luaDir / Str + ".lua";
	if (luaL_dofile(inL, TCHAR_TO_UTF8(*FilePath)))
	{
		UTableUtil::log(FString(lua_tostring(inL, -1)));
		ensureAlwaysMsgf(0, TEXT("Failed to dofile"));
	}
#else    
	FString Code = GetLuaCodeFromPath(Str);
	if (luaL_dostring(inL, TCHAR_TO_UTF8(*Code)))
	{
		UTableUtil::log(FString(lua_tostring(inL, -1)));
		ensureAlwaysMsgf(0, TEXT("Failed to dofile"));
	}
#endif
}

bool UTableUtil::CheckIsChildClass(int32 ParentType, int32 ChildTypeToCheck)
{
	if (ClassRelationShip.Find(ParentType*ChildMaxCount + ChildTypeToCheck))
		return true;
	return false;
}

TArray<UnrealLuaBlueFunc>* UTableUtil::CreateOverloadCandidate(lua_State*inL, const TArray<UnrealLuaBlueFunc>& Data)
{
	TArray<UnrealLuaBlueFunc>* NewCandidates = new TArray<UnrealLuaBlueFunc>(Data);
	auto& AllCandidate = OverloadFuncsCandidate.FindOrAdd(GetMainThread(inL));
	AllCandidate.Add(NewCandidates);
	return NewCandidates;
}

UnrealLua::ArgType UTableUtil::GetNewType(const FString& ClassName)
{
	if (int32* LuaTypePtr = HasInitClassType.Find(ClassName))
		return *LuaTypePtr;

	static int32 CountNow = (int32)(UnrealLua::Type::TYPE_MAX);
	CountNow++;
	HasInitClassType.Add(ClassName, CountNow);
	return CountNow;
}

void UTableUtil::BuildOverLoadFuncTree(lua_State*inL, TMap<FString, TArray<UnrealLuaBlueFunc>>& OverloadFuncs)
{
	for (auto paris : OverloadFuncs)
	{
		UTableUtil::push(inL, paris.Key);
		TArray<UnrealLuaBlueFunc>* CandidateFuncs = CreateOverloadCandidate(inL, paris.Value);
		lua_pushlightuserdata(inL, (void*)CandidateFuncs);
		lua_pushcclosure(inL, CallOverLoadFuncs, 1);
		lua_rawset(inL, -3);
	}
}

LUA_GLUE_ENUM_BEGIN(EStringTableLoadingPolicy, EStringTableLoadingPolicy)
LUA_GLUE_ENUMKEY(Find)
LUA_GLUE_ENUMKEY(FindOrLoad)
LUA_GLUE_ENUMKEY(FindOrFullyLoad)
LUA_GLUE_ENUM_END(EStringTableLoadingPolicy)

static int32 FText__tostring(lua_State* inL)
{
	FText text = popiml<FText>::pop(inL, 1);
	FString str = text.ToString();
	UTableUtil::push(inL, str);
	return 1;
}

static int32 FText_NsLocText(lua_State* inL)
{
	FString textNs; UTableUtil::read<FString>(textNs, inL, 1);
	FString textKey; UTableUtil::read<FString>(textKey, inL, 2);
	FString textLiteral;  UTableUtil::read<FString>(textLiteral, inL, 3);
	FText* Ret = new FText(FInternationalization::ForUseOnlyByLocMacroAndGraphNodeTextLiterals_CreateText(*textLiteral, *textNs, *textKey));
	pushstruct_gc(inL, "FText", Ret);
	return 1;
}

static FText FText_FromString(const FString& str) { return FText::FromString(str); }

static int32 FText_Format(lua_State* inL)
{
	FText* TextFmt = (FText*)tostruct(inL, 1);
	TArray<FFormatArgumentValue> Arr;
	for (auto i = 2; i <= lua_gettop(inL); ++i)
	{
		FText Tmp;
		UTableUtil::read<FText>(Tmp, inL, i);
		Arr.Add(MoveTemp(Tmp));
	}
	UTableUtil::push_ret(inL, FText::Format(*TextFmt, Arr));
	return 1;
}

LUA_GLUE_BEGIN_NOTRAIT(FText)
LUA_GLUE_FUNCTION_OUT(__tostring, FText__tostring)
LUA_GLUE_FUNCTION(FromStringTable)
LUA_GLUE_FUNCTION_OUT(NsLocText, FText_NsLocText)
LUA_GLUE_FUNCTION_OUT(FromString, FText_FromString)
LUA_GLUE_FUNCTION_OUT(Format, FText_Format)
LUA_GLUE_END()

LUA_GLUE_BEGIN(UTableUtil)
LUA_GLUE_FUNCTION_NAME(require, require_lua)
LUA_GLUE_END()
