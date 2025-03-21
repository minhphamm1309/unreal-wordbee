#include "WordBeeEditorConfigWindow.h"

#include "Tabs/SConnectTab.h"
#include "Widgets/Layout/SWidgetSwitcher.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SWordBeeEditorConfigWindow::Construct(const FArguments& InArgs)
{
	WidgetSwitcher = SNew(SWidgetSwitcher);

	WidgetSwitcher->AddSlot()
	[
		SNew(SConnectTab)
	];

	WidgetSwitcher->AddSlot()
	[
		SNew(STextBlock).Text(FText::FromString("Configuration Tab Content"))
	];

	WidgetSwitcher->AddSlot()
	[
		SNew(STextBlock).Text(FText::FromString("Active Document Info Tab Content"))
	];

	WidgetSwitcher->AddSlot()
	[
		SNew(STextBlock).Text(FText::FromString("About Tab Content"))
	];

	ChildSlot
	[
		SNew(SVerticalBox)

		// Create tab buttons at the top
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(5)
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.Padding(2)
			[
				SNew(SButton)
				.Text(FText::FromString("Connect"))
				.OnClicked(this, &SWordBeeEditorConfigWindow::OnConnectTabClicked)
			]

			+ SHorizontalBox::Slot()
			.Padding(2)
			[
				SNew(SButton)
				.Text(FText::FromString("Configuration"))
				.OnClicked(this, &SWordBeeEditorConfigWindow::OnConfigTabClicked)
			]

			+ SHorizontalBox::Slot()
			.Padding(2)
			[
				SNew(SButton)
				.Text(FText::FromString("Active Document Info"))
				.OnClicked(this, &SWordBeeEditorConfigWindow::OnActiveDocTabClicked)
			]

			+ SHorizontalBox::Slot()
			.Padding(2)
			[
				SNew(SButton)
				.Text(FText::FromString("About"))
				.OnClicked(this, &SWordBeeEditorConfigWindow::OnAboutTabClicked)
			]
		]

		// Widget switcher will switch content based on selected tab
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		.Padding(5)
		[
			WidgetSwitcher.ToSharedRef()  // Display the currently selected widget (tab content)
		]
	];
}

FReply SWordBeeEditorConfigWindow::OnConnectTabClicked()
{
	WidgetSwitcher->SetActiveWidgetIndex(0);
	return FReply::Handled();
}

FReply SWordBeeEditorConfigWindow::OnConfigTabClicked()
{
	WidgetSwitcher->SetActiveWidgetIndex(1);
	return FReply::Handled();
}

FReply SWordBeeEditorConfigWindow::OnActiveDocTabClicked()
{
	WidgetSwitcher->SetActiveWidgetIndex(2);
	return FReply::Handled();
}

FReply SWordBeeEditorConfigWindow::OnAboutTabClicked()
{
	WidgetSwitcher->SetActiveWidgetIndex(3);
	return FReply::Handled();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
