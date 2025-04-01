#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "WordBeeEditor/Models/WordbeeResponse.h"
#include "WordBeeEditor/Models/WordbeeUserData.h"

class SWorkFlowStatus : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SWorkFlowStatus) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	FWordbeeUserData userInfo;
	TArray<TSharedPtr<FWorkflowStatus>> WorkflowList;
	TSharedPtr<SListView<TSharedPtr<FWorkflowStatus>>> WorkflowListView;

	void FetchWorkflow();
	TSharedRef<ITableRow> GenerateRow(TSharedPtr<FWorkflowStatus> Item, const TSharedRef<STableViewBase>& OwnerTable);
};
