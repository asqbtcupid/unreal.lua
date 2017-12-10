// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#include "ModulePath.h"
#include "LuaglueGeneratorPrivatePCH.h"
#include "FileManager.h"
#include "Paths.h"
#include "Package.h"
#include "UObjectIterator.h"
#include "MetaData.h"


// TArray<FString> FModulePath::ModuleNames;
// TSet<FString> FModulePath::DisallowedHeaderNames;
// TMap<FString, FString> FModulePath::ModuleName2ModulePath;

void FModulePath::Init()
{
	ModuleNames.Reset();
	DisallowedHeaderNames.Empty();

	// Find all the build rules within the game and engine directories
	FindRootFilesRecursive(ModuleNames, *(FPaths::EngineDir() / TEXT("Source") / TEXT("Developer")), TEXT("*.Build.cs"));
	FindRootFilesRecursive(ModuleNames, *(FPaths::EngineDir() / TEXT("Source") / TEXT("Editor")), TEXT("*.Build.cs"));
	FindRootFilesRecursive(ModuleNames, *(FPaths::EngineDir() / TEXT("Source") / TEXT("Runtime")), TEXT("*.Build.cs"));
	FindRootFilesRecursive(ModuleNames, *(FPaths::GetPath(FPaths::GetProjectFilePath()) / TEXT("Source")), TEXT("*.Build.cs"));

	// Find list of disallowed header names in native (non-plugin) directories
	TArray<FString> HeaderFiles;
	for (const FString& ModuleName : ModuleNames)
	{
		IFileManager::Get().FindFilesRecursive(HeaderFiles, *(FPaths::GetPath(ModuleName) / TEXT("Classes")), TEXT("*.h"), true, false, false);
		IFileManager::Get().FindFilesRecursive(HeaderFiles, *(FPaths::GetPath(ModuleName) / TEXT("Public")), TEXT("*.h"), true, false, false);
	}

	for (const FString& HeaderFile : HeaderFiles)
	{
		DisallowedHeaderNames.Add(FPaths::GetBaseFilename(HeaderFile));
	}

	for (TObjectIterator<UClass> ClassIt; ClassIt; ++ClassIt)
	{
		DisallowedHeaderNames.Remove(ClassIt->GetName());
	}

	// Find all the plugin directories
	TArray<FString> PluginNames;

	FindRootFilesRecursive(PluginNames, *(FPaths::EngineDir() / TEXT("Plugins")), TEXT("*.uplugin"));
	FindRootFilesRecursive(PluginNames, *(FPaths::GetPath(FPaths::GetProjectFilePath()) / TEXT("Plugins")), TEXT("*.uplugin"));

	// Add all the files within plugin directories
	for (const FString& PluginName : PluginNames)
	{
		FindRootFilesRecursive(ModuleNames, *(FPaths::GetPath(PluginName) / TEXT("Source")), TEXT("*.Build.cs"));
	}
}

FString FModulePath::GetClassHeaderPath(UField* Field)
{
	UPackage *ModulePackage = Field->GetTypedOuter<UPackage>();
	FString RelativePath = ModulePackage->GetMetaData()->GetValue(Field, TEXT("ModuleRelativePath"));
	if (RelativePath.IsEmpty())
		return "";

	FString ModulePackageName = ModulePackage->GetName();
	int32 ModuleNameIdx;
	if (ModulePackageName.FindLastChar(TEXT('/'), ModuleNameIdx))
		ModulePackageName = ModulePackageName.Mid(ModuleNameIdx + 1);
	if (ModuleName2ModulePath.Find(ModulePackageName) == nullptr)
		return "";
	FString Path = ModuleName2ModulePath[ModulePackageName] / RelativePath;
	return Path;
}


FString FModulePath::GetClassModuleName(UField* Filed)
{
	UPackage *ModulePackage = Filed->GetTypedOuter<UPackage>();
	FString ModulePackageName = ModulePackage->GetName();
	int32 ModuleNameIdx;
	if (ModulePackageName.FindLastChar(TEXT('/'), ModuleNameIdx))
		return ModulePackageName.Mid(ModuleNameIdx + 1);
	return "";
}


FString FModulePath::GetModulePathByName(FString ModuleName)
{
	if (auto p = ModuleName2ModulePath.Find(ModuleName))
		return *p;
	return "";
}

void FModulePath::FindRootFilesRecursive(TArray<FString> &FileNames, const FString &BaseDirectory, const FString &Wildcard)
{
	TArray<FString> BasedFileNames;
	IFileManager::Get().FindFiles(BasedFileNames, *(BaseDirectory / Wildcard), true, false);

	// Append to the result if we have any, otherwise recurse deeper
	if (BasedFileNames.Num() == 0)
	{
		TArray<FString> DirectoryNames;
		IFileManager::Get().FindFiles(DirectoryNames, *(BaseDirectory / TEXT("*")), false, true);

		for (int32 Idx = 0; Idx < DirectoryNames.Num(); Idx++)
		{
			FindRootFilesRecursive(FileNames, BaseDirectory / DirectoryNames[Idx], Wildcard);
		}
	}
	else
	{
		for (int32 Idx = 0; Idx < BasedFileNames.Num(); Idx++)
		{
			FString Path = FPaths::ConvertRelativePathToFull(BaseDirectory / BasedFileNames[Idx]);
			FileNames.Add(Path);
			ModuleName2ModulePath.Add(FPaths::GetBaseFilename(FPaths::GetBaseFilename(Path)), FPaths::GetPath(Path));
		}
	}
}
