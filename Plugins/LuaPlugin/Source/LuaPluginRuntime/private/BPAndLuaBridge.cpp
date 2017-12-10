// Fill out your copyright notice in the Description page of Project Settings.

#include "BPAndLuaBridge.h"
#include "TableUtil.h"


bool UBPAndLuaBridge::IsBpIns(UObject *p)
{
	return p->GetClass()->HasAnyClassFlags(CLASS_CompiledFromBlueprint);
}

TMap<FString, UObject*> UBPAndLuaBridge::GetClassBPFunctions(UObject *p)
{
	TMap<FString, UObject*> Result;
	if (p)
	{
		UClass* Class = p->GetClass();
		if (Class->HasAnyClassFlags(CLASS_CompiledFromBlueprint))
		{
			for (TFieldIterator<UFunction> FuncIt(Class); FuncIt; ++FuncIt)
			{
				UFunction* Function = *FuncIt;
				if (!Function->HasAnyFunctionFlags(FUNC_Native))
				{
					Result.Add(Function->GetName(), Function);
				}
			}
		}
	}
	return Result;
}


TMap<FString, UObject*> UBPAndLuaBridge::GetClassBPPropertys(UObject *p)
{
	TMap<FString, UObject*> Result;
	if (p)
	{
		UClass* Class = p->GetClass();
		if (Class->HasAnyClassFlags(CLASS_CompiledFromBlueprint))
		{
			for (TFieldIterator<UProperty> PropertyIt(Class); PropertyIt; ++PropertyIt)
			{
				UProperty* Property = *PropertyIt;
				if (UClass* OuterClass = Cast<UClass>(Property->GetOuter()))
				{
					if (OuterClass->HasAnyClassFlags(CLASS_CompiledFromBlueprint))
						Result.Add(Property->GetName(), Property);
				}
			}

		}
	}
	return Result;
}


int32 UBPAndLuaBridge::CallBlueprintFunction(Flua_State inL, UObject* funcsig, UObject* Obj)
{
	UFunction* Function = (UFunction*)funcsig;
	uint8* Buffer = (uint8*)FMemory_Alloca(Function->ParmsSize);
	FScopedArguments scoped_arguments(Function, Buffer);
	
	int ArgIndex = 4;
	int ArgCount = lua_gettop(inL);

	// Iterate over input parameters
	TArray<UProperty*> PushBackParms;
	for (TFieldIterator<UProperty> It(Function); It && (It->GetPropertyFlags() & (CPF_Parm)); ++It)
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
		if (ArgIndex <= ArgCount)
		{
			UTableUtil::popproperty(inL, ArgIndex, Prop, Buffer);
			++ArgIndex;
		}
	}
	Obj->ProcessEvent(Function, Buffer);
	for (UProperty* Prop : PushBackParms)
	{
		UTableUtil::pushproperty(inL, Prop, Buffer);
	}

	return PushBackParms.Num();
}


int32 UBPAndLuaBridge::GetBlueprintProperty(Flua_State inL, UObject *Property, UObject* Obj)
{
	UTableUtil::pushproperty(inL.TheState, (UProperty*)Property, Obj);
	return 1;
}

void UBPAndLuaBridge::SetBlueprintProperty(Flua_State inL, UObject *Property, UObject* Obj, Flua_Index Index)
{
	UTableUtil::popproperty(inL.TheState, Index, (UProperty*)Property, Obj);
}




