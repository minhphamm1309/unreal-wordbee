#include "SSelectorDocumentSubWindow.h"

#include "WordBeeEditor/Command/LinkProject/ULinkDocumentCommand.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SSelectorDocumentSubWindow::Construct(const FArguments& InArgs)
{
	OnSubWindowClosed = InArgs._OnSubWindowClosed;

	ChildSlot
	[
		// title
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(FText::FromString("Select Document"))
			]
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			[
				SNew(SButton)
				.Text(FText::FromString("X"))
				.OnClicked(this, &SSelectorDocumentSubWindow::CloseWindow)
			]
		]

		// content
		+ SVerticalBox::Slot()
		.MinHeight(250)
		[
			SNew(SHorizontalBox)

			// Cột Project
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Fill)
			.HAlign(HAlign_Fill)
			.Padding(10)
			[
				SNew(SVerticalBox)

				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(10)
				[
					SNew(STextBlock)
					.Text(FText::FromString("Project"))
				]

				+ SVerticalBox::Slot()
				[
					SAssignNew(ProjectListView, SListView<TSharedPtr<FString>>)
					.ListItemsSource(&Preferences)
					.OnGenerateRow(this, &SSelectorDocumentSubWindow::GenerateProjectRow)
					.OnSelectionChanged(this, &SSelectorDocumentSubWindow::OnProjectSelected)
				]
			]

			// Cột Document
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Fill)
			.HAlign(HAlign_Fill)
			.Padding(10)
			[
				SNew(SVerticalBox)

				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(10)
				[
					SNew(STextBlock)
					.Text(FText::FromString("Document"))
				]

				+ SVerticalBox::Slot()
				[
					SAssignNew(DocumentListView, SListView<TSharedPtr<FDocumentData>>)
					.ListItemsSource(&FilteredDocuments)
					.OnGenerateRow(this, &SSelectorDocumentSubWindow::GenerateDocumentRow)
					.OnSelectionChanged(this, &SSelectorDocumentSubWindow::OnDocumentSelected)
				]

			]
		]

		// button
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			[
				SNew(SButton)
				.Text(FText::FromString("Link"))
				.OnClicked(this, &SSelectorDocumentSubWindow::CloseWindow)
			]
		]
	];
}

void SSelectorDocumentSubWindow::OnDocumentSelected(TSharedPtr<FDocumentData> SelectedItem,
                                                    ESelectInfo::Type SelectInfo)
{
	DocumentId = SelectedItem->Id;
}

void SSelectorDocumentSubWindow::Init(const struct FUserInfo InUserInfo,const TArray<FDocumentData>& InDocumentData)
{
	DocumentDataArray = InDocumentData;
	UserInfo = InUserInfo;
	for (const FDocumentData& Doc : DocumentDataArray)
	{
		TSharedPtr<FString> Preference = MakeShared<FString>(Doc.Preference);
		if (!Preferences.ContainsByPredicate([&](const TSharedPtr<FString>& ExistingPref)
		{
			return *ExistingPref == *Preference;
		}))
		{
			Preferences.Add(Preference);
		}
	}

	if (ProjectListView.IsValid())
	{
		ProjectListView->RequestListRefresh();
	}
}

FReply SSelectorDocumentSubWindow::CloseWindow() const
{
	if (OnSubWindowClosed.IsBound())
	{
		if (!DocumentId.IsEmpty())
		{
			ULinkDocumentCommand::Execute(UserInfo, DocumentId, FOnLinkDocumentComplete::CreateLambda([this](bool bSuccess)
			{
				OnSubWindowClosed.Execute(DocumentId);
			}));
		}
		else
		{
			OnSubWindowClosed.Execute(DocumentId);
		}
	}
	return FReply::Handled();
}

void SSelectorDocumentSubWindow::OnProjectSelected(TSharedPtr<FString> SelectedItem, ESelectInfo::Type SelectInfo)
{
	if (SelectedItem.IsValid())
	{
		FString SelectedPreference = *SelectedItem;
		UpdateFilteredDocuments(SelectedPreference);
		if (DocumentListView.IsValid())
		{
			DocumentListView->RequestListRefresh();
		}
	}
}

TSharedRef<ITableRow> SSelectorDocumentSubWindow::GenerateProjectRow(TSharedPtr<FString> InItem,
                                                                     const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(STableRow<TSharedPtr<FString>>, OwnerTable)
		[
			SNew(STextBlock).Text(FText::FromString(*InItem))
		];
}

TSharedRef<ITableRow> SSelectorDocumentSubWindow::GenerateDocumentRow(TSharedPtr<FDocumentData> InItem,
                                                                      const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(STableRow<TSharedPtr<FDocumentData>>, OwnerTable)
		[
			SNew(STextBlock).Text(FText::FromString(InItem->Name))
		];
}

void SSelectorDocumentSubWindow::UpdateFilteredDocuments(const FString& SelectedPreference)
{
	FilteredDocuments.Empty();

	for (const FDocumentData& Doc : DocumentDataArray)
	{
		if (Doc.Preference == SelectedPreference)
		{
			FilteredDocuments.Add(MakeShared<FDocumentData>(Doc));
		}
	}
}


END_SLATE_FUNCTION_BUILD_OPTIMIZATION
