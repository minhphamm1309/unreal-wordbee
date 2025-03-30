#include "SKeyViewerWidget.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SEditableTextBox.h"

// Construct the Widget
void SKeyViewerWidget::Construct(const FArguments& InArgs)
{
    // Sample data for testing
    CustomFields.Add(MakeShared<FCustomField>(FCustomField{TEXT("1"), TEXT("Title1"), TEXT("Value1")}));
    CustomFields.Add(MakeShared<FCustomField>(FCustomField{TEXT("2"), TEXT("Title2"), TEXT("Value2")}));

    ChildSlot
    [
        SNew(SVerticalBox)
        // String Table Selection
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(5)
        [
            SNew(STextBlock).Text(FText::FromString("Select String Table"))
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(5)
        [
            SNew(SSearchableComboBox)
            .OptionsSource(&AvailableStringTables)
            .OnSelectionChanged(this, &SKeyViewerWidget::OnStringTableSelected)
            .OnGenerateWidget_Lambda([](TSharedPtr<FString> InOption)
            {
                return SNew(STextBlock).Text(FText::FromString(*InOption));
            })
            [
                SNew(STextBlock).Text(FText::FromString("Select a table"))
            ]
        ]

        // Key Selection
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(5)
        [
            SNew(STextBlock).Text(FText::FromString("Select Key"))
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(5)
        [
            SAssignNew(KeyComboBox, SSearchableComboBox)
            .OptionsSource(&AvailableKeys)
            .OnSelectionChanged(this, &SKeyViewerWidget::OnKeySelected)
            .OnGenerateWidget_Lambda([](TSharedPtr<FString> InOption)
            {
                return SNew(STextBlock).Text(FText::FromString(*InOption));
            })
            [
                SNew(STextBlock).Text(FText::FromString("Select a key"))
            ]
        ]
        
        // Min Length Input Field
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
            .OnTextChanged(this, &SKeyViewerWidget::OnMinLengthChanged)
        ]

        // Max Length Input Field
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
            .OnTextChanged(this, &SKeyViewerWidget::OnMaxLengthChanged)
        ]

        // Custom Fields Grid View
        + SVerticalBox::Slot()
        .FillHeight(1.0f)
        .Padding(5)
        [
            SNew(SListView<TSharedPtr<FCustomField>>)
            .ListItemsSource(&CustomFields)
            .OnGenerateRow(this, &SKeyViewerWidget::OnGenerateRow)
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

// Handle Min Length Change
void SKeyViewerWidget::OnMinLengthChanged(const FText& NewText)
{
    // Handle min length change logic here
}

// Handle Max Length Change
void SKeyViewerWidget::OnMaxLengthChanged(const FText& NewText)
{
    // Handle max length change logic here
}

// Generate Row for ListView
TSharedRef<ITableRow> SKeyViewerWidget::OnGenerateRow(TSharedPtr<FCustomField> Item, const TSharedRef<STableViewBase>& OwnerTable)
{
    return SNew(STableRow<TSharedPtr<FCustomField>>, OwnerTable)
    [
        SNew(SHorizontalBox)

        // ID Column
        + SHorizontalBox::Slot()
        .AutoWidth()
        .Padding(5)
        [
            SNew(STextBlock).Text(FText::FromString(Item->Id))
        ]

        // Title Column
        + SHorizontalBox::Slot()
        .FillWidth(1)
        .Padding(5)
        [
            SNew(STextBlock).Text(FText::FromString(Item->Title))
        ]

        // Editable V Column
        + SHorizontalBox::Slot()
        .FillWidth(1)
        .Padding(5)
        [
            SNew(SEditableTextBox)
            .Text(FText::FromString(Item->V))
            .OnTextCommitted(this, &SKeyViewerWidget::OnVTextCommitted, Item)
        ]
    ];
}

// Handle V Column Text Edit
void SKeyViewerWidget::OnVTextCommitted(const FText& NewText, ETextCommit::Type CommitType, TSharedPtr<FCustomField> Item)
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

void SKeyViewerWidget::PopulateKeysForSelectedTable()
{
    
}

// Handle String Table Selection
void SKeyViewerWidget::OnStringTableSelected(TSharedPtr<FString> NewValue, ESelectInfo::Type)
{
    SelectedStringTable = NewValue;
    PopulateKeysForSelectedTable();
}

// Handle Key Selection
void SKeyViewerWidget::OnKeySelected(TSharedPtr<FString> NewValue, ESelectInfo::Type)
{
    SelectedKey = NewValue;
}