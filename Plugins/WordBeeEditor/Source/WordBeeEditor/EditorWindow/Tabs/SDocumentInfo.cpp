#include "SDocumentInfo.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Input/SButton.h"
#include "WordBeeEditor/API/API.h"
#include "WordBeeEditor/Utils/SingletonUtil.h"

void SDocumentInfo::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SBorder)
		.Padding(10)
		.BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
		.BorderBackgroundColor(FLinearColor(0.5f, 0.5f, 0.5f, 1.0f))
		[
			SNew(SVerticalBox)

			// Refresh Button
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Left)
			[
				SNew(SButton)
		             .Text(FText::FromString("Refresh"))
		             .IsEnabled_Lambda([this]() { return !bIsFetching; }) // Disable when fetching
		             .OnClicked_Lambda([this]() -> FReply
		             {
			             RefreshDocumentInfo();
			             return FReply::Handled();
		             })
			]

			// Flex Container Details Title
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(FMargin(0, 10, 0, 10))
			[
				SNew(STextBlock)
				.Text(FText::FromString("Flex Container Details"))
				.Font(FCoreStyle::Get().GetFontStyle("EmbossedText"))
			]

			// Document Details Grid
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SGridPanel)
				.FillColumn(1, 1.0f)
				+ SGridPanel::Slot(0, 0)
				.Padding(FMargin(0, 0, 10, 0))
				[
					SNew(STextBlock).Text(FText::FromString("Document ID:"))
				]
				+ SGridPanel::Slot(1, 0)
				[
					SNew(STextBlock)
					.Text_Lambda([this]() { return FText::FromString(DocumentID); }) // Dynamic binding
				]

				+ SGridPanel::Slot(0, 1)
				.Padding(FMargin(0, 0, 10, 0))
				[
					SNew(STextBlock).Text(FText::FromString("Document Name:"))
				]
				+ SGridPanel::Slot(1, 1)
				[
					SNew(STextBlock)
					.Text_Lambda([this]() { return FText::FromString(DocumentName); })
				]

				+ SGridPanel::Slot(0, 2)
				.Padding(FMargin(0, 0, 10, 0))
				[
					SNew(STextBlock).Text(FText::FromString("Source Language:"))
				]
				+ SGridPanel::Slot(1, 2)
				[
					SNew(STextBlock)
					.Text_Lambda([this]() { return FText::FromString(SourceLanguage); })
				]

				+ SGridPanel::Slot(0, 3)
				.Padding(FMargin(0, 0, 10, 0))
				[
					SNew(STextBlock).Text(FText::FromString("Target Languages:"))
				]
				+ SGridPanel::Slot(1, 3)
				[
					SNew(SBox)
					.MaxDesiredHeight(100) // Restrict height
					[
						SNew(SScrollBox)
						+ SScrollBox::Slot()
						[
							SNew(STextBlock)
							.Text_Lambda([this]()
							{
								return FText::FromString(FString::Join(TargetLanguages, TEXT(", ")));
							})
							.AutoWrapText(true)
						]
					]
				]

				+ SGridPanel::Slot(0, 4)
				.Padding(FMargin(0, 0, 10, 0))
				[
					SNew(STextBlock).Text(FText::FromString("Last TMS Change:"))
				]
				+ SGridPanel::Slot(1, 4)
				[
					SNew(STextBlock)
					.Text_Lambda([this]() { return FText::FromString(LastTMSChange); })
				]

				+ SGridPanel::Slot(0, 5)
				.Padding(FMargin(0, 0, 10, 0))
				[
					SNew(STextBlock).Text(FText::FromString("Last Synchronization:"))
				]
				+ SGridPanel::Slot(1, 5)
				[
					SNew(STextBlock)
					.Text_Lambda([this]() { return FText::FromString(LastSynchronization); })
				]
			]
		]
	];
}

void SDocumentInfo::RefreshDocumentInfo()
{
	if (bIsFetching)
	{
		return; // Prevent multiple fetches
	}
	bIsFetching = true;
	FWordbeeUserData userInfo = SingletonUtil::GetFromIni<FWordbeeUserData>();
	API::FetchDocumentById(userInfo, FString::FromInt(userInfo.DocumentId), [this](const FDocumentInfo& DocumentInfo)
	   {
	       DocumentID = FString::FromInt(DocumentInfo.Id);
	       DocumentName = DocumentInfo.Name;
	       SourceLanguage = FString::Printf(
	           TEXT("%s (%s)"), *DocumentInfo.Src.T, *DocumentInfo.Src.V);
	       TargetLanguages.Empty();
	       for (const FLanguageInfo& Lang : DocumentInfo.Trgs)
	       {
	           TargetLanguages.Add(FString::Printf(TEXT("%s (%s)"), *Lang.T, *Lang.V));
	       }
	       bIsFetching = false;
	   },
	   [this](const FString& ErrorMessage)
	   {
	       bIsFetching = false;
	       FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(ErrorMessage));
	   });
}
