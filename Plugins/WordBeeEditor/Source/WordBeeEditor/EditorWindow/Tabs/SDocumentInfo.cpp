#include "SDocumentInfo.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Input/SButton.h"
#include "WordBeeEditor/API/API.h"
#include "WordBeeEditor/Utils/SingletonUtil.h"

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
				// Document ID
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

				// Document Name
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


				// Source Language
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

				// Target Languages
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


				// Last TMS Change
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


				// Last Synchronization
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
	FWordbeeUserData userInfo = SingletonUtil::GetFromIni<FWordbeeUserData>();
	API::FetchDocumentById(userInfo, FString::FromInt(userInfo.DocumentId), [this](const FDocumentInfo& DocumentInfo)
	{
		DocumentID = FString::FromInt(DocumentInfo.Id);
		DocumentName = DocumentInfo.Name;
		SourceLanguage = FString::Printf(TEXT("%s (%s)"), *DocumentInfo.Src.T, *DocumentInfo.Src.V);
		// Clear previous target languages
		TargetLanguages.Empty();
		// Convert target languages to a string
		for (const FLanguageInfo& Lang : DocumentInfo.Trgs)
		{
			TargetLanguages.Add(FString::Printf(TEXT("%s (%s)"), *Lang.T, *Lang.V));
		}
	});
}
