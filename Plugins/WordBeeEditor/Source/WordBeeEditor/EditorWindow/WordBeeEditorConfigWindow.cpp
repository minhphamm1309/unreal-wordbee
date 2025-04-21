#include "WordBeeEditorConfigWindow.h"
#include "Tabs/SConnectTab.h"
#include "Tabs/SAboutTabUI.h"
#include "Tabs/SDocumentInfo.h"
#include "Tabs/SEditorConfigWidget.h"
#include "Widgets/Layout/SWidgetSwitcher.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SWordBeeEditorConfigWindow::Construct(const FArguments& InArgs)
{
	WidgetSwitcher = SNew(SWidgetSwitcher);

	WidgetSwitcher->AddSlot()
	[
		SAssignNew(ConnectTabWidget, SConnectTab)
	];

	WidgetSwitcher->AddSlot()
	[
		SAssignNew(EditorConfigWidget, SEditorConfigWidget)
	];

	WidgetSwitcher->AddSlot()
	[
		SAssignNew(DocumentInfoWidget, SDocumentInfo)
	];

	WidgetSwitcher->AddSlot()
	[
		SNew(SAboutTabUI)
	];
	// Pass parent to ConnectTab
	ConnectTabWidget->SetParentWindow(SharedThis(this));
	DocumentInfoWidget->SetParentWindow(SharedThis(this));
	OnDocumentChanged();
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
void SWordBeeEditorConfigWindow::OnDocumentChanged()
{
	if (DocumentInfoWidget.IsValid())
	{
		DocumentInfoWidget->RefreshDocumentInfo([this](bool bSuccess)
		{
			if (EditorConfigWidget.IsValid())
			{
				EditorConfigWidget->FetchLangsFromAPI();
			}
		});
	}
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
