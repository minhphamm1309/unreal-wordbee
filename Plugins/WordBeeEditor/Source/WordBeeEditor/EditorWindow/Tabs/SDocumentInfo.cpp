#include "SDocumentInfo.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Input/SButton.h"
#include "WordBeeEditor/API/API.h"

void SDocumentInfo::Construct(const FArguments& InArgs)
{
	DocumentID = InArgs._DocumentID;
	DocumentName = InArgs._DocumentName;
	SourceLanguage = InArgs._SourceLanguage;
	TargetLanguages = InArgs._TargetLanguages;
	LastTMSChange = InArgs._LastTMSChange;
	LastSynchronization = InArgs._LastSynchronization;

	// Convert target languages array to a single string
	FString TargetLanguagesStr = FString::Join(TargetLanguages, TEXT(", "));

	ChildSlot
	[
		SNew(SBorder)
		.Padding(10)
		.BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
		[
			SNew(SVerticalBox)

			// Refresh Button
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Left)
			[
				SNew(SButton)
				.Text(FText::FromString("Refresh"))
				.OnClicked_Lambda([this]() -> FReply
				{
					RefreshDocumentInfo();
					return FReply::Handled();
				})
			]

			// Flex Container Details Title
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(FMargin(0, 10, 0, 5))
			[
				SNew(STextBlock)
				.Text(FText::FromString("Flex Container Details"))
				.Font(FCoreStyle::Get().GetFontStyle("EmbossedText"))
			]

			// Document Details Grid
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SUniformGridPanel)
				.SlotPadding(FMargin(5))

				// Document ID
				+ SUniformGridPanel::Slot(0, 0)
				[
					SNew(STextBlock).Text(FText::FromString("Document ID:"))
				]
				+ SUniformGridPanel::Slot(1, 0)
				[
					SNew(STextBlock)
					.Text_Lambda([this]() { return FText::FromString(DocumentID); }) // Dynamic binding
				]

				// Document Name
				+ SUniformGridPanel::Slot(0, 1)
				[
					SNew(STextBlock).Text(FText::FromString("Document Name:"))
				]
				+ SUniformGridPanel::Slot(1, 1)
				[
					SNew(STextBlock)
					.Text_Lambda([this]() { return FText::FromString(DocumentName); })
				]


				// Source Language
				+ SUniformGridPanel::Slot(0, 2)
				[
					SNew(STextBlock).Text(FText::FromString("Source Language:"))
				]
				+ SUniformGridPanel::Slot(1, 2)
				[
					SNew(STextBlock)
					.Text_Lambda([this]() { return FText::FromString(SourceLanguage); })
				]

				// Target Languages
				+ SUniformGridPanel::Slot(0, 3)
				[
					SNew(STextBlock).Text(FText::FromString("Target Language:"))
				]
				+ SUniformGridPanel::Slot(1, 3)
				[
					SNew(STextBlock)
					.Text_Lambda([this]() { return FText::FromString(FString::Join(TargetLanguages, TEXT(", "))); })
				]

				// Last TMS Change
				+ SUniformGridPanel::Slot(0, 4)
				[
					SNew(STextBlock).Text(FText::FromString("Last TMS Change:"))
				]
				+ SUniformGridPanel::Slot(1, 4)
				[
					SNew(STextBlock)
					.Text_Lambda([this]() { return FText::FromString(LastTMSChange); })
				]


				// Last Synchronization
				+ SUniformGridPanel::Slot(0, 5)
				[
					SNew(STextBlock).Text(FText::FromString("Last Synchronization:"))
				]
				+ SUniformGridPanel::Slot(1, 5)
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
	UserInfo userInfo;
	userInfo.AccountId = "gcc-tpt";
	userInfo.ApiKey = "80e7bdc7-e521-4739-ba1d-2b53dc34b837";
	userInfo.AuthToken = "90d97e6c-c57e-45b3-bb6e-86158968d625";
	userInfo.BaseUrl = "eu.wordbee-translator.com";
	UAPI::FetchDocumentById(userInfo, "9395", [this](const FDocumentInfo& DocumentInfo)
	{
		// Directly update UI properties (since it's an editor plugin, no need for GameThread)
		DocumentID = FString::FromInt(DocumentInfo.Id);
		DocumentName = DocumentInfo.Name;
		SourceLanguage = DocumentInfo.Src.V;
		// Convert target languages to a string
		for (const FLanguageInfo& Lang : DocumentInfo.Trgs)
		{
			TargetLanguages.Add(Lang.V);
		}
	});
}
