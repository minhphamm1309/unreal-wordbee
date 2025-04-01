#include "SKeyViewerWidget.h"

#include "Misc/DefaultValueHelper.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "WordBeeEditor/API/API.h"

// Construct the Widget
void SKeyViewerWidget::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SVerticalBox)
		// Key Input Field with Reset Button
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(5)
		[
			SNew(SHorizontalBox)

			// Key Input Field
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f) // Allow input field to take most of the space
			[
				SNew(SEditableTextBox)
				.HintText(FText::FromString("Enter Key"))
				.OnTextChanged(this, &SKeyViewerWidget::OnKeyChanged)
			]

			// Reset Button with "Get data" tooltip
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(5, 0) // Add some spacing
			[
				SNew(SButton)
				             .ButtonStyle(FCoreStyle::Get(), "NoBorder") // No border for a cleaner look
				             .OnClicked(this, &SKeyViewerWidget::OnGetDataClicked)
				             .ToolTipText(FText::FromString("Get data"))
				[
					SNew(SImage)
					.Image(FCoreStyle::Get().GetBrush("Icons.Refresh")) // Use a refresh icon
				]
			]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(5)
		[
			SNew(STextBlock).Text(FText::FromString("Min Length"))
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(5)
		[
			SNew(SEditableTextBox)
			                      .Text_Lambda([this]() { return MinLength; }) // Binding to MinLengthText
			                      .OnTextChanged(this, &SKeyViewerWidget::OnMinLengthChanged)
		]

		// Max Length Input F
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(5)
		[
			SNew(STextBlock).Text(FText::FromString("Max Length"))
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(5)
		[
			SNew(SEditableTextBox)
			                      .Text_Lambda([this]() { return MaxLength; }) // Binding to MaxLengthText
			                      .OnTextChanged(this, &SKeyViewerWidget::OnMaxLengthChanged)
		]


		// Custom Fields Grid View
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		.Padding(5)
		[
			SAssignNew(CustomFieldsListView, SListView<TSharedPtr<FCustomField>>)
			.ListItemsSource(&CustomFields)
			.OnGenerateRow(this, &SKeyViewerWidget::OnGenerateRow)
			.HeaderRow(
				SNew(SHeaderRow)
				+ SHeaderRow::Column("ID")
				.DefaultLabel(FText::FromString("ID"))
				.FillWidth(0.2f)
				+ SHeaderRow::Column("Title")
				.DefaultLabel(FText::FromString("Title"))
				.FillWidth(0.4f)
				+ SHeaderRow::Column("Value")
				.DefaultLabel(FText::FromString("Value"))
				.FillWidth(0.4f)
			)
		]
		// Update Button
		+ SVerticalBox::Slot()
		  .AutoHeight()
		  .HAlign(HAlign_Center) // Centering the button
		  .Padding(5)
		[
			SNew(SButton)
			.Text(FText::FromString("Update"))
		]
	];
}

void SKeyViewerWidget::OnMinLengthChanged(const FText& NewText)
{
	if (NewText.IsEmpty())
	{
		MinLength = NewText;
	}
	else
	{
		const FString InputString = NewText.ToString();
		int32 ParsedValue;
		if (InputString.IsNumeric() && InputString.TrimStartAndEnd().Len() > 0 && FDefaultValueHelper::ParseInt(InputString, ParsedValue))
		{
			if (ParsedValue > 0 && ParsedValue < 1000)
			{
				MinLength = NewText;
			}
			else
			{
				MinLength = FText::FromString(TEXT(""));
			}
		}
		else
		{
			MinLength = FText::FromString(TEXT(""));
		}
	}
}

void SKeyViewerWidget::OnMaxLengthChanged(const FText& NewText)
{
	if (NewText.IsEmpty())
	{
		MaxLength = NewText;
	}
	else
	{
		const FString InputString = NewText.ToString();
		int32 ParsedValue;
		if (InputString.IsNumeric() && InputString.TrimStartAndEnd().Len() > 0 && FDefaultValueHelper::ParseInt(InputString, ParsedValue))
		{
			if (ParsedValue > 0 && ParsedValue < 1000)
			{
				MaxLength = NewText;
			}
			else
			{
				MaxLength = FText::FromString(TEXT(""));
			}
		}
		else
		{
			MaxLength = FText::FromString(TEXT(""));
		}
	}
}


// Generate Row for ListView
TSharedRef<ITableRow> SKeyViewerWidget::OnGenerateRow(TSharedPtr<FCustomField> Item,
                                                      const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(STableRow<TSharedPtr<FCustomField>>, OwnerTable)
		[
			SNew(SHorizontalBox)

			// ID Column (matches header "ID")
			+ SHorizontalBox::Slot()
			.FillWidth(0.2f)
			.Padding(5)
			[
				SNew(STextBlock).Text(FText::FromString(Item->Id))
			]

			// Title Column (matches header "Title")
			+ SHorizontalBox::Slot()
			.FillWidth(0.4f)
			.Padding(5)
			[
				SNew(STextBlock).Text(FText::FromString(Item->T))
			]

			// Editable Value Column (matches header "Value")
			+ SHorizontalBox::Slot()
			.FillWidth(0.4f)
			.Padding(5)
			[
				SNew(SEditableTextBox)
				.Text(FText::FromString(Item->V))
				.OnTextCommitted(this, &SKeyViewerWidget::OnVTextCommitted, Item)
			]
		];
}


// Handle V Column Text Edit
void SKeyViewerWidget::OnVTextCommitted(const FText& NewText, ETextCommit::Type CommitType,
                                        TSharedPtr<FCustomField> Item)
{
	if (Item.IsValid())
	{
		Item->V = NewText.ToString();
	}
}

// Populate String Tables
void SKeyViewerWidget::PopulateStringTables()
{
}

void SKeyViewerWidget::PopulateLocalizationKeys()
{
}

// Handle String Table Selection
void SKeyViewerWidget::OnStringTableSelected(TSharedPtr<FString> NewValue, ESelectInfo::Type)
{
	SelectedStringTable = NewValue;
	PopulateLocalizationKeys();
}

// Handle Key Selection
void SKeyViewerWidget::OnKeySelected(TSharedPtr<FString> NewValue, ESelectInfo::Type)
{
	SelectedKey = NewValue;
}

void SKeyViewerWidget::OnKeyChanged(const FText& NewText)
{
	EnteredKey = NewText.ToString(); // Store the entered key
}

FReply SKeyViewerWidget::OnGetDataClicked()
{
	userinfo = SingletonUtil::GetFromIni<FWordbeeUserData>();
	// Handle fetching data based on EnteredKey
	API::PullDocument(userinfo, FString::FromInt(userinfo.DocumentId), FOnPullDocumentComplete::CreateLambda(
		                  [this](FString Result)
		                  {
			                  CustomFields.Empty();
			                  FWordbeeDocument doc;
			                  if (FJsonObjectConverter::JsonObjectStringToUStruct(Result, &doc, 0, CPF_Transient))
			                  {
			                  	if (doc.Segments.Num() < 0)
			                  	{
			                  		return;
			                  	}
			                  	MinLength = FText::AsNumber(doc.Segments[0].chmin);
								  MaxLength = FText::AsNumber(doc.Segments[0].chmax);
			                  	
				                  AddCustomFieldsFromSegment(doc, CustomFields);
				                  UE_LOG(LogTemp, Log, TEXT("CustomFieldsCount: %d"), CustomFields.Num());
				                  if (CustomFieldsListView.IsValid())
				                  {
					                  CustomFieldsListView->RequestListRefresh();
				                  }
			                  }
		                  }
	                  ), {EnteredKey});
	return FReply::Handled();
}

void SKeyViewerWidget::AddCustomFieldsFromSegment(const FWordbeeDocument& Doc,
                                                  TArray<TSharedPtr<FCustomField>>& OutCustomFields)
{
	if (Doc.Segments.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No segments available in the document!"));
		return;
	}
	// Get the first segment
	const FSegment& FirstSegment = Doc.Segments[0];
	// Create a map of id -> value from segment[0].cfs
	TMap<FString, FString> SegmentValues;
	for (const FCustomField& Field : FirstSegment.Cfs)
	{
		SegmentValues.Add(Field.Id, Field.V);
	}
	// Loop through `CfsConfig` (since it's the complete list)
	for (const FCustomField& ConfigField : Doc.CfsConfig)
	{
		UE_LOG(LogTemp, Log, TEXT("CustomField: %s"), *ConfigField.T);
		TSharedPtr<FCustomField> NewField = MakeShared<FCustomField>();
		NewField->Id = ConfigField.Id;
		NewField->T = ConfigField.T;
		// Assign value if found, otherwise leave empty
		NewField->V = SegmentValues.Contains(ConfigField.Id) ? SegmentValues[ConfigField.Id] : TEXT("");
		// Add shared pointer to the array
		OutCustomFields.Add(NewField);
	}
}
