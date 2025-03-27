#include "SEditorConfigWidget.h"

#include "PropertyCustomizationHelpers.h"
#include "Widgets/Input/SFilePathPicker.h"
#include "Styling/AppStyle.h"  // Replaces FEditorStyle for UE5 compatibility
#include "WordbeeEditor/API/API.h"
#include "Internationalization/StringTable.h"
#include "Internationalization/Internationalization.h"
#include "LocalizationTargetTypes.h"
#include "LocalizationSettings.h"
#include "WordBeeEditor/Command/CreateDataAsset/SUserData.h"

struct FLocalizationTargetSettings;
class ULocalizationTarget;
class ULocalizationSettings;

void SEditorConfigWidget::Construct(const FArguments& InArgs)
{
	Config.Load();
	MissingLocales.Empty(); // Clear previous items
	CommonLocales.Empty();
	UUserData* userInfo = SUserData::Get();
	UAPI::FetchLanguages(userInfo, [this](const TArray<FLanguageInfo>& Languages)
	{
		UE_LOG(LogTemp, Log, TEXT("Language fetch success"));
		UE_LOG(LogTemp, Log, TEXT("Wordbee Locales (Total: %d):"), Languages.Num());
		for (const FLanguageInfo& Lang : Languages)
		{
			UE_LOG(LogTemp, Log, TEXT(" - %s"), *Lang.V);
		}
		// Get available cultures in Unreal's localization system, include derived cultures
		TArray<FString> UnrealLocales;
		FInternationalization::Get().GetCultureNames(UnrealLocales);
		// Log Unreal locales
		UE_LOG(LogTemp, Log, TEXT("Unreal Locales (Total: %d):"), UnrealLocales.Num());
		for (const FString& UnrealLocale : UnrealLocales)
		{
			UE_LOG(LogTemp, Log, TEXT(" - %s"), *UnrealLocale);
		}
		MissingLocales.Empty();
		CommonLocales.Empty(); // Clear out the CommonLocales array

		for (const FString& UnrealLocale : UnrealLocales)
		{
			// Check if Unreal's locale is missing in the API response
			bool bIsMissing = !Languages.ContainsByPredicate([&UnrealLocale](const FLanguageInfo& Lang)
			{
				return Lang.V == UnrealLocale;
			});

			if (bIsMissing)
			{
				FLanguageInfo NewLang;
				NewLang.V = UnrealLocale;
				NewLang.T = UnrealLocale;
				MissingLocales.Add(MakeShared<FLanguageInfo>(NewLang));

				UE_LOG(LogTemp, Log, TEXT("Added missing language: %s"), *UnrealLocale);
			}
			else
			{
				// If the language exists, add it to CommonLocales
				FLanguageInfo CommonLang;
				CommonLang.V = UnrealLocale;
				CommonLang.T = UnrealLocale;
				CommonLocales.Add(MakeShared<FLanguageInfo>(CommonLang));

				UE_LOG(LogTemp, Log, TEXT("Added common language: %s"), *UnrealLocale);
			}
		}
	});


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
				.ObjectPath(TAttribute<FString>::Create([this]() -> FString
				{
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
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(5)
			[
				SNew(SComboBox<TSharedPtr<FLanguageInfo>>)
				.OptionsSource(&MissingLocales)
				.OnGenerateWidget(this, &SEditorConfigWidget::GenerateLanguageOption)
				[
					SNew(STextBlock)
					                .Text(FText::FromString("Missing Languages")) // Default display text
					                .Font(FAppStyle::Get().GetFontStyle("NormalFont"))
				]
			]
			// Language Selection Label with Checkbox
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(5)
			[
				SNew(SHorizontalBox)
				// Label
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(5)
				[
					SNew(STextBlock)
					.Text(FText::FromString("Select Languages"))
					.Font(FAppStyle::Get().GetFontStyle("NormalFont"))
				]
				// Checkbox
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(5)
				[
					SNew(SCheckBox)
					.OnCheckStateChanged(this, &SEditorConfigWidget::OnLanguageCheckboxChanged)
				]
			]

			// Language Selection List with Checkboxes
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(5)
			[
				SNew(SScrollBox)
				.Orientation(Orient_Horizontal) // Set horizontal scrolling
				+ SScrollBox::Slot()
				[
					SNew(SHorizontalBox)
					// Iterate through CommonLocales and add each as a child slot
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(5)
					[
						SNew(SListView<TSharedPtr<FLanguageInfo>>)
						.ItemHeight(24)
						.ListItemsSource(&CommonLocales)
						.OnGenerateRow(this, &SEditorConfigWidget::GenerateLanguageCheckbox)
						.Orientation(EOrientation::Orient_Horizontal) // Ensure horizontal orientation
					]
				]
			]


		]
	];
}

TSharedRef<ITableRow> SEditorConfigWidget::GenerateLanguageCheckbox(TSharedPtr<FLanguageInfo> Item,
                                                                    const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(STableRow<TSharedPtr<FLanguageInfo>>, OwnerTable)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SCheckBox)
				.IsChecked_Lambda([this, Item]() -> ECheckBoxState
				{
					return Item->IsSelected ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
				})
				.OnCheckStateChanged_Lambda([this, Item](ECheckBoxState NewState)
				{
					// Update the IsSelected property of the language
					Item->IsSelected = (NewState == ECheckBoxState::Checked);
				})
				[
					SNew(STextBlock)
					.Text(FText::FromString(Item->T))
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

TSharedRef<SWidget> SEditorConfigWidget::GenerateLanguageOption(TSharedPtr<FLanguageInfo> Item)
{
	return SNew(STextBlock)
		.Text(FText::FromString(Item->T))
		.Font(FAppStyle::Get().GetFontStyle("NormalFont"));
}

FReply SEditorConfigWidget::OnSaveClicked()
{
	Config.Save();
	FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("Configuration saved successfully!"));
	return FReply::Handled();
}

void SEditorConfigWidget::OnLanguageCheckboxChanged(ECheckBoxState NewState)
{
	if (NewState == ECheckBoxState::Checked)
	{
		// Set all CommonLocales' IsSelected to true
		for (const TSharedPtr<FLanguageInfo>& Lang : CommonLocales)
		{
			Lang->IsSelected = true;
		}
	}
	else
	{
		// Set all CommonLocales' IsSelected to false
		for (const TSharedPtr<FLanguageInfo>& Lang : CommonLocales)
		{
			Lang->IsSelected = false;
		}
	}
}
