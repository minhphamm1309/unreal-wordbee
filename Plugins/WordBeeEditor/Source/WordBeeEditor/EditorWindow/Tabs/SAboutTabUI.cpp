#include "SAboutTabUI.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/SToolTip.h"
#include "Misc/EngineVersion.h"
#include "HAL/PlatformApplicationMisc.h"
const FString SAboutTabUI::DocumentationUrl = TEXT("https://help.wordbee.com/Link/wordbee-link-for-unity");
const FString SAboutTabUI::TutorialsUrl = TEXT("https://help.wordbee.com/Link/wordbee-link-for-unity");
const FString SAboutTabUI::LegalPolicyUrl = TEXT("https://www.wordbee.com/privacy");
const FString SAboutTabUI::ReleaseNotesUrl = TEXT("https://help.wordbee.com/Link/release-history");
const FString SAboutTabUI::WordbeeWebsiteUrl = TEXT("https://wordbee.com");

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
                    .Text(FText::FromString("@2023, Wordbee, All Rights Reserved"))
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
    return OpenLinkWithConfirmation(TEXT("Documentation & online help"), DocumentationUrl);
}

FReply SAboutTabUI::OnTutorialsClicked()
{
    return OpenLinkWithConfirmation(TEXT("Tutorials"), TutorialsUrl);
}

FReply SAboutTabUI::OnLegalClicked()
{
    return OpenLinkWithConfirmation(TEXT("Legal policy page"), LegalPolicyUrl);
}

FReply SAboutTabUI::OnReleaseNotesClicked()
{
    return OpenLinkWithConfirmation(TEXT("Release notes"), ReleaseNotesUrl);
}

FReply SAboutTabUI::OnWebsiteClicked()
{
    return OpenLinkWithConfirmation(TEXT("Wordbee website"), WordbeeWebsiteUrl);
}
FReply SAboutTabUI::OpenLinkWithConfirmation(const FString& LinkName, const FString& Url)
{
    const FText Title = FText::FromString(TEXT("Open External Link"));
    const FText Message = FText::Format(
        FText::FromString("You are about to open {0} in your browser.\n\nDo you want to continue?"),
        FText::FromString(LinkName)
    );

    if (FMessageDialog::Open(EAppMsgType::YesNo, Message) == EAppReturnType::Yes)
    {
        FPlatformProcess::LaunchURL(*Url, nullptr, nullptr);
    }

    return FReply::Handled();
}


