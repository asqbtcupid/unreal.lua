// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "VarNode.generated.h"

class UVarNode;

struct FVarNode
{
	UVarNode* UNode;
	bool bNeedExpandNextUpdate;
	FVarNode() :bNeedExpandNextUpdate(false) {};
	void GetChild(TArray<TSharedRef<FVarNode>> &OutChildren);
	FText GetName();
	FText GetValue();
};

UCLASS()
class VARWATCHER_API UVarNode : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY()
		TArray<UVarNode*> Children;

	UPROPERTY()
		TArray<UVarNode*> KeyChildren;
	
	UPROPERTY()
		bool HasExpand;

	UPROPERTY()
		int32 LuaIndex;

	UPROPERTY()
		FText Name;

	UPROPERTY()
		FText Value;

	UVarNode* Parent;
	TSharedPtr<FVarNode> FNode;

	UFUNCTION()
	void Init(FText VarName, FText VarValue, int32 _LuaIndex)
	{
		Name = VarName;
		Value = VarValue;
		LuaIndex = _LuaIndex;
		FNode = MakeShareable(new FVarNode);
		FNode->UNode = this;
		HasExpand = false;
	}
	
	UFUNCTION()
		void AddChild(UVarNode* Child, bool bIsKey = false);
	UFUNCTION()
		void Release();
	UFUNCTION()
		bool ConsumeNeedExpand();
	UFUNCTION()
		void AddToRootArr();
	UFUNCTION()
		bool HasChild();
	void RemoveFromRoot();
	void RemoveChild(UVarNode* Child);
	void Refresh();

	UFUNCTION()
		bool bIsExpanding();

	UFUNCTION()
		static bool NeedUpdate(UObject* GameInstance);

	UFUNCTION()
		static bool NeedShowFunction();
	
	static TSet<UObject*> HasUpdateState;
};
