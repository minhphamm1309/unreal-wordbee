#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class SAboutTabUI : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SAboutTabUI) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:
    FReply OnDocumentationClicked();
    FReply OnTutorialsClicked();
    FReply OnLegalClicked();
    FReply OnReleaseNotesClicked();
    FReply OnWebsiteClicked();
    static const FString DocumentationUrl;
    static const FString TutorialsUrl;
    static const FString LegalPolicyUrl;
    static const FString ReleaseNotesUrl;
    static const FString WordbeeWebsiteUrl;
    FReply OpenLinkWithConfirmation(const FString& LinkName, const FString& Url);
};
