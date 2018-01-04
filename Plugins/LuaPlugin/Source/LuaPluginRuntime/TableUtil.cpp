
#include "TableUtil.h"
#include "LuaScript.h"
#include "Paths.h"
#include "TextProperty.h"
#include "LuaFixLink.h"
#include "LuaMapHelper.h"
#include "../Launch/Resources/Version.h"

DEFINE_LOG_CATEGORY(LuaLog);

FLuaInitDelegates& UTableUtil::GetInitDelegates()
{
	static FLuaInitDelegates Delegates;
	return Delegates;
}


lua_State* UTableUtil::GetTheOnlyLuaState()
{
	return TheOnlyLuaState;
}

LuaTickObject* UTableUtil::PtrTickObject = nullptr;
lua_State* UTableUtil::TheOnlyLuaState = nullptr;
int32 UTableUtil::ManualInitCount = 0;
bool UTableUtil::HasManualInit = false;
TSet<FString> UTableUtil::SupportedNativeStruct;
FDelegateHandle LuaPreGarbageCollectDelegateHandle;
// bool UTableUtil::bIsInsCall = false;

#ifdef LuaDebug
TMap<FString, int> UTableUtil::countforgc;
#endif

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

static int32 LuaPanic(lua_State *L)
{
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
	LuaFixLink();
	if (IsManual)
	{
		++ManualInitCount;
	}
	if (TheOnlyLuaState != nullptr)
	{
		if (IsManual && !HasManualInit)
		{
			shutdown_internal();
		}
		else
			return;
	}

	SupportedNativeStruct.Reset();
	auto l = lua_newstate(LuaAlloc, nullptr);
	if (l) lua_atpanic(l, &LuaPanic);
	luaL_openlibs(l);
	TheOnlyLuaState = l;
	FString gameDir = FPaths::ConvertRelativePathToFull(FPaths::GameDir());
	FString luaDir = FPaths::ConvertRelativePathToFull(FPaths::GameDir() / TEXT("LuaSource"));

#if  WITH_EDITOR
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

		//when lua has correspond table of the ins, push the table
		lua_newtable(TheOnlyLuaState);
		lua_newtable(TheOnlyLuaState);
		lua_pushstring(TheOnlyLuaState, "kv");
		lua_setfield(TheOnlyLuaState, -2, "__mode");
		lua_setmetatable(TheOnlyLuaState, -2);
		lua_setfield(TheOnlyLuaState, LUA_GLOBALSINDEX, "_objectins2luatable");

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
		//register all function
		// 		ULuaLoad::LoadAll(L);
		// 		ULuaLoadGame::LoadAll(L);
		GetInitDelegates().Broadcast();
		if (PtrTickObject == nullptr)
			PtrTickObject = new LuaTickObject();
		call("Init", IsManual);
		LuaPreGarbageCollectDelegateHandle = FCoreUObjectDelegates::PreGarbageCollect.AddStatic(&UTableUtil::GC);
	}
	HasManualInit = IsManual;
}


void UTableUtil::GC()
{
	if (TheOnlyLuaState != nullptr)
	{
		call("GC");
		lua_gc(TheOnlyLuaState, LUA_GCCOLLECT, 0);
	}
}

void UTableUtil::shutdown()
{
	--ManualInitCount;
	if (TheOnlyLuaState != nullptr && ManualInitCount == 0)
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
	FCoreUObjectDelegates::PreGarbageCollect.Remove(LuaPreGarbageCollectDelegateHandle);
	TheOnlyLuaState = nullptr;
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
			lua_pcall(inL, 1, 1, 0);
		}
	}
	return 1;
}

int32 newindexFunc(lua_State* inL)
{
	if (lua_isuserdata(inL, 1))
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
			lua_call(inL, 2, 1);
		}
	}
	else if (lua_istable(inL, 1))
	{
		lua_rawset(inL, -3);
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


void UTableUtil::initmeta(bool bIsStruct, bool bNeedGc)
{
	lua_pushstring(TheOnlyLuaState, "__index");
	lua_pushcfunction(TheOnlyLuaState, indexFunc);
	lua_rawset(TheOnlyLuaState, -3);
	lua_pushstring(TheOnlyLuaState, "cast");
	lua_pushcfunction(TheOnlyLuaState, cast);
	lua_rawset(TheOnlyLuaState, -3);
	lua_pushstring(TheOnlyLuaState, "__newindex");
	lua_pushcfunction(TheOnlyLuaState, newindexFunc);
	lua_rawset(TheOnlyLuaState, -3);
	if (bNeedGc) 
	{
		lua_pushstring(TheOnlyLuaState, "__gc");
		if (!bIsStruct)
			lua_pushcfunction(TheOnlyLuaState, uobjcet_gcfunc);
		else
			lua_pushcfunction(TheOnlyLuaState, struct_gcfunc);
		lua_rawset(TheOnlyLuaState, -3);
	}
	lua_pushstring(TheOnlyLuaState, "__iscppclass");
	lua_pushboolean(TheOnlyLuaState, true);
	lua_rawset(TheOnlyLuaState, -3);
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
	if (lua_isnil(L, i))
	{
		ensureAlwaysMsgf(0, L"struct can't be nil");
		return nullptr;
	}
	return tovoid(L, i);
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

void* UTableUtil::tousertype(lua_State* InL, const char* classname, int i)
{
	return touobject(InL, i);
}

void UTableUtil::setmeta(lua_State *inL, const char* classname, int index, bool bIsStruct)
{
	luaL_getmetatable(inL, classname);
	if (lua_istable(inL, -1))
	{
		lua_setmetatable(inL, index - 1);
	}
	else
	{
		lua_pop(inL, 1);
		UTableUtil::addmodule(classname, bIsStruct, true);
		luaL_getmetatable(inL, classname);
		lua_setmetatable(inL, index - 1);
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


void UTableUtil::bpcall(const char* funcname, UFunction* funcsig, void* ptr)
{
	if (TheOnlyLuaState == nullptr)
		init();
	ue_lua_pushcfunction(TheOnlyLuaState, ErrHandleFunc);
	lua_getfield(TheOnlyLuaState, LUA_GLOBALSINDEX, "Call");
	push(TheOnlyLuaState, funcname);
	push(TheOnlyLuaState, (UObject*)ptr);
	int ParamCount = 2;
	TArray<UProperty*> PushBackParms;
	for (TFieldIterator<UProperty> It(funcsig); It && (It->GetPropertyFlags() & (CPF_Parm)); ++It)
	{
		auto Prop = *It;

		if ((Prop->GetPropertyFlags() & (CPF_ConstParm | CPF_OutParm | CPF_ReturnParm)) & CPF_ReturnParm)
		{
			PushBackParms.Insert(Prop, 0);
		}
		else if ((Prop->GetPropertyFlags() & (CPF_ConstParm | CPF_OutParm | CPF_ReturnParm)) & CPF_OutParm)
		{
			PushBackParms.Add(Prop);
		}
		else
		{
			pushproperty(TheOnlyLuaState, Prop, ptr);
			++ParamCount;
		}
	}
	if (lua_pcall(TheOnlyLuaState, ParamCount, 0, -(ParamCount + 2)))
	{
#ifdef LuaDebug
		FString error = lua_tostring(TheOnlyLuaState, -1);
		ensureAlwaysMsgf(0, *error);
#endif
		log(lua_tostring(TheOnlyLuaState, -1));
	}
	lua_pop(TheOnlyLuaState, 1);
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
	else
	{
		ensureAlwaysMsgf(0, L"Some type didn't process");
	}
}

void UTableUtil::push_ret_property(lua_State*inL, UProperty* property, const void* ptr)
{
	if (UStructProperty* p = Cast<UStructProperty>(property))
	{
		UScriptStruct* Struct = p->Struct;
		FString CppTypeName = Struct->GetStructCPPName();
		uint8* result = (uint8*)FMemory::Malloc(p->GetSize());
		ptr = (void*)p->ContainerPtrToValuePtr<uint8>(ptr);
		p->CopyCompleteValueFromScriptVM(result, ptr);
		pushstruct_gc(inL, TCHAR_TO_UTF8(*CppTypeName), result);
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
			ArrHelper->CopyFrom(p, (void*)ptr);
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
			Helper->CopyFrom(p, (void*)ptr);
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
			Helper->CopyFrom(p, (void*)ptr);
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
	lua_pushinteger(inL, (int64)p->GetPropertyValue_InContainer(ptr));
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
	lua_pushinteger(inL, (int)p->GetUnderlyingProperty()->GetValueTypeHash(ptr));
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

void UTableUtil::pushproperty_type(lua_State*inL, UStructProperty* p, const void*ptr)
{
	FString CppTypeName = p->Struct->GetStructCPPName();
	void* result = (void*)p->ContainerPtrToValuePtr<uint8>(ptr);
	pushstruct_nogc(inL, TCHAR_TO_UTF8(*CppTypeName), result);
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
	if (UIntProperty* p = Cast<UIntProperty>(property))
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
	int32 value = popiml<int64>::pop(inL, index);
	p->SetPropertyValue_InContainer(ptr, value);
}

void UTableUtil::popproperty_type(lua_State*inL, int index, UUInt32Property* p, void*ptr)
{
	int32 value = popiml<int>::pop(inL, index);
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
	popproperty(inL, index, p->GetUnderlyingProperty(), ptr);
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
		ArrHelper->CopyTo(p, ptr);
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
 		Helper->CopyTo(p, ptr);
	}
	else if(lua_istable(inL, index))
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
		Helper->CopyTo(p, ptr);
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

		UObject* UObject_p = static_cast<UObject*>(p);
		UClass* Class = UObject_p->GetClass();
		while (!Class->HasAnyClassFlags(CLASS_Native))
			Class = Class->GetSuperClass();
		const char* classname = TCHAR_TO_UTF8(*FString::Printf(TEXT("%s%s"), Class->GetPrefixCPP(), *Class->GetName()));
#ifdef LuaDebug
		if (UTableUtil::countforgc.Contains(classname))
			UTableUtil::countforgc[classname]++;
		else
			UTableUtil::countforgc.Add(classname, 1);
#endif
		UTableUtil::addgcref(static_cast<UObject*>(p));
		UTableUtil::setmeta(inL, classname, -1);
	}
	else
	{
		lua_getfield(inL, LUA_GLOBALSINDEX, "_objectins2luatable");
		lua_pushvalue(inL, -2);
		lua_rawget(inL, -2);
		if (lua_isnil(inL, -1))
		{
			lua_pop(inL, 2);
		}
		else
		{
			lua_remove(inL, -2);
			lua_remove(inL, -2);
		}
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
	}
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
	if (lua_isnil(inL, -1))
	{
		lua_pop(inL, 2);
		return false;
	}
	else
	{
		lua_getfield(inL, LUA_GLOBALSINDEX, "_objectins2luatable");
		lua_pushvalue(inL, -2);
		lua_rawget(inL, -2);
		if (lua_isnil(inL, -1))
		{
			lua_pop(inL, 3);
			return false;
		}
		else
		{
			lua_pop(inL, 3);
			return true;
		}
	}
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
