#include "SConnectTab.h"

#include "WordBeeEditor/API/API.h"
#include "WordBeeEditor/Command/DocumentList/UGetDocumentsCommand.h"
#include "WordBeeEditor/EditorWindow/SubWindow/SSelectorDocumentSubWindow.h"

void SConnectTab::Construct(const FArguments& InArgs)
{
	SetConnectingState(false);
	ChildSlot
	[
		SNew(SVerticalBox)

		// Connect panel
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SAssignNew(ConnectionPanel,SBorder)
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
						.Text(this, &SConnectTab::GetButtonText)  // Bind the button text to the current state
						.IsEnabled(this, &SConnectTab::IsConnectButtonEnabled)  // Bind button enabled state
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
					.Text(FText::GetEmpty())  // Initially empty
					.Visibility(EVisibility::Collapsed)  // Initially hidden
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
						.Text(FText::FromString("Select Document"))  // Bind the button text to the current state
						.IsEnabled(this, &SConnectTab::HasDocumentsFetched)  // Bind button enabled state
						.OnClicked(this, &SConnectTab::OnSelectDocumentClicked)
					]
				]

				// Document ID Label and TextBox
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(5)
				[
					SAssignNew(DocumentId, SEditableTextBox)
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
						.Text(FText::FromString("Link"))  // Bind the button text to the current state
						.IsEnabled(this, &SConnectTab::HasAuthenticatingCredentials)  // Bind button enabled state
						.OnClicked(this, &SConnectTab::OnLinkDocumentClicked)
					]
				]
			]
		]
	];

	GEditor->GetTimerManager()->SetTimerForNextTick([this]()
	{
		LoadSettings();  // Load the settings after a short delay
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
	UAPI* API = NewObject<UAPI>();
	API->Authenticate(SAccountId, SAPIKey, SUrl, FOnAuthCompleted::CreateSP(this, &SConnectTab::OnCompletedAuth));

	return FReply::Handled();
}

FReply SConnectTab::OnSelectDocumentClicked()
{
	SubWindow->SetVisibility(EVisibility::Visible);
	ConnectionPanel->SetVisibility(EVisibility::Collapsed);
	LinkPanel->SetVisibility(EVisibility::Collapsed);
	auto subView = SNew(SSelectorDocumentSubWindow)
		.OnSubWindowClosed(FOnSubWindowClosedDelegate::CreateSP(this, &SConnectTab::OnSubWindowClosed));
	subView->Init(UserInfo,DocumentsData);
	SubWindow->SetContent(subView);
	return FReply::Handled();
}

FReply SConnectTab::OnLinkDocumentClicked()
{
	return FReply::Handled();
}


void SConnectTab::SetConnectingState(bool bConnecting)
{
	this->bConnecting = bConnecting;

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
		UserInfo.AccountId = AccountId->GetText().ToString();
		UserInfo.ApiKey = APIKey->GetText().ToString();
		UserInfo.BaseUrl = URL->GetText().ToString();
		UserInfo.AuthToken = ResponseString.TrimChar('"');
		LoadDocumentSettings();
	}
}

bool SConnectTab::HasAuthenticatingCredentials() const
{
	return bIsAuthenticated;
}

void SConnectTab::SaveSettings() const
{
	FString ConfigSection = TEXT("ConnectSettings");
	
	FString CustomIniPath = FPaths::ProjectSavedDir() + "WordBee/Settings.ini";
	IFileManager::Get().MakeDirectory(*FPaths::GetPath(CustomIniPath), true);

	FConfigFile ConfigFile;
	ConfigFile.SetString(*ConfigSection, TEXT("url"), *URL->GetText().ToString());
	ConfigFile.SetString(*ConfigSection, TEXT("ApiKey"), *APIKey->GetText().ToString());
	ConfigFile.SetString(*ConfigSection, TEXT("AccountId"), *AccountId->GetText().ToString());

	// Write the config file to disk
	ConfigFile.Write(CustomIniPath);
}

void SConnectTab::LoadSettings() const
{
	// Load the settings
	FString ConfigSection = TEXT("ConnectSettings");
	FString CustomIniPath = FPaths::ProjectSavedDir() + "WordBee/Settings.ini";

	if (!FPaths::FileExists(CustomIniPath)) return;

	// Create a new FConfigFile to read from the custom INI file
	FConfigFile ConfigFile;

	// Read the config file from disk
	ConfigFile.Read(CustomIniPath);
	
	FString SUrl;
	if (ConfigFile.GetString(*ConfigSection, TEXT("url"), SUrl))
	{
		if (URL.IsValid())
			URL->SetText(FText::FromString(SUrl));
	}

	FString SApiKey;
	if (ConfigFile.GetString(*ConfigSection, TEXT("ApiKey"), SApiKey))
	{
		if (APIKey.IsValid())
			APIKey->SetText(FText::FromString(SApiKey));
	}

	FString SAccountId;
	if (ConfigFile.GetString(*ConfigSection, TEXT("AccountId"), SAccountId))
	{
		if (AccountId.IsValid())
			AccountId->SetText(FText::FromString(SAccountId));
	}
}

void SConnectTab::LoadDocumentSettings()
{
	UGetDocumentsCommand* HttpCommand = NewObject<UGetDocumentsCommand>();

	HttpCommand->ExecuteHttpRequest(UserInfo, FOnHttpRequestComplete::CreateSP(this, &SConnectTab::OnFetchDocumentsResponseReceived));
}

bool SConnectTab::HasDocumentsFetched() const
{
	return  bIsDocumentsFetched;
}

void SConnectTab::SetDocumentsFetched()
{
	bIsDocumentsFetched = true;
}

void SConnectTab::OnFetchDocumentsResponseReceived( const TArray<FDocumentData>& response)
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


void SConnectTab::OnSubWindowClosed(FString InDocumentId)
{
	if (!InDocumentId.IsEmpty())
		DocumentId->SetText(FText::FromString(InDocumentId));
	ConnectionPanel->SetVisibility(EVisibility::Visible);
	SubWindow->SetVisibility(EVisibility::Collapsed);
	LinkPanel->SetVisibility(EVisibility::Visible);
}

