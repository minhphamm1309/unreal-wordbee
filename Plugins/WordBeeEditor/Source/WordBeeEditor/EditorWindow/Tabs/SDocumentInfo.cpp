#include "SDocumentInfo.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Input/SButton.h"

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
                .OnClicked_Lambda([]() -> FReply { return FReply::Handled(); }) // Placeholder functionality
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
                    SNew(STextBlock).Text(FText::FromString(DocumentID))
                ]

                // Document Name
                + SUniformGridPanel::Slot(0, 1)
                [
                    SNew(STextBlock).Text(FText::FromString("Document Name:"))
                ]
                + SUniformGridPanel::Slot(1, 1)
                [
                    SNew(STextBlock).Text(FText::FromString(DocumentName))
                ]

                // Source Language
                + SUniformGridPanel::Slot(0, 2)
                [
                    SNew(STextBlock).Text(FText::FromString("Source Language:"))
                ]
                + SUniformGridPanel::Slot(1, 2)
                [
                    SNew(STextBlock).Text(FText::FromString(SourceLanguage))
                ]

                // Target Languages
                + SUniformGridPanel::Slot(0, 3)
                [
                    SNew(STextBlock).Text(FText::FromString("Target Language:"))
                ]
                + SUniformGridPanel::Slot(1, 3)
                [
                    SNew(STextBlock).Text(FText::FromString(TargetLanguagesStr))
                ]

                // Last TMS Change
                + SUniformGridPanel::Slot(0, 4)
                [
                    SNew(STextBlock).Text(FText::FromString("Last TMS Change:"))
                ]
                + SUniformGridPanel::Slot(1, 4)
                [
                    SNew(STextBlock).Text(FText::FromString(LastTMSChange))
                ]

                // Last Synchronization
                + SUniformGridPanel::Slot(0, 5)
                [
                    SNew(STextBlock).Text(FText::FromString("Last Synchronization:"))
                ]
                + SUniformGridPanel::Slot(1, 5)
                [
                    SNew(STextBlock).Text(FText::FromString(LastSynchronization))
                ]
            ]
        ]
    ];
}
