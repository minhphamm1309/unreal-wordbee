#pragma once

#include "CoreMinimal.h"
#include "Tabs/SConnectTab.h"
#include "Tabs/SDocumentInfo.h"
#include "Tabs/SEditorConfigWidget.h"
#include "Widgets/SCompoundWidget.h"

class SWordBeeEditorConfigWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SWordBeeEditorConfigWindow) {}
	SLATE_END_ARGS()

	/** Constructs the widget */
	void Construct(const FArguments& InArgs);
	void OnDocumentChanged();
private:
	TSharedPtr<SWidgetSwitcher> WidgetSwitcher;
	TSharedPtr<SConnectTab> ConnectTabWidget;
	TSharedPtr<SEditorConfigWidget> EditorConfigWidget;
	TSharedPtr<SDocumentInfo> DocumentInfoWidget;
	// Functions to switch between tabs
	FReply OnConnectTabClicked();
	FReply OnConfigTabClicked();
	FReply OnActiveDocTabClicked();
	FReply OnAboutTabClicked();
	
};
