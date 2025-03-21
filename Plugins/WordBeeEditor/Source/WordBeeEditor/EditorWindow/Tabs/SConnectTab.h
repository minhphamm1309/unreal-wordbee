#pragma once

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

	// Variables to hold the values entered into text boxes
	TSharedPtr<SEditableTextBox> URL;
	TSharedPtr<SEditableTextBox> AccountId;
	TSharedPtr<SEditableTextBox> APIKey;
	TSharedPtr<STextBlock> ResponseTextBlock;

	bool bConnecting = false;
	FText ButtonConnectStateText;
	void SetConnectingState(bool bConnecting);
	FText GetButtonText() const;
	bool IsConnectButtonEnabled() const;

	void OnCompletedAuth(FString ResponseString);
	void SaveSettings() const;
	void LoadSettings() const;
};



