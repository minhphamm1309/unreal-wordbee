#pragma once
#include "WordBeeEditor/Command/DocumentList/FDocumentData.h"
#include "WordBeeEditor/Command/CreateDataAsset/UserData.h"
#include "WordBeeEditor/Command/DocumentList/FDocumentData.h"


class SConnectTab : public SCompoundWidget 
{
public:
	SLATE_BEGIN_ARGS(SConnectTab) {}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

private:
	// Add functions for button click, etc.
	FReply OnConnectButtonClicked();

	FReply OnSelectDocumentClicked();
	FReply OnLinkDocumentClicked();

	// Variables to hold the values entered into text boxes
	TSharedPtr<SEditableTextBox> URL;
	TSharedPtr<SEditableTextBox> AccountId;
	TSharedPtr<SEditableTextBox> APIKey;
	TSharedPtr<SEditableTextBox> DocumentId;
	TSharedPtr<SBorder> ConnectionPanel;
	TSharedPtr<SBorder> LinkPanel;
	TSharedPtr<SBox> SubWindow;
	TSharedPtr<STextBlock> ResponseTextBlock;
	TArray<FDocumentData> DocumentsData;
	
	UUserData* UserInfo;
	bool bConnecting = false;
	FText ButtonConnectStateText;
	bool bIsAuthenticated = false;
	bool bIsDocumentsFetched = false;
	
	void SetConnectingState(bool bConnecting);
	FText GetButtonText() const;
	bool IsConnectButtonEnabled() const;

	void OnCompletedAuth(FString ResponseString);
	bool HasAuthenticatingCredentials() const;
	void SaveSettings();
	void LoadSettings() const;
	void LoadDocumentSettings();
	bool HasDocumentsFetched() const;
	void SetDocumentsFetched();
	
	void OnFetchDocumentsResponseReceived( const TArray<FDocumentData>&);
	void OnSubWindowClosed(FString InDocumentId);
};



