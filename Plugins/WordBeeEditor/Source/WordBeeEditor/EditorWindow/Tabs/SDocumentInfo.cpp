#include "SDocumentInfo.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Input/SButton.h"
#include "WordBeeEditor/API/API.h"
#include "WordBeeEditor/Utils/SingletonUtil.h"
#include "WordBeeEditor/EditorWindow/WordBeeEditorConfigWindow.h"

TSharedPtr<FDocumentInfo> SDocumentInfo::CachedDocumentInfo = nullptr;
void SDocumentInfo::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SBorder)
		.Padding(20) // Increased padding for breathing space
		.BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
		.BorderBackgroundColor(FLinearColor(0.5f, 0.5f, 0.5f, 1.0f))
		[
			SNew(SVerticalBox)

			// Refresh Button
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0, 0, 0, 10)  // Margin under the button
			.HAlign(HAlign_Left)
			[
				SNew(SButton)
				.Text(FText::FromString("Refresh"))
				.IsEnabled_Lambda([this]() { return !bIsFetching; })
				.OnClicked_Lambda([this]() -> FReply
				{
					NotifyParent();
					return FReply::Handled();
				})
			]

			// Title
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0, 0, 0, 15)  // Larger space under the title
			[
				SNew(STextBlock)
				.Text(FText::FromString("Flex Container Details"))
				.Font(FCoreStyle::Get().GetFontStyle("EmbossedText"))
			]

			// Info Grid
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SBox)
				.MaxDesiredWidth(600) // Limit width to prevent it from stretching too wide
				[
					SNew(SGridPanel)
					.FillColumn(1, 1.0f)

					+ SGridPanel::Slot(0, 0).Padding(5)[ SNew(STextBlock).Text(FText::FromString("Document ID:")) ]
					+ SGridPanel::Slot(1, 0).Padding(5)[ SNew(STextBlock).Text_Lambda([this]()
						{
							return DocumentID.IsEmpty() 
								? FText::FromString(TEXT("[Please link a document first to view info.]")) 
								: FText::FromString(DocumentID);
						})
					]
					+ SGridPanel::Slot(0, 1).Padding(5)[ SNew(STextBlock).Text(FText::FromString("Document Name:")) ]
					+ SGridPanel::Slot(1, 1).Padding(5)[ SNew(STextBlock).Text_Lambda([this]() { return FText::FromString(DocumentName); }) ]

					+ SGridPanel::Slot(0, 2).Padding(5)[ SNew(STextBlock).Text(FText::FromString("Source Language:")) ]
					+ SGridPanel::Slot(1, 2).Padding(5)[ SNew(STextBlock).Text_Lambda([this]() { return FText::FromString(SourceLanguage); }) ]

					+ SGridPanel::Slot(0, 3).Padding(5)[ SNew(STextBlock).Text(FText::FromString("Target Languages:")) ]
					+ SGridPanel::Slot(1, 3).Padding(5)
					[
						SNew(SBox)
						.MaxDesiredHeight(100)
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

					+ SGridPanel::Slot(0, 4).Padding(5)[ SNew(STextBlock).Text(FText::FromString("Last TMS Change:")) ]
					+ SGridPanel::Slot(1, 4).Padding(5)[ SNew(STextBlock).Text_Lambda([this]() { return FText::FromString(LastTMSChange); }) ]

					+ SGridPanel::Slot(0, 5).Padding(5)[ SNew(STextBlock).Text(FText::FromString("Last Synchronization:")) ]
					+ SGridPanel::Slot(1, 5).Padding(5)[ SNew(STextBlock).Text_Lambda([this]() { return FText::FromString(LastSynchronization); }) ]
				]
			]
		]
	];
}

void SDocumentInfo::RefreshDocumentInfo(TFunction<void(bool)> OnFinish)
{
	if (bIsFetching)
	{
		return; // Prevent multiple fetches
	}
	FWordbeeUserData userInfo = wordbee::SingletonUtil<FWordbeeUserData>::GetFromIni();
	if (!userInfo.IsValid())
	{
		ClearDocumentInfo();
		if (OnFinish) OnFinish(false);
		return;
	}
	bIsFetching = true;
	API::FetchDocumentById(userInfo, FString::FromInt(userInfo.DocumentId), [this, OnFinish](const FDocumentInfo& DocumentInfo)
	   {
			CachedDocumentInfo = MakeShared<FDocumentInfo>(DocumentInfo);
	       DocumentID = FString::FromInt(DocumentInfo.Id);
	       DocumentName = DocumentInfo.Name;
	       SourceLanguage = FString::Printf(
	           TEXT("%s (%s)"), *DocumentInfo.Src.T, *DocumentInfo.Src.V);
	       TargetLanguages.Empty();
	       for (const FLanguageInfo& Lang : DocumentInfo.Trgs)
	       {
	           TargetLanguages.Add(FString::Printf(TEXT("%s (%s)"), *Lang.T, *Lang.V));
	       }
			LastTMSChange = DocumentInfo.DtChange;
	       bIsFetching = false;
		if (OnFinish) OnFinish(true); 
	   },
	   [this, OnFinish](const FString& ErrorMessage)
		{
			bIsFetching = false;
			if (OnFinish) OnFinish(false);  // Failure
		});
}

void SDocumentInfo::ClearDocumentInfo()
{
	CachedDocumentInfo.Reset();
	DocumentID.Empty();
	DocumentName.Empty();
	SourceLanguage.Empty();
	TargetLanguages.Empty();
	LastTMSChange.Empty();
}

void SDocumentInfo::SetParentWindow(TWeakPtr<SWordBeeEditorConfigWindow> InParent)
{
	ParentWindow = InParent;
}

void SDocumentInfo::NotifyParent()
{
	if (ParentWindow.IsValid())
	{
		TSharedPtr<SWordBeeEditorConfigWindow> Parent = ParentWindow.Pin();
		Parent->OnDocumentChanged();
	}
}