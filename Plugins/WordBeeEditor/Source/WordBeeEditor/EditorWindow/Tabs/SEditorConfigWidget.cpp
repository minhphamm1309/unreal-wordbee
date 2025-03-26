#include "SEditorConfigWidget.h"

#include "PropertyCustomizationHelpers.h"
#include "Widgets/Input/SFilePathPicker.h"
#include "Styling/AppStyle.h"  // Replaces FEditorStyle for UE5 compatibility
#include "Internationalization/StringTable.h"

void SEditorConfigWidget::Construct(const FArguments& InArgs)
{
	Config.Load();

	TargetSyncOptions = {
		MakeShared<FString>(TEXT("IfAnyProblem")),
		MakeShared<FString>(TEXT("IfAllProblem")),
		MakeShared<FString>(TEXT("IfNotAllOk")),
		MakeShared<FString>(TEXT("None"))
	};

	ChildSlot
	[
		SNew(SScrollBox)
		+ SScrollBox::Slot()
		[
			SNew(SVerticalBox)

			// AutoSync Options Label
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(5)
			[
				SNew(STextBlock)
				.Text(FText::FromString("AutoSync Options"))
				.Font(FAppStyle::Get().GetFontStyle("NormalFont"))
			]

			// AutoSync Enabled & Sync Interval (Same Line)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(5)
			[
				SNew(SHorizontalBox)

				// AutoSync Checkbox
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(5)
				[
					SNew(SCheckBox)
					.IsChecked(Config.bAutoSyncEnabled ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
					.OnCheckStateChanged(this, &SEditorConfigWidget::OnAutoSyncChanged)
					.Content()
					[
						SNew(STextBlock).Text(FText::FromString("AutoSync Enabled"))
					]
				]

				// Sync Interval Label
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(5, 0) // Reduce padding to keep them closer
				[
					SNew(STextBlock)
					.Text(FText::FromString("Sync Interval (seconds):"))
					.Font(FAppStyle::Get().GetFontStyle("NormalFont"))
				]

				// Sync Interval TextBox
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				.Padding(5, 0)
				.VAlign(VAlign_Center)
				[
					SNew(SEditableTextBox)
					                      .Text(FText::AsNumber(Config.SyncIntervalSeconds))
					                      .IsEnabled_Lambda([this]()
					                      {
						                      return Config.bAutoSyncEnabled;
					                      }) // Dynamic update
					                      .OnTextCommitted(this, &SEditorConfigWidget::OnSyncIntervalChanged)
					                      .HintText(FText::FromString("Enter seconds"))
				]
			]

			// Target Synchronization Label
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(5)
			[
				SNew(STextBlock)
				.Text(FText::FromString("Target Synchronization"))
				.Font(FAppStyle::Get().GetFontStyle("NormalFont"))
			]

			// Target Synchronization ComboBox
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(5)
			[
				SNew(SComboBox<TSharedPtr<FString>>)
				.OptionsSource(&TargetSyncOptions)
				.OnSelectionChanged(this, &SEditorConfigWidget::OnTargetSyncChanged)
				.OnGenerateWidget(this, &SEditorConfigWidget::MakeComboWidget)
				.InitiallySelectedItem(GetCurrentTargetSyncOption()) // Set the initial selection
				[
					SNew(STextBlock)
					.Text_Lambda([this]()
					{
						return FText::FromString(Config.TargetSynchronization);
					}) // Bind text dynamically
				]
			]
			// String Table Picker Label
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(5)
			[
				SNew(STextBlock)
				.Text(FText::FromString("Select String Table"))
				.Font(FAppStyle::Get().GetFontStyle("NormalFont"))
			]

			// String Table Picker
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(5)
			[
				SNew(SObjectPropertyEntryBox)
				.AllowedClass(UStringTable::StaticClass())
				.ObjectPath(TAttribute<FString>::Create([this]() -> FString {
		return Config.SelectedCollectionPath;
	}))
				.OnObjectChanged(this, &SEditorConfigWidget::OnStringTableSelected)
			]
			// Save Button
			+ SVerticalBox::Slot()
			.Padding(5)
			.HAlign(HAlign_Right)
			[
				SNew(SButton)
				.Text(FText::FromString("Save"))
				.OnClicked(this, &SEditorConfigWidget::OnSaveClicked)
			]
		]
	];
}

void SEditorConfigWidget::OnAutoSyncChanged(ECheckBoxState NewState)
{
	Config.bAutoSyncEnabled = (NewState == ECheckBoxState::Checked);
}

void SEditorConfigWidget::OnSyncIntervalChanged(const FText& NewText, ETextCommit::Type CommitType)
{
	Config.SyncIntervalSeconds = FCString::Atod(*NewText.ToString());
}

void SEditorConfigWidget::OnTargetSyncChanged(TSharedPtr<FString> NewValue, ESelectInfo::Type)
{
	if (NewValue.IsValid())
	{
		Config.TargetSynchronization = *NewValue;
	}
}
TSharedPtr<FString> SEditorConfigWidget::GetCurrentTargetSyncOption() const
{
	for (const TSharedPtr<FString>& Option : TargetSyncOptions)
	{
		if (*Option == Config.TargetSynchronization)
		{
			return Option;
		}
	}
	return nullptr;
}

TSharedRef<SWidget> SEditorConfigWidget::MakeComboWidget(TSharedPtr<FString> InOption)
{
	return SNew(STextBlock).Text(FText::FromString(*InOption));
}

void SEditorConfigWidget::OnStringTableSelected(const FAssetData& AssetData)
{
	Config.SelectedCollectionPath = AssetData.GetObjectPathString();
}
FReply SEditorConfigWidget::OnSaveClicked()
{
	Config.Save();
	FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("Configuration saved successfully!"));
	return FReply::Handled();
}
