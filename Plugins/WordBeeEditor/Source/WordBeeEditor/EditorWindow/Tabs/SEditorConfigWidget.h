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
#include "Widgets/Views/STileView.h"
#include "WordBeeEditor/Models/FDocumentInfo.h"
#include "WordBeeEditor/Models/FEditorConfig.h"
#include "WordBeeEditor/Models/WordbeeUserData.h"

struct FTargetSyncOption;

class SEditorConfigWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SEditorConfigWidget) {}
	SLATE_END_ARGS()
	void Construct(const FArguments& InArgs);
	void FetchLangsFromAPI();
private:
	bool bIsSyncInProgress = false;
	FWordbeeUserData UserInfo;
	FEditorConfig Config;
	TArray<TSharedPtr<FTargetSyncOption>> TargetSyncOptions;
	TArray<TSharedPtr<FLanguageInfo>> MissingLocales;
	TArray<TSharedPtr<FLanguageInfo>> CommonLocales;
	TSharedPtr<SComboBox<TSharedPtr<FLanguageInfo>>> MissingLocalesCbo;
	TSharedPtr<STileView<TSharedPtr<FLanguageInfo>>> CommonLocalesTileView;
	TSharedPtr<SCheckBox> pushOnlyChangedCheckbox;
	FDelegateHandle WatcherHandle;
	void OnAutoSyncChanged(ECheckBoxState NewState);
	void OnSyncIntervalChanged(const FText& NewText, ETextCommit::Type CommitType);
	void OnTargetSyncChanged(TSharedPtr<FTargetSyncOption> NewValue, ESelectInfo::Type);
	TSharedRef<SWidget> MakeComboWidget(TSharedPtr<FTargetSyncOption> InOption);
	FReply OnSaveClicked();
	FReply OnCPullButtonClicked();
	FReply OnPushButtonClicked();
	TSharedPtr<FTargetSyncOption> GetCurrentTargetSyncOption() const;
	TSharedRef<SWidget>GenerateLanguageOption(TSharedPtr<FLanguageInfo> Item);
	void OnLanguageCheckboxChanged(ECheckBoxState NewState);
	TSharedPtr<SNotificationItem> NotificationPtr;
	void ShowNotification(const FString& Message, bool bSuccess);
	bool bAllLangsChecked = false;
	TSharedRef<ITableRow> GenerateLanguageTile(TSharedPtr<FLanguageInfo> Item, const TSharedRef<STableViewBase>& OwnerTable);
	TSharedPtr<FDocumentInfo> DocInfo = nullptr;
};

struct FTargetSyncOption
{
	FString DisplayName;
	FString Value;
	FTargetSyncOption(const FString& InDisplayName, const FString& InValue)
		: DisplayName(InDisplayName), Value(InValue) {}
};

