// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "LuaEditorPlugins.h"
#include "LuaEditorPluginsStyle.h"
#include "LuaEditorPluginsCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "UnrealLua.h"
#include "LuaGlue/EditorMenuGlue.h"

static const FName LuaEditorPluginsTabName("LuaEditorPlugins");

#define LOCTEXT_NAMESPACE "FLuaEditorPluginsModule"
void FLuaEditorPluginsModule::OnPreExit()
{
	TheLuaState->OnPreExit();
	TheLuaState = nullptr;
	LuaContextMgr::Get().ShutdownLuaStateByVoidPtr(this);
}

void FLuaEditorPluginsModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	LuaOnPreExit = FCoreDelegates::OnPreExit.AddRaw(this, &FLuaEditorPluginsModule::OnPreExit);

	TheLuaState = LuaContextMgr::Get().GetLuaStateByVoidPtr(this);
	TheLuaState->CreateEditorTick();
	FLuaEditorPluginsStyle::Initialize();
	FLuaEditorPluginsStyle::ReloadTextures();

	FLuaEditorPluginsCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FLuaEditorPluginsCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FLuaEditorPluginsModule::PluginButtonClicked),
		FCanExecuteAction());
		
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	
	{
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
		MenuExtender->AddMenuExtension("WindowLayout", EExtensionHook::After, PluginCommands, FMenuExtensionDelegate::CreateRaw(this, &FLuaEditorPluginsModule::AddMenuExtension));

		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	}
	
	{
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FLuaEditorPluginsModule::AddToolbarExtension));
		
		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(LuaEditorPluginsTabName, FOnSpawnTab::CreateRaw(this, &FLuaEditorPluginsModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FLuaEditorPluginsTabTitle", "LuaEditorPlugins"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FLuaEditorPluginsModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	FLuaEditorPluginsStyle::Shutdown();

	FLuaEditorPluginsCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(LuaEditorPluginsTabName);
}

TSharedRef<SDockTab> FLuaEditorPluginsModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
		FText::FromString(TEXT("FLuaEditorPluginsModule::OnSpawnPluginTab")),
		FText::FromString(TEXT("LuaEditorPlugins.cpp"))
		);
	UTableUtil::DoString(TheLuaState->TheContextLuaState, "require \"editor.editormain\"");
	TSharedRef<SDockTab> Result = UTableUtil::callr_with_state<TSharedRef<SDockTab>>(TheLuaState->TheContextLuaState, "SomeSimpleExampleForEditor");
	return Result;
	
	static TSharedRef<SDockTab> Result1 = 
	SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Put your tab content here!
			SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(WidgetText)
			]
		];
	return Result1;
}

void FLuaEditorPluginsModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->InvokeTab(LuaEditorPluginsTabName);
}

void FLuaEditorPluginsModule::AddMenuExtension(FMenuBuilder& Builder)
{
	Builder.AddMenuEntry(FLuaEditorPluginsCommands::Get().OpenPluginWindow);
}

void FLuaEditorPluginsModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
	Builder.AddToolBarButton(FLuaEditorPluginsCommands::Get().OpenPluginWindow);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FLuaEditorPluginsModule, LuaEditorPlugins)