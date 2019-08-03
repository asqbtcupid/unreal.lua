#pragma once
#include "CoreMinimal.h"
#include "UnrealLua.h"
#include "BaseEngineLuaGlue.h"
#include "UserDefineClassMacro.h"
#include "IToolTip.h"

LUA_GLUE_INTERFACE(ISlateStyle, ISlateStyle)
LUA_GLUE_END()
LUA_GLUE_SHARED(ISlateStyle, ISlateStyle)
LUA_GLUE_SHARED(const_ISlateStyle, const ISlateStyle)

static int32 SetStyleForFArguments(lua_State*inL, const FString& StyleName)
{
	int32 TopCount = ue_lua_gettop(inL);
	UTableUtil::push(inL, "classname");
	ue_lua_gettable(inL, 2);
	FString Param1ClassName = UTableUtil::pop<FString>(inL, -1);
	if (Param1ClassName == "USlateWidgetStyleAsset")
	{
		UTableUtil::push(inL, StyleName+"1");
	}
	else if (Param1ClassName == "ISlateStyle")
	{
		UTableUtil::push(inL, StyleName + "2");
	}
	else
	{
		UTableUtil::push(inL, "_IsTSharedPtr");
		ue_lua_gettable(inL, 2);
		if(!ue_lua_isnil(inL, -1))
			UTableUtil::push(inL, StyleName + "3");
		else
			UTableUtil::push(inL, StyleName + "0");
	}
	ue_lua_gettable(inL, 1);
	for (int i = 1; i <= TopCount; i++)
	{
		ue_lua_pushvalue(inL, i);
	}
	ue_lua_call(inL, TopCount, 1);
	return 1;
}

LUA_GLUE_INTERFACE(SWidget, SWidget)
LUA_GLUE_END()

LUA_GLUE_STRUCT_TRAIT_NAME(TSharedPtr_SWidget, TSharedPtr<SWidget>)
LUA_GLUE_STRUCT_TRAIT_NAME(TSharedRef_SWidget, TSharedRef<SWidget>)

#define LUA_GLUE_SLATE_ARGS(SLATE_TYPE)

#define LUA_SLATE_ATTRIBUTE(TYPE,NAME) LUA_GLUE_FUNCTION_OVERLOAD(NAME, NAME, TheClassType& (TheClassType::*)(const TAttribute<TYPE>&)) 
#define LUA_SLATE_ARGUMENT(TYPE,NAME) LUA_GLUE_FUNCTION(NAME)
#define LUA_SLATE_DEFAULT_SLOT(TYPE,NAME) LUA_GLUE_FUNCTION_NAME(__call, operator[])

#define LUA_SLATE_STYLE_ARGUMENT(ArgType, ArgName)LUA_GLUE_OVERLOAD(ArgName, TheClassType&(TheClassType::*)(const ArgType*))\
												LUA_GLUE_OVERLOAD(ArgName, TheClassType&(TheClassType::*)(const class USlateWidgetStyleAsset* const))\
												LUA_GLUE_OVERLOAD(ArgName, TheClassType&(TheClassType::*)(const class ISlateStyle* InSlateStyle, const FName& , const ANSICHAR* ), nullptr)\
												LUA_GLUE_OVERLOAD(ArgName, TheClassType&(TheClassType::*)(const TSharedPtr< const class ISlateStyle >&, const FName& , const ANSICHAR* ), nullptr)

#define LUA_SLATE_NAMED_SLOT(...) 
#define LUA_SLATE_SUPPORTS_SLOT(...) LUA_GLUE_FUNCTION_NAME(__addslot, operator+)

#define LUA_SLATE_EVENT(TYPE, NAME) LUA_GLUE_FUNCTION_OVERLOAD(NAME, NAME, TheClassType& (TheClassType::*)(const TheSlateType::TYPE&))
#define LUA_SLATE_EVENT_RAW(TYPE, NAME) LUA_GLUE_FUNCTION_OVERLOAD(NAME, NAME, TheClassType& (TheClassType::*)(const TYPE&))

template<class T>
struct MakeFromArgs;

template<class T, class... Args>
struct MakeFromArgs<void(T::*)(const typename T::FArguments&, Args...)>
{
	using Ret = TSharedRef<T>;
	static Ret Call(const typename T::FArguments& FArgs, Args... args)
	{
		return MakeTDecl<T>(TTraitStructTypeName<T>(), __FILE__, __LINE__, RequiredArgs::MakeRequiredArgs(args...)) <<= FArgs;
	}
};

LUA_GLUE_INTERFACE(IToolTip, IToolTip)
LUA_GLUE_END()
LUA_GLUE_SHARED(IToolTip, IToolTip)

LUA_GLUE_BEGIN(EVisibility)
LUA_GLUE_PROPERTY_GET(Visible)
LUA_GLUE_PROPERTY_GET(Collapsed)
LUA_GLUE_PROPERTY_GET(Hidden)
LUA_GLUE_PROPERTY_GET(HitTestInvisible)
LUA_GLUE_PROPERTY_GET(SelfHitTestInvisible)
LUA_GLUE_PROPERTY_GET(All)
LUA_GLUE_END()

#define LUA_SLATE_ARGS_BASE(TYPE)\
	LUA_GLUE_BEGIN_NOTRAIT_NAME(TYPE##_FArguments, TYPE::FArguments)\
	LUA_GLUE_FUNCTION_OUT(MakeFromArgs, MakeFromArgs<decltype(&TYPE::Construct)>::Call)\
	LUA_SLATE_ATTRIBUTE( FText, ToolTipText )\
	LUA_SLATE_ARGUMENT(TSharedPtr<IToolTip>, ToolTip)\
	LUA_SLATE_ATTRIBUTE(TOptional<EMouseCursor::Type>, Cursor)\
	LUA_SLATE_ATTRIBUTE(bool, IsEnabled)\
	LUA_SLATE_ATTRIBUTE(EVisibility, Visibility)\
	LUA_SLATE_ARGUMENT(float, RenderOpacity)\
	LUA_SLATE_ATTRIBUTE(TOptional<FSlateRenderTransform>, RenderTransform)\
	LUA_SLATE_ATTRIBUTE(FVector2D, RenderTransformPivot)\
	LUA_SLATE_ARGUMENT(FName, Tag)\
	LUA_SLATE_ARGUMENT(bool, ForceVolatile)\
	LUA_SLATE_ARGUMENT(EWidgetClipping, Clipping)

#define LUA_SLATE_BEGIN(TYPE)\
LUA_GLUE_STRUCT_TRAIT_NAME(TYPE##_FArguments, TYPE::FArguments)\
LUA_GLUE_STRUCT_TRAIT_NAME(TYPE, TYPE)\
LUA_GLUE_SHARED(TYPE, TYPE)\
namespace LUA_SLATE_##TYPE{\
	using TheSlateType = TYPE;\
	LUA_SLATE_ARGS_BASE(TYPE)


#define LUA_SLATE_BODY(TYPE, ...)\
LUA_GLUE_END(__VA_ARGS__)\
LUA_GLUE_INTERFACE_BODY(TYPE, TYPE)\

#define LUA_SLATE_END(...)\
LUA_GLUE_END(__VA_ARGS__)}\

#define LUA_SLATE_SLOT_BEGIN(TYPE, SLOT_TYPE)\
LUA_GLUE_STRUCT_TRAIT_NAME(TYPE##_FArguments, TYPE::FArguments)\
LUA_GLUE_STRUCT_TRAIT_NAME(TYPE, TYPE)\
LUA_GLUE_SHARED(TYPE, TYPE)\
LUA_GLUE_STRUCT_TRAIT_NAME(TYPE##_FSlot, TYPE::SLOT_TYPE)\
namespace LUA_SLATE_##TYPE{\
	using TheSlateType = TYPE;\
	LUA_GLUE_BEGIN_NOTRAIT_NAME(TYPE##_FSlot, TYPE::SLOT_TYPE)\
	LUA_GLUE_FUNCTION_NAME(__call, operator[])

#define LUA_SLATE_ARGS_BEGIN(TYPE, ...)\
	LUA_GLUE_END(__VA_ARGS__)\
	LUA_SLATE_ARGS_BASE(TYPE)

#define LUA_SLATE_ARGS_END(TYPE, ...) LUA_SLATE_BODY(TYPE, ##__VA_ARGS__)

#define LUA_SLATE_SLOT_END(...)\
LUA_GLUE_FUNCTION_OUT(__Slot, TheSlateType::Slot)\
LUA_GLUE_END(__VA_ARGS__)}


template<class T>
struct CustomTypeToLua<TAttribute<T>> :UnrealLua::DefaultCustomEnum
{
	enum {
		 	Value = true,
		};
	static void ReadPtrFromLuaStack(TAttribute<T>*& Value, lua_State*inL, int32 Index) 
	{
		static TAttribute<T> Temp;
		if(ue_lua_isfunction(inL, Index))\
		{
			TSharedPtr<UnrealLuaFunctor> LuaCallBack = MakeShared<UnrealLuaFunctor>(inL, Index); 
			auto Lambd = [LuaCallBack]() { return LuaCallBack->Callr<T>(); };
			Temp = TAttribute<T>::Create(Lambd);
		}
		else
		{
			Temp.Set(UTableUtil::pop<T&>(inL, Index));
		}
		Value = &Temp;
	}
	static void ReadValueFromLuaStack(TAttribute<T>& Value, lua_State*inL, int32 Index) 
	{
		TAttribute<T>* Temp;
		ReadPtrFromLuaStack(Temp, inL, Index);
		Value = *Temp;
	}

	static int PushValueToLuaStack(lua_State*inL, const TAttribute<T>& Value)
	{
		UTableUtil::push(inL, Value.Get());
		return 1;
	}
};


template<class T>
struct CustomTypeToLua<TOptional<T>> :UnrealLua::DefaultCustomEnum
{
	enum {
		Value = true,
	};
	
	static void ReadValueFromLuaStack(TOptional<T>& Value, lua_State*inL, int32 Index)
	{
		static TOptional<T> Temp;
		Temp = TOptional<T>(UTableUtil::pop<T&>(inL, Index));
		Value = Temp;
	}
};

template<>
struct CustomTypeToLua<FOptionalSize> :UnrealLua::DefaultCustomEnum
{
	enum {
		Value = true,
	};

	static void ReadValueFromLuaStack(FOptionalSize& Value, lua_State*inL, int32 Index)
	{
		static FOptionalSize Temp;
		Temp = FOptionalSize(UTableUtil::pop<float>(inL, Index));
		Value = Temp;
	}
};

template<class T>
struct TCheckArgIsFit<TAttribute<T>> {
	static UnrealLua::CheckResult ToCheck(UnrealLua::ArgType _Type, lua_State*inL = nullptr, int32 Index = 0)
	{
		return TCheckArgIsFit<T>::ToCheck(_Type, inL, Index);
	}
};

template<class T>
struct TCheckArgIsFit<TOptional<T>> {
	static UnrealLua::CheckResult ToCheck(UnrealLua::ArgType _Type, lua_State*inL = nullptr, int32 Index = 0)
	{
		return TCheckArgIsFit<T>::ToCheck(_Type, inL, Index);
	}
};