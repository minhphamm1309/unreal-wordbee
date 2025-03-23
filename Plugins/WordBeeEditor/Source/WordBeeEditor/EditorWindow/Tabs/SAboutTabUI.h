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
};
