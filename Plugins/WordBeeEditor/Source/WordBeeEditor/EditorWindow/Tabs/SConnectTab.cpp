#include "SConnectTab.h"

#include "WordBeeEditor/API/API.h"
#include "WordBeeEditor/Command/DocumentList/UGetDocumentsCommand.h"
#include "WordBeeEditor/EditorWindow/SubWindow/SSelectorDocumentSubWindow.h"
#include "WordBeeEditor/Utils/SingletonUtil.h"
#include "WordBeeEditor/EditorWindow/WordBeeEditorConfigWindow.h"
#include "WordBeeEditor/Models/FDocumentData.h"

void SConnectTab::Construct(const FArguments& InArgs)
{
	UserInfo = wordbee::SingletonUtil<FWordbeeUserData>::GetFromIni();
	bIsAuthenticated = !UserInfo.AuthToken.IsEmpty();
	SetConnectingState(false);
	ChildSlot
	[
		SNew(SVerticalBox)

		// Connect panel
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SAssignNew(ConnectionPanel, SBorder)
			[
				SNew(SVerticalBox)
				// IP Address Label and TextBox
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(5)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.FillWidth(1.0f)
					.Padding(5)
					[
						SNew(STextBlock)
						.Text(FText::FromString("URL:"))
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(5)
					[
						SNew(SButton)
						             .Text(this, &SConnectTab::GetButtonText)
 // Bind the button text to the current state
						             .IsEnabled(this, &SConnectTab::IsConnectButtonEnabled) // Bind button enabled state
						             .OnClicked(this, &SConnectTab::OnConnectButtonClicked)
					]
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(5)
				[
					SAssignNew(URL, SEditableTextBox)
					.HintText(FText::FromString("Enter url"))
				]

				// Port Label and TextBox
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(5)
				[
					SNew(STextBlock)
					.Text(FText::FromString("Account ID:"))
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(5)
				[
					SAssignNew(AccountId, SEditableTextBox)
					.HintText(FText::FromString("Enter Account ID"))
				]

				// API Key Label and TextBox
				// Port Label and TextBox
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(5)
				[
					SNew(STextBlock)
					.Text(FText::FromString("API key:"))
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(5)
				[
					SAssignNew(APIKey, SEditableTextBox)
					.HintText(FText::FromString("Enter API key"))
				]

				// AuthenId Label
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(5)
				[
					SAssignNew(ResponseTextBlock, STextBlock)
					                                         .Text(FText::GetEmpty()) // Initially empty
					                                         .Visibility(EVisibility::Collapsed) // Initially hidden
				]
			]
		]

		// Subwindow
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SAssignNew(SubWindow, SBox)
		]

		// Document panel
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(5)
		[
			SAssignNew(LinkPanel, SBorder)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(5)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.FillWidth(1.0f)
					.Padding(5)
					[
						SNew(STextBlock)
						.Text(FText::FromString("Document ID:"))
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(5)
					[
						SNew(SButton)
						             .Text(FText::FromString("Select Document"))
 // Bind the button text to the current state
						             .IsEnabled(this, &SConnectTab::IsSelectDocsReady) // Bind button enabled state
						             .OnClicked(this, &SConnectTab::OnSelectDocumentClicked)
					]
				]

				// Document ID Label and TextBox
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(5)
				[
					SAssignNew(DocumentId, SEditableTextBox)
					.Text(FText::AsNumber(UserInfo.DocumentId, &FNumberFormattingOptions::DefaultNoGrouping()))
					.HintText(FText::FromString("Enter document ID"))
					.IsEnabled(this, &SConnectTab::HasAuthenticatingCredentials)
				]

				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(5)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.HAlign(HAlign_Right)
					.Padding(5)
					[
						SNew(SButton)
						             .Text(this, &SConnectTab::GetLinkButtonText)
 // Bind the button text to the current state
						             .IsEnabled(this, &SConnectTab::HasAuthenticatingCredentials)
 // Bind button enabled state
						             .OnClicked(this, &SConnectTab::OnLinkDocumentClicked)
					]
				]
			]
		]
	];

	GEditor->GetTimerManager()->SetTimerForNextTick([this]()
	{
		LoadSettings(); // Load the settings after a short delay
	});
}

FReply SConnectTab::OnConnectButtonClicked()
{
	FString SUrl = URL->GetText().ToString();
	FString SAccountId = AccountId->GetText().ToString();
	FString SAPIKey = APIKey->GetText().ToString();
	SaveSettings();
	SetConnectingState(true);
	// Call the API to authenticate
	API::Authenticate(SAccountId, SAPIKey, SUrl, FOnAuthCompleted::CreateSP(this, &SConnectTab::OnCompletedAuth));
	return FReply::Handled();
}

FReply SConnectTab::OnSelectDocumentClicked()
{
	SubWindow->SetVisibility(EVisibility::Visible);
	ConnectionPanel->SetVisibility(EVisibility::Collapsed);
	LinkPanel->SetVisibility(EVisibility::Collapsed);
	auto subView = SNew(SSelectorDocumentSubWindow)
		.OnSubWindowClosed(FOnSubWindowClosedDelegate::CreateSP(this, &SConnectTab::OnSubWindowClosed));
	subView->Init(UserInfo, DocumentsData);
	SubWindow->SetContent(subView);
	return FReply::Handled();
}

FReply SConnectTab::OnLinkDocumentClicked()
{
	FString SDocumentId = DocumentId->GetText().ToString();
	if (UserInfo.DocumentId == 0)
	{
		bIsLinkReadyToClick = false;
		API::FetchDocumentById(UserInfo, SDocumentId, [this, SDocumentId](const FDocumentInfo& DocumentInfo)
		{
			bIsLinkReadyToClick = true;
			UserInfo.DocumentId = FCString::Atoi(*SDocumentId);
		    SaveSettings();
		    NotifyParent();
		},
		[this](const FString& ErrorMessage)
		{
			bIsLinkReadyToClick = true;
			FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(ErrorMessage));
		});
	}
	else
	{
		UserInfo.DocumentId = 0;
		UserInfo.ProjectId.Empty();
		wordbee::SingletonUtil<FDocumentData>::ClearFromIni();
		SaveSettings();
		NotifyParent();
	}
	return FReply::Handled();
}

FText SConnectTab::GetLinkButtonText() const
{
	return UserInfo.DocumentId != 0 ? FText::FromString("UnLink") : FText::FromString("Link");
}

void SConnectTab::SetConnectingState(bool bIsConnecting)
{
	this->bConnecting = bIsConnecting;

	// Change the button text based on the state
	ButtonConnectStateText = bConnecting ? FText::FromString("Connecting...") : FText::FromString("Test Credentials");
}

FText SConnectTab::GetButtonText() const
{
	return ButtonConnectStateText;
}

bool SConnectTab::IsConnectButtonEnabled() const
{
	return !bConnecting;
}

void SConnectTab::OnCompletedAuth(FString ResponseString)
{
	SetConnectingState(false);
	if (ResponseString.IsEmpty())
	{
		ResponseTextBlock->SetVisibility(EVisibility::Collapsed);
		bIsAuthenticated = false;
	}
	else
	{
		FString AuthToken = FString::Printf(TEXT("AuthToken: %s"), *ResponseString);
		// Display the response in the text block
		ResponseTextBlock->SetText(FText::FromString(AuthToken));
		ResponseTextBlock->SetVisibility(EVisibility::Visible);
		bIsAuthenticated = true;
		UserInfo.AuthToken = ResponseString.TrimChar('"');
		SaveSettings();
		LoadDocumentSettings();
	}
}

bool SConnectTab::HasAuthenticatingCredentials() const
{
	return bIsAuthenticated && bIsLinkReadyToClick;
}

void SConnectTab::SaveSettings()
{
	UserInfo.Url = *URL->GetText().ToString();
	UserInfo.AccountId = *AccountId->GetText().ToString();
	UserInfo.ApiKey = *APIKey->GetText().ToString();
	wordbee::SingletonUtil<FWordbeeUserData>::SaveToIni(UserInfo);
}

void SConnectTab::LoadSettings() const
{
	URL->SetText(FText::FromString(UserInfo.Url));
	AccountId->SetText(FText::FromString(UserInfo.AccountId));
	APIKey->SetText(FText::FromString(UserInfo.ApiKey));
}

void SConnectTab::LoadDocumentSettings()
{
	UGetDocumentsCommand::ExecuteHttpRequest(UserInfo,
	                                         FOnHttpRequestComplete::CreateSP(
		                                         this, &SConnectTab::OnFetchDocumentsResponseReceived));
}

bool SConnectTab::IsSelectDocsReady() const
{
	return bIsDocumentsFetched && UserInfo.DocumentId == 0;
}

void SConnectTab::SetDocumentsFetched()
{
	bIsDocumentsFetched = true;
}

void SConnectTab::OnFetchDocumentsResponseReceived(const TArray<FDocumentDataResponse>& response)
{
	if (response.Num() != 0)
	{
		DocumentsData = response;
		SetDocumentsFetched();
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("SConnectTab::OnFetchDocumentsResponseReceived"));
	}
}


void SConnectTab::OnSubWindowClosed(bool isLinked, FString InProjectId, FString InDocumentId)
{
	if (!InDocumentId.IsEmpty())
		DocumentId->SetText(FText::FromString(InDocumentId));
	ConnectionPanel->SetVisibility(EVisibility::Visible);
	SubWindow->SetVisibility(EVisibility::Collapsed);
	LinkPanel->SetVisibility(EVisibility::Visible);
	UserInfo.ProjectId = InProjectId;
	UserInfo.DocumentId = FCString::Atoi(*InDocumentId);
	SaveSettings();
	NotifyParent();
}

void SConnectTab::SetParentWindow(TWeakPtr<SWordBeeEditorConfigWindow> InParent)
{
	ParentWindow = InParent;
}

void SConnectTab::NotifyParent()
{
	if (ParentWindow.IsValid())
	{
		TSharedPtr<SWordBeeEditorConfigWindow> Parent = ParentWindow.Pin();
		Parent->OnDocumentChanged();
	}
}
