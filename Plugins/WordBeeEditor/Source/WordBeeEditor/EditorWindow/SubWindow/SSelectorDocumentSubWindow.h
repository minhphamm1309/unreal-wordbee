#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "WordBeeEditor/Command/DocumentList/FDocumentDataResponse.h"
#include "WordBeeEditor/Models/WordbeeUserData.h"

struct FDocument;
class WordbeeUUserData;
DECLARE_DELEGATE_ThreeParams(FOnSubWindowClosedDelegate, bool /*linked*/, FString /*project id*/ , FString /*document id*/);
class SSelectorDocumentSubWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SSelectorDocumentSubWindow) {}
		SLATE_EVENT(FOnSubWindowClosedDelegate, OnSubWindowClosed)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	void Init(FWordbeeUserData InUserData, const TArray<FDocumentDataResponse>& InDocumentData);
	FReply LinkAndCloseWindow();
	FReply CloseWindow() const;
	bool GetReadyLinkButtonState() const;

private:
	FWordbeeUserData UserData;
	TArray<FDocumentDataResponse> DocumentDataArray;
	TArray<TSharedPtr<FString>> Preferences;

	TArray<TSharedPtr<FDocumentDataResponse>> FilteredDocuments;

	void OnProjectSelected(TSharedPtr<FString> SelectedItem , ESelectInfo::Type SelectInfo);
	void OnDocumentSelected(TSharedPtr<FDocumentDataResponse> SelectedItem, ESelectInfo::Type SelectInfo);
	FText GetButtonLinkStateText() const;
	// Widget layout
	TSharedPtr<SListView<TSharedPtr<FString>>> ProjectListView;
	TSharedPtr<SListView<TSharedPtr<FDocumentDataResponse>>> DocumentListView;

	TSharedRef<ITableRow> GenerateProjectRow(TSharedPtr<FString> InItem, const TSharedRef<STableViewBase>& OwnerTable);
	TSharedRef<ITableRow> GenerateDocumentRow(TSharedPtr<FDocumentDataResponse> InItem, const TSharedRef<STableViewBase>& OwnerTable);

	void UpdateFilteredDocuments(const FString& SelectedPreference);
	FText ButtonLinkStateText;
	bool bIsReadyToLink {true};
	

	
	FOnSubWindowClosedDelegate OnSubWindowClosed;

	FString DocumentId;
	FString ProjectId;
};


