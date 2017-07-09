// Fill out your copyright notice in the Description page of Project Settings.

#include "LuaFactory.h"
#include "Misc/FileHelper.h"
#include "LuaScript.h"


ULuaFactory::ULuaFactory()
{
	SupportedClass = ULuaScript::StaticClass();

	bCreateNew = false;
	bEditorImport = true;
	bText = true;
	bEditAfterNew = true;

	Formats.Add(TEXT("lua;lua script file"));
}

bool ULuaFactory::FactoryCanImport(const FString& Filename)
{
	return true;
// 	FString FileContent;
// 	if (FFileHelper::LoadFileToString(/*out*/ FileContent, *Filename))
// 	{
// 	}
// 	return true;
}

UObject* ULuaFactory::FactoryCreateText(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, const TCHAR* Type, const TCHAR*& Buffer, const TCHAR* BufferEnd, FFeedbackContext* Warn)
{
	ULuaScript* NewCode = NewObject<ULuaScript>(InParent, InName, Flags);
	NewCode->Code = FString(BufferEnd - Buffer, Buffer);
	return NewCode;
}
