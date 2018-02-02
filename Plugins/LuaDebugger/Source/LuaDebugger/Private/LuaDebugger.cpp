// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "LuaDebugger.h"
#include "LuaDebuggerStyle.h"
#include "LuaDebuggerCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "EditorStyleSet.h"
#include "FileManager.h"
#include "FileHelper.h"
#include "DebuggerSetting.h"
#include "SlateApplication.h"
#include "SSearchBox.h"
#include "CoreDelegates.h"

static const FName LuaDebuggerTabName("LuaDebugger");

#define LOCTEXT_NAMESPACE "FLuaDebuggerModule"


FLuaDebuggerModule* FLuaDebuggerModule::Ptr;

void FLuaDebuggerModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	Ptr = this;
	IsDebugRun = true;
	IsEnterDebugMode = false;
	ptr_HandleKeyDown = MakeShareable(new FHandleKeyDown());
	StackListState = EStackListState::CallStack;
	LastTimeFileOffset = UDebuggerSetting::Get()->LastTimeFileOffset;
	RecentFilePath = UDebuggerSetting::Get()->RecentFilePath;
	for (FBreakPointNode &Node : UDebuggerSetting::Get()->RecentBreakPoint)
	{
		ToggleBreakPoint(Node.FilePath, Node.Line);
	}
	FCoreDelegates::OnPreExit.AddRaw(this, &FLuaDebuggerModule::BeforeExit);
	FLuaDebuggerStyle::Initialize();
	FLuaDebuggerStyle::ReloadTextures();

	FLuaDebuggerCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FLuaDebuggerCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FLuaDebuggerModule::PluginButtonClicked),
		FCanExecuteAction());
		
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	
	{
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
		MenuExtender->AddMenuExtension("WindowLayout", EExtensionHook::After, PluginCommands, FMenuExtensionDelegate::CreateRaw(this, &FLuaDebuggerModule::AddMenuExtension));

		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	}
	
// 	{
// 		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
// 		ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FLuaDebuggerModule::AddToolbarExtension));
// 		
// 		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
// 	}
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(LuaDebuggerTabName, FOnSpawnTab::CreateRaw(this, &FLuaDebuggerModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FLuaDebuggerTabTitle", "LuaDebugger"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FLuaDebuggerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	
	FLuaDebuggerStyle::Shutdown();

	FLuaDebuggerCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(LuaDebuggerTabName);
}

TSharedRef<SDockTab> FLuaDebuggerModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	UDebuggerSetting::Get()->SetTabIsOpen(true);
	DebugStateChange();
	BreakPointChange();
	RefreshLuaFileData();
	FSlateApplication::Get().RegisterInputPreProcessor(ptr_HandleKeyDown);
	TSharedRef<SDockTab> Tab = SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		.OnTabClosed_Raw(this, &FLuaDebuggerModule::DebugTabClose)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SCheckBox)
					.Style(FCoreStyle::Get(), "ToggleButtonCheckbox")
					.IsChecked_Lambda([&]() {return IsDebugRun ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; })
					.OnCheckStateChanged_Raw(this, &FLuaDebuggerModule::ToggleStartDebug)
					[
						SNew(SBox)
						.VAlign(VAlign_Center)
						.HAlign(HAlign_Center)
						.Padding(FMargin(4.0, 2.0))
						[
							SNew(STextBlock)
							.Text(FText::FromString("Start"))
						]
					]
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SButton)
					.OnClicked_Raw(this, &FLuaDebuggerModule::DebugContinue)
					[
						SNew(STextBlock)
						.Text(FText::FromString("Continue F5"))
					]
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SButton)
					.OnClicked_Raw(this, &FLuaDebuggerModule::DebugStepover)
					[
						SNew(STextBlock)
						.Text(FText::FromString("StepOver F10"))
					]
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SButton)
					.OnClicked_Raw(this, &FLuaDebuggerModule::DebugStepin)
					[
						SNew(STextBlock)
						.Text(FText::FromString("StepIn F11"))
					]
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SButton)
					.OnClicked_Raw(this, &FLuaDebuggerModule::DebugStepout)
					[
						SNew(STextBlock)
						.Text(FText::FromString("StepOut shift+F11"))
					]
				]
				+ SHorizontalBox::Slot()
				[
					SNew(SBox)
					.HAlign(HAlign_Right)
					[
						SNew(STextBlock)
						.Text_Lambda([&]()->FText {return FText::FromString(NowLuaCodeFilePath.Mid(GetLuaSourceDir().Len()+1)); })
					]
				]
			]
			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			[
				SNew(SSplitter)
				.Orientation(Orient_Vertical)
				+ SSplitter::Slot()
				.Value(0.7f)
				[
					SNew(SSplitter)
					.Orientation(Orient_Horizontal)
					+ SSplitter::Slot()
					.Value(0.2f)
					[
						SNew(SBorder)
						.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
						[
							SNew(SVerticalBox)
							+ SVerticalBox::Slot()
							.AutoHeight()
							[
								SNew(SSearchBox)
								.HintText(FText::FromString(L"Search..."))
								.OnTextChanged_Raw(this, &FLuaDebuggerModule::OnFileFilterTextChanged)
// 								.OnTextCommitted(this, &SSceneOutliner::OnFilterTextCommitted)
							]
							+ SVerticalBox::Slot()
							[
								SAssignNew(LuaFileTreePtr, SLuaFileTree)
								.ItemHeight(24.0f)
								.TreeItemsSource(&AfterFilterLuaFiles)
								.OnGenerateRow_Raw(this, &FLuaDebuggerModule::HandleFileTreeGenerateRow)
								.OnGetChildren_Raw(this, &FLuaDebuggerModule::HandleFileTreeGetChildren)
 								.OnSelectionChanged_Raw(this, &FLuaDebuggerModule::HandleFileTreeSelectionChanged)
								.OnExpansionChanged_Raw(this, &FLuaDebuggerModule::HandleFileNodeExpansion)
							]
						]
					]
					+ SSplitter::Slot()
					.Value(0.8f)
					[
						SNew(SBorder)
						.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
						.Padding(FMargin(2.0f,2.0f,2.0f,2.0f))
						[
							SNew(SBorder)
							.BorderImage(FEditorStyle::GetBrush("MessageLog.ListBorder"))
							[
								SAssignNew(LuaCodeListPtr, SLuaCodeList)
								.ItemHeight(24.0f)
								.ListItemsSource(&NowLuaCodes)
								.OnGenerateRow_Raw(this, &FLuaDebuggerModule::HandleCodeListGenerateRow)
								.OnSelectionChanged_Raw(this, &FLuaDebuggerModule::HandleCodeListSelectionChanged)
							]
						]
					]
				]
				+ SSplitter::Slot()
				.Value(0.3f)
				[
					SNew(SBorder)
					.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
					[
						SNew(SSplitter)
						.Orientation(Orient_Horizontal)
						+ SSplitter::Slot()
						.Value(0.618f)
						[
							SNew(SBorder)
							.BorderImage(FEditorStyle::GetBrush("MessageLog.ListBorder"))
							[
								SAssignNew(DebuggerVarTree, SDebuggerVarTree)
								.ItemHeight(24.0f)
								.TreeItemsSource(&NowVars)
								.OnGenerateRow_Raw(this, &FLuaDebuggerModule::HandleVarsTreeGenerateRow)
								.OnGetChildren_Raw(this, &FLuaDebuggerModule::HandleVarsTreeGetChildren)
								.OnSelectionChanged_Raw(this, &FLuaDebuggerModule::HandleVarsTreeSelectionChanged)
								.HeaderRow
								(
									SNew(SHeaderRow)
									+ SHeaderRow::Column(SDebuggerVarTreeWidgetItem::Col_Name)
									.DefaultLabel(LOCTEXT("VarName1", "Key"))
									.FillWidth(20.0f)

									+ SHeaderRow::Column(SDebuggerVarTreeWidgetItem::Col_Value)
									.DefaultLabel(LOCTEXT("VarValue1", "Value"))
									.FillWidth(20.0f)
								)
							]
						]
						+ SSplitter::Slot()
						.Value(1.0f - 0.618f)
						[

							SNew(SBorder)
							.BorderImage(FEditorStyle::GetBrush("MessageLog.ListBorder"))
							[
								SNew(SVerticalBox)
								+ SVerticalBox::Slot()
								[
									SAssignNew(LuaStackListPtr, SLuaStackList)
									.Visibility_Lambda([&]()->EVisibility {return (StackListState == EStackListState::CallStack) ? EVisibility::Visible : EVisibility::Collapsed; })
									.ItemHeight(24.0f)
									.ListItemsSource(&NowLuaStack)
									.SelectionMode(ESelectionMode::Single)
									.OnGenerateRow_Raw(this, &FLuaDebuggerModule::HandleStackListGenerateRow)
									.OnSelectionChanged_Raw(this, &FLuaDebuggerModule::HandleStackListSelectionChanged)
								]
								+ SVerticalBox::Slot()
								[
									SAssignNew(BreakPointListPtr, SBreakPointList)
									.Visibility_Lambda([&]()->EVisibility {return (StackListState == EStackListState::BreakPoints) ? EVisibility::Visible : EVisibility::Collapsed; })
									.ItemHeight(24.0f)
									.ListItemsSource(&BreakPointForView)
									.SelectionMode(ESelectionMode::Single)
									.OnGenerateRow_Raw(this, &FLuaDebuggerModule::HandleBreakPointListGenerateRow)
									.OnSelectionChanged_Raw(this, &FLuaDebuggerModule::HandleBreakPointListSelectionChanged)
								]
								+ SVerticalBox::Slot()
								.VAlign(VAlign_Bottom)
								.AutoHeight()
								[
									SNew(SHorizontalBox)
									+ SHorizontalBox::Slot()
									.HAlign(HAlign_Left)
									.AutoWidth()
									[
										SNew(SCheckBox)
										.Style(FCoreStyle::Get(), "ToggleButtonCheckbox")
										.IsChecked_Lambda([&]() {return (StackListState==EStackListState::CallStack) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; })
										.OnCheckStateChanged_Raw(this, &FLuaDebuggerModule::ToggleStackListState, EStackListState::CallStack)
										[
											SNew(SBox)
											.VAlign(VAlign_Center)
											.HAlign(HAlign_Center)
											.Padding(FMargin(4.0, 2.0))
											[
												SNew(STextBlock)
												.Text(FText::FromString("CallStack"))
											]
										]
									]
									+ SHorizontalBox::Slot()
									.HAlign(HAlign_Left)
									.AutoWidth()
									[
										SNew(SCheckBox)
										.Style(FCoreStyle::Get(), "ToggleButtonCheckbox")
										.IsChecked_Lambda([&]() {return (StackListState == EStackListState::BreakPoints) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; })
										.OnCheckStateChanged_Raw(this, &FLuaDebuggerModule::ToggleStackListState, EStackListState::BreakPoints)
										[
											SNew(SBox)
											.VAlign(VAlign_Center)
											.HAlign(HAlign_Center)
											.Padding(FMargin(4.0, 2.0))
											[
												SNew(STextBlock)
												.Text(FText::FromString("Breakpoints"))
											]
										]
									]
								]
							]
						]
					]
				]
			]
		];
	ShowCode(RecentFilePath);
	return Tab;
}

void FLuaDebuggerModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->InvokeTab(LuaDebuggerTabName);
}


bool FLuaDebuggerModule::HasBreakPoint(FString& FilePath, int32 CodeLine)
{
	if (TSet<int32>* p = EnableBreakPoint.Find(FilePath))
	{
		if (p->Contains(CodeLine))
			return true;
	}
	return false;
}

void FLuaDebuggerModule::ToggleBreakPoint(FString& FilePath, int32 CodeLine)
{
	TSet<int32>& Set = EnableBreakPoint.FindOrAdd(FilePath);
	if (Set.Contains(CodeLine))
	{
		Set.Remove(CodeLine);
		
		for (int32 i = 0; i < BreakPointForView.Num(); i++)
		{
			FBreakPointNode_Ref&Node = BreakPointForView[i];
			if (Node->FilePath == FilePath && Node->Line == CodeLine)
			{
				BreakPointForView.RemoveAt(i);
				break;
			}
		}
	}
	else
	{
		Set.Add(CodeLine);
		FBreakPointNode_Ref NewNode = MakeShareable(new FBreakPointNode(CodeLine, FilePath));
		BreakPointForView.Add(NewNode);
	}
	BreakPointChange();
}


void FLuaDebuggerModule::ToggleStartDebug(const ECheckBoxState NewState)
{
	IsDebugRun = (NewState == ECheckBoxState::Checked) ? true : false;
	DebugStateChange();
}


void FLuaDebuggerModule::ToggleStackListState(const ECheckBoxState NewState, EStackListState State)
{
	if (NewState == ECheckBoxState::Checked)
	{
		StackListState = State;
	}
}

void FLuaDebuggerModule::BreakPointChange()
{
	if (BreakPointListPtr.IsValid())
		BreakPointListPtr.Pin()->RequestListRefresh();
	RefreshCodeList();
	UDebuggerSetting::Get()->UpdateBreakPoint(EnableBreakPoint);
}


void FLuaDebuggerModule::DebugStateChange()
{
	UDebuggerSetting::Get()->ToggleDebugStart(IsDebugRun);
}

void FLuaDebuggerModule::RefreshCodeList()
{
	if (LuaCodeListPtr.IsValid())
		LuaCodeListPtr.Pin()->RequestListRefresh();
}


void FLuaDebuggerModule::RefreshStackList()
{
	if (LuaStackListPtr.IsValid())
	{
		LuaStackListPtr.Pin()->RequestListRefresh();
	}
}

FString FLuaDebuggerModule::GetLuaSourceDir()
{
	return FPaths::ConvertRelativePathToFull(FPaths::GameDir() / TEXT("LuaSource"));
}

FString FLuaDebuggerModule::LuaPathToFilePath(FString LuaFilePath)
{
	if (LuaFilePath.Find("LuaSource"))
	{
		LuaFilePath.Replace(TEXT("\\"), TEXT("/"));
	}
	else
	{
		if (LuaFilePath.Find("."))
		{
			LuaFilePath.Replace(TEXT("."), TEXT("/"));
			LuaFilePath += ".lua";
			LuaFilePath = GetLuaSourceDir() / LuaFilePath;
		}
	}
	return LuaFilePath;
}

void FLuaDebuggerModule::EnterDebug(const FString& LuaFilePath, int32 Line)
{
	ShowCode(LuaFilePath, Line);
// todo
	ShowStackVars(3);
	IsEnterDebugMode = true;
	FSlateApplication::Get().EnterDebuggingMode();
}


void FLuaDebuggerModule::SetStackData(const TArray<FString>& Content, const TArray<int32>& Lines, const TArray<FString>& FilePaths, const TArray<int32>& StackIndex)
{
	NowLuaStack.Reset();
	for (int i = 0; i < Content.Num(); i++)
	{
		FStackListNode_Ref NewNode = MakeShareable(new FStackListNode(Content[i], Lines[i], FilePaths[i], StackIndex[i]));
		NowLuaStack.Add(NewNode);
	}
	RefreshStackList();
}

void FLuaDebuggerModule::ShowCode(const FString& FilePath, int32 Line /*= 0*/)
{

	if (LuaCodeListPtr.IsValid())
	{
		float NowOffset = LuaCodeListPtr.Pin()->GetScrollOffset();
		LastTimeFileOffset.Add(NowLuaCodeFilePath, NowOffset);
	}
	RecentFilePath = FilePath;
	NowLuaCodeFilePath = FilePath;
	FString RawCode;
	FFileHelper::LoadFileToString(RawCode, *FilePath);
	TArray<FString> CodeArr;
	RawCode.ParseIntoArrayLines(CodeArr, false);
	NowLuaCodes.Reset();
	for (int32 i = 0; i < CodeArr.Num(); i++)
	{
		FString& Code = CodeArr[i];
		FCodeListNode_Ref NewNode = MakeShareable(new FCodeListNode(Code, i + 1, FilePath));
		NowLuaCodes.Add(NewNode);
	}
	if (LuaCodeListPtr.IsValid())
	{
		if (float* Offset = LastTimeFileOffset.Find(NowLuaCodeFilePath))
		{
			LuaCodeListPtr.Pin()->SetScrollOffset(*Offset);
		}
		else
		{
			LuaCodeListPtr.Pin()->SetScrollOffset(0);
		}
	}
	RefreshCodeList();

	if (Line > 0)
	{
		if (LuaCodeListPtr.IsValid())
		{
			LuaCodeListPtr.Pin()->ClearSelection();
			LuaCodeListPtr.Pin()->SetItemSelection(NowLuaCodes[Line - 1], true);
			float NowOffset = LuaCodeListPtr.Pin()->GetScrollOffset();
// todo
			if (FMath::Abs(NowOffset-Line)>5)
				LuaCodeListPtr.Pin()->SetScrollOffset(Line-5);
		}
		
	}
}


void FLuaDebuggerModule::ShowStackVars(int32 StackIndex)
{
	if (StackIndex == -1)
	{
		NowVars.Reset();
	}
	else
	{
		UDebuggerSetting::Get()->GetStackVars(StackIndex, NowVars);
	}
	if (DebuggerVarTree.IsValid())
	{
		DebuggerVarTree.Pin()->RequestTreeRefresh();
	}
}

TSharedRef<ITableRow> FLuaDebuggerModule::HandleFileTreeGenerateRow(FLuaFileTreeNode_Ref InNode, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(SLuaFileTreeWidgetItem, OwnerTable, InNode);
}


void FLuaDebuggerModule::HandleFileTreeGetChildren(FLuaFileTreeNode_Ref InNode, TArray<FLuaFileTreeNode_Ref>& OutChildren)
{
	InNode->GetAllChildren(OutChildren);
}


void FLuaDebuggerModule::HandleFileTreeSelectionChanged(TSharedPtr<FLuaFileTreeNode> InNode, ESelectInfo::Type SelectInfo)
{
	if (InNode.IsValid() && !InNode->IsDir)
	{
		ShowCode(InNode->GetFilePath());
	}
}


void FLuaDebuggerModule::HandleFileNodeExpansion(FLuaFileTreeNode_Ref InNode, bool bIsExpaned)
{

}


TSharedRef<ITableRow> FLuaDebuggerModule::HandleCodeListGenerateRow(FCodeListNode_Ref InNode, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(SCodeWidgetItem, OwnerTable, InNode);
}


void FLuaDebuggerModule::HandleCodeListSelectionChanged(TSharedPtr<FCodeListNode>, ESelectInfo::Type)
{

}


TSharedRef<ITableRow> FLuaDebuggerModule::HandleStackListGenerateRow(FStackListNode_Ref InNode, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(SLuaStackWidgetItem, OwnerTable, InNode);
}


void FLuaDebuggerModule::HandleStackListSelectionChanged(TSharedPtr<FStackListNode> InNode, ESelectInfo::Type)
{
	if (InNode.IsValid())
	{
		if (!InNode->FilePath.IsEmpty())
		{
			ShowCode(InNode->FilePath, InNode->Line);
		}
		ShowStackVars(InNode->StackIndex);
	}
}


TSharedRef<ITableRow> FLuaDebuggerModule::HandleBreakPointListGenerateRow(FBreakPointNode_Ref InNode, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(SBreakPointWidgetItem, OwnerTable, InNode);
}


void FLuaDebuggerModule::HandleBreakPointListSelectionChanged(TSharedPtr<FBreakPointNode> InNode, ESelectInfo::Type)
{
	if(InNode.IsValid())
		ShowCode(InNode->FilePath, InNode->Line);
}

TSharedRef<ITableRow> FLuaDebuggerModule::HandleVarsTreeGenerateRow(FDebuggerVarNode_Ref InNode, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(SDebuggerVarTreeWidgetItem, OwnerTable, InNode);
}


void FLuaDebuggerModule::HandleVarsTreeGetChildren(FDebuggerVarNode_Ref InNode, TArray<FDebuggerVarNode_Ref>& OutChildren)
{
	InNode->GetChildren(OutChildren);
}


void FLuaDebuggerModule::HandleVarsTreeSelectionChanged(TSharedPtr<FDebuggerVarNode>, ESelectInfo::Type)
{

}

void FLuaDebuggerModule::RefreshLuaFileData()
{
	FString BaseDir = GetLuaSourceDir();
	FLuaFileTreeNode_Ref RootNode = MakeShareable(new FLuaFileTreeNode(true, FString(""), BaseDir));
	RootNode->FindChildren();
	RootNode->GetAllChildren(LuaFiles);
	const FText EmptyText = FText::FromString(L"");
	OnFileFilterTextChanged(EmptyText);
}


void FLuaDebuggerModule::CleanDebugInfo()
{
	NowLuaStack.Reset();
	RefreshStackList();
	ShowStackVars(-1);
}

FReply FLuaDebuggerModule::DebugContinue()
{
	if (IsEnterDebugMode)
	{
		CleanDebugInfo();
		FSlateApplication::Get().LeaveDebuggingMode();
		IsEnterDebugMode = false;
	}
	return FReply::Handled();
}


FReply FLuaDebuggerModule::DebugStepover()
{
	if (IsEnterDebugMode)
	{
		CleanDebugInfo();
		FSlateApplication::Get().LeaveDebuggingMode();
		IsEnterDebugMode = false;
		UDebuggerSetting::Get()->StepOver();
	}
	return FReply::Handled();
}


FReply FLuaDebuggerModule::DebugStepin()
{
	if (IsEnterDebugMode)
	{
		CleanDebugInfo();
		FSlateApplication::Get().LeaveDebuggingMode();
		IsEnterDebugMode = false;
		UDebuggerSetting::Get()->StepIn();
	}
	return FReply::Handled();
}


FReply FLuaDebuggerModule::DebugStepout()
{
	if (IsEnterDebugMode)
	{
		CleanDebugInfo();
		FSlateApplication::Get().LeaveDebuggingMode();
		IsEnterDebugMode = false;
		UDebuggerSetting::Get()->StepOut();
	}
	return FReply::Handled();
}

void FLuaDebuggerModule::DebugTabClose(TSharedRef<SDockTab> DockTab)
{
	UDebuggerSetting::Get()->SetTabIsOpen(false);
	DebugContinue();
	FSlateApplication::Get().UnregisterInputPreProcessor(ptr_HandleKeyDown);
	SaveDebuggerConfig();
}


void FLuaDebuggerModule::RegisterKeyDown()
{
	FSlateApplication::Get().RegisterInputPreProcessor(ptr_HandleKeyDown);
}


void FLuaDebuggerModule::BeforeExit()
{
	SaveDebuggerConfig();
}

void FLuaDebuggerModule::SaveDebuggerConfig()
{
	if (LuaCodeListPtr.IsValid())
	{
		float NowOffset = LuaCodeListPtr.Pin()->GetScrollOffset();
		LastTimeFileOffset.Add(NowLuaCodeFilePath, NowOffset);
	}
	UDebuggerSetting::Get()->LastTimeFileOffset = LastTimeFileOffset;
	UDebuggerSetting::Get()->RecentFilePath = RecentFilePath;
	UDebuggerSetting::Get()->RecentBreakPoint.Reset();
	for (FBreakPointNode_Ref &Node : BreakPointForView)
	{
		UDebuggerSetting::Get()->RecentBreakPoint.Add(*Node);
	}
	UDebuggerSetting::Get()->SaveConfig();
}

void FLuaDebuggerModule::OnFileFilterTextChanged(const FText& InFilterText)
{
	if (InFilterText.IsEmpty())
		AfterFilterLuaFiles = LuaFiles;
	else
	{
		FString FileStr = InFilterText.ToString();
		FString FilePatternStr = ".*";
		for (int i = 0; i < FileStr.Len(); i++)
		{
			FilePatternStr += FileStr[i];
			FilePatternStr += ".*";
		}
		FRegexPattern FilePattern(FilePatternStr);
		AfterFilterLuaFiles.Reset();
		for (FLuaFileTreeNode_Ref& FileNode : LuaFiles)
		{
			FielterFileNode(FilePattern, FileNode);
		}
	}
	if (LuaFileTreePtr.IsValid())
		LuaFileTreePtr.Pin()->RequestTreeRefresh();
}


void FLuaDebuggerModule::FielterFileNode(FRegexPattern& Pattern, FLuaFileTreeNode_Ref& FileNode)
{
	if (FileNode->IsDir)
	{
		TArray<FLuaFileTreeNode_Ref> OutChildren;
		FileNode->GetAllChildren(OutChildren);
		for (FLuaFileTreeNode_Ref& Node : OutChildren)
		{
			FielterFileNode(Pattern, Node);
		}
	}
	else
	{
		FRegexMatcher Matcher(Pattern, FileNode->FileName);
		if (Matcher.FindNext())
		{
			AfterFilterLuaFiles.Add(FileNode);
		}
	}
}

void FLuaDebuggerModule::AddMenuExtension(FMenuBuilder& Builder)
{
	Builder.AddMenuEntry(FLuaDebuggerCommands::Get().OpenPluginWindow);
}

void FLuaDebuggerModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
	Builder.AddToolBarButton(FLuaDebuggerCommands::Get().OpenPluginWindow);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FLuaDebuggerModule, LuaDebugger)

void FLuaFileTreeNode::FindChildren()
{
	if (IsDir)
	{
		TArray<FString> DirectoryNames;
		IFileManager::Get().FindFiles(DirectoryNames, *(BasePath / "*"), false, true);

		TArray<FString> NowDirNames;
		DirChildren.GenerateKeyArray(NowDirNames);
		for (auto& Name: NowDirNames)
		{
			if (!DirectoryNames.Contains(Name))
				DirChildren.Remove(Name);
		}

		for (auto& Name : DirectoryNames)
		{
			if (!DirChildren.Find(Name))
			{
				FLuaFileTreeNode_Ref NewOne = MakeShareable(new FLuaFileTreeNode(true, Name, BasePath / Name));
				DirChildren.Add(Name, NewOne);
			}
		}

		TArray<FString> FileNames;
		IFileManager::Get().FindFiles(FileNames, *(BasePath / "*"), true, false);

		TArray<FString> NowFileNames;
		FileChildren.GenerateKeyArray(NowFileNames);
		for (auto& Name : NowFileNames)
		{
			if (!FileNames.Contains(Name))
				FileChildren.Remove(Name);
		}

		for (auto& Name : FileNames)
		{
			if (!FileChildren.Find(Name))
			{
				FLuaFileTreeNode_Ref NewOne = MakeShareable(new FLuaFileTreeNode(false, Name, BasePath));
				FileChildren.Add(Name, NewOne);
			}
		}
	}
}

void FLuaFileTreeNode::GetAllChildren(TArray<TSharedRef<FLuaFileTreeNode>>& OutArr)
{
	FindChildren();
	DirChildren.GenerateValueArray(OutArr);
	OutArr.StableSort([&](const TSharedRef<FLuaFileTreeNode>& a, const TSharedRef<FLuaFileTreeNode>& b) {return a->FileName < b->FileName; });
	TArray<TSharedRef<FLuaFileTreeNode>> FileChildrenArr;
	FileChildren.GenerateValueArray(FileChildrenArr);
	FileChildrenArr.StableSort([&](const TSharedRef<FLuaFileTreeNode>& a, const TSharedRef<FLuaFileTreeNode>& b) {return a->FileName < b->FileName; });
	OutArr.Append(FileChildrenArr);
}


void SCodeWidgetItem::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView, FCodeListNode_Ref Node)
{
	CodeNode = Node;
	static FSlateFontInfo CodeFont = FSlateFontInfo();
	CodeFont.Size = 24;
	STableRow<FCodeListNode_Ref>::Construct(STableRow<FCodeListNode_Ref>::FArguments(), InOwnerTableView);
	ChildSlot
	[
			// 			SNew(SBorder)
			// 			.Padding(FMargin(0.0f, 0.0f, 5.0f,0.0f))
		SNew(SBox)
		.MinDesiredHeight(20)
		.VAlign(VAlign_Center)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.HAlign(HAlign_Left)
			[
				SNew(SBox)
				[
					SNew(SButton)
					.ForegroundColor(FSlateColor::UseForeground())
					.OnClicked(this, &SCodeWidgetItem::HandleClickBreakPoint)
					[
						SNew(SImage)
						.Image(FEditorStyle::GetBrush("Level.VisibleHighlightIcon16x"))
						.Visibility(this, &SCodeWidgetItem::BreakPointVisible)
					]
				]
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SBox)
				.MinDesiredWidth(40)
				[
					SNew(STextBlock)
					.Text(FText::FromString(FString::Printf(L"%d", Node->Line)))
				]
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(STextBlock)
				.Text(FText::FromString(Node->Code))
			]
		]
	];
}

EVisibility SCodeWidgetItem::BreakPointVisible() const
{
	return FLuaDebuggerModule::Get()->HasBreakPoint(CodeNode->FilePath, CodeNode->Line) ? EVisibility::Visible : EVisibility::Hidden;
}

FReply SCodeWidgetItem::HandleClickBreakPoint()
{
	FLuaDebuggerModule::Get()->ToggleBreakPoint(CodeNode->FilePath, CodeNode->Line);
	return FReply::Handled();
}

void SLuaStackWidgetItem::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView, FStackListNode_Ref Node)
{
	STableRow<FStackListNode_Ref>::Construct(STableRow<FStackListNode_Ref>::FArguments(), InOwnerTableView);
	StackNode = Node;
	ChildSlot
	[
		SNew(SBox)
		.MinDesiredHeight(20)
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
			.Text(FText::FromString(Node->Code))
		]
	];
}

FName SDebuggerVarTreeWidgetItem::Col_Name = "key";
FName SDebuggerVarTreeWidgetItem::Col_Value = "value";

TSharedRef<SWidget> SDebuggerVarTreeWidgetItem::GenerateWidgetForColumn(const FName& ColumnName)
{
	if (ColumnName == Col_Name)
	{
		return SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(SExpanderArrow, SharedThis(this))
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text_Lambda([&]()->FText {return VarInfoNode->Name; })
			];
	}
	else
	{
		return SNew(STextBlock)
			.Text_Lambda([&]()->FText {return VarInfoNode->Value; })
			;
	}
}

bool FLuaDebuggerModule::FHandleKeyDown::HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::F5)
	{
		FLuaDebuggerModule::Get()->DebugContinue();
		return true;
	}
	else if (InKeyEvent.GetKey() == EKeys::F10)
	{
		FLuaDebuggerModule::Get()->DebugStepover();
		return true;
	}
	else if (InKeyEvent.GetKey() == EKeys::F11)
	{
		if(InKeyEvent.IsLeftShiftDown() || InKeyEvent.IsRightShiftDown())
			FLuaDebuggerModule::Get()->DebugStepout();
		else
			FLuaDebuggerModule::Get()->DebugStepin();
		return true;
	}
	return false;
}

void SBreakPointWidgetItem::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView, FBreakPointNode_Ref Node)
{
	STableRow<FBreakPointNode_Ref>::Construct(STableRow<FBreakPointNode_Ref>::FArguments(), InOwnerTableView);

	FString LuaSourceDir = FLuaDebuggerModule::Get()->GetLuaSourceDir();
	ChildSlot
	[
		SNew(SBox)
		.MinDesiredHeight(20)
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
			.Text(FText::FromString(FString::Printf(L"%s@Line %d", *Node->FilePath.Mid(LuaSourceDir.Len() + 1), Node->Line)))
		]
	];
}
