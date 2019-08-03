#pragma once
#include "unreallua_template.h"
#include "LuaApi.h"
#include <string>

namespace UnrealLua
{
	enum class Type
	{
		TYPE_TNIL = 0,
		TYPE_TBOOLEAN = 1,
		TYPE_TLIGHTUSERDATA = 2,
		TYPE_TNUMBER = 3,
		TYPE_TSTRING = 4,
		TYPE_TTABLE = 5,
		TYPE_TFUNCTION = 6,
		TYPE_TUSERDATA = 7,
		TYPE_TTHREAD = 8,
		TYPE_UOBJECT = 9,

		TYPE_INTERGER = 10,
		TYPE_INTERGER8 = 11,
		TYPE_INTERGERu8 = 12,
		TYPE_INTERGER16 = 13,
		TYPE_INTERGERu16 = 14,
		TYPE_INTERGER32 = 15,
		TYPE_INTERGERu32 = 16,
		TYPE_INTERGER64 = 17,
		TYPE_INTERGERu64 = 18,
		TYPE_TNUMBERfloat = 19,
		TYPE_TNUMBERdouble = 20,
		TYPE_Name = 21,
		TYPE_String = 22,
		TYPE_Text = 23,
		TYPE_Byte = 24,

		TYPE_MAX = 100,
	};
	const int MaxTypeCount = 100000;
	enum class TemplateFactor
	{
		TSharedPtr = 1,
		TSharedRef = 2,
	};
	struct ArgType
	{
		ArgType():TypeInner(-1){}
		ArgType(int32 _Type) :TypeInner(_Type) {}
		ArgType(Type _Type) :TypeInner((int32)_Type) {}
		ArgType(TemplateFactor Factor, int32 _Type){
			TypeInner = (int32)Factor*MaxTypeCount + _Type;
		}
		bool IsTemplate(TemplateFactor Factor)
		{
			return (TypeInner / MaxTypeCount) == int32(Factor);
		}
		int32 InnerStructType(TemplateFactor Factor)
		{
			int32 TrueFactor = TypeInner / MaxTypeCount;
			if (TrueFactor == (int32)Factor)
				return TypeInner % ((int32)Factor*MaxTypeCount);
			else
				return -1;
		}
		operator int32() { return TypeInner; }
		bool operator==(const Type& Right)
		{
			return TypeInner == (int32)Right;
		}
		bool operator==(const ArgType& Right)
		{
			return TypeInner == Right.TypeInner;
		}
		bool operator==(const int32& Right)
		{
			return TypeInner == Right;
		}
		bool IsStructType() const
		{
			return TypeInner > (int32)Type::TYPE_MAX && TypeInner < 1 << 15;
		}
		int32 TypeInner;
	};
	static ArgType GetArgType(lua_State*inL, int32 Index)
	{
		int LuaType = ue_lua_type(inL, Index);
		int32 TypeResult = -1;
		switch (LuaType)
		{
			case LUA_TNIL:
				return (int32)UnrealLua::Type::TYPE_TNIL;
			case LUA_TBOOLEAN:
				return (int32)UnrealLua::Type::TYPE_TBOOLEAN;
			case LUA_TLIGHTUSERDATA:
				return (int32)UnrealLua::Type::TYPE_TLIGHTUSERDATA;
			case LUA_TNUMBER:
				if (ue_lua_isinteger(inL, Index))
					return (int32)UnrealLua::Type::TYPE_INTERGER;
				else
					return (int32)UnrealLua::Type::TYPE_TNUMBER;
			case LUA_TSTRING:
				return (int32)UnrealLua::Type::TYPE_TSTRING;
			case LUA_TFUNCTION:
				return (int32)UnrealLua::Type::TYPE_TFUNCTION;
			case LUA_TTHREAD:
				return (int32)UnrealLua::Type::TYPE_TTHREAD;
			case LUA_TTABLE:
				ue_lua_pushstring(inL, "_cppinstance_");
				ue_lua_rawget(inL, Index);
				if (ue_lua_isnil(inL, -1))
					TypeResult = (int32)UnrealLua::Type::TYPE_TTABLE;
				else
					TypeResult = (int32)UnrealLua::Type::TYPE_UOBJECT;
				ue_lua_pop(inL, 1);
				return TypeResult;
			case LUA_TUSERDATA:
				ue_lua_getmetatable(inL, Index);
				if (ue_lua_isnil(inL, -1))
				{
					ue_lua_pop(inL, 1);
					TypeResult = (int32)UnrealLua::Type::TYPE_TUSERDATA;
				}
				else
				{
					ue_lua_getfield(inL, -1, "_type_");
					if (ue_lua_isnil(inL, -1))
						TypeResult = (int32)UnrealLua::Type::TYPE_TUSERDATA;
					else
						TypeResult = ue_lua_tointeger(inL, -1);

					ue_lua_pop(inL, 2);
				}
				return TypeResult;
			default:
				ensureAlwaysMsgf(0, TEXT("BUG"));
				return -1;
		}
	}

	struct LUAPLUGINRUNTIME_API CheckResult
	{
		static const CheckResult FitType;
		static const CheckResult ChildType;
		static const CheckResult NotFitType;
		CheckResult(bool IsTrue)
		{
			Weight = IsTrue ? 1 : 0;
		}
		CheckResult(int _Weight) :Weight(_Weight) {}
		
		operator bool() { return Weight > 0; }
		operator int32() { return Weight; }

		int32 Weight;
	};
}

template<class T, bool IsUObject>
struct GetStructNewType_imp;

template<class T>
struct GetStructNewType_imp<T, true>
{
	static UnrealLua::ArgType GetType()
	{
		return UnrealLua::Type::TYPE_UOBJECT;
	}
};

template<class T>
struct GetStructNewType:public GetStructNewType_imp<T, std::is_convertible<T*, UObject*>::value>
{};


template<class T, bool IsStruct, bool IsObject, bool IsCustom>
struct TCheckArgIsFit_Imp;

template<class T>
struct TCheckArgIsFit_Imp<T, false,false,false>
{
	static UnrealLua::CheckResult ToCheck(UnrealLua::ArgType _Type, lua_State*inL = nullptr, int32 Index = 0)
	{
		return _Type == UnrealLua::Type::TYPE_INTERGER;
	}
};

template<class T>
struct TCheckArgIsFit_Imp<T, false, true, false>
{
	static UnrealLua::CheckResult ToCheck(UnrealLua::ArgType _Type, lua_State*inL = nullptr, int32 Index = 0)
	{
		return _Type == UnrealLua::Type::TYPE_UOBJECT || _Type == UnrealLua::Type::TYPE_TNIL;
	}
};

TEST_HAS_FUNC(ToCheckType, UnrealLua::CheckResult(*)(UnrealLua::ArgType, lua_State*, int32))

template<class T>
struct TCheckArgIsFit_Imp<T, false, false, true>
{
	static UnrealLua::CheckResult ToCheck(UnrealLua::ArgType _Type, lua_State*inL = nullptr, int32 Index = 0)
	{
		return CustomTypeToLua<T>::ToCheckType(_Type, inL, Index);
	}
};

template<class T>
struct TCheckArgIsFit_Imp<T, true, false, true>:public TCheckArgIsFit_Imp<T, false, false, true>
{};

template<class T, class T1 = typename TRemovePointer<typename TDecay<T>::Type>::Type>
struct TCheckArgIsFit :public TCheckArgIsFit_Imp<T1, 
	TIsStruct<T1>::Value,
	std::is_convertible<T1*, UObject*>::value,
	THas_ToCheckType<T1>::Value
>{};

template<>
struct TCheckArgIsFit<float> {
	static UnrealLua::CheckResult ToCheck(UnrealLua::ArgType _Type, lua_State*inL = nullptr, int32 Index = 0)
	{return _Type == UnrealLua::Type::TYPE_TNUMBER;}
};

template<>
struct TCheckArgIsFit<double> {
	static UnrealLua::CheckResult ToCheck(UnrealLua::ArgType _Type, lua_State*inL = nullptr, int32 Index = 0)
	{return _Type == UnrealLua::Type::TYPE_TNUMBER;}
};

template<>
struct TCheckArgIsFit<FString> {
	static UnrealLua::CheckResult ToCheck(UnrealLua::ArgType _Type, lua_State*inL = nullptr, int32 Index = 0)
	{return _Type == UnrealLua::Type::TYPE_TSTRING || _Type == UnrealLua::Type::TYPE_String;}
};

template<>
struct TCheckArgIsFit<FName> {
	static UnrealLua::CheckResult ToCheck(UnrealLua::ArgType _Type, lua_State*inL = nullptr, int32 Index = 0)
	{
		return _Type == UnrealLua::Type::TYPE_TSTRING || _Type == UnrealLua::Type::TYPE_Name;
	}
};

template<>
struct TCheckArgIsFit<const TCHAR*> {
	static UnrealLua::CheckResult ToCheck(UnrealLua::ArgType _Type, lua_State*inL = nullptr, int32 Index = 0)
	{
		return _Type == UnrealLua::Type::TYPE_TSTRING;
	}
};

template<>
struct TCheckArgIsFit<const ANSICHAR*> {
	static UnrealLua::CheckResult ToCheck(UnrealLua::ArgType _Type, lua_State*inL = nullptr, int32 Index = 0)
	{
		return _Type == UnrealLua::Type::TYPE_TSTRING;
	}
};

template<>
struct TCheckArgIsFit<std::string> {
	static UnrealLua::CheckResult ToCheck(UnrealLua::ArgType _Type, lua_State*inL = nullptr, int32 Index = 0)
	{
		return _Type == UnrealLua::Type::TYPE_TSTRING;
	}
};

template<>
struct TCheckArgIsFit<bool> {
	static UnrealLua::CheckResult ToCheck(UnrealLua::ArgType _Type, lua_State*inL = nullptr, int32 Index = 0)
	{
		return _Type == UnrealLua::Type::TYPE_TBOOLEAN;
	}
};
