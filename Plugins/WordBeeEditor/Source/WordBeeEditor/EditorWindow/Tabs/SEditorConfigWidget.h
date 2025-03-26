#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "WordBeeEditor/Models/FEditorConfig.h"

class SEditorConfigWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SEditorConfigWidget) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	FEditorConfig Config;
	TArray<TSharedPtr<FString>> TargetSyncOptions;

	void OnAutoSyncChanged(ECheckBoxState NewState);
	void OnSyncIntervalChanged(const FText& NewText, ETextCommit::Type CommitType);
	void OnTargetSyncChanged(TSharedPtr<FString> NewValue, ESelectInfo::Type);
	TSharedRef<SWidget> MakeComboWidget(TSharedPtr<FString> InOption);
	FReply OnSaveClicked();
	TSharedPtr<FString> GetCurrentTargetSyncOption() const;
	void OnStringTableSelected(const FAssetData& AssetData);
};
