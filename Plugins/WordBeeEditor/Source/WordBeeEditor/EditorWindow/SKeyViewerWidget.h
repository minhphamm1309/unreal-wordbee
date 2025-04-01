#pragma once

#include "CoreMinimal.h"
#include "SSearchableComboBox.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/SListView.h"
#include "WordBeeEditor/Models/FCustomField.h"
#include "WordBeeEditor/Models/WordbeeResponse.h"
#include "WordBeeEditor/Models/WordbeeUserData.h"

// SKeyViewerWidget Class
class SKeyViewerWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SKeyViewerWidget) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	FWordbeeUserData userinfo;
	FString EnteredKey;
	FText MinLength;
	FText MaxLength;
	TArray<TSharedPtr<FCustomField>> CustomFields;
	void OnMinLengthChanged(const FText& NewText);
	void OnMaxLengthChanged(const FText& NewText);
	void OnVTextCommitted(const FText& NewText, ETextCommit::Type CommitType, TSharedPtr<FCustomField> Item);
	TSharedRef<ITableRow> OnGenerateRow(TSharedPtr<FCustomField> Item, const TSharedRef<STableViewBase>& OwnerTable);
	TArray<TSharedPtr<FString>> AvailableStringTables;
	TArray<TSharedPtr<FString>> AvailableKeys;

	TSharedPtr<FString> SelectedStringTable;
	TSharedPtr<FString> SelectedKey;

	TSharedPtr<SSearchableComboBox> StringTableComboBox;
	TSharedPtr<SSearchableComboBox> KeyComboBox;
	TSharedPtr<SListView<TSharedPtr<FCustomField>>> CustomFieldsListView;

	void PopulateStringTables();
	void PopulateLocalizationKeys();

	void OnStringTableSelected(TSharedPtr<FString> NewValue, ESelectInfo::Type);
	void OnKeySelected(TSharedPtr<FString> NewValue, ESelectInfo::Type);
	static void AddCustomFieldsFromSegment(const FWordbeeDocument& Doc, TArray<TSharedPtr<FCustomField>>& OutCustomFields);
	void OnKeyChanged(const FText& NewText);
	FReply OnGetDataClicked();
};
