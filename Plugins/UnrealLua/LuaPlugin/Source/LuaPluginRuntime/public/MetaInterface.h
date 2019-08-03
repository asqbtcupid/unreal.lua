#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "UObject/UnrealType.h"
#include "UObject/ScriptMacros.h"
#include "LuaApi.h"
#include "TableUtil.h"
#include "LuaDelegateSingle.h"
#include "LuaArrayHelper.h"
struct LuaBaseBpInterface
{
	virtual ~LuaBaseBpInterface() {};
};

struct LuaBasePropertyInterface:public LuaBaseBpInterface
{
	virtual UProperty* GetProperty() = 0;

	virtual void push(lua_State* inL, const void* ValuePtr) = 0;
	virtual void push_ret(lua_State* inL, const void* ValuePtr) = 0;
	virtual void push_ref(lua_State* inL, int32 LuaStackIndex, const void* ValuePtr) = 0;
	virtual void pop(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) = 0;
	
	virtual void push_container(lua_State* inL, const void* ContainerPtr) = 0;
	virtual void push_ret_container(lua_State* inL, const void* ContainerPtr) = 0;
	virtual void push_ref_container(lua_State* inL, int32 LuaStackIndex, const void* ContainerPtr) = 0;
	virtual void pop_container(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr) = 0;

};

static TSharedPtr<LuaBasePropertyInterface> CreatePropertyInterface(lua_State*inL, UProperty* Property);

struct LuaUBoolProperty :public LuaBasePropertyInterface
{
	UBoolProperty* Property;
	//Some hook
	virtual UProperty* GetProperty(){return Property;}
	virtual ~LuaUBoolProperty(){}
	LuaUBoolProperty(lua_State*inL, UBoolProperty* InProperty):Property(InProperty)
	{
		
	}
	virtual void push(lua_State* inL, const void* ValuePtr) override
	{
		push_novirtual(inL, ValuePtr);
	}
	void push_novirtual(lua_State* inL, const void* ValuePtr) 
	{
		ue_lua_pushboolean(inL, (bool)Property->GetPropertyValue(ValuePtr));
	}
	virtual void push_ret(lua_State* inL, const void* ValuePtr) override
	{
		ue_lua_pushboolean(inL, (bool)Property->GetPropertyValue(ValuePtr));
	}
	virtual void push_ref(lua_State* inL, int32 LuaStackIndex, const void* ValuePtr) override
	{
		ue_lua_pushboolean(inL, (bool)Property->GetPropertyValue(ValuePtr));
	}
	virtual void pop(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) override
	{
		pop_novirtual(inL, LuaStackIndex, ValuePtr);
	}
	void pop_novirtual(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) 
	{
		bool value = popiml<bool>::pop(inL, LuaStackIndex);
		Property->SetPropertyValue(ValuePtr, value);
	}
	void push_container_novirtual(lua_State* inL, const void* ContainerPtr) 
	{
		push_novirtual(inL, (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr));
	}
	virtual void push_container(lua_State* inL, const void* ContainerPtr) override
	{
		ue_lua_pushboolean(inL, (bool)Property->GetPropertyValue_InContainer(ContainerPtr));
	}
	virtual void push_ret_container(lua_State* inL, const void* ContainerPtr) override
	{
		ue_lua_pushboolean(inL, (bool)Property->GetPropertyValue_InContainer(ContainerPtr));
	}
	virtual void push_ref_container(lua_State* inL, int32 LuaStackIndex, const void* ContainerPtr) override
	{
		ue_lua_pushboolean(inL, (bool)Property->GetPropertyValue_InContainer(ContainerPtr));
	}
	virtual void pop_container(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr) override
	{
		pop_container_novirtual(inL, LuaStackIndex, ContainerPtr);
	}
	void pop_container_novirtual(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr)
	{
		bool value = popiml<bool>::pop(inL, LuaStackIndex);
		Property->SetPropertyValue_InContainer((void*)ContainerPtr, value);
	}
};
struct LuaUIntProperty :public LuaBasePropertyInterface
{
	UIntProperty* Property;
	//Some hook
	virtual UProperty* GetProperty(){return Property;}
	virtual ~LuaUIntProperty(){}
	LuaUIntProperty(lua_State*inL, UIntProperty* InProperty):Property(InProperty)
	{
		
	}
	virtual void push(lua_State* inL, const void* ValuePtr) override
	{
		push_novirtual(inL, ValuePtr);
	}
	void push_novirtual(lua_State* inL, const void* ValuePtr) 
	{
		ue_lua_pushinteger(inL, (int32)Property->GetPropertyValue(ValuePtr));
	}
	virtual void push_ret(lua_State* inL, const void* ValuePtr) override
	{
		ue_lua_pushinteger(inL, (int32)Property->GetPropertyValue(ValuePtr));
	}
	virtual void push_ref(lua_State* inL, int32 LuaStackIndex, const void* ValuePtr) override
	{
		ue_lua_pushinteger(inL, (int32)Property->GetPropertyValue(ValuePtr));
	}
	virtual void pop(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) override
	{
		pop_novirtual(inL, LuaStackIndex, ValuePtr);
	}
	void pop_novirtual(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) 
	{
		int32 value = popiml<int>::pop(inL, LuaStackIndex);
		Property->SetPropertyValue(ValuePtr, value);
	}
	void push_container_novirtual(lua_State* inL, const void* ContainerPtr) 
	{
		push_novirtual(inL, (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr));
	}
	virtual void push_container(lua_State* inL, const void* ContainerPtr) override
	{
		ue_lua_pushinteger(inL, (int32)Property->GetPropertyValue_InContainer(ContainerPtr));
	}
	virtual void push_ret_container(lua_State* inL, const void* ContainerPtr) override
	{
		ue_lua_pushinteger(inL, (int32)Property->GetPropertyValue_InContainer(ContainerPtr));
	}
	virtual void push_ref_container(lua_State* inL, int32 LuaStackIndex, const void* ContainerPtr) override
	{
		ue_lua_pushinteger(inL, (int32)Property->GetPropertyValue_InContainer(ContainerPtr));
	}
	virtual void pop_container(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr) override
	{
		pop_container_novirtual(inL, LuaStackIndex, ContainerPtr);
	}
	void pop_container_novirtual(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr)
	{
		int32 value = popiml<int>::pop(inL, LuaStackIndex);
		Property->SetPropertyValue_InContainer((void*)ContainerPtr, value);
	}
};
struct LuaUUInt16Property :public LuaBasePropertyInterface
{
	UUInt16Property* Property;
	//Some hook
	virtual UProperty* GetProperty(){return Property;}
	virtual ~LuaUUInt16Property(){}
	LuaUUInt16Property(lua_State*inL, UUInt16Property* InProperty):Property(InProperty)
	{
		
	}
	virtual void push(lua_State* inL, const void* ValuePtr) override
	{
		push_novirtual(inL, ValuePtr);
	}
	void push_novirtual(lua_State* inL, const void* ValuePtr) 
	{
		ue_lua_pushinteger(inL, (uint16)Property->GetPropertyValue(ValuePtr));
	}
	virtual void push_ret(lua_State* inL, const void* ValuePtr) override
	{
		ue_lua_pushinteger(inL, (uint16)Property->GetPropertyValue(ValuePtr));
	}
	virtual void push_ref(lua_State* inL, int32 LuaStackIndex, const void* ValuePtr) override
	{
		ue_lua_pushinteger(inL, (uint16)Property->GetPropertyValue(ValuePtr));
	}
	virtual void pop(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) override
	{
		pop_novirtual(inL, LuaStackIndex, ValuePtr);
	}
	void pop_novirtual(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) 
	{
		uint16 value = popiml<uint16>::pop(inL, LuaStackIndex);
		Property->SetPropertyValue(ValuePtr, value);
	}
	void push_container_novirtual(lua_State* inL, const void* ContainerPtr) 
	{
		push_novirtual(inL, (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr));
	}
	virtual void push_container(lua_State* inL, const void* ContainerPtr) override
	{
		ue_lua_pushinteger(inL, (uint16)Property->GetPropertyValue_InContainer(ContainerPtr));
	}
	virtual void push_ret_container(lua_State* inL, const void* ContainerPtr) override
	{
		ue_lua_pushinteger(inL, (uint16)Property->GetPropertyValue_InContainer(ContainerPtr));
	}
	virtual void push_ref_container(lua_State* inL, int32 LuaStackIndex, const void* ContainerPtr) override
	{
		ue_lua_pushinteger(inL, (uint16)Property->GetPropertyValue_InContainer(ContainerPtr));
	}
	virtual void pop_container(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr) override
	{
		pop_container_novirtual(inL, LuaStackIndex, ContainerPtr);
	}
	void pop_container_novirtual(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr)
	{
		uint16 value = popiml<uint16>::pop(inL, LuaStackIndex);
		Property->SetPropertyValue_InContainer((void*)ContainerPtr, value);
	}
};
struct LuaUInt16Property :public LuaBasePropertyInterface
{
	UInt16Property* Property;
	//Some hook
	virtual UProperty* GetProperty(){return Property;}
	virtual ~LuaUInt16Property(){}
	LuaUInt16Property(lua_State*inL, UInt16Property* InProperty):Property(InProperty)
	{
		
	}
	virtual void push(lua_State* inL, const void* ValuePtr) override
	{
		push_novirtual(inL, ValuePtr);
	}
	void push_novirtual(lua_State* inL, const void* ValuePtr) 
	{
		ue_lua_pushinteger(inL, (int16)Property->GetPropertyValue(ValuePtr));
	}
	virtual void push_ret(lua_State* inL, const void* ValuePtr) override
	{
		ue_lua_pushinteger(inL, (int16)Property->GetPropertyValue(ValuePtr));
	}
	virtual void push_ref(lua_State* inL, int32 LuaStackIndex, const void* ValuePtr) override
	{
		ue_lua_pushinteger(inL, (int16)Property->GetPropertyValue(ValuePtr));
	}
	virtual void pop(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) override
	{
		pop_novirtual(inL, LuaStackIndex, ValuePtr);
	}
	void pop_novirtual(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) 
	{
		int16 value = popiml<int16>::pop(inL, LuaStackIndex);
		Property->SetPropertyValue(ValuePtr, value);
	}
	void push_container_novirtual(lua_State* inL, const void* ContainerPtr) 
	{
		push_novirtual(inL, (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr));
	}
	virtual void push_container(lua_State* inL, const void* ContainerPtr) override
	{
		ue_lua_pushinteger(inL, (int16)Property->GetPropertyValue_InContainer(ContainerPtr));
	}
	virtual void push_ret_container(lua_State* inL, const void* ContainerPtr) override
	{
		ue_lua_pushinteger(inL, (int16)Property->GetPropertyValue_InContainer(ContainerPtr));
	}
	virtual void push_ref_container(lua_State* inL, int32 LuaStackIndex, const void* ContainerPtr) override
	{
		ue_lua_pushinteger(inL, (int16)Property->GetPropertyValue_InContainer(ContainerPtr));
	}
	virtual void pop_container(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr) override
	{
		pop_container_novirtual(inL, LuaStackIndex, ContainerPtr);
	}
	void pop_container_novirtual(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr)
	{
		int16 value = popiml<int16>::pop(inL, LuaStackIndex);
		Property->SetPropertyValue_InContainer((void*)ContainerPtr, value);
	}
};
struct LuaUUInt32Property :public LuaBasePropertyInterface
{
	UUInt32Property* Property;
	//Some hook
	virtual UProperty* GetProperty(){return Property;}
	virtual ~LuaUUInt32Property(){}
	LuaUUInt32Property(lua_State*inL, UUInt32Property* InProperty):Property(InProperty)
	{
		
	}
	virtual void push(lua_State* inL, const void* ValuePtr) override
	{
		push_novirtual(inL, ValuePtr);
	}
	void push_novirtual(lua_State* inL, const void* ValuePtr) 
	{
		ue_lua_pushinteger(inL, (uint32)Property->GetPropertyValue(ValuePtr));
	}
	virtual void push_ret(lua_State* inL, const void* ValuePtr) override
	{
		ue_lua_pushinteger(inL, (uint32)Property->GetPropertyValue(ValuePtr));
	}
	virtual void push_ref(lua_State* inL, int32 LuaStackIndex, const void* ValuePtr) override
	{
		ue_lua_pushinteger(inL, (uint32)Property->GetPropertyValue(ValuePtr));
	}
	virtual void pop(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) override
	{
		pop_novirtual(inL, LuaStackIndex, ValuePtr);
	}
	void pop_novirtual(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) 
	{
		uint32 value = popiml<uint32>::pop(inL, LuaStackIndex);
		Property->SetPropertyValue(ValuePtr, value);
	}
	void push_container_novirtual(lua_State* inL, const void* ContainerPtr) 
	{
		push_novirtual(inL, (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr));
	}
	virtual void push_container(lua_State* inL, const void* ContainerPtr) override
	{
		ue_lua_pushinteger(inL, (uint32)Property->GetPropertyValue_InContainer(ContainerPtr));
	}
	virtual void push_ret_container(lua_State* inL, const void* ContainerPtr) override
	{
		ue_lua_pushinteger(inL, (uint32)Property->GetPropertyValue_InContainer(ContainerPtr));
	}
	virtual void push_ref_container(lua_State* inL, int32 LuaStackIndex, const void* ContainerPtr) override
	{
		ue_lua_pushinteger(inL, (uint32)Property->GetPropertyValue_InContainer(ContainerPtr));
	}
	virtual void pop_container(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr) override
	{
		pop_container_novirtual(inL, LuaStackIndex, ContainerPtr);
	}
	void pop_container_novirtual(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr)
	{
		uint32 value = popiml<uint32>::pop(inL, LuaStackIndex);
		Property->SetPropertyValue_InContainer((void*)ContainerPtr, value);
	}
};
struct LuaUInt64Property :public LuaBasePropertyInterface
{
	UInt64Property* Property;
	//Some hook
	virtual UProperty* GetProperty(){return Property;}
	virtual ~LuaUInt64Property(){}
	LuaUInt64Property(lua_State*inL, UInt64Property* InProperty):Property(InProperty)
	{
		
	}
	virtual void push(lua_State* inL, const void* ValuePtr) override
	{
		push_novirtual(inL, ValuePtr);
	}
	void push_novirtual(lua_State* inL, const void* ValuePtr) 
	{
		ue_lua_pushinteger(inL, (int64)Property->GetPropertyValue(ValuePtr));
	}
	virtual void push_ret(lua_State* inL, const void* ValuePtr) override
	{
		ue_lua_pushinteger(inL, (int64)Property->GetPropertyValue(ValuePtr));
	}
	virtual void push_ref(lua_State* inL, int32 LuaStackIndex, const void* ValuePtr) override
	{
		ue_lua_pushinteger(inL, (int64)Property->GetPropertyValue(ValuePtr));
	}
	virtual void pop(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) override
	{
		pop_novirtual(inL, LuaStackIndex, ValuePtr);
	}
	void pop_novirtual(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) 
	{
		int64 value = popiml<int64>::pop(inL, LuaStackIndex);
		Property->SetPropertyValue(ValuePtr, value);
	}
	void push_container_novirtual(lua_State* inL, const void* ContainerPtr) 
	{
		push_novirtual(inL, (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr));
	}
	virtual void push_container(lua_State* inL, const void* ContainerPtr) override
	{
		ue_lua_pushinteger(inL, (int64)Property->GetPropertyValue_InContainer(ContainerPtr));
	}
	virtual void push_ret_container(lua_State* inL, const void* ContainerPtr) override
	{
		ue_lua_pushinteger(inL, (int64)Property->GetPropertyValue_InContainer(ContainerPtr));
	}
	virtual void push_ref_container(lua_State* inL, int32 LuaStackIndex, const void* ContainerPtr) override
	{
		ue_lua_pushinteger(inL, (int64)Property->GetPropertyValue_InContainer(ContainerPtr));
	}
	virtual void pop_container(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr) override
	{
		pop_container_novirtual(inL, LuaStackIndex, ContainerPtr);
	}
	void pop_container_novirtual(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr)
	{
		int64 value = popiml<int64>::pop(inL, LuaStackIndex);
		Property->SetPropertyValue_InContainer((void*)ContainerPtr, value);
	}
};
struct LuaUUInt64Property :public LuaBasePropertyInterface
{
	UUInt64Property* Property;
	//Some hook
	virtual UProperty* GetProperty(){return Property;}
	virtual ~LuaUUInt64Property(){}
	LuaUUInt64Property(lua_State*inL, UUInt64Property* InProperty):Property(InProperty)
	{
		
	}
	virtual void push(lua_State* inL, const void* ValuePtr) override
	{
		push_novirtual(inL, ValuePtr);
	}
	void push_novirtual(lua_State* inL, const void* ValuePtr) 
	{
		ue_lua_pushinteger(inL, (uint64)Property->GetPropertyValue(ValuePtr));
	}
	virtual void push_ret(lua_State* inL, const void* ValuePtr) override
	{
		ue_lua_pushinteger(inL, (uint64)Property->GetPropertyValue(ValuePtr));
	}
	virtual void push_ref(lua_State* inL, int32 LuaStackIndex, const void* ValuePtr) override
	{
		ue_lua_pushinteger(inL, (uint64)Property->GetPropertyValue(ValuePtr));
	}
	virtual void pop(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) override
	{
		pop_novirtual(inL, LuaStackIndex, ValuePtr);
	}
	void pop_novirtual(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) 
	{
		uint64 value = popiml<uint64>::pop(inL, LuaStackIndex);
		Property->SetPropertyValue(ValuePtr, value);
	}
	void push_container_novirtual(lua_State* inL, const void* ContainerPtr) 
	{
		push_novirtual(inL, (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr));
	}
	virtual void push_container(lua_State* inL, const void* ContainerPtr) override
	{
		ue_lua_pushinteger(inL, (uint64)Property->GetPropertyValue_InContainer(ContainerPtr));
	}
	virtual void push_ret_container(lua_State* inL, const void* ContainerPtr) override
	{
		ue_lua_pushinteger(inL, (uint64)Property->GetPropertyValue_InContainer(ContainerPtr));
	}
	virtual void push_ref_container(lua_State* inL, int32 LuaStackIndex, const void* ContainerPtr) override
	{
		ue_lua_pushinteger(inL, (uint64)Property->GetPropertyValue_InContainer(ContainerPtr));
	}
	virtual void pop_container(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr) override
	{
		pop_container_novirtual(inL, LuaStackIndex, ContainerPtr);
	}
	void pop_container_novirtual(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr)
	{
		uint64 value = popiml<uint64>::pop(inL, LuaStackIndex);
		Property->SetPropertyValue_InContainer((void*)ContainerPtr, value);
	}
};
struct LuaUFloatProperty :public LuaBasePropertyInterface
{
	UFloatProperty* Property;
	//Some hook
	virtual UProperty* GetProperty(){return Property;}
	virtual ~LuaUFloatProperty(){}
	LuaUFloatProperty(lua_State*inL, UFloatProperty* InProperty):Property(InProperty)
	{
		
	}
	virtual void push(lua_State* inL, const void* ValuePtr) override
	{
		push_novirtual(inL, ValuePtr);
	}
	void push_novirtual(lua_State* inL, const void* ValuePtr) 
	{
		ue_lua_pushnumber(inL, (float)Property->GetPropertyValue(ValuePtr));
	}
	virtual void push_ret(lua_State* inL, const void* ValuePtr) override
	{
		ue_lua_pushnumber(inL, (float)Property->GetPropertyValue(ValuePtr));
	}
	virtual void push_ref(lua_State* inL, int32 LuaStackIndex, const void* ValuePtr) override
	{
		ue_lua_pushnumber(inL, (float)Property->GetPropertyValue(ValuePtr));
	}
	virtual void pop(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) override
	{
		pop_novirtual(inL, LuaStackIndex, ValuePtr);
	}
	void pop_novirtual(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) 
	{
		float value = popiml<float>::pop(inL, LuaStackIndex);
		Property->SetPropertyValue(ValuePtr, value);
	}
	void push_container_novirtual(lua_State* inL, const void* ContainerPtr) 
	{
		push_novirtual(inL, (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr));
	}
	virtual void push_container(lua_State* inL, const void* ContainerPtr) override
	{
		ue_lua_pushnumber(inL, (float)Property->GetPropertyValue_InContainer(ContainerPtr));
	}
	virtual void push_ret_container(lua_State* inL, const void* ContainerPtr) override
	{
		ue_lua_pushnumber(inL, (float)Property->GetPropertyValue_InContainer(ContainerPtr));
	}
	virtual void push_ref_container(lua_State* inL, int32 LuaStackIndex, const void* ContainerPtr) override
	{
		ue_lua_pushnumber(inL, (float)Property->GetPropertyValue_InContainer(ContainerPtr));
	}
	virtual void pop_container(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr) override
	{
		pop_container_novirtual(inL, LuaStackIndex, ContainerPtr);
	}
	void pop_container_novirtual(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr)
	{
		float value = popiml<float>::pop(inL, LuaStackIndex);
		Property->SetPropertyValue_InContainer((void*)ContainerPtr, value);
	}
};
struct LuaUDoubleProperty :public LuaBasePropertyInterface
{
	UDoubleProperty* Property;
	//Some hook
	virtual UProperty* GetProperty(){return Property;}
	virtual ~LuaUDoubleProperty(){}
	LuaUDoubleProperty(lua_State*inL, UDoubleProperty* InProperty):Property(InProperty)
	{
		
	}
	virtual void push(lua_State* inL, const void* ValuePtr) override
	{
		push_novirtual(inL, ValuePtr);
	}
	void push_novirtual(lua_State* inL, const void* ValuePtr) 
	{
		ue_lua_pushnumber(inL, (double)Property->GetPropertyValue(ValuePtr));
	}
	virtual void push_ret(lua_State* inL, const void* ValuePtr) override
	{
		ue_lua_pushnumber(inL, (double)Property->GetPropertyValue(ValuePtr));
	}
	virtual void push_ref(lua_State* inL, int32 LuaStackIndex, const void* ValuePtr) override
	{
		ue_lua_pushnumber(inL, (double)Property->GetPropertyValue(ValuePtr));
	}
	virtual void pop(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) override
	{
		pop_novirtual(inL, LuaStackIndex, ValuePtr);
	}
	void pop_novirtual(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) 
	{
		double value = popiml<double>::pop(inL, LuaStackIndex);
		Property->SetPropertyValue(ValuePtr, value);
	}
	void push_container_novirtual(lua_State* inL, const void* ContainerPtr) 
	{
		push_novirtual(inL, (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr));
	}
	virtual void push_container(lua_State* inL, const void* ContainerPtr) override
	{
		ue_lua_pushnumber(inL, (double)Property->GetPropertyValue_InContainer(ContainerPtr));
	}
	virtual void push_ret_container(lua_State* inL, const void* ContainerPtr) override
	{
		ue_lua_pushnumber(inL, (double)Property->GetPropertyValue_InContainer(ContainerPtr));
	}
	virtual void push_ref_container(lua_State* inL, int32 LuaStackIndex, const void* ContainerPtr) override
	{
		ue_lua_pushnumber(inL, (double)Property->GetPropertyValue_InContainer(ContainerPtr));
	}
	virtual void pop_container(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr) override
	{
		pop_container_novirtual(inL, LuaStackIndex, ContainerPtr);
	}
	void pop_container_novirtual(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr)
	{
		double value = popiml<double>::pop(inL, LuaStackIndex);
		Property->SetPropertyValue_InContainer((void*)ContainerPtr, value);
	}
};
struct LuaUObjectPropertyBase :public LuaBasePropertyInterface
{
	UObjectPropertyBase* Property;
	//Some hook
	virtual UProperty* GetProperty(){return Property;}
	virtual ~LuaUObjectPropertyBase(){}
	LuaUObjectPropertyBase(lua_State*inL, UObjectPropertyBase* InProperty):Property(InProperty)
	{
		
	}
	virtual void push(lua_State* inL, const void* ValuePtr) override
	{
		push_novirtual(inL, ValuePtr);
	}
	void push_novirtual(lua_State* inL, const void* ValuePtr) 
	{
		pushuobject(inL, Property->GetObjectPropertyValue(ValuePtr));
	}
	virtual void push_ret(lua_State* inL, const void* ValuePtr) override
	{
		pushuobject(inL, Property->GetObjectPropertyValue(ValuePtr));
	}
	virtual void push_ref(lua_State* inL, int32 LuaStackIndex, const void* ValuePtr) override
	{
		pushuobject(inL, Property->GetObjectPropertyValue(ValuePtr));
	}
	virtual void pop(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) override
	{
		pop_novirtual(inL, LuaStackIndex, ValuePtr);
	}
	void pop_novirtual(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) 
	{
		UObject* value = popiml<UObject*>::pop(inL, LuaStackIndex);
		Property->SetObjectPropertyValue(ValuePtr, value);
	}
	void push_container_novirtual(lua_State* inL, const void* ContainerPtr) 
	{
		push_novirtual(inL, (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr));
	}
	virtual void push_container(lua_State* inL, const void* ContainerPtr) override
	{
		pushuobject(inL, Property->GetObjectPropertyValue_InContainer(ContainerPtr));
	}
	virtual void push_ret_container(lua_State* inL, const void* ContainerPtr) override
	{
		pushuobject(inL, Property->GetObjectPropertyValue_InContainer(ContainerPtr));
	}
	virtual void push_ref_container(lua_State* inL, int32 LuaStackIndex, const void* ContainerPtr) override
	{
		pushuobject(inL, Property->GetObjectPropertyValue_InContainer(ContainerPtr));
	}
	virtual void pop_container(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr) override
	{
		pop_container_novirtual(inL, LuaStackIndex, ContainerPtr);
	}
	void pop_container_novirtual(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr)
	{
		UObject* value = popiml<UObject*>::pop(inL, LuaStackIndex);
		Property->SetObjectPropertyValue_InContainer((void*)ContainerPtr, value);
	}
};
struct LuaUObjectProperty :public LuaBasePropertyInterface
{
	UObjectProperty* Property;
	//Some hook
	virtual UProperty* GetProperty(){return Property;}
	virtual ~LuaUObjectProperty(){}
	LuaUObjectProperty(lua_State*inL, UObjectProperty* InProperty):Property(InProperty)
	{
		
	}
	virtual void push(lua_State* inL, const void* ValuePtr) override
	{
		push_novirtual(inL, ValuePtr);
	}
	void push_novirtual(lua_State* inL, const void* ValuePtr) 
	{
		pushuobject(inL, Property->GetObjectPropertyValue(ValuePtr));
	}
	virtual void push_ret(lua_State* inL, const void* ValuePtr) override
	{
		pushuobject(inL, Property->GetObjectPropertyValue(ValuePtr));
	}
	virtual void push_ref(lua_State* inL, int32 LuaStackIndex, const void* ValuePtr) override
	{
		pushuobject(inL, Property->GetObjectPropertyValue(ValuePtr));
	}
	virtual void pop(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) override
	{
		pop_novirtual(inL, LuaStackIndex, ValuePtr);
	}
	void pop_novirtual(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) 
	{
		UObject* value = popiml<UObject*>::pop(inL, LuaStackIndex);
		Property->SetObjectPropertyValue(ValuePtr, value);
	}
	void push_container_novirtual(lua_State* inL, const void* ContainerPtr) 
	{
		push_novirtual(inL, (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr));
	}
	virtual void push_container(lua_State* inL, const void* ContainerPtr) override
	{
		pushuobject(inL, Property->GetObjectPropertyValue_InContainer(ContainerPtr));
	}
	virtual void push_ret_container(lua_State* inL, const void* ContainerPtr) override
	{
		pushuobject(inL, Property->GetObjectPropertyValue_InContainer(ContainerPtr));
	}
	virtual void push_ref_container(lua_State* inL, int32 LuaStackIndex, const void* ContainerPtr) override
	{
		pushuobject(inL, Property->GetObjectPropertyValue_InContainer(ContainerPtr));
	}
	virtual void pop_container(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr) override
	{
		pop_container_novirtual(inL, LuaStackIndex, ContainerPtr);
	}
	void pop_container_novirtual(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr)
	{
		UObject* value = popiml<UObject*>::pop(inL, LuaStackIndex);
		Property->SetObjectPropertyValue_InContainer((void*)ContainerPtr, value);
	}
};
struct LuaUClassProperty :public LuaBasePropertyInterface
{
	UClassProperty* Property;
	//Some hook
	virtual UProperty* GetProperty(){return Property;}
	virtual ~LuaUClassProperty(){}
	LuaUClassProperty(lua_State*inL, UClassProperty* InProperty):Property(InProperty)
	{
		
	}
	virtual void push(lua_State* inL, const void* ValuePtr) override
	{
		push_novirtual(inL, ValuePtr);
	}
	void push_novirtual(lua_State* inL, const void* ValuePtr) 
	{
		pushuobject(inL, Property->GetObjectPropertyValue(ValuePtr));
	}
	virtual void push_ret(lua_State* inL, const void* ValuePtr) override
	{
		pushuobject(inL, Property->GetObjectPropertyValue(ValuePtr));
	}
	virtual void push_ref(lua_State* inL, int32 LuaStackIndex, const void* ValuePtr) override
	{
		pushuobject(inL, Property->GetObjectPropertyValue(ValuePtr));
	}
	virtual void pop(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) override
	{
		pop_novirtual(inL, LuaStackIndex, ValuePtr);
	}
	void pop_novirtual(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) 
	{
		UObject* value = popiml<UObject*>::pop(inL, LuaStackIndex);
		Property->SetObjectPropertyValue(ValuePtr, value);
	}
	void push_container_novirtual(lua_State* inL, const void* ContainerPtr) 
	{
		push_novirtual(inL, (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr));
	}
	virtual void push_container(lua_State* inL, const void* ContainerPtr) override
	{
		pushuobject(inL, Property->GetObjectPropertyValue_InContainer(ContainerPtr));
	}
	virtual void push_ret_container(lua_State* inL, const void* ContainerPtr) override
	{
		pushuobject(inL, Property->GetObjectPropertyValue_InContainer(ContainerPtr));
	}
	virtual void push_ref_container(lua_State* inL, int32 LuaStackIndex, const void* ContainerPtr) override
	{
		pushuobject(inL, Property->GetObjectPropertyValue_InContainer(ContainerPtr));
	}
	virtual void pop_container(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr) override
	{
		pop_container_novirtual(inL, LuaStackIndex, ContainerPtr);
	}
	void pop_container_novirtual(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr)
	{
		UObject* value = popiml<UObject*>::pop(inL, LuaStackIndex);
		Property->SetObjectPropertyValue_InContainer((void*)ContainerPtr, value);
	}
};
struct LuaUStrProperty :public LuaBasePropertyInterface
{
	UStrProperty* Property;
	//Some hook
	virtual UProperty* GetProperty(){return Property;}
	virtual ~LuaUStrProperty(){}
	LuaUStrProperty(lua_State*inL, UStrProperty* InProperty):Property(InProperty)
	{
		
	}
	virtual void push(lua_State* inL, const void* ValuePtr) override
	{
		push_novirtual(inL, ValuePtr);
	}
	void push_novirtual(lua_State* inL, const void* ValuePtr) 
	{
		ue_lua_pushstring(inL, TCHAR_TO_UTF8(*(Property->GetPropertyValue(ValuePtr))));
	}
	virtual void push_ret(lua_State* inL, const void* ValuePtr) override
	{
		ue_lua_pushstring(inL, TCHAR_TO_UTF8(*(Property->GetPropertyValue(ValuePtr))));
	}
	virtual void push_ref(lua_State* inL, int32 LuaStackIndex, const void* ValuePtr) override
	{
		ue_lua_pushstring(inL, TCHAR_TO_UTF8(*(Property->GetPropertyValue(ValuePtr))));
	}
	virtual void pop(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) override
	{
		pop_novirtual(inL, LuaStackIndex, ValuePtr);
	}
	void pop_novirtual(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) 
	{
		FString value = popiml<FString>::pop(inL, LuaStackIndex);
		Property->SetPropertyValue(ValuePtr, value);
	}
	void push_container_novirtual(lua_State* inL, const void* ContainerPtr) 
	{
		push_novirtual(inL, (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr));
	}
	virtual void push_container(lua_State* inL, const void* ContainerPtr) override
	{
		ue_lua_pushstring(inL, TCHAR_TO_UTF8(*(Property->GetPropertyValue_InContainer(ContainerPtr))));
	}
	virtual void push_ret_container(lua_State* inL, const void* ContainerPtr) override
	{
		ue_lua_pushstring(inL, TCHAR_TO_UTF8(*(Property->GetPropertyValue_InContainer(ContainerPtr))));
	}
	virtual void push_ref_container(lua_State* inL, int32 LuaStackIndex, const void* ContainerPtr) override
	{
		ue_lua_pushstring(inL, TCHAR_TO_UTF8(*(Property->GetPropertyValue_InContainer(ContainerPtr))));
	}
	virtual void pop_container(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr) override
	{
		pop_container_novirtual(inL, LuaStackIndex, ContainerPtr);
	}
	void pop_container_novirtual(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr)
	{
		FString value = popiml<FString>::pop(inL, LuaStackIndex);
		Property->SetPropertyValue_InContainer((void*)ContainerPtr, value);
	}
};
struct LuaUNameProperty :public LuaBasePropertyInterface
{
	UNameProperty* Property;
	//Some hook
	virtual UProperty* GetProperty(){return Property;}
	virtual ~LuaUNameProperty(){}
	LuaUNameProperty(lua_State*inL, UNameProperty* InProperty):Property(InProperty)
	{
		
	}
	virtual void push(lua_State* inL, const void* ValuePtr) override
	{
		push_novirtual(inL, ValuePtr);
	}
	void push_novirtual(lua_State* inL, const void* ValuePtr) 
	{
		FName result = (FName)Property->GetPropertyValue(ValuePtr);
		ue_lua_pushstring(inL, TCHAR_TO_UTF8(*result.ToString()));
	}
	virtual void push_ret(lua_State* inL, const void* ValuePtr) override
	{
		FName result = (FName)Property->GetPropertyValue(ValuePtr);
		ue_lua_pushstring(inL, TCHAR_TO_UTF8(*result.ToString()));
	}
	virtual void push_ref(lua_State* inL, int32 LuaStackIndex, const void* ValuePtr) override
	{
		FName result = (FName)Property->GetPropertyValue(ValuePtr);
		ue_lua_pushstring(inL, TCHAR_TO_UTF8(*result.ToString()));
	}
	virtual void pop(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) override
	{
		pop_novirtual(inL, LuaStackIndex, ValuePtr);
	}
	void pop_novirtual(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) 
	{
		FName value = popiml<FName>::pop(inL, LuaStackIndex);
		Property->SetPropertyValue(ValuePtr, value);
	}
	void push_container_novirtual(lua_State* inL, const void* ContainerPtr) 
	{
		push_novirtual(inL, (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr));
	}
	virtual void push_container(lua_State* inL, const void* ContainerPtr) override
	{
		FName result = (FName)Property->GetPropertyValue_InContainer(ContainerPtr);
		ue_lua_pushstring(inL, TCHAR_TO_UTF8(*result.ToString()));
	}
	virtual void push_ret_container(lua_State* inL, const void* ContainerPtr) override
	{
		FName result = (FName)Property->GetPropertyValue_InContainer(ContainerPtr);
		ue_lua_pushstring(inL, TCHAR_TO_UTF8(*result.ToString()));
	}
	virtual void push_ref_container(lua_State* inL, int32 LuaStackIndex, const void* ContainerPtr) override
	{
		FName result = (FName)Property->GetPropertyValue_InContainer(ContainerPtr);
		ue_lua_pushstring(inL, TCHAR_TO_UTF8(*result.ToString()));
	}
	virtual void pop_container(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr) override
	{
		pop_container_novirtual(inL, LuaStackIndex, ContainerPtr);
	}
	void pop_container_novirtual(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr)
	{
		FName value = popiml<FName>::pop(inL, LuaStackIndex);
		Property->SetPropertyValue_InContainer((void*)ContainerPtr, value);
	}
};
struct LuaUTextProperty :public LuaBasePropertyInterface
{
	UTextProperty* Property;
	//Some hook
	virtual UProperty* GetProperty(){return Property;}
	virtual ~LuaUTextProperty(){}
	LuaUTextProperty(lua_State*inL, UTextProperty* InProperty):Property(InProperty)
	{
		
	}
	virtual void push(lua_State* inL, const void* ValuePtr) override
	{
		push_novirtual(inL, ValuePtr);
	}
	void push_novirtual(lua_State* inL, const void* ValuePtr) 
	{
		pushstruct_nogc(inL, "FText", "FText_nogc", (void*)ValuePtr);
	}
	virtual void push_ret(lua_State* inL, const void* ValuePtr) override
	{
		FText* Text_ptr = (FText*)ValuePtr;
		UTableUtil::push_ret(inL, *Text_ptr);
	}
	virtual void push_ref(lua_State* inL, int32 LuaStackIndex, const void* ValuePtr) override
	{
		if (!ue_lua_isuserdata(inL, LuaStackIndex))
		{
			FText* Text_ptr = (FText*)ValuePtr;
			UTableUtil::push(inL, Text_ptr->ToString());
		}
		else
		{
			void* DestPtr = tovoid(inL, LuaStackIndex);
			Property->CopyCompleteValueFromScriptVM(DestPtr, ValuePtr);
			ue_lua_pushvalue(inL, LuaStackIndex);
		}
	}
	virtual void pop(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) override
	{
		pop_novirtual(inL, LuaStackIndex, ValuePtr);
	}
	void pop_novirtual(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) 
	{
		FText value = popiml<FText>::pop(inL, LuaStackIndex);
		Property->SetPropertyValue(ValuePtr, value);
	}
	void push_container_novirtual(lua_State* inL, const void* ContainerPtr) 
	{
		push_novirtual(inL, (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr));
	}
	virtual void push_container(lua_State* inL, const void* ContainerPtr) override
	{
		pushstruct_nogc(inL, "FText", "FText_nogc", (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr));
	}
	virtual void push_ret_container(lua_State* inL, const void* ContainerPtr) override
	{
		FText* Text_ptr = (FText*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr);
		UTableUtil::push_ret(inL, *Text_ptr);
	}
	virtual void push_ref_container(lua_State* inL, int32 LuaStackIndex, const void* ContainerPtr) override
	{
		if (!ue_lua_isuserdata(inL, LuaStackIndex))
		{
			FText* Text_ptr = (FText*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr);
			UTableUtil::push(inL, Text_ptr->ToString());
		}
		else
		{
			void* DestPtr = tovoid(inL, LuaStackIndex);
			Property->CopyCompleteValueFromScriptVM(DestPtr, (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr));
			ue_lua_pushvalue(inL, LuaStackIndex);
		}
	}
	virtual void pop_container(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr) override
	{
		pop_container_novirtual(inL, LuaStackIndex, ContainerPtr);
	}
	void pop_container_novirtual(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr)
	{
		FText value = popiml<FText>::pop(inL, LuaStackIndex);
		Property->SetPropertyValue_InContainer((void*)ContainerPtr, value);
	}
};
struct LuaUByteProperty :public LuaBasePropertyInterface
{
	UByteProperty* Property;
	//Some hook
	virtual UProperty* GetProperty(){return Property;}
	virtual ~LuaUByteProperty(){}
	LuaUByteProperty(lua_State*inL, UByteProperty* InProperty):Property(InProperty)
	{
		
	}
	virtual void push(lua_State* inL, const void* ValuePtr) override
	{
		push_novirtual(inL, ValuePtr);
	}
	void push_novirtual(lua_State* inL, const void* ValuePtr) 
	{
		ue_lua_pushinteger(inL, (int32)Property->GetPropertyValue(ValuePtr));
	}
	virtual void push_ret(lua_State* inL, const void* ValuePtr) override
	{
		ue_lua_pushinteger(inL, (int32)Property->GetPropertyValue(ValuePtr));
	}
	virtual void push_ref(lua_State* inL, int32 LuaStackIndex, const void* ValuePtr) override
	{
		ue_lua_pushinteger(inL, (int32)Property->GetPropertyValue(ValuePtr));
	}
	virtual void pop(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) override
	{
		pop_novirtual(inL, LuaStackIndex, ValuePtr);
	}
	void pop_novirtual(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) 
	{
		int32 value = popiml<int>::pop(inL, LuaStackIndex);
		Property->SetPropertyValue(ValuePtr, value);
	}
	void push_container_novirtual(lua_State* inL, const void* ContainerPtr) 
	{
		push_novirtual(inL, (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr));
	}
	virtual void push_container(lua_State* inL, const void* ContainerPtr) override
	{
		ue_lua_pushinteger(inL, (int32)Property->GetPropertyValue_InContainer(ContainerPtr));
	}
	virtual void push_ret_container(lua_State* inL, const void* ContainerPtr) override
	{
		ue_lua_pushinteger(inL, (int32)Property->GetPropertyValue_InContainer(ContainerPtr));
	}
	virtual void push_ref_container(lua_State* inL, int32 LuaStackIndex, const void* ContainerPtr) override
	{
		ue_lua_pushinteger(inL, (int32)Property->GetPropertyValue_InContainer(ContainerPtr));
	}
	virtual void pop_container(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr) override
	{
		pop_container_novirtual(inL, LuaStackIndex, ContainerPtr);
	}
	void pop_container_novirtual(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr)
	{
		int32 value = popiml<int>::pop(inL, LuaStackIndex);
		Property->SetPropertyValue_InContainer((void*)ContainerPtr, value);
	}
};
struct LuaUEnumProperty :public LuaBasePropertyInterface
{
	UEnumProperty* Property;
	TSharedPtr<LuaBasePropertyInterface> UnderlyingProperty;
	//Some hook
	virtual UProperty* GetProperty(){return Property;}
	virtual ~LuaUEnumProperty(){}
	LuaUEnumProperty(lua_State*inL, UEnumProperty* InProperty):Property(InProperty)
	{
		UnderlyingProperty = CreatePropertyInterface(inL, InProperty->GetUnderlyingProperty());
	}
	virtual void push(lua_State* inL, const void* ValuePtr) override
	{
		push_novirtual(inL, ValuePtr);
	}
	void push_novirtual(lua_State* inL, const void* ValuePtr) 
	{
		UnderlyingProperty->push(inL, ValuePtr);
	}
	virtual void push_ret(lua_State* inL, const void* ValuePtr) override
	{
		UnderlyingProperty->push(inL, ValuePtr);
	}
	virtual void push_ref(lua_State* inL, int32 LuaStackIndex, const void* ValuePtr) override
	{
		UnderlyingProperty->push(inL, ValuePtr);
	}
	virtual void pop(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) override
	{
		pop_novirtual(inL, LuaStackIndex, ValuePtr);
	}
	void pop_novirtual(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) 
	{
		UnderlyingProperty->pop(inL, LuaStackIndex, ValuePtr);
	}
	void push_container_novirtual(lua_State* inL, const void* ContainerPtr) 
	{
		push_novirtual(inL, (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr));
	}
	virtual void push_container(lua_State* inL, const void* ContainerPtr) override
	{
		push_container_novirtual(inL, ContainerPtr);
	}
	virtual void push_ret_container(lua_State* inL, const void* ContainerPtr) override
	{
		push_container_novirtual(inL, ContainerPtr);
	}
	virtual void push_ref_container(lua_State* inL, int32 LuaStackIndex, const void* ContainerPtr) override
	{
		push_container_novirtual(inL, ContainerPtr);
	}
	virtual void pop_container(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr) override
	{
		pop_container_novirtual(inL, LuaStackIndex, ContainerPtr);
	}
	void pop_container_novirtual(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr)
	{
		pop_novirtual(inL, LuaStackIndex, (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr));
	}
};
struct LuaUStructProperty :public LuaBasePropertyInterface
{
	UStructProperty* Property;
	FString TypeName;
	FString TypeName_nogc;
	const char* PtrTypeName;
	const char* PtrTypeName_nogc;
	//Some hook
	virtual UProperty* GetProperty(){return Property;}
	virtual ~LuaUStructProperty(){}
	LuaUStructProperty(lua_State*inL, UStructProperty* InProperty):Property(InProperty)
	{
		FString TempName;
		if (UUserDefinedStruct* BpStruct = Cast<UUserDefinedStruct>(Property->Struct))
		{
			UTableUtil::MayAddNewStructType(inL, BpStruct);
			TempName = BpStruct->GetName();
		}
		else
			TempName = Property->Struct->GetStructCPPName();
		TypeName = TempName;
		TypeName_nogc = TempName+"_nogc";
	}
	virtual void push(lua_State* inL, const void* ValuePtr) override
	{
		push_novirtual(inL, ValuePtr);
	}
	void push_novirtual(lua_State* inL, const void* ValuePtr) 
	{
		pushstruct_nogc(inL, TCHAR_TO_UTF8(*TypeName), TCHAR_TO_UTF8(*TypeName_nogc), (void*)ValuePtr);
	}
	virtual void push_ret(lua_State* inL, const void* ValuePtr) override
	{
		uint8* result = GetBpStructTempIns(TypeName, Property->GetSize());
		Property->InitializeValue(result);
		Property->CopyCompleteValueFromScriptVM(result, ValuePtr);
		pushstruct_nogc(inL, TCHAR_TO_UTF8(*TypeName), TCHAR_TO_UTF8(*TypeName_nogc), result);
	}
	virtual void push_ref(lua_State* inL, int32 LuaStackIndex, const void* ValuePtr) override
	{
		void* DestPtr = tovoid(inL, LuaStackIndex);
		if(DestPtr)
			Property->CopyCompleteValueFromScriptVM(DestPtr, ValuePtr);
		else
			ensureAlwaysMsgf(0, TEXT("Bug"));
		lua_pushvalue(inL, LuaStackIndex);
	}
	virtual void pop(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) override
	{
		pop_novirtual(inL, LuaStackIndex, ValuePtr);
	}
	void pop_novirtual(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) 
	{
		void* value = tostruct(inL, LuaStackIndex);
		if (value)
			Property->CopyCompleteValue(ValuePtr, value);
		else
			ensureAlwaysMsgf(0, TEXT("Bug"));
	}
	void push_container_novirtual(lua_State* inL, const void* ContainerPtr) 
	{
		lua_pushvalue(inL, lua_upvalueindex(2));
		if (!lua_isnil(inL, -1))
		{
			lua_pushvalue(inL, 1);
			lua_rawget(inL, -2);
			if (!lua_isnil(inL, -1))
			{
				return;
			}
		}
		push_novirtual(inL, (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr));
		lua_pushvalue(inL, lua_upvalueindex(2));
		if (!lua_isnil(inL, -1))
		{
			lua_pushvalue(inL, 1);
			lua_pushvalue(inL, -3);
			lua_rawset(inL, -3);
			lua_pop(inL, 1);
		}
	}
	virtual void push_container(lua_State* inL, const void* ContainerPtr) override
	{
		pushstruct_nogc(inL, TCHAR_TO_UTF8(*TypeName), TCHAR_TO_UTF8(*TypeName_nogc), (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr));
	}
	virtual void push_ret_container(lua_State* inL, const void* ContainerPtr) override
	{
		void* ValuePtr = (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr);
		uint8* result = GetBpStructTempIns(TypeName, Property->GetSize());
		Property->InitializeValue(result);
		Property->CopyCompleteValueFromScriptVM(result, ValuePtr);
		pushstruct_nogc(inL, TCHAR_TO_UTF8(*TypeName), TCHAR_TO_UTF8(*TypeName_nogc), result);
	}
	virtual void push_ref_container(lua_State* inL, int32 LuaStackIndex, const void* ContainerPtr) override
	{
		void* ValuePtr = (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr);
		void* DestPtr = tovoid(inL, LuaStackIndex);
		if (DestPtr)
			Property->CopyCompleteValueFromScriptVM(DestPtr, ValuePtr);
		else
			ensureAlwaysMsgf(0, TEXT("Bug"));
		lua_pushvalue(inL, LuaStackIndex);
	}
	virtual void pop_container(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr) override
	{
		pop_container_novirtual(inL, LuaStackIndex, ContainerPtr);
	}
	void pop_container_novirtual(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr)
	{
		void* value = tostruct(inL, LuaStackIndex);
		if (value)
			Property->CopyCompleteValue(Property->ContainerPtrToValuePtr<void>((void*)ContainerPtr), value);
		else
			ensureAlwaysMsgf(0, TEXT("Bug"));
	}
};
struct LuaUMulticastDelegateProperty :public LuaBasePropertyInterface
{
	UMulticastDelegateProperty* Property;
	//Some hook
	virtual UProperty* GetProperty(){return Property;}
	virtual ~LuaUMulticastDelegateProperty(){}
	LuaUMulticastDelegateProperty(lua_State*inL, UMulticastDelegateProperty* InProperty):Property(InProperty)
	{
		
	}
	virtual void push(lua_State* inL, const void* ValuePtr) override
	{
		push_novirtual(inL, ValuePtr);
	}
	void push_novirtual(lua_State* inL, const void* ValuePtr) 
	{
		UFunction* FunSig = Property->SignatureFunction;
		auto delegateproxy = NewObject<ULuaDelegateMulti>();
		delegateproxy->Init((void*)ValuePtr, FunSig);
		pushuobject(inL, (void*)delegateproxy);
	}
	virtual void push_ret(lua_State* inL, const void* ValuePtr) override
	{
		check(0);
	}
	virtual void push_ref(lua_State* inL, int32 LuaStackIndex, const void* ValuePtr) override
	{
		check(0);
	}
	virtual void pop(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) override
	{
		pop_novirtual(inL, LuaStackIndex, ValuePtr);
	}
	void pop_novirtual(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) 
	{
		check(0);
	}
	void push_container_novirtual(lua_State* inL, const void* ContainerPtr) 
	{
		lua_pushvalue(inL, lua_upvalueindex(2));
		if (!lua_isnil(inL, -1))
		{
			lua_pushvalue(inL, 1);
			lua_rawget(inL, -2);
			if (!lua_isnil(inL, -1))
			{
				return;
			}
		}
		push_novirtual(inL, (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr));
		lua_pushvalue(inL, lua_upvalueindex(2));
		if (!lua_isnil(inL, -1))
		{
			lua_pushvalue(inL, 1);
			lua_pushvalue(inL, -3);
			lua_rawset(inL, -3);
			lua_pop(inL, 1);
		}
	}
	virtual void push_container(lua_State* inL, const void* ContainerPtr) override
	{
		void* ValuePtr = (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr);
		push_novirtual(inL, ValuePtr);
	}
	virtual void push_ret_container(lua_State* inL, const void* ContainerPtr) override
	{
		check(0);
	}
	virtual void push_ref_container(lua_State* inL, int32 LuaStackIndex, const void* ContainerPtr) override
	{
		check(0);
	}
	virtual void pop_container(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr) override
	{
		pop_container_novirtual(inL, LuaStackIndex, ContainerPtr);
	}
	void pop_container_novirtual(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr)
	{
		check(0);
	}
};
struct LuaUDelegateProperty :public LuaBasePropertyInterface
{
	UDelegateProperty* Property;
	//Some hook
	virtual UProperty* GetProperty(){return Property;}
	virtual ~LuaUDelegateProperty(){}
	LuaUDelegateProperty(lua_State*inL, UDelegateProperty* InProperty):Property(InProperty)
	{
		
	}
	virtual void push(lua_State* inL, const void* ValuePtr) override
	{
		push_novirtual(inL, ValuePtr);
	}
	void push_novirtual(lua_State* inL, const void* ValuePtr) 
	{
		FScriptDelegate* DelegatePtr = (FScriptDelegate*)Property->GetPropertyValuePtr(ValuePtr);
		ULuaDelegateSingle* NewOne = ULuaDelegateSingle::CreateInCppRef(DelegatePtr, Property->SignatureFunction);
		UTableUtil::push(inL, NewOne);
	}
	virtual void push_ret(lua_State* inL, const void* ValuePtr) override
	{
		check(0);
	}
	virtual void push_ref(lua_State* inL, int32 LuaStackIndex, const void* ValuePtr) override
	{
		check(0);
	}
	virtual void pop(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) override
	{
		pop_novirtual(inL, LuaStackIndex, ValuePtr);
	}
	void pop_novirtual(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) 
	{
		check(0);
	}
	void push_container_novirtual(lua_State* inL, const void* ContainerPtr) 
	{
		lua_pushvalue(inL, lua_upvalueindex(2));
		if (!lua_isnil(inL, -1))
		{
			lua_pushvalue(inL, 1);
			lua_rawget(inL, -2);
			if (!lua_isnil(inL, -1))
			{
				return;
			}
		}
		push_novirtual(inL, (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr));
		lua_pushvalue(inL, lua_upvalueindex(2));
		if (!lua_isnil(inL, -1))
		{
			lua_pushvalue(inL, 1);
			lua_pushvalue(inL, -3);
			lua_rawset(inL, -3);
			lua_pop(inL, 1);
		}
	}
	virtual void push_container(lua_State* inL, const void* ContainerPtr) override
	{
		check(0);
	}
	virtual void push_ret_container(lua_State* inL, const void* ContainerPtr) override
	{
		check(0);
	}
	virtual void push_ref_container(lua_State* inL, int32 LuaStackIndex, const void* ContainerPtr) override
	{
		check(0);
	}
	virtual void pop_container(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr) override
	{
		pop_container_novirtual(inL, LuaStackIndex, ContainerPtr);
	}
	void pop_container_novirtual(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr)
	{
		check(0);
	}
};
struct LuaUWeakObjectProperty :public LuaBasePropertyInterface
{
	UWeakObjectProperty* Property;
	//Some hook
	virtual UProperty* GetProperty(){return Property;}
	virtual ~LuaUWeakObjectProperty(){}
	LuaUWeakObjectProperty(lua_State*inL, UWeakObjectProperty* InProperty):Property(InProperty)
	{
		
	}
	virtual void push(lua_State* inL, const void* ValuePtr) override
	{
		push_novirtual(inL, ValuePtr);
	}
	void push_novirtual(lua_State* inL, const void* ValuePtr) 
	{
		pushuobject(inL, Property->GetObjectPropertyValue(ValuePtr));
	}
	virtual void push_ret(lua_State* inL, const void* ValuePtr) override
	{
		pushuobject(inL, Property->GetObjectPropertyValue(ValuePtr));
	}
	virtual void push_ref(lua_State* inL, int32 LuaStackIndex, const void* ValuePtr) override
	{
		pushuobject(inL, Property->GetObjectPropertyValue(ValuePtr));
	}
	virtual void pop(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) override
	{
		pop_novirtual(inL, LuaStackIndex, ValuePtr);
	}
	void pop_novirtual(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) 
	{
		UObject* value = popiml<UObject*>::pop(inL, LuaStackIndex);
		Property->SetObjectPropertyValue(ValuePtr, value);
	}
	void push_container_novirtual(lua_State* inL, const void* ContainerPtr) 
	{
		push_novirtual(inL, (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr));
	}
	virtual void push_container(lua_State* inL, const void* ContainerPtr) override
	{
		pushuobject(inL, Property->GetObjectPropertyValue_InContainer(ContainerPtr));
	}
	virtual void push_ret_container(lua_State* inL, const void* ContainerPtr) override
	{
		pushuobject(inL, Property->GetObjectPropertyValue_InContainer(ContainerPtr));
	}
	virtual void push_ref_container(lua_State* inL, int32 LuaStackIndex, const void* ContainerPtr) override
	{
		pushuobject(inL, Property->GetObjectPropertyValue_InContainer(ContainerPtr));
	}
	virtual void pop_container(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr) override
	{
		pop_container_novirtual(inL, LuaStackIndex, ContainerPtr);
	}
	void pop_container_novirtual(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr)
	{
		UObject* value = popiml<UObject*>::pop(inL, LuaStackIndex);
		Property->SetObjectPropertyValue_InContainer((void*)ContainerPtr, value);
	}
};
struct LuaUArrayProperty :public LuaBasePropertyInterface
{
	TSharedPtr<LuaBasePropertyInterface> InnerProperty;
	UArrayProperty* Property;
	//Some hook
	virtual UProperty* GetProperty(){return Property;}
	virtual ~LuaUArrayProperty(){}
	LuaUArrayProperty(lua_State*inL, UArrayProperty* InProperty):Property(InProperty)
	{
		InnerProperty = CreatePropertyInterface(inL, Property->Inner);
	}
	virtual void push(lua_State* inL, const void* ValuePtr) override
	{
		push_novirtual(inL, ValuePtr);
	}
	void push_novirtual(lua_State* inL, const void* ValuePtr) 
	{
		pushstruct_gc(inL, "ULuaArrayHelper", ULuaArrayHelper::GetHelperCPP_ValuePtr((void*)ValuePtr, Property));
	}
	virtual void push_ret(lua_State* inL, const void* ValuePtr) override
	{
		FScriptArrayHelper result(Property, ValuePtr);
		lua_newtable(inL);
		for (int32 i = 0; i < result.Num(); ++i)
		{
			ue_lua_pushinteger(inL, i + 1);
// will bug for struct type
			InnerProperty->push_ret_container(inL, result.GetRawPtr(i));
			ue_lua_rawset(inL, -3);
		}
	}
	virtual void push_ref(lua_State* inL, int32 LuaStackIndex, const void* ValuePtr) override
	{
		if (UnrealLua::IsGlueTArray(inL, LuaStackIndex))
		{
			void* Ptr = tovoid(inL, LuaStackIndex);
			ULuaArrayHelper::GlueArrCopyTo(Property, ValuePtr, Ptr);
			ue_lua_pushvalue(inL, LuaStackIndex);
		}
		else if (ULuaArrayHelper* ArrHelper = (ULuaArrayHelper*)UnrealLua::IsCppPtr(inL, LuaStackIndex))
		{
			ArrHelper->CopyFrom(Property, ValuePtr);
			ue_lua_pushvalue(inL, LuaStackIndex);
		}
		else if (lua_istable(inL, LuaStackIndex))
		{
			FScriptArrayHelper Arr(Property, ValuePtr);
			ue_lua_pushvalue(inL, LuaStackIndex);
			int table_len = lua_objlen(inL, -1);
			for (int i = 1; i <= FMath::Max(table_len, Arr.Num()); i++)
			{
				ue_lua_pushinteger(inL, i);
				if (i <= Arr.Num())
					InnerProperty->push_ret_container(inL, Arr.GetRawPtr(i-1));
				else
					ue_lua_pushnil(inL);
				ue_lua_rawset(inL, -3);
			}
		}
		else
		{
			ensureAlwaysMsgf(0, TEXT("not arr"));
			UnrealLua::ReportError(inL, "not arr");
		}
	}
	virtual void pop(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) override
	{
		pop_novirtual(inL, LuaStackIndex, ValuePtr);
	}
	void pop_novirtual(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) 
	{
		if (UnrealLua::IsGlueTArray(inL, LuaStackIndex))
		{
			void* ArrPtr = (void*)tovoid(inL, LuaStackIndex);
			ULuaArrayHelper::GlueArrCopyTo(Property, ArrPtr, ValuePtr);
		}
		else
		{
			ULuaArrayHelper* ArrHelper = (ULuaArrayHelper*)UnrealLua::IsCppPtr(inL, LuaStackIndex);
			if (ArrHelper)
			{
				ArrHelper->CopyTo(Property, ValuePtr);
			}
			else
			{
				ue_lua_pushvalue(inL, LuaStackIndex);
				int32 len = lua_objlen(inL, -1);
				FScriptArrayHelper result(Property, ValuePtr);
				result.Resize(len);
				ue_lua_pushnil(inL);
				while (ue_lua_next(inL, -2))
				{
					int32 i = ue_lua_tointeger(inL, -2) - 1;
					InnerProperty->pop_container(inL, -1, result.GetRawPtr(i));
					ue_lua_pop(inL, 1);
				}
				ue_lua_pop(inL, 1);
			}
		}
	}
	void push_container_novirtual(lua_State* inL, const void* ContainerPtr) 
	{
		push_novirtual(inL, (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr));
	}
	virtual void push_container(lua_State* inL, const void* ContainerPtr) override
	{
		push(inL, (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr));
	}
	virtual void push_ret_container(lua_State* inL, const void* ContainerPtr) override
	{
		push_ret(inL, (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr));
	}
	virtual void push_ref_container(lua_State* inL, int32 LuaStackIndex, const void* ContainerPtr) override
	{
		push_ref(inL, LuaStackIndex, (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr));
	}
	virtual void pop_container(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr) override
	{
		pop_container_novirtual(inL, LuaStackIndex, ContainerPtr);
	}
	void pop_container_novirtual(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr)
	{
		pop_novirtual(inL, LuaStackIndex, (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr));
	}
};
struct LuaUMapProperty :public LuaBasePropertyInterface
{
	TSharedPtr<LuaBasePropertyInterface> KeyProperty;
	TSharedPtr<LuaBasePropertyInterface> ValueProperty;
	int32 KeyOffset;
	int32 KeyPropertySize;
	UMapProperty* Property;
	//Some hook
	virtual UProperty* GetProperty(){return Property;}
	virtual ~LuaUMapProperty(){}
	LuaUMapProperty(lua_State*inL, UMapProperty* InProperty):Property(InProperty)
	{
		KeyProperty = CreatePropertyInterface(inL, Property->KeyProp);
		ValueProperty = CreatePropertyInterface(inL, Property->ValueProp);
		KeyOffset = 0;
		KeyPropertySize = Property->KeyProp->ElementSize * Property->KeyProp->ArrayDim;
	}
	virtual void push(lua_State* inL, const void* ValuePtr) override
	{
		push_novirtual(inL, ValuePtr);
	}
	void push_novirtual(lua_State* inL, const void* ValuePtr) 
	{
		pushstruct_gc(inL, "ULuaMapHelper", ULuaMapHelper::GetHelperCPP_ValuePtr((void*)ValuePtr, Property));
	}
	virtual void push_ret(lua_State* inL, const void* ValuePtr) override
	{
		FScriptMapHelper result(Property, ValuePtr);
		lua_newtable(inL);
		for (int32 i = 0; i < result.Num(); ++i)
		{
			uint8* PairPtr = result.GetPairPtr(i);
			KeyProperty->push_ret_container(inL, PairPtr + KeyOffset);
			ValueProperty->push_ret_container(inL, PairPtr);
			lua_rawset(inL, -3);
		}
	}
	virtual void push_ref(lua_State* inL, int32 LuaStackIndex, const void* ValuePtr) override
	{
		if (UnrealLua::IsGlueTMap(inL, LuaStackIndex))
		{
			void* ArrPtr = (void*)tovoid(inL, LuaStackIndex);
			ULuaMapHelper::GlueMapCopyTo(Property, ValuePtr, ArrPtr);
			ue_lua_pushvalue(inL, LuaStackIndex);
		}
		else if (ULuaMapHelper* Helper = (ULuaMapHelper*)UnrealLua::IsCppPtr(inL, LuaStackIndex))
		{
			Helper->CopyFrom(Property, (void*)ValuePtr);
			ue_lua_pushvalue(inL, LuaStackIndex);
		}
		else if (ue_lua_istable(inL, LuaStackIndex))
		{
			FScriptMapHelper result(Property, ValuePtr);
			UProperty* CurrKeyProp = Property->KeyProp;
			void* KeyStorageSpace = FMemory_Alloca(KeyPropertySize);
			CurrKeyProp->InitializeValue(KeyStorageSpace);

			ue_lua_newtable(inL);
			ue_lua_pushvalue(inL, LuaStackIndex);
			ue_lua_pushnil(inL);
			int i = 1;
			while (ue_lua_next(inL, -2) != 0)
			{
				ue_lua_pop(inL, 1);
				KeyProperty->pop_container(inL, -1, KeyStorageSpace);
				uint8* Result = result.FindValueFromHash(KeyStorageSpace);
				if (Result == nullptr)
				{
					ue_lua_pushvalue(inL, -1);
					ue_lua_rawseti(inL, -4, i);
					i++;
				}
			}
			CurrKeyProp->DestroyValue(KeyStorageSpace);

			ue_lua_pushnil(inL);
			while (ue_lua_next(inL, -3) != 0)
			{
				ue_lua_pushnil(inL);
				ue_lua_rawset(inL, -4);
			}
			ue_lua_remove(inL, -2);
			for (int32 i = 0; i < result.Num(); ++i)
			{
				uint8* PairPtr = result.GetPairPtr(i);
				KeyProperty->push_ret_container(inL, PairPtr + KeyOffset);
				ValueProperty->push_ret_container(inL, PairPtr);
				ue_lua_rawset(inL, -3);
			}
		}
		else
		{
			ensureAlwaysMsgf(0, TEXT("not map"));
			UnrealLua::ReportError(inL, "not map");
		}
	}
	virtual void pop(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) override
	{
		pop_novirtual(inL, LuaStackIndex, ValuePtr);
	}
	void pop_novirtual(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) 
	{
		if (UnrealLua::IsGlueTMap(inL, LuaStackIndex))
		{
			void* Ptr = (void*)tovoid(inL, LuaStackIndex);
			ULuaMapHelper::GlueMapCopyTo(Property, Ptr, ValuePtr);
		}
		else
		{
			ULuaMapHelper* Helper = (ULuaMapHelper*)UnrealLua::IsCppPtr(inL, LuaStackIndex);
			if (Helper)
			{
				Helper->CopyTo(Property, ValuePtr);
			}
			else if (ue_lua_istable(inL, LuaStackIndex))
			{
				ue_lua_pushvalue(inL, LuaStackIndex);
				FScriptMapHelper result(Property, ValuePtr);
				result.EmptyValues();
				ue_lua_pushnil(inL);
				while (ue_lua_next(inL, -2))
				{
					int32 i = result.AddDefaultValue_Invalid_NeedsRehash();
					uint8* PairPtr = result.GetPairPtr(i);
					KeyProperty->pop_container(inL, -2, PairPtr + KeyOffset);
					ValueProperty->pop_container(inL, -1, PairPtr);
					ue_lua_pop(inL, 1);
				}
				result.Rehash();
				ue_lua_pop(inL, 1);
			}
			else
			{
				ensureAlwaysMsgf(0, TEXT("not map"));
			}
		}
	}
	void push_container_novirtual(lua_State* inL, const void* ContainerPtr) 
	{
		push_novirtual(inL, (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr));
	}
	virtual void push_container(lua_State* inL, const void* ContainerPtr) override
	{
		push(inL, (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr));
	}
	virtual void push_ret_container(lua_State* inL, const void* ContainerPtr) override
	{
		push_ret(inL, (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr));
	}
	virtual void push_ref_container(lua_State* inL, int32 LuaStackIndex, const void* ContainerPtr) override
	{
		push_ref(inL, LuaStackIndex, (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr));
	}
	virtual void pop_container(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr) override
	{
		pop_container_novirtual(inL, LuaStackIndex, ContainerPtr);
	}
	void pop_container_novirtual(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr)
	{
		pop_novirtual(inL, LuaStackIndex, (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr));
	}
};
struct LuaUSetProperty :public LuaBasePropertyInterface
{
	TSharedPtr<LuaBasePropertyInterface> ElementProp;
	USetProperty* Property;
	//Some hook
	virtual UProperty* GetProperty(){return Property;}
	virtual ~LuaUSetProperty(){}
	LuaUSetProperty(lua_State*inL, USetProperty* InProperty):Property(InProperty)
	{
		ElementProp = CreatePropertyInterface(inL, Property->ElementProp);
	}
	virtual void push(lua_State* inL, const void* ValuePtr) override
	{
		push_novirtual(inL, ValuePtr);
	}
	void push_novirtual(lua_State* inL, const void* ValuePtr) 
	{
		pushstruct_gc(inL, "ULuaSetHelper", ULuaSetHelper::GetHelperCPP_ValuePtr((void*)ValuePtr, Property));
	}
	virtual void push_ret(lua_State* inL, const void* ValuePtr) override
	{
		FScriptSetHelper result(Property, ValuePtr);
		ue_lua_newtable(inL);
		for (int32 i = 0; i < result.Num(); ++i)
		{
			ElementProp->push_ret_container(inL, result.GetElementPtr(i));
			ue_lua_pushboolean(inL, true);
			ue_lua_rawset(inL, -3);
		}
	}
	virtual void push_ref(lua_State* inL, int32 LuaStackIndex, const void* ValuePtr) override
	{
		if (UnrealLua::IsGlueTSet(inL, LuaStackIndex))
		{
			void* ArrPtr = (void*)tovoid(inL, LuaStackIndex);
			ULuaSetHelper::GlueSetCopyTo(Property, (void*)ValuePtr, ArrPtr);
			ue_lua_pushvalue(inL, LuaStackIndex);
		}
		else if (ULuaSetHelper* Helper = (ULuaSetHelper*)UnrealLua::IsCppPtr(inL, LuaStackIndex))
		{
			Helper->CopyFrom(Property, (void*)ValuePtr);
			ue_lua_pushvalue(inL, LuaStackIndex);
		}
		else if (ue_lua_istable(inL, LuaStackIndex))
		{
			ue_lua_newtable(inL);
			ue_lua_pushvalue(inL, LuaStackIndex);
			ue_lua_pushnil(inL);
			int i = 1;
			FScriptSetHelper result(Property, ValuePtr);
			UProperty* CurrKeyProp = Property->ElementProp;
			const int32 KeyPropertySize = CurrKeyProp->ElementSize * CurrKeyProp->ArrayDim;
			void* KeyStorageSpace = FMemory_Alloca(KeyPropertySize);
			CurrKeyProp->InitializeValue(KeyStorageSpace);

			uint8* keyptr = nullptr;
			int32 Index = result.FindElementIndexFromHash(KeyStorageSpace);
			if (Index != INDEX_NONE)
			{
				keyptr = result.GetElementPtr(Index);
			}
			while (ue_lua_next(inL, -2) != 0)
			{
				ue_lua_pop(inL, 1);
				ElementProp->pop_container(inL, -1, KeyStorageSpace);
				uint8* keyptr = nullptr;
				int32 Index = result.FindElementIndexFromHash(KeyStorageSpace);
				if (Index != INDEX_NONE)
				{
					keyptr = result.GetElementPtr(Index);
				}
				if (keyptr == nullptr)
				{
					ue_lua_pushvalue(inL, -1);
					ue_lua_rawseti(inL, -4, i);
					i++;
				}
			}
			ue_lua_pushnil(inL);
			while (ue_lua_next(inL, -3) != 0)
			{
				ue_lua_pushnil(inL);
				ue_lua_rawset(inL, -4);
			}
			ue_lua_remove(inL, -2);

			for (int32 i = 0; i < result.Num(); ++i)
			{
				ElementProp->push_ret_container(inL, result.GetElementPtr(i));
				ue_lua_pushboolean(inL, true);
				ue_lua_rawset(inL, -3);
			}
		}
		else
		{
			ensureAlwaysMsgf(0, TEXT("not set"));
			UnrealLua::ReportError(inL, "not set");
		}
	}
	virtual void pop(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) override
	{
		pop_novirtual(inL, LuaStackIndex, ValuePtr);
	}
	void pop_novirtual(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) 
	{
		if (UnrealLua::IsGlueTSet(inL, LuaStackIndex))
		{
			void* ArrPtr = (void*)tovoid(inL, LuaStackIndex);
			ULuaSetHelper::GlueSetCopyTo(Property, ArrPtr, (void*)ValuePtr);
		}
		else {
			ULuaSetHelper* Helper = (ULuaSetHelper*)UnrealLua::IsCppPtr(inL, LuaStackIndex);
			if (Helper)
			{
				Helper->CopyTo(Property, (void*)ValuePtr);
			}
			else if (ue_lua_istable(inL, LuaStackIndex))
			{
				ue_lua_pushvalue(inL, LuaStackIndex);
				FScriptSetHelper result(Property, ValuePtr);
				result.EmptyElements();
				ue_lua_pushnil(inL);
				while (ue_lua_next(inL, -2))
				{
					int32 i = result.AddDefaultValue_Invalid_NeedsRehash();
					uint8* ElementPtr = result.GetElementPtr(i);
					ElementProp->pop_container(inL, -2, ElementPtr);
					ue_lua_pop(inL, 1);
				}
				result.Rehash();
				ue_lua_pop(inL, 1);
			}
			else
			{
				ensureAlwaysMsgf(0, TEXT("not set"));
			}
		}
	}
	void push_container_novirtual(lua_State* inL, const void* ContainerPtr) 
	{
		push_novirtual(inL, (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr));
	}
	virtual void push_container(lua_State* inL, const void* ContainerPtr) override
	{
		push(inL, (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr));
	}
	virtual void push_ret_container(lua_State* inL, const void* ContainerPtr) override
	{
		push_ret(inL, (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr));
	}
	virtual void push_ref_container(lua_State* inL, int32 LuaStackIndex, const void* ContainerPtr) override
	{
		push_ref(inL, LuaStackIndex, (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr));
	}
	virtual void pop_container(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr) override
	{
		pop_container_novirtual(inL, LuaStackIndex, ContainerPtr);
	}
	void pop_container_novirtual(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr)
	{
		pop_novirtual(inL, LuaStackIndex, (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr));
	}
};
struct LuaUInterfaceProperty :public LuaBasePropertyInterface
{
	UInterfaceProperty* Property;
	//Some hook
	virtual UProperty* GetProperty(){return Property;}
	virtual ~LuaUInterfaceProperty(){}
	LuaUInterfaceProperty(lua_State*inL, UInterfaceProperty* InProperty):Property(InProperty)
	{
		
	}
	virtual void push(lua_State* inL, const void* ValuePtr) override
	{
		push_novirtual(inL, ValuePtr);
	}
	void push_novirtual(lua_State* inL, const void* ValuePtr) 
	{
		FScriptInterface* result = (FScriptInterface*)Property->GetPropertyValuePtr(ValuePtr);
		pushuobject(inL, (void*)result->GetObject());
	}
	virtual void push_ret(lua_State* inL, const void* ValuePtr) override
	{
		FScriptInterface* result = (FScriptInterface*)Property->GetPropertyValuePtr(ValuePtr);
		pushuobject(inL, (void*)result->GetObject());
	}
	virtual void push_ref(lua_State* inL, int32 LuaStackIndex, const void* ValuePtr) override
	{
		FScriptInterface* result = (FScriptInterface*)Property->GetPropertyValuePtr(ValuePtr);
		pushuobject(inL, (void*)result->GetObject());
	}
	virtual void pop(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) override
	{
		pop_novirtual(inL, LuaStackIndex, ValuePtr);
	}
	void pop_novirtual(lua_State* inL, int32 LuaStackIndex, void* ValuePtr) 
	{
		FScriptInterface* result = (FScriptInterface*)Property->GetPropertyValuePtr(ValuePtr);
		UObject* value = (UObject*)touobject(inL, LuaStackIndex);
		result->SetObject(value);
	}
	void push_container_novirtual(lua_State* inL, const void* ContainerPtr) 
	{
		push_novirtual(inL, (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr));
	}
	virtual void push_container(lua_State* inL, const void* ContainerPtr) override
	{
		push(inL, (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr));
	}
	virtual void push_ret_container(lua_State* inL, const void* ContainerPtr) override
	{
		push_ret(inL, (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr));
	}
	virtual void push_ref_container(lua_State* inL, int32 LuaStackIndex, const void* ContainerPtr) override
	{
		push_ref(inL, LuaStackIndex, (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr));
	}
	virtual void pop_container(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr) override
	{
		pop_container_novirtual(inL, LuaStackIndex, ContainerPtr);
	}
	void pop_container_novirtual(lua_State* inL, int32 LuaStackIndex, const void * ContainerPtr)
	{
		pop_novirtual(inL, LuaStackIndex, (void*)Property->ContainerPtrToValuePtr<uint8>(ContainerPtr));
	}
};

#define NEWPROPERTY_INTERFACE(Type)\
	static LuaBasePropertyInterface* NewPropertyInterfaceBy##Type(lua_State*inL, UProperty* Property)\
	{\
		return new Lua##Type(inL, (Type*)Property);\
	}


NEWPROPERTY_INTERFACE(UBoolProperty)
NEWPROPERTY_INTERFACE(UIntProperty)
NEWPROPERTY_INTERFACE(UUInt16Property)
NEWPROPERTY_INTERFACE(UInt16Property)
NEWPROPERTY_INTERFACE(UUInt32Property)
NEWPROPERTY_INTERFACE(UInt64Property)
NEWPROPERTY_INTERFACE(UUInt64Property)
NEWPROPERTY_INTERFACE(UFloatProperty)
NEWPROPERTY_INTERFACE(UDoubleProperty)
NEWPROPERTY_INTERFACE(UObjectPropertyBase)
NEWPROPERTY_INTERFACE(UObjectProperty)
NEWPROPERTY_INTERFACE(UClassProperty)
NEWPROPERTY_INTERFACE(UStrProperty)
NEWPROPERTY_INTERFACE(UNameProperty)
NEWPROPERTY_INTERFACE(UTextProperty)
NEWPROPERTY_INTERFACE(UByteProperty)
NEWPROPERTY_INTERFACE(UEnumProperty)
NEWPROPERTY_INTERFACE(UStructProperty)
NEWPROPERTY_INTERFACE(UMulticastDelegateProperty)
NEWPROPERTY_INTERFACE(UDelegateProperty)
NEWPROPERTY_INTERFACE(UWeakObjectProperty)
NEWPROPERTY_INTERFACE(UArrayProperty)
NEWPROPERTY_INTERFACE(UMapProperty)
NEWPROPERTY_INTERFACE(USetProperty)
NEWPROPERTY_INTERFACE(UInterfaceProperty)

#define MAP_CREATEFUNC_TOTYPE(Type) TheMap.Add(Type::StaticClass(), NewPropertyInterfaceBy##Type);\

static TMap<UClass*, TFunction< LuaBasePropertyInterface*(lua_State*, UProperty*)> > GetCreateFuncMap()
{
	TMap <UClass*, TFunction< LuaBasePropertyInterface*(lua_State*, UProperty*)>> TheMap;

	MAP_CREATEFUNC_TOTYPE(UBoolProperty)
		MAP_CREATEFUNC_TOTYPE(UIntProperty)
		MAP_CREATEFUNC_TOTYPE(UUInt16Property)
		MAP_CREATEFUNC_TOTYPE(UInt16Property)
		MAP_CREATEFUNC_TOTYPE(UUInt32Property)
		MAP_CREATEFUNC_TOTYPE(UInt64Property)
		MAP_CREATEFUNC_TOTYPE(UUInt64Property)
		MAP_CREATEFUNC_TOTYPE(UFloatProperty)
		MAP_CREATEFUNC_TOTYPE(UDoubleProperty)
		MAP_CREATEFUNC_TOTYPE(UObjectPropertyBase)
		MAP_CREATEFUNC_TOTYPE(UObjectProperty)
		MAP_CREATEFUNC_TOTYPE(UClassProperty)
		MAP_CREATEFUNC_TOTYPE(UStrProperty)
		MAP_CREATEFUNC_TOTYPE(UNameProperty)
		MAP_CREATEFUNC_TOTYPE(UTextProperty)
		MAP_CREATEFUNC_TOTYPE(UByteProperty)
		MAP_CREATEFUNC_TOTYPE(UEnumProperty)
		MAP_CREATEFUNC_TOTYPE(UStructProperty)
		MAP_CREATEFUNC_TOTYPE(UMulticastDelegateProperty)
		MAP_CREATEFUNC_TOTYPE(UDelegateProperty)
		MAP_CREATEFUNC_TOTYPE(UWeakObjectProperty)
		MAP_CREATEFUNC_TOTYPE(UArrayProperty)
		MAP_CREATEFUNC_TOTYPE(UMapProperty)
		MAP_CREATEFUNC_TOTYPE(USetProperty)
		MAP_CREATEFUNC_TOTYPE(UInterfaceProperty)

		return TheMap;
}

static LuaBasePropertyInterface* CreatePropertyInterfaceRaw(lua_State*inL, UProperty* Property)
{
	static TMap<UClass*, TFunction< LuaBasePropertyInterface*(lua_State*, UProperty*)>> CreateFuncs = GetCreateFuncMap();
	if (auto* ProcessFunc = CreateFuncs.Find(Property->GetClass()))
	{
		return (*ProcessFunc)(inL, Property);
	}
	else
	{
		if (UStructProperty* p = Cast<UStructProperty>(Property))
		{
			return NewPropertyInterfaceByUStructProperty(inL, p);
		}

		else if (UObjectPropertyBase* p = Cast<UObjectPropertyBase>(Property))
		{
			return NewPropertyInterfaceByUObjectPropertyBase(inL, p);
		}
		else
		{
			ensureAlwaysMsgf(0, TEXT("Some type didn't process"));
		}
	}
	return nullptr;
}

TSharedPtr<LuaBasePropertyInterface> CreatePropertyInterface(lua_State*inL, UProperty* Property)
{
	return MakeShareable(CreatePropertyInterfaceRaw(inL, Property));
}

struct LuaUFunctionInterface : public LuaBaseBpInterface
{
	~LuaUFunctionInterface(){}
	LuaUFunctionInterface(lua_State*inL, UFunction* Function)
	{
		IsStatic = (Function->FunctionFlags & FUNC_Static)!=0;
		TheFunc = Function;
		int32 ArgIndex;
		if (IsStatic) 
		{
			ArgIndex = 1;
			DefaultObj = ((UClass*)Function->GetOuter())->GetDefaultObject();
		}
		else
		{
			ArgIndex = 2;
		}
		StartIndex = ArgIndex;
		for (TFieldIterator<UProperty> It(Function); It && (It->GetPropertyFlags() & (CPF_Parm)); ++It)
		{
			auto Prop = *It;
			bool HasAdd = false;
			TSharedPtr<LuaBasePropertyInterface> PropInterface = CreatePropertyInterface(inL, Prop);
			InitAndDestroyParams.Add(PropInterface);
			if ((Prop->GetPropertyFlags() & (CPF_ReturnParm)))
			{
				ReturnValues.Insert(PropInterface, 0);
				continue;
			}
			if (!HasAdd && Prop->GetPropertyFlags() & CPF_ReferenceParm)
			{
				HasAdd = true;
				RefParams.Add(PropInterface);
				StackIndexs.Add(ArgIndex);
			}

			if (!HasAdd && Prop->GetPropertyFlags() & CPF_OutParm)
			{
				if (Function->HasAnyFunctionFlags(FUNC_Native))
				{
					RefParams.Add(PropInterface);
					StackIndexs.Add(ArgIndex);
				}
				else
				{
					ReturnValues.Insert(CreatePropertyInterface(inL, Prop), 0);
					continue;
				}
			}
			Params.Add(PropInterface);
			++ArgIndex;

		}
		ReturnCount = ReturnValues.Num() + RefParams.Num();
	}

	int32 GetBufferSize()
	{
		return TheFunc->ParmsSize;
	}

	void InitBuffer(uint8* Buffer)
	{
		for (auto& Itf : InitAndDestroyParams)
		{
			Itf->GetProperty()->InitializeValue_InContainer(Buffer);
		}
	}

	void DestroyBuffer(uint8* Buffer)
	{
		for (auto& Itf : InitAndDestroyParams)
		{
			Itf->GetProperty()->DestroyValue_InContainer(Buffer);
		}
	}

	bool BuildTheBuffer(lua_State*inL, uint8* Buffer)
	{	
		int32 LuaTop = ue_lua_gettop(inL);
#if LuaDebug
		if (Params.Num() + StartIndex - 1 > LuaTop)
		{
			ensureAlwaysMsgf(0, TEXT("arguments is not enough"));
			return false;
		}
#endif
		for (int i = 0; i < Params.Num()  ; i++)
		{
			Params[i]->pop_container(inL, StartIndex + i, Buffer);
		}
		return true;
	}

	bool Call(lua_State*inL, uint8* Buffer, UObject* Ptr = nullptr)
	{
		if (Ptr == nullptr)
		{
			if (IsStatic)
			{
#if LuaDebug
				if (DefaultObj == nullptr)
				{
					ensureAlwaysMsgf(0, TEXT("Bug"));
					return false;
				}
#endif
				Ptr = DefaultObj;
			}
			else
			{
				Ptr = (UObject*)touobject(inL, 1);
			}
		}
		if (Ptr)
			Ptr->ProcessEvent(TheFunc, Buffer);
		else 
		{
			ensureAlwaysMsgf(0, TEXT("Ptr Can't be null"));
			return false;
		}
		return true;
	}

	int32 PushRet(lua_State*inL, uint8* Buffer)
	{
		for (auto& Itf : ReturnValues)
		{
			Itf->push_ret_container(inL, Buffer);
		}
		for (int i = 0; i < RefParams.Num(); i++)
		{
			RefParams[i]->push_ref_container(inL, StackIndexs[i], Buffer);
		}
		return ReturnCount;
	}
	
	bool IsStatic;
	UObject* DefaultObj;
	UFunction* TheFunc;
	TArray<int32> StackIndexs;
	int32 StartIndex;
	TArray<TSharedPtr<LuaBasePropertyInterface>> Params;
	TArray<TSharedPtr<LuaBasePropertyInterface>> InitAndDestroyParams;
	TArray<TSharedPtr<LuaBasePropertyInterface>> ReturnValues;
	TArray<TSharedPtr<LuaBasePropertyInterface>> RefParams;
	int32 ReturnCount;
};

struct MuldelegateBpInterface
{
	bool HasReturn;
	int ParamCount;
	TSharedPtr<LuaBasePropertyInterface> ReturnValue;
	TArray<TSharedPtr<LuaBasePropertyInterface>> Params;
	MuldelegateBpInterface(UFunction* Function)
	{
		HasReturn = false;
		for (TFieldIterator<UProperty> ParamIt(Function); ParamIt; ++ParamIt)
		{
			UProperty* Param = *ParamIt;
			if (Param->GetName() != "ReturnValue")
			{
				Params.Add(CreatePropertyInterface(nullptr, Param));
			}
			else
			{
				ReturnValue = CreatePropertyInterface(nullptr, Param);
				HasReturn = true;
			}
		}
		ParamCount = Params.Num();
	}
	void Call(lua_State*inL, void* Buffer)
	{
		for (auto& Itf : Params)
		{
			Itf->push_container(inL, Buffer);
		}
		if (lua_pcall(inL, ParamCount, HasReturn ? 1 : 0, 0))
		{
#if LuaDebug
			FString error = lua_tostring(inL, -1);
			ensureAlwaysMsgf(0, *error);
			UnrealLua::ReportError(inL, error);
#endif
			UTableUtil::log(lua_tostring(inL, -1));
		}

		if (HasReturn)
			ReturnValue->pop_container(inL, -1, Buffer);
	}

};