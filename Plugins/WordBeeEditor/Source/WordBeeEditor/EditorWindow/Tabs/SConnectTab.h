#pragma once
#include "WordBeeEditor/Command/DocumentList/FDocumentDataResponse.h"
#include "WordBeeEditor/Models/WordbeeUserData.h"

class SWordBeeEditorConfigWindow;
class SConnectTab : public SCompoundWidget 
{
public:
	SLATE_BEGIN_ARGS(SConnectTab) {}
	SLATE_END_ARGS()
	
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	void SetParentWindow(TWeakPtr<SWordBeeEditorConfigWindow> InParent);
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
	TArray<FDocumentDataResponse> DocumentsData;
	
	FWordbeeUserData UserInfo;
	bool bConnecting = false;
	FText ButtonConnectStateText;
	bool bIsAuthenticated = false;
	bool bIsDocumentsFetched = false;
	bool bIsLinkReadyToClick{true};

	FText GetLinkButtonText() const;
	void SetConnectingState(bool bConnecting);
	FText GetButtonText() const;
	bool IsConnectButtonEnabled() const;

	void OnCompletedAuth(FString ResponseString);
	bool HasAuthenticatingCredentials() const;
	void SaveSettings();
	void LoadSettings() const;
	void LoadDocumentSettings();
	bool IsSelectDocsReady() const;
	void SetDocumentsFetched();
	
	void OnFetchDocumentsResponseReceived(const TArray<FDocumentDataResponse>& response);
	void OnSubWindowClosed(bool isLinked, FString InProjectId, FString InDocumentId);
	void OnSubWindowClosed(bool isLinked, FString InProjectId, FString InDocumentId) const;
	TWeakPtr<SWordBeeEditorConfigWindow> ParentWindow;
	void NotifyParent();
};



