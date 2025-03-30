// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"
#include "WordBeeEditorModule.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "WordBeeEditor/EditorWindow/WordBeeEditorConfigWindow.h"

static const FName WordBeeConfigEditorTabName("WordBeeConfigEditorTab");
static const FName WordBeeKeyViewerTabName("WordBeeKeyViewerTab");
/**
 * This is the module definition for the editor mode. You can implement custom functionality
 * as your plugin module starts up and shuts down. See IModuleInterface for more extensibility options.
 */
class FWordBeeEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;

	virtual void ShutdownModule() override;

	TSharedRef<SDockTab> OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs);
	TSharedRef<SDockTab> OnSpawnKeyViewerTab(const FSpawnTabArgs& SpawnTabArgs);
	void RegisterMenus();

	void OnMenuButtonClicked();
	void OnKeyViewerClicked();
};

IMPLEMENT_MODULE(FWordBeeEditorModule, MyEditor)
