// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ModuleManager.h"
#include "VarNode.h"
#include "STableRow.h"
#include "STreeView.h"

class FToolBarBuilder;
class FMenuBuilder;

class SVarTreeWidgetItem :public SMultiColumnTableRow<TSharedRef<FVarNode>>
{
public:
	static FName Col_Name;
	static FName Col_Value;
	TSharedPtr<FVarNode> VarInfoNode;
	SLATE_BEGIN_ARGS(SVarTreeWidgetItem)
		:_VarInfoToVisualize()
	{}
		SLATE_ARGUMENT(TSharedPtr<FVarNode>, VarInfoToVisualize)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
	{
		VarInfoNode = InArgs._VarInfoToVisualize;
		SMultiColumnTableRow<TSharedRef<FVarNode>>::Construct(SMultiColumnTableRow< TSharedRef<FVarNode> >::FArguments().Padding(1), InOwnerTableView);
	}
		virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override;
};

class FVarWatcherModule : public IModuleInterface
{
public:
	static FVarWatcherModule* Ptr;
	static FVarWatcherModule* Get() { return Ptr; }
	/** IModuleInterface implementation */
	typedef STreeView<TSharedRef<FVarNode>> SVarsTree;
	TArray<TSharedRef<FVarNode>> SelectedNodes;
	TArray<TSharedRef<FVarNode>> VarTreeRoot;
	TWeakPtr<SVarsTree> VarTreePtr;
	bool bNeedTickTreeView;
	bool bNeedUpdateData;

	TSharedRef<ITableRow> HandleVarTreeGenerateRow(TSharedRef<FVarNode> InVarNode, const TSharedRef<STableViewBase>& OwnerTable);
	void HandleVarTreeGetChildren(TSharedRef<FVarNode> InVarNode, TArray<TSharedRef<FVarNode>>& OutChildren);
	void HandleVarTreeSelectionChanged(TSharedPtr<FVarNode>, ESelectInfo::Type /*SelectInfo*/);
	void HandleNodeExpansion(TSharedRef<FVarNode> VarNode, bool bIsExpaned);

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command (by default it will bring up plugin window) */
	void PluginButtonClicked();
	void Tick(float Delta);
private:

	void AddToolbarExtension(FToolBarBuilder& Builder);
	void AddMenuExtension(FMenuBuilder& Builder);

	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);

private:
	TSharedPtr<class FUICommandList> PluginCommands;
};