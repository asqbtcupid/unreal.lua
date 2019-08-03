// Fill out your copyright notice in the Description page of Project Settings.

#include "LuaFactory.h"
#include "Misc/FileHelper.h"
#include "LuaScript.h"
#include "Paths.h"


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
}

UObject* ULuaFactory::FactoryCreateText(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, const TCHAR* Type, const TCHAR*& Buffer, const TCHAR* BufferEnd, FFeedbackContext* Warn)
{
	ULuaScript* NewCode = NewObject<ULuaScript>(InParent, InName, Flags);
	NewCode->Code = FString(BufferEnd - Buffer, Buffer);
	NewCode->ImportPath = GetCurrentFilename();
	FPaths::MakePathRelativeTo(NewCode->ImportPath, *FPaths::ProjectDir());
	return NewCode;
}

bool ULuaFactory::CanReimport(UObject* Obj, TArray<FString>& OutFilenames)
{
	if(Cast<ULuaScript>(Obj))
		return true;
	return false;
}

void ULuaFactory::SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths)
{

}

EReimportResult::Type ULuaFactory::Reimport(UObject* Obj)
{
	ULuaScript* ScriptObj = Cast<ULuaScript>(Obj);
	if (ScriptObj)
	{
		FString FilePath = FPaths::ProjectDir() / ScriptObj->ImportPath;
		FString Code;
		FFileHelper::LoadFileToString(Code, *FilePath);
		if (Code != ScriptObj->Code)
		{
			ScriptObj->Code = Code;
			ScriptObj->MarkPackageDirty();
			return EReimportResult::Type::Succeeded;
		}
		else
			return EReimportResult::Type::Cancelled;
	}
	return EReimportResult::Type::Failed;

}
