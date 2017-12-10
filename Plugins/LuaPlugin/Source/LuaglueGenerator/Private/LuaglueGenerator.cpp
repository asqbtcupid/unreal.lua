// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#include "LuaglueGeneratorPrivatePCH.h"
#include "ScriptCodeGeneratorBase.h"
#include "LuaScriptCodeGenerator.h"
#include "IProjectManager.h"
#include "Runtime/Core/Public/Features/IModularFeatures.h"
#include "ProjectDescriptor.h"
#include "ModulePath.h"
DEFINE_LOG_CATEGORY(LogScriptGenerator);

FString GameModuleName;
FString luaconfiginiPath;
class FLuaglueGenerator : public ILuaglueGenerator
{
	/** Specialized script code generator */
	TAutoPtr<FLuaScriptCodeGenerator> CodeGenerator;

	TArray<FString> SupportModules;
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/** ILuaglueGenerator interface */
	// 	virtual FString GetGeneratedCodeModuleName() const override { return GameModuleName; }
	virtual FString GetGeneratedCodeModuleName() const override { return GameModuleName; }
	virtual bool ShouldExportClassesForModule(const FString& ModuleName, EBuildModuleType::Type ModuleType, const FString& ModuleGeneratedIncludeDirectory) const override;
	virtual bool SupportsTarget(const FString& TargetName) const override;
	virtual void Initialize(const FString& RootLocalPath, const FString& RootBuildPath, const FString& OutputDirectory, const FString& IncludeBase) override;
	virtual void ExportClass(UClass* Class, const FString& SourceHeaderFilename, const FString& GeneratedHeaderFilename, bool bHasChanged) override;
	virtual void FinishExport() override;
	virtual FString GetGeneratorName() const override;
};

IMPLEMENT_MODULE( FLuaglueGenerator, LuaglueGenerator )


void FLuaglueGenerator::StartupModule()
{
	// Register ourselves as an editor feature
	IModularFeatures::Get().RegisterModularFeature(TEXT("ScriptGenerator"), this);
}

void FLuaglueGenerator::ShutdownModule()
{
	CodeGenerator.Reset();

	// Unregister our feature
	IModularFeatures::Get().UnregisterModularFeature(TEXT("ScriptGenerator"), this);
}

FString FLuaglueGenerator::GetGeneratorName() const
{
	return TEXT("Lua Code Generator Plugin");
}

void FLuaglueGenerator::Initialize(const FString& RootLocalPath, const FString& RootBuildPath, const FString& OutputDirectory, const FString& IncludeBase)
{
	UE_LOG(LogScriptGenerator, Log, TEXT("Using Lua Script Generator."));
	GConfig->GetArray(TEXT("Lua"), TEXT("SupportedModules"), SupportModules, luaconfiginiPath);
	CodeGenerator = new FLuaScriptCodeGenerator(RootLocalPath, RootBuildPath, OutputDirectory, IncludeBase, luaconfiginiPath);
	CodeGenerator->GameModuleName = GameModuleName;
	CodeGenerator->SupportModules = SupportModules;
}

bool FLuaglueGenerator::ShouldExportClassesForModule(const FString& ModuleName, EBuildModuleType::Type ModuleType, const FString& ModuleGeneratedIncludeDirectory) const
{ 
	return SupportModules.Contains(ModuleName);
}

void FLuaglueGenerator::ExportClass(UClass* Class, const FString& SourceHeaderFilename, const FString& GeneratedHeaderFilename, bool bHasChanged)
{
	CodeGenerator->ExportClass(Class, SourceHeaderFilename, GeneratedHeaderFilename, bHasChanged);
}

void FLuaglueGenerator::FinishExport()
{
	CodeGenerator->FinishExport();
}

bool FLuaglueGenerator::SupportsTarget(const FString& TargetName) const
{
	if (FPaths::IsProjectFilePathSet())
	{
		luaconfiginiPath = FPaths::GetPath(FPaths::GetProjectFilePath()) /"Config"/ "luaconfig.ini";
		FConfigFile* File = GConfig->Find(luaconfiginiPath, false);
		if (File == nullptr)
		{
			luaconfiginiPath = FPaths::GetPath(FPaths::GetProjectFilePath()) /"Plugins"/ "LuaPlugin" / "Config" / "luaconfig.ini";
			File = GConfig->Find(luaconfiginiPath, false);
		}

		if (File != nullptr)
		{
			FModulePath::Get().Init();
			GConfig->GetString(TEXT("Lua"), TEXT("GameModuleName"), GameModuleName, luaconfiginiPath);
			if (GameModuleName.IsEmpty())
			{
				FProjectDescriptor ProjectDescriptor;
				FText OutError;
				ProjectDescriptor.Load(FPaths::GetProjectFilePath(), OutError);
				GameModuleName = ProjectDescriptor.Modules[0].Name.ToString();
			}
			return true;
		}
	}
	return false;
}