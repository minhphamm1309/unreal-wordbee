#include "SConnectTab.h"

#include "WordBeeEditor/API/API.h"

void SConnectTab::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SVerticalBox)

		// IP Address Label and TextBox
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(5)
		[
			SNew(STextBlock)
			.Text(FText::FromString("URL:"))
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
			SAssignNew(AccountId, SEditableTextBox)
			.HintText(FText::FromString("Enter API key"))
		]

		// Connect Button
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(5)
		[
			SNew(SButton)
			.Text(FText::FromString("Connect"))
			.OnClicked(this, &SConnectTab::OnConnectButtonClicked)
		]
	];
}

FReply SConnectTab::OnConnectButtonClicked()
{
	FString SUrl = URL->GetText().ToString();
	FString SAccountId = AccountId->GetText().ToString();
	FString SAPIKey = APIKey->GetText().ToString();

	// Call the API to authenticate
	UAPI* API = NewObject<UAPI>();
	API->Authenticate(SAccountId, SAPIKey, SUrl);

	return FReply::Handled();
}
