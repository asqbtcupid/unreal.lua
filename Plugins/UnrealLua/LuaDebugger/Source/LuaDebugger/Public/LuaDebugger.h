// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ModuleManager.h"
#include "STableRow.h"
#include "STreeView.h"
#include "DebuggerSetting.h"
#include "IInputProcessor.h"
#include "Regex.h"

class FToolBarBuilder;
class FMenuBuilder;

struct FLuaFileTreeNode
{
	FLuaFileTreeNode(bool bIsDir, FString Name, FString Path)
		: FileName(Name), BasePath(Path), IsDir(bIsDir)
	{
	}

	void FindChildren();
	void GetAllChildren(TArray<TSharedRef<FLuaFileTreeNode>>& OutArr);
	FString GetFilePath()
	{
		return BasePath / FileName;
	}
	FString FileName;
	FString BasePath;
	bool IsDir;
// 	TArray<TSharedRef<FLuaFileTreeNode>> DirChildren;
// 	TArray<TSharedRef<FLuaFileTreeNode>> FileChildren;

	TMap<FString, TSharedRef<FLuaFileTreeNode>> DirChildren;
	TMap<FString, TSharedRef<FLuaFileTreeNode>> FileChildren;
};

using FLuaFileTreeNode_Ref = TSharedRef<FLuaFileTreeNode>;
using SLuaFileTree = STreeView<FLuaFileTreeNode_Ref>;

class SLuaFileTreeWidgetItem :public STableRow<FLuaFileTreeNode_Ref>
{
public:
	
	SLATE_BEGIN_ARGS(SLuaFileTreeWidgetItem)
	{}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView, FLuaFileTreeNode_Ref Node)
	{
		STableRow<FLuaFileTreeNode_Ref>::Construct(STableRow<FLuaFileTreeNode_Ref>::FArguments(), InOwnerTableView);
		ChildSlot
		[
			SNew(SHorizontalBox)
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
				.Text(FText::FromString(Node->FileName))
			]
		];
	}
};

struct FCodeListNode
{
	FCodeListNode(FString _Code, int32 _Line, FString _FilePath)
		:FilePath(_FilePath), Code(_Code), Line(_Line),  HasBreakPoint(false)
	{}
	FString FilePath;
	FString Code;
	int32 Line;
	bool HasBreakPoint;
};
using FCodeListNode_Ref = TSharedRef<FCodeListNode>;
using SLuaCodeList = SListView<FCodeListNode_Ref>;

class SCodeWidgetItem :public STableRow<FCodeListNode_Ref>
{
public:

	SLATE_BEGIN_ARGS(SCodeWidgetItem)
	{}
	SLATE_END_ARGS()
	TSharedPtr<FCodeListNode> CodeNode;
	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView, FCodeListNode_Ref Node);
	
	EVisibility BreakPointVisible() const;
	FReply HandleClickBreakPoint();
	FReply OnRightClickBreakpoint(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
	void OnBreakConditionCommit(const FText& ConditionText, ETextCommit::Type CommitType);
	
};

struct FStackListNode
{
	FStackListNode(FString _Code, int32 _Line, FString _FilePath, int32 _StackIndex, const FString& _FuncInfo)
		:FilePath(_FilePath), Code(_Code), Line(_Line), StackIndex(_StackIndex), FuncInfo(_FuncInfo)
	{}
	FString FilePath;
	FString Code;
	int32 Line;
	int32 StackIndex;
	FString FuncInfo;
};

using FStackListNode_Ref = TSharedRef<FStackListNode>;
using SLuaStackList = SListView<FStackListNode_Ref>;

class SLuaStackWidgetItem :public STableRow<FStackListNode_Ref>
{
public:
	TSharedPtr<FStackListNode> StackNode;
	SLATE_BEGIN_ARGS(SLuaStackWidgetItem)
	{}
	SLATE_END_ARGS()
	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView, FStackListNode_Ref Node);

};




class SDebuggerVarTreeWidgetItem :public SMultiColumnTableRow<TSharedRef<FDebuggerVarNode>>
{
public:
	static FName Col_Name;
	static FName Col_Value;
	TSharedPtr<FDebuggerVarNode> VarInfoNode;
	SLATE_BEGIN_ARGS(SDebuggerVarTreeWidgetItem)
	{}
	SLATE_END_ARGS()

		void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView, FDebuggerVarNode_Ref Node)
	{
		VarInfoNode = Node;
		SMultiColumnTableRow<TSharedRef<FDebuggerVarNode>>::Construct(SMultiColumnTableRow< TSharedRef<FDebuggerVarNode> >::FArguments().Padding(1), InOwnerTableView);
	}
	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override;
};

using FBreakPointNode_Ref = TSharedRef<FBreakPointNode>;
using SBreakPointList = SListView<FBreakPointNode_Ref>;

class SBreakPointWidgetItem :public STableRow<FBreakPointNode_Ref>
{
public:
	TSharedPtr<FBreakPointNode> Node;
	SLATE_BEGIN_ARGS(SBreakPointWidgetItem)
	{}
	SLATE_END_ARGS()
		void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView, FBreakPointNode_Ref Node);

};

enum class EStackListState : uint8
{
	CallStack,
	BreakPoints,
};

// struct ViewCodeHistory
// {
// 	int32 NowHeadIndex;
// 	int32 NowIndex;
// 	struct ViewAddr
// 	{
// 		FString FilePath;
// 		int32 Line;
// 		bool operator==(const ViewAddr& Right)
// 		{
// 			return FilePath == Right.FilePath && Line == Right.Line;
// 		}
// 	};
// 
// 	void Append(ViewAddr NewAddr)
// 	{
// 		if (NowIndex != NowHeadIndex)
// 		{
// 			NowHeadIndex = NowIndex;
// 		}
// 		NowHeadIndex++;
// // 		History.ResizeTo(NowHeadIndex + 1);
// 
// 	}
// 	TArray<ViewAddr> History;
// };

class FLuaDebuggerModule : public IModuleInterface
{
public:
	static FLuaDebuggerModule* Ptr;
	static FLuaDebuggerModule* Get() { return Ptr; }
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command (by default it will bring up plugin window) */
	void PluginButtonClicked();

	TArray<FLuaFileTreeNode_Ref> LuaFiles;
	TArray<FLuaFileTreeNode_Ref> AfterFilterLuaFiles;
	TWeakPtr<SLuaFileTree> LuaFileTreePtr;

	FString NowLuaCodeFilePath;
	FDateTime ModifyTimeOfNowFile;
	TArray<FCodeListNode_Ref> NowLuaCodes;
	TWeakPtr<SLuaCodeList> LuaCodeListPtr;

	EStackListState StackListState;
	TArray<FStackListNode_Ref> NowLuaStack;
	TWeakPtr<SLuaStackList> LuaStackListPtr;

	TArray<FBreakPointNode_Ref> BreakPointForView;
	TWeakPtr<SBreakPointList> BreakPointListPtr;

	TArray<FDebuggerVarNode_Ref> NowVars;
	TWeakPtr<SDebuggerVarTree> DebuggerVarTree;
	FString RecentFilePath;

	TMap<FString, TSet<int32>> EnableBreakPoint;
	bool IsDebugRun;
	bool IsDebugRemote;
	bool IsEnterDebugMode;
	bool HasBreakPoint(FString& FilePath, int32 CodeLine);
	FString LastShowVarFuncName;
	TMap<FString, float> LastTimeFileOffset;
	void ToggleBreakPoint(FString& FilePath, int32 CodeLine);
	TSharedPtr<FBreakPointNode> GetViewBreakPoint(FString& FilePath, int32 CodeLine);
	FText GetBreakPointHitConditionText(FString& FilePath, int32 CodeLine);
	void ToggleStartDebug(const ECheckBoxState NewState);
	void ToggleRemoteDebug(const ECheckBoxState NewState);
	void ToggleStackListState(const ECheckBoxState NewState, EStackListState State);
	void BreakPointChange();
	void DebugStateChange();
	void RemoteStateChange();
	void SyncState();
	void RefreshCodeList();
	void RefreshStackList();
	static FString GetLuaSourceDir();
	static FString LuaPathToFilePath(FString LuaFilePath);
	void EnterDebug(const FString& LuaFilePath, int32 Line);
	void SetStackData(const TArray<FString>& Content, const TArray<int32>& Lines, const TArray<FString>& FilePaths, const TArray<int32>& StackIndex, const TArray<FString>& FuncInfos);
	void ShowCode(const FString& FilePath, int32 Line = 0);
	void ShowStackVars(int32 StackIndex);

	TSharedRef<ITableRow> HandleFileTreeGenerateRow(FLuaFileTreeNode_Ref InNode, const TSharedRef<STableViewBase>& OwnerTable);
	void HandleFileTreeGetChildren(FLuaFileTreeNode_Ref InNode, TArray<FLuaFileTreeNode_Ref>& OutChildren);
	void HandleFileTreeSelectionChanged(TSharedPtr<FLuaFileTreeNode>, ESelectInfo::Type);
	void HandleFileNodeExpansion(FLuaFileTreeNode_Ref InNode, bool bIsExpaned);

	TSharedRef<ITableRow> HandleCodeListGenerateRow(FCodeListNode_Ref InNode, const TSharedRef<STableViewBase>& OwnerTable);
	void HandleCodeListSelectionChanged(TSharedPtr<FCodeListNode>, ESelectInfo::Type);

	TSharedRef<ITableRow> HandleStackListGenerateRow(FStackListNode_Ref InNode, const TSharedRef<STableViewBase>& OwnerTable);
	void HandleStackListSelectionChanged(TSharedPtr<FStackListNode>, ESelectInfo::Type);

	TSharedRef<ITableRow> HandleBreakPointListGenerateRow(FBreakPointNode_Ref InNode, const TSharedRef<STableViewBase>& OwnerTable);
	void HandleBreakPointListSelectionChanged(TSharedPtr<FBreakPointNode>, ESelectInfo::Type);

	TSharedRef<ITableRow> HandleVarsTreeGenerateRow(FDebuggerVarNode_Ref InNode, const TSharedRef<STableViewBase>& OwnerTable);
	void HandleVarsTreeGetChildren(FDebuggerVarNode_Ref InNode, TArray<FDebuggerVarNode_Ref>& OutChildren);
	void HandleVarsTreeSelectionChanged(TSharedPtr<FDebuggerVarNode>, ESelectInfo::Type);

	void RefreshLuaFileData();

	void CleanDebugInfo();
	FReply DebugContinue();
	FReply DebugStepover();
	FReply DebugStepin();
	FReply DebugStepout();
	void DebugTabClose(TSharedRef<SDockTab> DockTab);
	void RegisterKeyDown();
	void BeforeExit();
	void SaveDebuggerConfig();
	class FHandleKeyDown :public IInputProcessor
	{
		virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor);
		virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent);

	};
	TSharedPtr<FHandleKeyDown> ptr_HandleKeyDown;

	void OnFileFilterTextChanged(const FText& InFilterText);
	void OnSearchLineChange(const FText& LineText);
	void FielterFileNode(FRegexPattern& Pattern, FLuaFileTreeNode_Ref& FileNode);
	float IntervalToCheckFileChange;
	void Tick(float Delta);
private:

	void AddToolbarExtension(FToolBarBuilder& Builder);
	void AddMenuExtension(FMenuBuilder& Builder);

	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);

private:
	TSharedPtr<class FUICommandList> PluginCommands;
};