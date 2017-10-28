// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "VarWatcher.h"
#include "VarWatcherStyle.h"
#include "VarWatcherCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"

static const FName VarWatcherTabName("VarWatcher");

#define LOCTEXT_NAMESPACE "FVarWatcherModule"


FVarWatcherModule* FVarWatcherModule::Ptr = nullptr;

TSharedRef<ITableRow> FVarWatcherModule::HandleVarTreeGenerateRow(TSharedRef<FVarNode> InVarNode, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(SVarTreeWidgetItem, OwnerTable)
		.VarInfoToVisualize(InVarNode)
		;
}


void FVarWatcherModule::HandleVarTreeGetChildren(TSharedRef<FVarNode> InVarNode, TArray<TSharedRef<FVarNode>>& OutChildren)
{
	InVarNode->GetChild(OutChildren);
}


void FVarWatcherModule::HandleVarTreeSelectionChanged(TSharedPtr<FVarNode>, ESelectInfo::Type /*SelectInfo*/)
{
	bNeedUpdateData = true;
}


void FVarWatcherModule::HandleNodeExpansion(TSharedRef<FVarNode> VarNode, bool bIsExpaned)
{
	VarNode->bNeedExpandNextUpdate = bIsExpaned;
	bNeedUpdateData = true;
}

void FVarWatcherModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	Ptr = this;
	FVarWatcherStyle::Initialize();
	FVarWatcherStyle::ReloadTextures();

	FVarWatcherCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FVarWatcherCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FVarWatcherModule::PluginButtonClicked),
		FCanExecuteAction());
		
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	
	{
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
		MenuExtender->AddMenuExtension("WindowLayout", EExtensionHook::After, PluginCommands, FMenuExtensionDelegate::CreateRaw(this, &FVarWatcherModule::AddMenuExtension));

		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	}
	
	{
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FVarWatcherModule::AddToolbarExtension));
		
		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(VarWatcherTabName, FOnSpawnTab::CreateRaw(this, &FVarWatcherModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FVarWatcherTabTitle", "VarWatcher"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);

	auto TickDelegate = FTickerDelegate::CreateLambda(
		[&](float DeltaTime)->bool {this->Tick(DeltaTime); return true; }
	);
	FTicker::GetCoreTicker().AddTicker(TickDelegate, 0);
}

void FVarWatcherModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FVarWatcherStyle::Shutdown();

	FVarWatcherCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(VarWatcherTabName);
}

TSharedRef<SDockTab> FVarWatcherModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	bNeedTickTreeView = true;
	bShowFunction = false;
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SCheckBox)
					.Style(FCoreStyle::Get(), "ToggleButtonCheckbox")
					.IsChecked_Lambda([&]() {return bNeedTickTreeView ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; })
					.OnCheckStateChanged_Lambda([&](const ECheckBoxState NewState) { bNeedTickTreeView = (NewState == ECheckBoxState::Checked) ? true : false; })
					[
						SNew(SBox)
						.VAlign(VAlign_Center)
						.HAlign(HAlign_Center)
						.Padding(FMargin(4.0, 2.0))
						[
							SNew(STextBlock)
							.Text(LOCTEXT("VarWatcherNeedTick", "NeedTick"))
						]
					]
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SCheckBox)
					.Style(FCoreStyle::Get(), "ToggleButtonCheckbox")
					.IsChecked_Lambda([&]() {return bShowFunction ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; })
					.OnCheckStateChanged_Lambda([&](const ECheckBoxState NewState) { bShowFunction = (NewState == ECheckBoxState::Checked) ? true : false; })
					[
						SNew(SBox)
						.VAlign(VAlign_Center)
						.HAlign(HAlign_Center)
						.Padding(FMargin(4.0, 2.0))
						[
							SNew(STextBlock)
							.Text(LOCTEXT("VarWatcherShowFunction", "ShowFunction"))
						]
					]
				]
			]
			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			[
				SNew(SBorder)
				.Padding(0)
				.BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
				[
					SAssignNew(VarTreePtr, SVarsTree)
					.ItemHeight(24.0f)
					.TreeItemsSource(&VarTreeRoot)
					.OnGenerateRow_Raw(this, &FVarWatcherModule::HandleVarTreeGenerateRow)
					.OnGetChildren_Raw(this, &FVarWatcherModule::HandleVarTreeGetChildren)
					.OnSelectionChanged_Raw(this, &FVarWatcherModule::HandleVarTreeSelectionChanged)
					.OnExpansionChanged_Raw(this, &FVarWatcherModule::HandleNodeExpansion)
					.HeaderRow
					(
						SNew(SHeaderRow)
						+SHeaderRow::Column(SVarTreeWidgetItem::Col_Name)
						.DefaultLabel(LOCTEXT("VarName1", "Key"))
						.FillWidth(20.0f)

						+ SHeaderRow::Column(SVarTreeWidgetItem::Col_Value)
						.DefaultLabel(LOCTEXT("VarValue1", "Value"))
						.FillWidth(20.0f)
					)
				]
			]
		];
}

void FVarWatcherModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->InvokeTab(VarWatcherTabName);
}


void FVarWatcherModule::Tick(float Delta)
{
	if (bNeedTickTreeView)
		bNeedUpdateData = true;
}		

void FVarWatcherModule::AddMenuExtension(FMenuBuilder& Builder)
{
	Builder.AddMenuEntry(FVarWatcherCommands::Get().OpenPluginWindow);
}

void FVarWatcherModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
	Builder.AddToolBarButton(FVarWatcherCommands::Get().OpenPluginWindow);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FVarWatcherModule, VarWatcher)


FName SVarTreeWidgetItem::Col_Name = "VarName";
FName SVarTreeWidgetItem::Col_Value = "VarValue";

TSharedRef<SWidget> SVarTreeWidgetItem::GenerateWidgetForColumn(const FName& ColumnName)
{
	if (ColumnName == Col_Name)
	{
		return SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(SExpanderArrow, SharedThis(this))
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text_Lambda([&]()->FText {return VarInfoNode->GetName(); })
			];
	}
	else
	{
		return SNew(STextBlock)
			.Text_Lambda([&]()->FText {return VarInfoNode->GetValue(); })
			;
	}
}
