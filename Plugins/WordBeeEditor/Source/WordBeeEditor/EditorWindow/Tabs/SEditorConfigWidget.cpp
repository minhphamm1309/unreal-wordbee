#include "SEditorConfigWidget.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Styling/AppStyle.h"  // Replaces FEditorStyle for UE5 compatibility
#include "WordbeeEditor/API/API.h"
#include "Internationalization/Culture.h"
#include "Internationalization/Internationalization.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "WordBeeEditor/Command/CreateDataAsset/SUserData.h"
#include "WordBeeEditor/Models/FRecord.h"
#include "WordBeeEditor/Service/DocumentService.h"
#include "WordBeeEditor/Utils/FileChangeUtil.h"
#include "WordBeeEditor/Utils/LocalizeUtil.h"
#include "WordBeeEditor/Utils/SingletonUtil.h"

struct FLocalizationTargetSettings;
class ULocalizationTarget;
class ULocalizationSettings;

void SEditorConfigWidget::Construct(const FArguments& InArgs)
{
	Config = SingletonUtil::GetFromIni<FEditorConfig>();
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
				  .VAlign(VAlign_Center)
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
			// // String Table Picker
			// + SVerticalBox::Slot()
			// .AutoHeight()
			// .Padding(5)
			// [
			// 	SNew(STextBlock)
			// 	.Text(FText::FromString("Select String Table"))
			// 	.Font(FAppStyle::Get().GetFontStyle("NormalFont"))
			// ]
			// + SVerticalBox::Slot()
			// .AutoHeight()
			// .Padding(5)
			// [
			// 	SNew(SObjectPropertyEntryBox)
			// 	.AllowedClass(UStringTable::StaticClass())
			// 	.ObjectPath(TAttribute<FString>::Create([this]() -> FString
			// 	{
			// 		return Config.SelectedCollectionPath;
			// 	}))
			// 	.OnObjectChanged(this, &SEditorConfigWidget::OnStringTableSelected)
			// ]
			
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
				SNew(SHorizontalBox)

				// Reload Button (Curved Arrow Icon)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(5, 0)
				[
					SNew(SButton)
					             .ButtonStyle(FAppStyle::Get(), "HoverHintOnly") // Style for minimal appearance
					             .OnClicked(this, &SEditorConfigWidget::OnReloadLanguagesClicked)
					             .ToolTipText(FText::FromString("Reload Missing and Common Languages"))
					             .IsEnabled_Lambda([this]() { return !bIsFetching; }) // Disable when fetching
					             .Cursor(EMouseCursor::Hand) // Change cursor on hover
					[
						SNew(SImage)
						.Image(FAppStyle::Get().GetBrush("Icons.Refresh"))
					]
					[
						SNew(SImage)
						.Image(FAppStyle::Get().GetBrush("Icons.Refresh"))
					]
				]

				// Missing Languages Dropdown
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				.Padding(5, 0)
				[
					SAssignNew(MissingLocalesCbo, SComboBox<TSharedPtr<FLanguageInfo>>)
					.OptionsSource(&MissingLocales)
					.OnGenerateWidget(this, &SEditorConfigWidget::GenerateLanguageOption)
					[
						SNew(STextBlock)
						.Text(FText::FromString("Missing Languages"))
						.Font(FAppStyle::Get().GetFontStyle("NormalFont"))
					]
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
					.IsChecked_Lambda([this]()
					{
						return bAllLangsChecked ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
					})
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
					SAssignNew(CommonLocalesListView, SListView<TSharedPtr<FLanguageInfo>>)
					.ItemHeight(24)
					.ListItemsSource(&CommonLocales)
					.OnGenerateRow(this, &SEditorConfigWidget::GenerateLanguageCheckbox)
					.Orientation(EOrientation::Orient_Horizontal) // Ensure horizontal orientation
				]
			]
			// Pull Data Region Label
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(5)
			[
				SNew(STextBlock)
				.Text(FText::FromString("Pull Data Region"))
				.Font(FAppStyle::Get().GetFontStyle("NormalFont"))
			]

			// Pull Data Button (Full Width)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(5)
			[
				SNew(SButton)
				             .HAlign(HAlign_Fill) // Make it full-width
				             .Text(FText::FromString("Pull Data"))
				             .HAlign(HAlign_Center) // Center horizontally
				             .VAlign(VAlign_Center)
				             .OnClicked(this, &SEditorConfigWidget::OnCPullButtonClicked)
			]

			// Push Data Region Label
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(5)
			[
				SNew(STextBlock)
				.Text(FText::FromString("Push Data Region"))
				.Font(FAppStyle::Get().GetFontStyle("NormalFont"))
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(5)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(FText::FromString("Push Only changed data"))
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SAssignNew(pushOnlyChangedCheckbox, SCheckBox)
					.AccessibleText(FText::FromString("Push Only changed data"))
					.OnCheckStateChanged(this, &SEditorConfigWidget::OnPushCheckboxChanged)
				]
			]

			// Push Data Button (Full Width)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(5)
			[
				SNew(SButton)
				             .HAlign(HAlign_Fill) // Make it full-width
				             .Text(FText::FromString("Push Data"))
				             .HAlign(HAlign_Center) // Center horizontally
				             .VAlign(VAlign_Center)
				             .OnClicked(this, &SEditorConfigWidget::OnPushButtonClicked)
			]
		]
	];
}

void SEditorConfigWidget::OnPushCheckboxChanged(ECheckBoxState CheckBoxState)
{
}

TSharedRef<ITableRow> SEditorConfigWidget::GenerateLanguageCheckbox(TSharedPtr<FLanguageInfo> Item,
                                                                    const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(STableRow<TSharedPtr<FLanguageInfo>>, OwnerTable)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(FMargin(0, 0, 10, 0))
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
	SingletonUtil::SaveToIni<FEditorConfig>(Config);
	FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("Configuration saved successfully!"));
	return FReply::Handled();
}

FReply SEditorConfigWidget::OnCPullButtonClicked()
{
	// Create the shared pointer directly
	TSharedPtr<TArray<FString>> SelectedLanguages = MakeShared<TArray<FString>>();
	FString srcCode;
	for (const TSharedPtr<FLanguageInfo>& Lang : CommonLocales)
	{
		if (Lang.IsValid())
		{
			if (Lang->Src=="true")
			{
				srcCode = Lang->V;
				SelectedLanguages->Add(srcCode);
			}
			else if (Lang->IsSelected)
			{
				SelectedLanguages->Add(Lang->V);
			}
		}
	}
	if (SelectedLanguages->Num() < 2)
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("You must select at least one language."));
		return FReply::Handled();
	}
	DocumentService::PullDocument(SelectedLanguages, srcCode);
	return FReply::Handled();
}


FReply SEditorConfigWidget::OnPushButtonClicked()
{
	LocalizeUtil* localizeUtil = Locate<LocalizeUtil>::Get();
	TArray<FRecord> RecordsToCommit;
	if (pushOnlyChangedCheckbox.Get()->IsChecked())
	{
		RecordsToCommit = localizeUtil->RecordsChanged;
	}
	else
	{
		// Push all records
		RecordsToCommit = FileChangeUtil::GetCurrentRecords();
	}
	if (RecordsToCommit.Num() == 0)
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(" No data to push to Wordbee."));
		return FReply::Handled();
	}
	
	API::PushRecords(RecordsToCommit, FOnUpdateDocumentComplete::CreateLambda(
		                 [](bool bSuccess, const int32& _, const FString& message)
		                 {
			                 if (bSuccess)
			                 {
				                 FNotificationInfo Info(
					                 FText::FromString("Push data to Wordbee completed successfully."));
				                 Info.bFireAndForget = true; // Set this to true so it automatically fades out
				                 Info.ExpireDuration = 2.0f; // How long it takes to fade out when closing

				                 TSharedPtr<SNotificationItem> NotificationPtr = FSlateNotificationManager::Get().
					                 AddNotification(Info);
				                 if (NotificationPtr.IsValid())
				                 {
					                 NotificationPtr->SetCompletionState(SNotificationItem::CS_Success);
					                 NotificationPtr->ExpireAndFadeout();
				                 }

				                 Locate<LocalizeUtil>::Get()->RecordsChanged.Empty();
				                 FileChangeUtil::CopyLocalizeToSaved();
			                 }
			                 else
			                 {
				                 FMessageDialog::Open(EAppMsgType::Ok,
				                                      FText::FromString("Failed to push data to Wordbee: " + message));
			                 }
		                 }));

	return FReply::Handled();
}

void SEditorConfigWidget::OnLanguageCheckboxChanged(ECheckBoxState NewState)
{
	if (NewState == ECheckBoxState::Checked)
	{
		bAllLangsChecked = true;
		// Set all CommonLocales' IsSelected to true
		for (const TSharedPtr<FLanguageInfo>& Lang : CommonLocales)
		{
			Lang->IsSelected = true;
		}
	}
	else
	{
		bAllLangsChecked = false;
		// Set all CommonLocales' IsSelected to false
		for (const TSharedPtr<FLanguageInfo>& Lang : CommonLocales)
		{
			Lang->IsSelected = false;
		}
	}
}

FReply SEditorConfigWidget::OnReloadLanguagesClicked()
{
	if (bIsFetching)
	{
		return FReply::Handled();
	}
	bIsFetching = true; // Start fetching, disable the button
	FetchLangsFromAPI();
	return FReply::Handled();
}

void SEditorConfigWidget::FetchLangsFromAPI()
{
	bAllLangsChecked = false;
	MissingLocales.Empty();
	CommonLocales.Empty();
	UserInfo = SingletonUtil::GetFromIni<FWordbeeUserData>();
	API::FetchLanguages(UserInfo, [this](const TArray<FLanguageInfo>& Languages)
    {
        bIsFetching = false;
        TArray<FString> UnrealLocales;
        UnrealLocales = FTextLocalizationManager::Get().GetLocalizedCultureNames(
            ELocalizationLoadFlags::Engine);
        for (const FString& UnrealLocale : UnrealLocales)
        {
            TSharedPtr<FCulture> Culture = FInternationalization::Get().GetCulture(UnrealLocale);
            FString DisplayName = Culture.IsValid() ? Culture->GetDisplayName() : UnrealLocale;
            // Check if Unreal's locale is missing in the API response
            bool bIsMissing = !Languages.ContainsByPredicate(
                [&UnrealLocale](const FLanguageInfo& Lang)
                {
                    return Lang.V == UnrealLocale;
                });
            if (bIsMissing)
            {
                FLanguageInfo NewLang;
                NewLang.V = UnrealLocale;
                NewLang.T = DisplayName;
                MissingLocales.Add(MakeShared<FLanguageInfo>(NewLang));
            }
        }
		for (const FLanguageInfo& WbLocale : Languages)
        {
			CommonLocales.Add(MakeShared<FLanguageInfo>(WbLocale));
		}
        CommonLocalesListView->RequestListRefresh();
        MissingLocalesCbo->RefreshOptions();
        ShowNotification("Languages reloaded successfully!", true);
    }, [this](const FString& ErrorMessage)
    {
        bIsFetching = false;
        ShowNotification("Failed to reload languages: " + ErrorMessage, false);
    });
}

void SEditorConfigWidget::ShowNotification(const FString& Message, bool bSuccess)
{
	FNotificationInfo Info(FText::FromString(Message));
	Info.ExpireDuration = 3.0f;
	Info.bUseSuccessFailIcons = true;
	Info.Image = bSuccess ? FAppStyle::Get().GetBrush("Icons.Success") : FAppStyle::Get().GetBrush("Icons.Error");
	FSlateNotificationManager::Get().AddNotification(Info);
}
