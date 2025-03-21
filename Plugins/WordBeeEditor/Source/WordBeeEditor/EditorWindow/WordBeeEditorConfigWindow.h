#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class SWordBeeEditorConfigWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SWordBeeEditorConfigWindow) {}
	SLATE_END_ARGS()

	/** Constructs the widget */
	void Construct(const FArguments& InArgs);

private:
	TSharedPtr<SWidgetSwitcher> WidgetSwitcher;

	// Functions to switch between tabs
	FReply OnConnectTabClicked();
	FReply OnConfigTabClicked();
	FReply OnActiveDocTabClicked();
	FReply OnAboutTabClicked();
	
};
