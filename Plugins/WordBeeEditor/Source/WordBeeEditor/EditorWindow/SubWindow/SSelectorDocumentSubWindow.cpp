#include "SSelectorDocumentSubWindow.h"

#include "WordBeeEditor/Command/LinkProject/ULinkDocumentCommand.h"
#include "WordBeeEditor/Models/WordbeeResponse.h"


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SSelectorDocumentSubWindow::Construct(const FArguments& InArgs)
{
	OnSubWindowClosed = InArgs._OnSubWindowClosed;
	ButtonLinkStateText = FText::FromString("Link");
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
					SAssignNew(DocumentListView, SListView<TSharedPtr<FDocumentDataResponse>>)
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
				.IsEnabled(this, &SSelectorDocumentSubWindow::GetReadyLinkButtonState)
				.Text(this, &SSelectorDocumentSubWindow::GetButtonLinkStateText)
				.OnClicked(this, &SSelectorDocumentSubWindow::LinkAndCloseWindow)
			]
		]
	];
}

void SSelectorDocumentSubWindow::OnDocumentSelected(TSharedPtr<FDocumentDataResponse> SelectedItem,
                                                    ESelectInfo::Type SelectInfo)
{
	DocumentId = SelectedItem->Id;
}

FText SSelectorDocumentSubWindow::GetButtonLinkStateText() const
{
	return ButtonLinkStateText;
}

void SSelectorDocumentSubWindow::Init(FWordbeeUserData InUserData, const TArray<FDocumentDataResponse>& InDocumentData)
{
	DocumentDataArray = InDocumentData;
	UserData = InUserData;
	for (const FDocumentDataResponse& Doc : DocumentDataArray)
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

FReply SSelectorDocumentSubWindow::LinkAndCloseWindow()
{
	if (OnSubWindowClosed.IsBound())
	{
		if (!DocumentId.IsEmpty())
		{
			bIsReadyToLink = false;
			ButtonLinkStateText = FText::FromString("Linking...");
			ULinkDocumentCommand::Execute(UserData, DocumentId, FOnLinkDocumentComplete::CreateLambda(
                  [this](bool bSuccess, const FWordbeeDocument& Document)
                  {
                      if (bSuccess)
                      {
                          ButtonLinkStateText = FText::FromString("UnLink");
         //                  FString documentName = DocumentDataArray.FindByPredicate(
         //                      [&](const FDocumentDataResponse& Doc)
         //                      {
	        //                       return Doc.Id == DocumentId;
         //                      })->Name;
         //              		FString projectName = DocumentDataArray.FindByPredicate(
							  // [&](const FDocumentDataResponse& Doc)
							  // {
	        //                       return Doc.Id == DocumentId;
							  // })->Preference;
         //                  ULinkDocumentCommand::SaveDocument(
         //                      Document, ProjectId, projectName, documentName);
                          OnSubWindowClosed.Execute(true, ProjectId, DocumentId);
                      }
                      else
                      {
                          bIsReadyToLink = true;
                          ButtonLinkStateText = FText::FromString("Link Failed");
                      }
                  }));
		}
		else
		{
			bIsReadyToLink = true;
			OnSubWindowClosed.Execute(false, ProjectId, DocumentId);
		}
	}
	return FReply::Handled();
}

FReply SSelectorDocumentSubWindow::CloseWindow() const
{
	OnSubWindowClosed.Execute(false, ProjectId, DocumentId);
	return FReply::Handled();
}

bool SSelectorDocumentSubWindow::GetReadyLinkButtonState() const
{
	return bIsReadyToLink;
}

void SSelectorDocumentSubWindow::OnProjectSelected(TSharedPtr<FString> SelectedItem, ESelectInfo::Type SelectInfo)
{
	if (SelectedItem.IsValid())
	{
		FString projectName = *SelectedItem;
		// find project name by preference from DocumentDataArray
		int32 pId = DocumentDataArray.FindByPredicate([&](const FDocumentDataResponse& Doc)
		{
			return Doc.Preference == projectName;
		})->Pid;
		ProjectId = FString::FromInt(pId);
		UpdateFilteredDocuments(projectName);
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

TSharedRef<ITableRow> SSelectorDocumentSubWindow::GenerateDocumentRow(TSharedPtr<FDocumentDataResponse> InItem,
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

	for (const FDocumentDataResponse& Doc : DocumentDataArray)
	{
		if (Doc.Preference == SelectedPreference)
		{
			FilteredDocuments.Add(MakeShared<FDocumentDataResponse>(Doc));
		}
	}
}


END_SLATE_FUNCTION_BUILD_OPTIMIZATION
