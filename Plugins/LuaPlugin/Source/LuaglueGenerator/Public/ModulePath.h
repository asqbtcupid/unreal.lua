// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "coreminimal.h"

class FModulePath 
{
public:
	static FModulePath& Get() 
	{
		static FModulePath Ins;
		return Ins;
	};
	TArray<FString> ModuleNames;
	TSet<FString> DisallowedHeaderNames;
	TMap<FString, FString> ModuleName2ModulePath;
	void Init();
	FString GetClassHeaderPath(UField* Filed);
	FString GetClassModuleName(UField* Filed);
	FString GetModulePathByName(FString ModuleName);
	void FindRootFilesRecursive(TArray<FString> &FileNames, const FString &BaseDirectory, const FString &Wildcard);
};

