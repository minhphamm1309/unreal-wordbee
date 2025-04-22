#include "SWorkFlowStatus.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Views/SHeaderRow.h"
#include "Widgets/Layout/SScrollBox.h"
#include "WordbeeEditor/API/API.h"
#include "Widgets/Input/SButton.h"

void SWorkFlowStatus::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SVerticalBox)
		// Table List
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SScrollBox)
			+ SScrollBox::Slot()
			[
				SAssignNew(WorkflowListView, SListView<TSharedPtr<FWorkflowStatus>>)
				.ItemHeight(24)
				.ListItemsSource(&WorkflowList)
				.OnGenerateRow(this, &SWorkFlowStatus::GenerateRow)
				.HeaderRow
				(
					SNew(SHeaderRow)
					+ SHeaderRow::Column("Language").DefaultLabel(FText::FromString("Language"))
					+ SHeaderRow::Column("Status").DefaultLabel(FText::FromString("Status"))
				)
			]
		]
		// Sync Button (aligned to the right)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(5)
		.HAlign(HAlign_Right)
		[
			SNew(SButton)
			.Text(FText::FromString("Sync"))
			.OnClicked_Lambda([this]()
			{
				FetchWorkflow();
				return FReply::Handled();
			})
		]
	];
	// Initial Fetch
	FetchWorkflow();
}

void SWorkFlowStatus::FetchWorkflow()
{
	userInfo = wordbee::SingletonUtil<FWordbeeUserData>::GetFromIni();
	// Simulate fetching data from API
	API::FetchWorkflow(userInfo, TEXT("SomeDocumentId"),
		[this](const TArray<FWorkflowStatus>& FetchedWorkflows)
		{
			WorkflowList.Empty();
			for (const FWorkflowStatus& Status : FetchedWorkflows)
			{
				WorkflowList.Add(MakeShared<FWorkflowStatus>(Status));
			}

			if (WorkflowListView.IsValid())
			{
				WorkflowListView->RequestListRefresh();
			}
		},
		[this](const FString& ErrorMessage)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to fetch workflow: %s"), *ErrorMessage);
		});
}

TSharedRef<ITableRow> SWorkFlowStatus::GenerateRow(TSharedPtr<FWorkflowStatus> Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(STableRow<TSharedPtr<FWorkflowStatus>>, OwnerTable)
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.FillWidth(0.5f)
		[
			SNew(STextBlock).Text(FText::FromString(Item->Loct))
		]
		+ SHorizontalBox::Slot()
		.FillWidth(0.5f)
		[
			SNew(STextBlock).Text(FText::FromString(Item->GetDisplayedStatus()))
		]
	];
}
