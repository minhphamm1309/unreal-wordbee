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

class SEditorConfigWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SEditorConfigWidget) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	void StoreData();

private:
	void OnPushCheckboxChanged(ECheckBoxState CheckBoxState);
	bool bIsFetching = false;
	FWordbeeUserData UserInfo;
	FEditorConfig Config;
	TArray<TSharedPtr<FString>> TargetSyncOptions;
	TArray<TSharedPtr<FLanguageInfo>> MissingLocales;
	TArray<TSharedPtr<FLanguageInfo>> CommonLocales;
	TSharedPtr<SComboBox<TSharedPtr<FLanguageInfo>>> MissingLocalesCbo;
	TSharedPtr<SListView<TSharedPtr<FLanguageInfo>>> CommonLocalesListView;
	TSharedPtr<STileView<TSharedPtr<FLanguageInfo>>> LanguageTileView;
	TSharedPtr<SCheckBox> pushOnlyChangedCheckbox;
	
	FDelegateHandle WatcherHandle;
	void OnAutoSyncChanged(ECheckBoxState NewState);
	void OnSyncIntervalChanged(const FText& NewText, ETextCommit::Type CommitType);
	void OnTargetSyncChanged(TSharedPtr<FString> NewValue, ESelectInfo::Type);
	TSharedRef<SWidget> MakeComboWidget(TSharedPtr<FString> InOption);
	FReply OnSaveClicked();
	FReply OnCPullButtonClicked();
	FReply OnPushButtonClicked();
	TSharedPtr<FString> GetCurrentTargetSyncOption() const;
	void OnStringTableSelected(const FAssetData& AssetData);
	TSharedRef<SWidget>GenerateLanguageOption(TSharedPtr<FLanguageInfo> Item);
	void OnLanguageCheckboxChanged(ECheckBoxState NewState);
	TSharedRef<ITableRow> GenerateLanguageCheckbox(TSharedPtr<FLanguageInfo> Item, const TSharedRef<STableViewBase>& OwnerTable);
	TSharedPtr<SNotificationItem> NotificationPtr;
	FReply OnReloadLanguagesClicked();
	void FetchLangsFromAPI();
	void ShowNotification(const FString& Message, bool bSuccess);
	bool bAllLangsChecked = false;
};
