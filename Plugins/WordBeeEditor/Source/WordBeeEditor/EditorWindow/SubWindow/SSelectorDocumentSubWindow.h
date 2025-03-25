#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "WordBeeEditor/Command/DocumentList/FDocumentData.h"

class UUserData;
DECLARE_DELEGATE_OneParam(FOnSubWindowClosedDelegate, FString);
class SSelectorDocumentSubWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SSelectorDocumentSubWindow) {}
		SLATE_EVENT(FOnSubWindowClosedDelegate, OnSubWindowClosed)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	void Init(UUserData* InUserData, const TArray<FDocumentData>& InDocumentData);
	FReply CloseWindow() const;

private:
	UUserData* UserData = nullptr;
	TArray<FDocumentData> DocumentDataArray;
	TArray<TSharedPtr<FString>> Preferences;

	TArray<TSharedPtr<FDocumentData>> FilteredDocuments;

	void OnProjectSelected(TSharedPtr<FString> SelectedItem , ESelectInfo::Type SelectInfo);
	void OnDocumentSelected(TSharedPtr<FDocumentData> SelectedItem, ESelectInfo::Type SelectInfo);

	// Widget layout
	TSharedPtr<SListView<TSharedPtr<FString>>> ProjectListView;
	TSharedPtr<SListView<TSharedPtr<FDocumentData>>> DocumentListView;

	TSharedRef<ITableRow> GenerateProjectRow(TSharedPtr<FString> InItem, const TSharedRef<STableViewBase>& OwnerTable);
	TSharedRef<ITableRow> GenerateDocumentRow(TSharedPtr<FDocumentData> InItem, const TSharedRef<STableViewBase>& OwnerTable);

	void UpdateFilteredDocuments(const FString& SelectedPreference);

	
	FOnSubWindowClosedDelegate OnSubWindowClosed;

	FString DocumentId;
};

