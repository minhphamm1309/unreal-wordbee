#include "SAboutTabUI.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/SToolTip.h"
#include "Misc/EngineVersion.h"
#include "HAL/PlatformApplicationMisc.h"

void SAboutTabUI::Construct(const FArguments& InArgs)
{
    ChildSlot
    [
        SNew(SVerticalBox)

        // About Header
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(STextBlock)
            .Text(FText::FromString("About"))
            .Font(FCoreStyle::GetDefaultFontStyle("Bold", 14))
        ]

        // Version and Copyright
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(5)
        [
            SNew(SBox)
            .Padding(10)
            [
                SNew(SVerticalBox)
                
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(STextBlock)
                    .Text(FText::FromString("Version and Copyright"))
                    .Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
                ]
                
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(STextBlock)
                    .Text(FText::FromString("Wordbee Link v1.2.83-staging"))
                ]
                
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(STextBlock)
                    .Text(FText::FromString("©2023, Wordbee, All Rights Reserved"))
                ]
            ]
        ]

        // Links Section
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(5)
        [
            SNew(SBox)
            .Padding(10)
            [
                SNew(SVerticalBox)

                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(STextBlock)
                    .Text(FText::FromString("Links"))
                    .Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
                ]

                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(SButton)
                    .Text(FText::FromString("> Documentation & online help"))
                    .OnClicked(this, &SAboutTabUI::OnDocumentationClicked)
                ]

                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(SButton)
                    .Text(FText::FromString("> Tutorials"))
                    .OnClicked(this, &SAboutTabUI::OnTutorialsClicked)
                ]

                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(SButton)
                    .Text(FText::FromString("> Legal policy page"))
                    .OnClicked(this, &SAboutTabUI::OnLegalClicked)
                ]

                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(SButton)
                    .Text(FText::FromString("> Release notes"))
                    .OnClicked(this, &SAboutTabUI::OnReleaseNotesClicked)
                ]

                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(SButton)
                    .Text(FText::FromString("> Wordbee website"))
                    .OnClicked(this, &SAboutTabUI::OnWebsiteClicked)
                ]
            ]
        ]
    ];
}

FReply SAboutTabUI::OnDocumentationClicked()
{
    FPlatformProcess::LaunchURL(TEXT("https://documentation.url"), nullptr, nullptr);
    return FReply::Handled();
}

FReply SAboutTabUI::OnTutorialsClicked()
{
    FPlatformProcess::LaunchURL(TEXT("https://tutorials.url"), nullptr, nullptr);
    return FReply::Handled();
}

FReply SAboutTabUI::OnLegalClicked()
{
    FPlatformProcess::LaunchURL(TEXT("https://legal.url"), nullptr, nullptr);
    return FReply::Handled();
}

FReply SAboutTabUI::OnReleaseNotesClicked()
{
    FPlatformProcess::LaunchURL(TEXT("https://releasenotes.url"), nullptr, nullptr);
    return FReply::Handled();
}

FReply SAboutTabUI::OnWebsiteClicked()
{
    FPlatformProcess::LaunchURL(TEXT("https://wordbee.com"), nullptr, nullptr);
    return FReply::Handled();
}
