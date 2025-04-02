#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class SDocumentInfo : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SDocumentInfo) {}
        SLATE_ARGUMENT(FString, DocumentID)
        SLATE_ARGUMENT(FString, DocumentName)
        SLATE_ARGUMENT(FString, SourceLanguage)
        SLATE_ARGUMENT(TArray<FString>, TargetLanguages)
        SLATE_ARGUMENT(FString, LastTMSChange)
        SLATE_ARGUMENT(FString, LastSynchronization)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:
    bool bIsFetching = false;
    FString DocumentID;
    FString DocumentName;
    FString SourceLanguage;
    TArray<FString> TargetLanguages;
    FString LastTMSChange;
    FString LastSynchronization;
    void RefreshDocumentInfo();
};
