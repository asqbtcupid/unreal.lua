#pragma once
#include "LuaApi.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Tickable.h"
#include "GCObject.h"
#include "ConstructorHelpers.h"
#include "LuaDelegateMulti.h"
#include "SubclassOf.h"
#include "Class.h"
#include "LuaArrayHelper.h"
#include "LuaMapHelper.h"
#include "LuaSetHelper.h"
#include "AndOrNot.h"
#include "Engine/UserDefinedStruct.h"
#include "unreallua_template.h"
#include "functionoverload.h"
#include <string>
#include "TableUtil.generated.h"

static uint8* GetBpStructTempIns(const FString& StructTypeName, int32 PropertySize)
{
	const int32 size = 30;
	struct InsConfig
	{
		InsConfig(int32 PropertySize) :StructSize(PropertySize), index(-1)
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
		InsConfig NewConfig(PropertySize);
		AllStructMemPool.Add(StructTypeName, NewConfig);
		ConfigPtr = AllStructMemPool.Find(StructTypeName);
	}
	return ConfigPtr->GetTempIns();
}

DECLARE_MULTICAST_DELEGATE_OneParam(FLuaBugReport, FString)
DECLARE_DELEGATE_TwoParams(FGCRefChange, UObject*, bool)

class UTableUtil;
struct EnumGlueStruct
{
	FString Name;
	int32 Value;
};

enum ELuaFuncExportFlag
{
	RF_NoFlag = 0x00000000,	///< No flags, used to avoid a cast
	RF_NoExport = 0x00000001,	/// Don't Export
	RF_GetPropertyFunc = 0x00000002,	
	RF_SetPropertyFunc = 0x00000004,
	RF_IsStaticProperty = 0x00000008,
	RF_IndexFuncExtend = 0x00000010,
	RF_NewIndexFuncExtend = 0x00000020,
	RF_OverLoad = 0x00000040,
	RF_IsStructProperty = 0x00000080,
};

struct UnrealLuaBlueFunc
{
	const char *name;
	lua_CFunction func;
	uint32 ExportFlag=0;
	using CheckFuncType = TFunction<UnrealLua::CheckResult(UnrealLua::ArgType, lua_State*, int32)>;
	struct CheckFuncsType
	{
		int32 IndexOffset;
		TArray<bool> ValidParamCount;
		TArray<CheckFuncType> CheckFuncs;
	};
// 	using CheckFuncsType = TMap<uint32, TArray<TFunction<bool(lua_State*, int32)>>>;
	CheckFuncsType TheCheckFuncs;
	UnrealLuaBlueFunc(const char * _name, lua_CFunction _func, uint32 _ExportFlag = 0, CheckFuncsType _TheCheckFuncs = CheckFuncsType())
		:name(_name), func(_func), ExportFlag(_ExportFlag), TheCheckFuncs(_TheCheckFuncs){}
	operator luaL_Reg()
	{
		return luaL_Reg{ name, func };
	}
};

struct UnrealLuaClass
{
	UnrealLua::ArgType MyClassType;
	TArray<UnrealLua::ArgType> BaseClassType;

	UnrealLuaClass(int32 _MyClassType = -1, TArray<int32> _BaseClassType = TArray<int32>()):
		MyClassType(_MyClassType), BaseClassType(_BaseClassType)
	{}
};

struct UserDefinedClassConfig
{
	FString ClassName;
	UnrealLua::ArgType DefinedClassType;
	UserDefinedClassConfig(FString _ClassName, UnrealLua::ArgType _Type):
		ClassName(_ClassName), DefinedClassType(_Type)
	{}
};

namespace LuaSpace {
	struct StackValue
	{
		StackValue(int _StatckIndex) :StatckIndex(_StatckIndex - 1) {}
		int StatckIndex;
	};
}

struct LuaStringBuffer
{
	const char* Buffer;
	int Len;
};

#ifndef USE_LUASOURCE
#define USE_LUASOURCE	0
#endif

#ifndef STRONG_CHECK_GC_REF
#define STRONG_CHECK_GC_REF	0
#endif

#ifndef LuaDebug
#define LuaDebug 0
#endif

#define REFLECTION_FUNC(PropertyType) static void pushproperty_##PropertyType(lua_State*inL, UProperty* p, const void*ptr)\
										{pushproperty_type(inL, (PropertyType*)p, ptr); }\
									 static void popproperty_##PropertyType(lua_State*inL, int index, UProperty* p, void*ptr)\
										{popproperty_type(inL, index, (PropertyType*)p, ptr); }\
									static void pushproperty_type(lua_State*inL, PropertyType* p, const void*ptr);\
									static void pushproperty_type_valueptr(lua_State*inL, PropertyType* p, const void*ptr);\
									static void popproperty_type(lua_State*inL, int index, PropertyType* p, void*ptr);

typedef int luavalue_ref;
DECLARE_LOG_CATEGORY_EXTERN(LuaLog, Log, All);

using luafunc = int(struct lua_State*);

LUAPLUGINRUNTIME_API bool existdata(lua_State*inL, void * p);
LUAPLUGINRUNTIME_API void pushuobject(lua_State *inL, void* p, bool bgcrecord = false);
LUAPLUGINRUNTIME_API void pushstruct_gc(lua_State *inL, const char* structname, void* p);
LUAPLUGINRUNTIME_API void pushstruct_nogc(lua_State *inL, const char* structname, const char* structname_nogc, void* p);
LUAPLUGINRUNTIME_API void pushstruct_temp(lua_State *inL, const char* structname, const char* structname_nogc, void* p);
LUAPLUGINRUNTIME_API void* touobject(lua_State* L, int i);
#if LuaDebug
LUAPLUGINRUNTIME_API void* tostruct(lua_State* L, int i);
#else
FORCEINLINE void* tostruct(lua_State* L, int i)
{
	auto u = static_cast<void**>(lua_touserdata(L, i));
	return *u;
}
#endif
LUAPLUGINRUNTIME_API int ErrHandleFunc(lua_State*L);
LUAPLUGINRUNTIME_API FString PrintLuaStackOfL(lua_State* inL);
#if LuaDebug
LUAPLUGINRUNTIME_API void* tovoid(lua_State* L, int i);
#else
FORCEINLINE void* tovoid(lua_State* L, int i)
{
	int LuaType = ue_lua_type(L, i);
	void **u = nullptr;
	switch (LuaType)
	{
	case LUA_TUSERDATA:
		u = static_cast<void**>(lua_touserdata(L, i));
		return *u;
	case LUA_TTABLE:
		if (i < 0)
			i = lua_gettop(L) + i + 1;
		lua_rawgeti(L, i, CppInstanceIndex);
		u = static_cast<void**>(lua_touserdata(L, -1));
		lua_pop(L, 1);
		return *u;
	case LUA_TNIL:
		return nullptr;
	case LUA_TNONE:
		return nullptr;
	default:
		return nullptr;
	};
}
#endif
FORCEINLINE void* tostructornull(lua_State* L, int i)
{
	auto u = static_cast<void**>(lua_touserdata(L, i));
	if (u)
		return *u;
	else
		return nullptr;
}

template<class T>
T PopInternal(lua_State *L, int index, typename TEnableIf<!TIsStruct<T>::Value, int>::Type* p = nullptr)
{
	return (T)ue_lua_tointeger(L, index);
}

template<class T>
T PopInternal(lua_State *L, int index, typename TEnableIf<TIsStruct<T>::Value, int>::Type* p = nullptr)
{
	T* ptr = (T*)tostruct(L, index);
#if LuaDebug
	check(ptr);
#endif
	return *ptr;
}

template<class T>
void* PopPointerByType(lua_State*L, int index, typename TEnableIf<TIsStruct<T>::Value, int>::Type* p = nullptr)
{
	return tostruct(L, index);
}

template<class T>
void* PopPointerByType(lua_State*L, int index, typename TEnableIf<!TIsStruct<T>::Value, int>::Type* p = nullptr)
{
	return tovoid(L, index);
}

template<class T>
void* PopPointerByTypeOrNull(lua_State*L, int index, typename TEnableIf<TIsStruct<T>::Value, int>::Type* p = nullptr)
{
	return tostructornull(L, index);
}

template<class T>
void* PopPointerByTypeOrNull(lua_State*L, int index, typename TEnableIf<!TIsStruct<T>::Value, int>::Type* p = nullptr)
{
	return tovoid(L, index);
}

template<class T> T* tovoidtype(lua_State* inL, int index) { return (T*)(PopPointerByType<T>(inL, index)); }
template<class T> T* tovoidtypeornil(lua_State* inL, int index) { return (T*)(PopPointerByTypeOrNull<T>(inL, index)); }
template<class T>struct popiml {static T pop(lua_State *L, int index){return (T)PopInternal<T>(L, index);}};
template<class T>struct popiml<T*> {static T* pop(lua_State *L, int index){return (T*)(PopPointerByType<T>(L, index));}};
template<> struct popiml<void*> {static void* pop(lua_State *L, int index) { return tovoid(L, index);}};
template<> struct popiml<uint8> {static uint8 pop(lua_State *L, int index) { return (uint8)ue_lua_tointeger(L, index); }};
template<> struct popiml<int32> {static int pop(lua_State *L, int index) { return (int32)ue_lua_tointeger(L, index); }};
template<class T> struct popiml<TWeakObjectPtr<T>> {static T* pop(lua_State *L, int index) { return (T*)touobject(L, index); }};
template<class T> struct popiml<TEnumAsByte<T>> {static T pop(lua_State *L, int index) { return (T)ue_lua_tointeger(L, index); }};
template<class T> struct popiml<TSubclassOf<T>> {static UClass* pop(lua_State *L, int index) { return (UClass*)touobject(L, index); }};
template<> struct popiml<bool> {static bool pop(lua_State *L, int index) { return !!(ue_lua_toboolean(L, index)); }};
template<> struct popiml<FName> {static FName pop(lua_State *L, int index) { return FName(UTF8_TO_TCHAR(ue_lua_tolstring(L, index, NULL))); }};
template<> struct popiml<FString> { static FString pop(lua_State *L, int index) { return UTF8_TO_TCHAR(ue_lua_tolstring(L, index, NULL)); } };
template<> struct popiml<const ANSICHAR*> { static const ANSICHAR* pop(lua_State *L, int index) { return ue_lua_tolstring(L, index, NULL); } };
template<> struct popiml<std::string> {	static std::string pop(lua_State *L, int index) { return std::string(ue_lua_tolstring(L, index, NULL)); }};
template<> struct popiml<float> {static float pop(lua_State *L, int index) { return (float)ue_lua_tonumber(L, index); }};
template<> struct popiml<double> {static double pop(lua_State *L, int index) { return (double)ue_lua_tonumber(L, index); }};
template<> struct popiml<void> {static void pop(lua_State *L, int index) { ue_lua_pop(L, 1); }};
template<> struct popiml<uint32> {
#ifdef USE_LUA53 
	static uint32 pop(lua_State *L, int index) { return (uint32)ue_lua_tointeger(L, index); }
#else
	static uint32 pop(lua_State *L, int index) { return (uint32)ue_lua_tonumber(L, index); }
#endif
};
template<> struct popiml<uint64> {
#ifdef USE_LUA53 
	static uint64 pop(lua_State *L, int index) { return (uint64)ue_lua_tointeger(L, index); }
#else
	static uint64 pop(lua_State *L, int index) { return (uint64)ue_lua_tonumber(L, index); }
#endif
};
template<> struct popiml<int64> {
#ifdef USE_LUA53 
	static int64 pop(lua_State *L, int index) { return (int64)ue_lua_tointeger(L, index); }
#else 
	static int64 pop(lua_State *L, int index) { return (int64)ue_lua_tonumber(L, index); }
#endif
};
template<> struct popiml<FText> {
	static FText pop(lua_State *L, int index) {
		if (ue_lua_isstring(L, index))
			return FText::FromString(UTF8_TO_TCHAR(ue_lua_tolstring(L, index, NULL)));
		else if (ue_lua_isuserdata(L, index))
		{
			FText* Ptr = (FText*)tostruct(L, index);
			if (Ptr)
				return *Ptr;
			else
				return FText::FromString("");
		}
		else {
			ensureAlwaysMsgf(0, TEXT("bug"));
			return FText::FromString("");
		}

		return FText::GetEmpty();
	}
};
template<> struct popiml<const TCHAR*> {
	static const TCHAR* pop(lua_State *L, int index) { 
		static FString Cached[5];
		static int i= -1;
		i = (i + 1) % 5;
		Cached[i] = UTF8_TO_TCHAR(ue_lua_tolstring(L, index, NULL));
		return *Cached[i];
	}
};

namespace UnrealLua {
	LUAPLUGINRUNTIME_API void* IsCppPtr(lua_State* L, int32 i);
	LUAPLUGINRUNTIME_API void ReportError(lua_State*inL, FString Error);
	LUAPLUGINRUNTIME_API bool IsGlueTMap(lua_State*inL, int32 Index);
	LUAPLUGINRUNTIME_API bool IsGlueTSet(lua_State*inL, int32 Index);
	LUAPLUGINRUNTIME_API bool IsGlueTArray(lua_State*inL, int32 Index);
	
	template<typename T>
	inline void readimp(T& result, lua_State*inL, int index)
	{
		result = popiml<T>::pop(inL, index);
	}

	inline void readimp(lua_State* & result, lua_State*inL, int index)
	{
		result = inL;
	}

	template<class T>
	void read(T& result, lua_State*inL, int index, typename TEnableIf<!THas_ReadValueFromLuaStack<T>::Value, int>::Type *p = nullptr)
	{
		readimp(result, inL, index);
	}

	template<class T>
	void read(T& result, lua_State*inL, int index, typename TEnableIf<THas_ReadValueFromLuaStack<T>::Value, int>::Type *p = nullptr)
	{
		CustomTypeToLua<T>::ReadValueFromLuaStack(result, inL, index);
	}

	template<class T>
	void read(T*& result, lua_State*inL, int index, typename TEnableIf<!THas_ReadPtrFromLuaStack<T>::Value, int>::Type *p = nullptr)
	{
		readimp(result, inL, index);
	}

	template<class T>
	void read(T*& result, lua_State*inL, int index, typename TEnableIf<THas_ReadPtrFromLuaStack<T>::Value, int>::Type *p = nullptr)
	{
		CustomTypeToLua<T>::ReadPtrFromLuaStack(result, inL, index);
	}


	template<class T, bool IsStruct>
	struct PopFromStack_imp
	{
		static T pop(lua_State*inL, int index)
		{
			T result;
			read(result, inL, index);
			ue_lua_pop(inL, 1);
			return result;
		}
	};

	template<class T>
	struct PopFromStack_imp<T, true>
	{
		static T& pop(lua_State*inL, int index)
		{
			T* result;
			read(result, inL, index);
			ue_lua_pop(inL, 1);
			if (result)
				return *result;
			else
				return GetTempIns<T>();
		}
	};

	template<>
	struct PopFromStack_imp<FText, true>
	{
		static FText pop(lua_State*inL, int index)
		{
			return popiml<FText>::pop(inL, index);
		}
	};

	template<typename T>
	struct PopFromStack:public PopFromStack_imp<T, TIsStruct<T>::Value>
	{
		enum{Value = 1};
	};

	template<>
	struct PopFromStack<void>
	{
		enum { Value = 0 };
		static void pop(lua_State*inL, int index)
		{}
	};
}

DECLARE_MULTICAST_DELEGATE(FLuaInitDelegates);
DECLARE_MULTICAST_DELEGATE_OneParam(FLuaOnPowerStateDelegate, lua_State*);

UCLASS()
class LUAPLUGINRUNTIME_API UTableUtil : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	static TMap<UObject*, TSet<lua_State*>> ObjectReferencedLuaState;
	static TSet<lua_State*> HasShutdownLuaState;
	static TMap<lua_State*, TArray<struct LuaBaseBpInterface*>> ExistBpInterfaceForState;
	static TMap<UFunction*, struct MuldelegateBpInterface*> MultiDlgInterfaces;
	static TMap<FString, TMap<FString, TArray<UnrealLuaBlueFunc>>> ClassOverloadFuncs;
	static TMap<FString, int32> ClassDefineTypeInLua;
	static TMap<int32, TArray<int32>> ChildsParentTypesInLua;
	static TMap<int32, bool> ClassRelationShip;
	static TMap<FString, UserDefinedClassConfig> HasAddedUserDefinedClass;
	static TMap<FString, int32> HasInitClassType;
	static TMap<lua_State*, TArray<TArray<UnrealLuaBlueFunc>*>> OverloadFuncsCandidate;
	static void BuildOverLoadFuncTree(lua_State*inL, TMap<FString, TArray<UnrealLuaBlueFunc>>& OverloadFuncs);
public:
	static bool CheckIsChildClass(int32 ParentType, int32 ChildTypeToCheck);
	static TArray<UnrealLuaBlueFunc>* CreateOverloadCandidate(lua_State*inL, const TArray<UnrealLuaBlueFunc>& Data);
	static UnrealLua::ArgType GetNewType(const FString& ClassName);
	static struct LuaBaseBpInterface* GetBpPropertyInterface(lua_State*inL, UProperty* BpField);
	static struct LuaBaseBpInterface* GetBpFuncInterface(lua_State*inL, UFunction* BpFunction);
	static struct MuldelegateBpInterface* GetMultiDlgInterface(UFunction* SigFunction);
	static bool IsStateShutdown(lua_State*inL);
	static FLuaBugReport LuaBugReportDelegate;
	FORCEINLINE static void JustPushAndRecord(lua_State*inL, void * p)
	{
		*(void**)ue_lua_newuserdata(inL, sizeof(void *)) = p;
		lua_geti(inL, LUA_REGISTRYINDEX, ExistTableIndex);
		lua_pushlightuserdata(inL, p);
		lua_pushvalue(inL, -3);
		lua_rawset(inL, -3);
		lua_pop(inL, 1);
	}


	static bool check_exist_and_add(lua_State*inL, void * p)
	{
		if (!existdata(inL, p))
		{
			JustPushAndRecord(inL, p);
			return false;
		}
		else
			return true;
	};

	static bool HasInit;
	static void MapPropertyToPushPopFunction();
	static FLuaInitDelegates& GetInitDelegates();
	static void Init();
	static FLuaOnPowerStateDelegate& GetOnPowerStateDelegate();
	static TMap<lua_State*, TMap<UObject*, TMap<FString, UClass*>>> NeedGcBpClassName;
	static TMap<lua_State*, TMap<UClass*, TSharedPtr<FTCHARToUTF8>>> HasAddUClass;
	static TMap<lua_State*, TSet<FString>> HasRequire;
#if LuaDebug
	static TMap<lua_State*, TMap<FString, int>> countforgc;
	static void AddGcCount(lua_State*inL, const FString& classname);
	static void SubGcCount(lua_State*inL, const FString& classname);
#endif
	static TMap<FString, UUserDefinedStruct*> bpname2bpstruct;
	static void addmodule(lua_State *inL, const char* classname, bool bIsStruct, bool bNeedGc, const char* luaclassname = nullptr);
	static void useCustomLoader(lua_State *inL);
	UFUNCTION()
	static void useCustomLoader(UObject *inL);

	static void PowerTheState(lua_State* inL);
	UFUNCTION()
		static void PowerTheState(UObject* inL);

	static void SetAIsBParents(const TArray<int32>& A, int32 B);
	static void InitClassInheritRelationship();
	static void openmodule(lua_State *inL, const char* classname);
	static void closemodule(lua_State *inL);
	static void addfunc(lua_State *inL,const char* classname, luafunc f);
	static void initmeta(lua_State *inL, const char* classname, bool bIsStruct, bool bNeedGc = true, const char* luaclassname = nullptr);
	static void init_refelction_native_uclass_meta(lua_State* inL, const char* classname, UClass* TheClass);
	static void init_reflection_struct_meta(lua_State* inL, const char* structname, UScriptStruct* StructClass, bool bIsNeedGc);
	static void setmeta(lua_State *inL, const char* classname, int index, bool bIsStruct = false, bool bNeedGc = true);
	static bool ExistClassInGlobal(lua_State* inL, const char* classname);
	static void set_uobject_meta(lua_State *inL, UObject* Obj, int index);
	static void OnWorldCleanUp(lua_State*inL, UWorld* World);
	static void MayAddNewStructType(lua_State *inL, UUserDefinedStruct* BpStruct);
	static void AddAndOpenUserDefineClass(const char* ClassName, const UnrealLuaBlueFunc BasicFuncList[], TArray<UnrealLuaBlueFunc> funclist, TArray<FString> BaseClass = TArray<FString>(), UnrealLuaClass ClassConfig = UnrealLuaClass());
	static void AddExpandClassGlue(const char* ClassName, TArray<UnrealLuaBlueFunc> meta_funclist, TArray<UnrealLuaBlueFunc> funclist, TArray<FString> BaseClass = TArray<FString>());
	static void AddEnumGlue(const char* EnumName, TArray<EnumGlueStruct> ArrGlue);
	static void AddBaseClassFuncList(TMap<FString, UnrealLuaBlueFunc>* MyFuncList, FString MyClassName);
	static void AddAliasName(const FString& AliasName, const FString& OriginalName);
	
	static TMap<FString, FString> GlueClassAlias;
	static TMap<FString, TArray<FString>> ClassBaseClass;
	static TMap<FString, TMap<FString, UnrealLuaBlueFunc>> UserDefineGlue;
	static TMap<FString, TMap<FString, UnrealLuaBlueFunc>> ExpandClassGlue;
	static TMap<FString, TArray<EnumGlueStruct> > ManualEnumGlue;

	static TMap<UClass*, TFunction<void(lua_State*, UProperty*, const void*)> > PropertyClassToPushFuncMap;
	static TMap<UClass*, TFunction<void(lua_State*, int, UProperty*, void*)> > PropertyClassToPopFuncMap;
	static bool requirecpp(lua_State* inL, const FString& classname);
	static bool requirecpp(lua_State* inL, const char* classname);
	
	static int require_lua(lua_State* inL);

	static lua_State* GetMainThread(lua_State *inL)
	{
		lua_geti(inL, LUA_REGISTRYINDEX, LUA_RIDX_MAINTHREAD);
		lua_State* MainState = lua_tothread(inL, -1);
		lua_pop(inL, 1);
		return MainState;
	}

	static void DoString(lua_State* inL, FString Str);
	static void DoFile(lua_State* inL, FString Str);

	static void loadlib(lua_State *inL, TMap<FString, UnrealLuaBlueFunc>& funclist, const char* classname, bool bIsStruct = false, bool bNeedGc = true, const char* luaclassname = nullptr);
	static void AddStaticMetaToTable(lua_State*inL, TMap<FString, UnrealLuaBlueFunc>& funclist,UObject* Class = nullptr, bool IsObject = false);
	static void loadstruct(lua_State *inL, TMap<FString, UnrealLuaBlueFunc>& funclist, const char* classname);
	static luavalue_ref ref_luavalue(lua_State*inL, int index);
	static void unref(lua_State*inL, luavalue_ref r);

	static void log(const FString& content);

	static void ShutdownTheState(lua_State* inL);
	static bool existluains(lua_State*inL, void * p);

	static void rmgcref(lua_State*inL, UObject* p);
	static void addgcref(lua_State*inL, UObject* p);
	static void push_totable(lua_State*inL, UScriptStruct* StructType, const void* p);
	static void push_totable(lua_State*inL, UProperty* Property, const void* p);
	static void push_totable(lua_State*inL, UArrayProperty* Property, const void* p);
	static void push_totable(lua_State*inL, UMapProperty* Property, const void* p);
	static void push_totable(lua_State*inL, USetProperty* Property, const void* p);

	REFLECTION_FUNC(UBoolProperty)
		REFLECTION_FUNC(UIntProperty)
		REFLECTION_FUNC(UUInt16Property)
		REFLECTION_FUNC(UInt16Property)
		REFLECTION_FUNC(UUInt32Property)
		REFLECTION_FUNC(UInt64Property)
		REFLECTION_FUNC(UUInt64Property)
		REFLECTION_FUNC(UFloatProperty)
		REFLECTION_FUNC(UDoubleProperty)
		REFLECTION_FUNC(UObjectPropertyBase)
		REFLECTION_FUNC(UObjectProperty)
		REFLECTION_FUNC(UClassProperty)
		REFLECTION_FUNC(UStrProperty)
		REFLECTION_FUNC(UNameProperty)
		REFLECTION_FUNC(UTextProperty)
		REFLECTION_FUNC(UByteProperty)
		REFLECTION_FUNC(UEnumProperty)
		REFLECTION_FUNC(UStructProperty)
		REFLECTION_FUNC(UMulticastDelegateProperty)
		REFLECTION_FUNC(UDelegateProperty)
		REFLECTION_FUNC(UWeakObjectProperty)
		REFLECTION_FUNC(UArrayProperty)
		REFLECTION_FUNC(UMapProperty)
		REFLECTION_FUNC(USetProperty)
		REFLECTION_FUNC(UInterfaceProperty)

// knowtype push start
	static int push(lua_State *inL) { return 0; }
	FORCEINLINE static int push(lua_State *inL, int8 value)
	{
		ue_lua_pushinteger(inL, value);
		return 1;
	}
	FORCEINLINE static int push(lua_State *inL, uint8 value)
	{
		ue_lua_pushinteger(inL, value);
		return 1;
	}
	FORCEINLINE static int push(lua_State *inL, int16 value)
	{
		ue_lua_pushinteger(inL, value);
		return 1;
	}
	FORCEINLINE static int push(lua_State *inL, uint16 value)
	{
		ue_lua_pushinteger(inL, value);
		return 1;
	}
	FORCEINLINE static int push(lua_State *inL, int32 value)
	{
		ue_lua_pushinteger(inL, value);
		return 1;
	}
	FORCEINLINE static int push(lua_State *inL, uint32 value)
	{
#ifdef USE_LUA53 
		ue_lua_pushinteger(inL, value);
#else 
		ue_lua_pushnumber(inL, value);
#endif
		return 1;
	}
	FORCEINLINE static int push(lua_State *inL, int64 value)
	{
#ifdef USE_LUA53 
		ue_lua_pushinteger(inL, value);
#else 
		ue_lua_pushnumber(inL, value);
#endif
		return 1;
	}
	FORCEINLINE static int push(lua_State *inL, uint64 value)
	{
#ifdef USE_LUA53 
		ue_lua_pushinteger(inL, value);
#else 
		ue_lua_pushnumber(inL, value);
#endif
		return 1;
	}
	FORCEINLINE static int push(lua_State *inL, float value)
	{
		ue_lua_pushnumber(inL, value);
		return 1;
	}
	FORCEINLINE static int push(lua_State *inL, double value)
	{
		ue_lua_pushnumber(inL, value);
		return 1;
	}
	FORCEINLINE static int push(lua_State *inL, bool value)
	{
		ue_lua_pushboolean(inL, value);
		return 1;
	}
	FORCEINLINE static int push(lua_State *inL, const FString& value)
	{
		ue_lua_pushstring(inL, TCHAR_TO_UTF8(*value));
		return 1;
	}
	FORCEINLINE static int push(lua_State *inL, const std::string& value)
	{
		ue_lua_pushstring(inL, value.c_str());
		return 1;
	}
	FORCEINLINE static int push(lua_State *inL, const LuaStringBuffer& value)
	{
		ue_lua_pushlstring(inL, value.Buffer, value.Len);
		return 1;
	}
	FORCEINLINE static int push(lua_State *inL, lua_CFunction Func)
	{
		ue_lua_pushlightuserdata(inL, (void*)Func);
		return 1;
	}

	FORCEINLINE static int push(lua_State *inL, const FName& value)
	{
		ue_lua_pushstring(inL, TCHAR_TO_UTF8(*value.ToString()));
		return 1;
	}
	FORCEINLINE static int push(lua_State *inL, const char* value)
	{
		ue_lua_pushstring(inL, value);
		return 1;
	}
	FORCEINLINE static int push(lua_State *inL, const TCHAR* value)
	{
		ue_lua_pushstring(inL, TCHAR_TO_UTF8(value));
		return 1;
	}
	FORCEINLINE static int push(lua_State *inL, const UClass* value)
	{
		pushuobject(inL, (void*)value);
		return 1;
	}
	FORCEINLINE static int push(lua_State *inL, const LuaSpace::StackValue& StackIndexValue)
	{
		ue_lua_pushvalue(inL, StackIndexValue.StatckIndex);
		return 1;
	}
	static int push(lua_State *inL, const void* VoidPtr);
	static int push(lua_State *inL, void* VoidPtr) { return push(inL, (const void*)VoidPtr); };
// knowtype push end

// know templateType push end
	template<class T>
	static int push(lua_State*inL, const TSubclassOf<T>& value)
	{
		pushuobject(inL, value.Get());
		return 1;
	}
	template<class T>
	static int push(lua_State*inL, const TScriptInterface<T>& value)
	{
		pushuobject(inL, (void*)value.GetObject());
		return 1;
	}
	template<class T>
	static int push(lua_State *inL, const TWeakObjectPtr<T>& value)
	{
		pushuobject(inL, (void*)value.Get());
		return 1;
	}
// know templateType push end

	template<class T>
	static int push(lua_State *inL, const T& value, typename TEnableIf<!THas_PushValueToLuaStack<T>::Value, int>::Type *p = nullptr)
	{
		return pushimp(inL, value);
	}

	template<class T>
	static int push(lua_State *inL, const T& value, typename TEnableIf<THas_PushValueToLuaStack<T>::Value, int>::Type *p = nullptr)
	{
		return CustomTypeToLua<T>::PushValueToLuaStack(inL, value);
	}

	template<typename T>
	static int pushimp(lua_State *inL, const T& value, typename TEnableIf<TIsStruct<T>::Value, char>::Type* ppp = nullptr)
	{
		pushstruct_nogc(inL, TTraitStructTypeName<T>(), TTraitStructTypeName_Nogc<T>(), (void*)(&value));
		return 1;
	}

	template<typename T>
	static int pushimp(lua_State *inL, const T& value, typename TEnableIf<!TIsStruct<T>::Value, int>::Type* ppp = nullptr)
	{
		push(inL, (int32)value);
		return 1;
	}

	template<typename T>
	static int push(lua_State *inL, T* value, typename TEnableIf<!TIsStruct<T>::Value && !TUnrealLuaIsBasicType<T>::Value, int>::Type* ppp = nullptr)
	{
		pushuobject(inL, (void*)value);
		return 1;
	}

	template<typename T>
	static int push(lua_State *inL, T* value, typename TEnableIf<!TIsStruct<T>::Value && TUnrealLuaIsBasicType<T>::Value, int>::Type* ppp = nullptr)
	{
		push(inL, (const void*)value);
		return 1;
	}

	template<typename T>
	static int push(lua_State *inL, T* value, typename TEnableIf<TIsStruct<T>::Value, int>::Type* ppp = nullptr)
	{
		pushstruct_nogc(inL, TTraitStructTypeName<T>(), TTraitStructTypeName_Nogc<T>(),(void*)value);
		return 1;
	}

	static void pushproperty(lua_State*inL, UProperty* property, const void* ptr);
	static void push_ret_property(lua_State*inL, UProperty* property, const void* ptr);
	static void pushback_ref_property(lua_State*inL, int32 LuaStackIndex, UProperty* property, const void* ptr);

	static void pushproperty_valueptr(lua_State*inL, UProperty* property, const void* ptr);
	static void popproperty(lua_State* inL, int index, UProperty* property, void* ptr);


	FORCEINLINE static void pushall(lua_State *inL)
	{}

	template<class T1, class... T2>
	FORCEINLINE static void pushall(lua_State *inL, const T1& value, const T2&... args)
	{
		push(inL, value);
		pushall(inL, args...);
	}

	template<class T>
	static void pushback(lua_State*inL, int index, T* Value, typename TEnableIf<!THas_PushBackPtrToLuaStack<typename TDecay<T>::Type>::Value, int>::Type *p = nullptr)
	{
		push(inL, Value);
	}

	template<class T>
	static void pushback(lua_State*inL, int index, T* Value, typename TEnableIf<THas_PushBackPtrToLuaStack<typename TDecay<T>::Type>::Value, int>::Type *p = nullptr)
	{
		CustomTypeToLua<typename TDecay<T>::Type>::PushBackPtrToLuaStack(inL, Value, index);
	}

	template<class T>
	static void pushback(lua_State*inL, int index, T& Value, typename TEnableIf<!THas_PushBackValueToLuaStack<typename TDecay<T>::Type>::Value, int>::Type *p = nullptr)
	{
		push(inL, Value);
	}

	template<class T>
	static void pushback(lua_State*inL, int index, T& Value, typename TEnableIf<THas_PushBackValueToLuaStack<typename TDecay<T>::Type>::Value, int>::Type *p = nullptr)
	{
		CustomTypeToLua<typename TDecay<T>::Type>::PushBackValueToLuaStack(inL, Value, index);
	}

	template<class returnType, class... T>
	static returnType callr_with_state(lua_State* TheState, const char* funcname, const T&... args)
	{
		ue_lua_pushcfunction(TheState, ErrHandleFunc);
		ue_lua_getglobal(TheState, funcname);
		pushall(TheState, args...);
		const int32 ParamCount = sizeof...(args);
		if (ue_lua_pcall(TheState, ParamCount, UnrealLua::PopFromStack<returnType>::Value, -(ParamCount + 2)))
		{
			log(ue_lua_tostring(TheState, -1));
		}
		ue_lua_remove(TheState, -1 - UnrealLua::PopFromStack<returnType>::Value);
		return UnrealLua::PopFromStack<returnType>::pop(TheState, -1);
	}

	template<class... T>
	static void call_with_state(lua_State* TheState, const char* funcname, const T&... args)
	{
		callr_with_state<void>(TheState, funcname, args...);
	}

	static void call(lua_State* inL, int funcid, UFunction* funcsig, void* ptr);
	static void call(lua_State*inL, FFrame& Stack, RESULT_DECL);

	template<class T>
	static void read(T& value, lua_State*inL, int Index)
	{
		UnrealLua::read(value, inL, Index);
	}

	template<class RET, class... T>
	static RET call_ref_func_ret(lua_State* inL, int func_ref, const T&... args)
	{
		ue_lua_pushcfunction(inL, ErrHandleFunc);
		ue_lua_rawgeti(inL, LUA_REGISTRYINDEX, func_ref);
		pushall(inL, args...);
		const int32 ParamCount = sizeof...(args);
		if (ue_lua_pcall(inL, ParamCount, UnrealLua::PopFromStack<RET>::Value, -(ParamCount + 2)))
		{
			log(ue_lua_tostring(inL, -1));
		}
		ue_lua_remove(inL, -1 - UnrealLua::PopFromStack<RET>::Value);
		return UnrealLua::PopFromStack<RET>::pop(inL, -1);
	}

	template<class T>
	static int push_ret(lua_State*inL, const T& Value, 
		typename TEnableIf<!THas_PushRetValueToLuaStack<typename TDecay<T>::Type>::Value, int>::Type *p = nullptr, 
		typename TEnableIf<!(TIsStruct<T>::Value &&UnrealLua::CheckNeedTempIns<T>::Value), int>::Type *p1 = nullptr)
	{
		const T& Temp = UnrealLua::MayNeedTempIns<T>(Value);
		return push(inL, Temp);
	}

	template<class T>
	static int push_ret(lua_State*inL, const T& Value, 
		typename TEnableIf<!THas_PushRetValueToLuaStack<typename TDecay<T>::Type>::Value, int>::Type *p = nullptr,
		typename TEnableIf<TIsStruct<T>::Value && UnrealLua::CheckNeedTempIns<T>::Value, int>::Type *p1 = nullptr)
	{
		const T& Temp = UnrealLua::MayNeedTempIns<T>(Value);
		pushstruct_temp(inL, TTraitStructTypeName<T>(), TTraitStructTypeName_Nogc<T>(), (void*)&Temp);
		return 1;
	}

	template<class T>
	static int push_ret(lua_State*inL, const T& Value, typename TEnableIf<THas_PushRetValueToLuaStack<typename TDecay<T>::Type>::Value, int>::Type *p = nullptr)
	{
		return CustomTypeToLua<typename TDecay<T>::Type>::PushRetValueToLuaStack(inL, Value);
	}

	template<class T>
	static int push_ret(lua_State*inL, const T* Value, typename TEnableIf<!THas_PushRetPtrToLuaStack<typename TDecay<T>::Type>::Value, int>::Type *p = nullptr)
	{
		return push(inL, Value);
	}

	template<class T>
	static int push_ret(lua_State*inL, const T* Value, typename TEnableIf<THas_PushRetPtrToLuaStack<typename TDecay<T>::Type>::Value, int>::Type *p = nullptr)
	{
		return CustomTypeToLua<typename TDecay<T>::Type>::PushRetPtrToLuaStack(inL, Value);
	}

	template<class T>
	static int push_new(lua_State*inL, T* Value, typename TEnableIf<!THas_PushNewPtrToLuaStack<typename TDecay<T>::Type>::Value, int>::Type *p = nullptr)
	{
		pushstruct_gc(inL, TTraitStructTypeName<T>(), Value);
		return 1;
	}

	template<class T>
	static int push_new(lua_State*inL, T* Value, typename TEnableIf<THas_PushNewPtrToLuaStack<typename TDecay<T>::Type>::Value, int>::Type *p = nullptr)
	{
		return CustomTypeToLua<typename TDecay<T>::Type>::PushNewPtrToLuaStack(inL, Value);
	}

	template<class T>
	static int push_new(lua_State*inL, const T& Value, typename TEnableIf<!THas_PushNewValueToLuaStack<typename TDecay<T>::Type>::Value, int>::Type *p = nullptr, typename TEnableIf<TIsStruct<T>::Value, int>::Type *p1 = nullptr)
	{
		pushstruct_gc(inL, TTraitStructTypeName<T>(), new T(Value));
		return 1;
	}

	template<class T>
	static int push_new(lua_State*inL, const T& Value, typename TEnableIf<!THas_PushNewValueToLuaStack<typename TDecay<T>::Type>::Value, int>::Type *p = nullptr, typename TEnableIf<!TIsStruct<T>::Value, int>::Type *p1 = nullptr)
	{
		push(inL, Value);
		return 1;
	}

	template<class T>
	static int push_new(lua_State*inL, const T& Value, typename TEnableIf<THas_PushNewValueToLuaStack<typename TDecay<T>::Type>::Value, int>::Type *p = nullptr)
	{
		return CustomTypeToLua<typename TDecay<T>::Type>::PushNewValueToLuaStack(inL, Value);
	}

	template<class T>
	static T pop(lua_State*inL, int Index, typename TEnableIf<!TIsLValueReferenceType<T>::Value, int>::Type*p = nullptr)
	{
		T Data;
		read(Data, inL, Index);
		return Data;
	}

	template<class T>
	static typename TDecay<T>::Type pop(lua_State*inL, int Index,
		typename TEnableIf<TIsLValueReferenceType<T>::Value, int>::Type*p = nullptr,
		typename TEnableIf<!TIsPopByPointer<T>::Value, int>::Type*p1 = nullptr)
	{
		typename TDecay<T>::Type Data;
		read(Data, inL, Index);
		return Data;
	}

	template<class T>
	static T pop(lua_State*inL, int Index, 
		typename TEnableIf<TIsLValueReferenceType<T>::Value, int>::Type*p = nullptr, 
		typename TEnableIf<TIsPopByPointer<T>::Value, int>::Type*p1 = nullptr)
	{
		typename TDecay<T>::Type* Ptr;
		read(Ptr, inL, Index);
		return *Ptr;
	}

	static UnrealLua::ArgType GetTypeOfProperty(UProperty* Property);
};

class FLuaGcObj : public FGCObject
{
public:
	TSet<UObject*> objs;
	static FLuaGcObj* Get()
	{
		static FLuaGcObj* Singleton = nullptr;
		if (Singleton == nullptr)
			Singleton = new FLuaGcObj;
		return Singleton;
	}

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
};

template<class... T>
void AddFuncToTable(lua_State* inL, int TableIndex, const char* Key, luafunc Func, const T&... Upvalues)
{
	const int UpvalueCount = sizeof...(Upvalues);
	UTableUtil::push(inL, Key);
	UTableUtil::pushall(inL, Upvalues...);
	ue_lua_pushcclosure(inL, Func, UpvalueCount);
	ue_lua_rawset(inL, TableIndex - 2);
}

template<class T, bool IsStruct>
struct GetStructNewType_impForStruct;

template<class T>
struct GetStructNewType_impForStruct<T, false>
{
	static UnrealLua::ArgType GetType()
	{
		return -1;
	}
};

template<class T>
struct GetStructNewType_impForStruct<T, true>
{
	static UnrealLua::ArgType GetType()
	{
		static int TheType = UTableUtil::GetNewType(TTraitStructTypeName<T>());
		return TheType;
	}
};

template<class T>
struct GetStructNewType_imp<T, false>:public GetStructNewType_impForStruct<T, TIsStruct<T>::Value>{};

template<class T>
struct TCheckArgIsFit_Imp<T, true, false, false>
{
	static UnrealLua::CheckResult ToCheck(UnrealLua::ArgType _Type, lua_State*inL = nullptr, int32 Index = 0)
	{
		int32 _T = GetStructNewType<T>::GetType();
		if (_Type == _T)
			return UnrealLua::CheckResult::FitType;
		else if (_Type.IsStructType() && UTableUtil::CheckIsChildClass(_T, _Type))
			return UnrealLua::CheckResult::ChildType;
		else
			return UnrealLua::CheckResult::NotFitType;
	}
};
