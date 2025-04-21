#include "SEditorConfigWidget.h"
#include "SDocumentInfo.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Styling/AppStyle.h"  // Replaces FEditorStyle for UE5 compatibility
#include "Internationalization/Culture.h"
#include "Internationalization/Internationalization.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "WordBeeEditor/Service/DocumentService.h"
#include "WordBeeEditor/Utils/SingletonUtil.h"

struct FLocalizationTargetSettings;
class ULocalizationTarget;
class ULocalizationSettings;

void SEditorConfigWidget::Construct(const FArguments& InArgs)
{
	Config = wordbee::SingletonUtil<FEditorConfig>::GetFromIni();
	TargetSyncOptions = {
		MakeShared<FTargetSyncOption>(TEXT("Skip Segment if Any Sub Segment Has Error Status"), TEXT("IfAnyProblem")),
		MakeShared<FTargetSyncOption>(TEXT("Skip Segment if All Sub Segments Have Error Status"), TEXT("IfAllProblem")),
		MakeShared<FTargetSyncOption>(TEXT("Sync Segment if All Sub Segments Have Confirmed Status"), TEXT("IfNotAllOk")),
		MakeShared<FTargetSyncOption>(TEXT("Sync All Segments Regardless of Status"), TEXT("None"))
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
					.Text(FText::FromString("Sync Interval (min):"))
					.Font(FAppStyle::Get().GetFontStyle("NormalFont"))
				]

				// Sync Interval TextBox
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				.Padding(5, 0)
				.VAlign(VAlign_Center)
				[
					SNew(SEditableTextBox)
						.Text_Lambda([this]()
								{
								return FText::AsNumber(Config.SyncIntervalSeconds / 60);
								})

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
				SNew(SComboBox<TSharedPtr<FTargetSyncOption>>)
				.OptionsSource(&TargetSyncOptions)
				.OnSelectionChanged(this, &SEditorConfigWidget::OnTargetSyncChanged)
				.OnGenerateWidget(this, &SEditorConfigWidget::MakeComboWidget)
				.InitiallySelectedItem(GetCurrentTargetSyncOption())
				[
					SNew(STextBlock)
					.Text_Lambda([this]()
					{
						TSharedPtr<FTargetSyncOption> Found = GetCurrentTargetSyncOption();
						return Found.IsValid()
							       ? FText::FromString(Found->DisplayName)
							       : FText::FromString("Select an Option");
					})
				]

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
				SNew(SHorizontalBox)

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

			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(5)
			[
				SNew(SBox)
				.MaxDesiredHeight(300) // You can adjust this height!
				[
					SAssignNew(CommonLocalesTileView, STileView<TSharedPtr<FLanguageInfo>>)
					.ItemWidth(120)  // Width per tile
					.ItemHeight(40)  // Height per tile
					.ListItemsSource(&CommonLocales)
					.OnGenerateTile(this, &SEditorConfigWidget::GenerateLanguageTile)
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
					.IsEnabled_Lambda([this]() { return !bIsSyncInProgress; })
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
				.IsEnabled_Lambda([this]() { return !bIsSyncInProgress; })
			]
		]
	];
}

TSharedRef<ITableRow> SEditorConfigWidget::GenerateLanguageTile(TSharedPtr<FLanguageInfo> Item,
																const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(STableRow<TSharedPtr<FLanguageInfo>>, OwnerTable)
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(5)
		[
			SNew(SCheckBox)
			.IsChecked_Lambda([Item]() { return Item->IsSelected || Item->Src=="true"? ECheckBoxState::Checked : ECheckBoxState::Unchecked; })
			.IsEnabled_Lambda([Item]() { return Item->Src != "true"; })
			.OnCheckStateChanged_Lambda([Item](ECheckBoxState State)
			{
				if (Item->Src != "true") // Only allow changing non-source items
				{
					Item->IsSelected = (State == ECheckBoxState::Checked);
				}
			})
			[
				SNew(STextBlock)
					.Text(FText::FromString(Item->T))
					.MinDesiredWidth(80)   // force a minimum readable width
					.WrapTextAt(100) 
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
	int32 EnteredMinutes = FCString::Atoi(*NewText.ToString());
	EnteredMinutes = FMath::Clamp(EnteredMinutes, 1, 30);
	Config.SyncIntervalSeconds = EnteredMinutes * 60;
}

void SEditorConfigWidget::OnTargetSyncChanged(TSharedPtr<FTargetSyncOption> NewValue, ESelectInfo::Type)
{
	if (NewValue.IsValid())
	{
		Config.TargetSynchronization = NewValue->Value;
	}
}


TSharedPtr<FTargetSyncOption> SEditorConfigWidget::GetCurrentTargetSyncOption() const
{
	for (const TSharedPtr<FTargetSyncOption>& Option : TargetSyncOptions)
	{
		if (Option->Value == Config.TargetSynchronization)
		{
			return Option;
		}
	}
	return nullptr;
}


TSharedRef<SWidget> SEditorConfigWidget::MakeComboWidget(TSharedPtr<FTargetSyncOption> InOption)
{
	return SNew(STextBlock).Text(FText::FromString(InOption->DisplayName));
}


TSharedRef<SWidget> SEditorConfigWidget::GenerateLanguageOption(TSharedPtr<FLanguageInfo> Item)
{
	return SNew(STextBlock)
		.Text(FText::FromString(Item->T))
		.Font(FAppStyle::Get().GetFontStyle("NormalFont"));
}

FReply SEditorConfigWidget::OnSaveClicked()
{
	wordbee::SingletonUtil<FEditorConfig>::SaveToIni(Config);
	FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("Configuration saved successfully!"));
	return FReply::Handled();
}

FReply SEditorConfigWidget::OnCPullButtonClicked()
{
	bIsSyncInProgress = true;
	// Create the shared pointer directly
	TSharedPtr<TArray<FString>> SelectedLanguages = MakeShared<TArray<FString>>();
	for (const TSharedPtr<FLanguageInfo>& Lang : CommonLocales)
	{
		if (Lang.IsValid())
		{
			if (Lang->IsSelected)
			{
				SelectedLanguages->Add(Lang->V);
			}
		}
	}
	if (SelectedLanguages->Num() < 1)
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("No document linked."));
		bIsSyncInProgress = false;
		return FReply::Handled();
	}
	DocumentService::PullDocument(SelectedLanguages, DocInfo->Src.V, [this](bool bSuccess) {
		bIsSyncInProgress = false;
	});
	return FReply::Handled();
}


FReply SEditorConfigWidget::OnPushButtonClicked()
{
	bIsSyncInProgress = true;
	TSharedPtr<TArray<FString>> SelectedLanguages = MakeShared<TArray<FString>>();
	for (const TSharedPtr<FLanguageInfo>& Lang : CommonLocales)
	{
		if (Lang.IsValid() && Lang->IsSelected)
		{
			SelectedLanguages->Add(Lang->V);
		}
	}
	if (SelectedLanguages->Num() < 1)
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("No document linked."));
		bIsSyncInProgress = false;
		return FReply::Handled();
	}
	DocumentService::PushDocument(SelectedLanguages, pushOnlyChangedCheckbox.Get()->IsChecked(), [this](bool bSuccess) {
		if (!bSuccess)
		{
			FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(" No data to push to Wordbee."));
		}
		bIsSyncInProgress = false;
	});
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
			if (Lang->Src != "true")
			{
				Lang->IsSelected = false;
			}
		}
	}
}

void SEditorConfigWidget::FetchLangsFromAPI()
{
	bAllLangsChecked = false;
	MissingLocales.Empty();
	CommonLocales.Empty();
	DocInfo = SDocumentInfo::CachedDocumentInfo;
	if (!DocInfo.IsValid())
	{
		CommonLocalesTileView->RequestListRefresh();
		MissingLocalesCbo->RefreshOptions();
		return;
	}
	TArray<FLanguageInfo> Languages = DocInfo->Trgs;
	auto srcLang = DocInfo->Src;
	srcLang.IsSelected = true;
	srcLang.Src="true";
	Languages.Add(srcLang);
        TArray<FString> UnrealLocales = FTextLocalizationManager::Get().GetLocalizedCultureNames(ELocalizationLoadFlags::Game);
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
		CommonLocalesTileView->RequestListRefresh();
        MissingLocalesCbo->RefreshOptions();
}

void SEditorConfigWidget::ShowNotification(const FString& Message, bool bSuccess)
{
	FNotificationInfo Info(FText::FromString(Message));
	Info.ExpireDuration = 3.0f;
	Info.bUseSuccessFailIcons = true;
	Info.Image = bSuccess ? FAppStyle::Get().GetBrush("Icons.Success") : FAppStyle::Get().GetBrush("Icons.Error");
	FSlateNotificationManager::Get().AddNotification(Info);
}

