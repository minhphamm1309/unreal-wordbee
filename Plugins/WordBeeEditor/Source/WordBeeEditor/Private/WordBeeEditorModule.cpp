#include "WordBeeEditorModule.h"

#include "WordBeeEditor/Command/CreateDataAsset/CreateConfigDataAssetCommand.h"
#include "WordBeeEditor/Command/CreateDataAsset/CreateUserDataAssetCommand.h"
#include "WordBeeEditor/EditorWindow/SKeyViewerWidget.h"
#include "WordBeeEditor/EditorWindow/SWorkFlowStatus.h"

void FWordBeeEditorModule::StartupModule()
{
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(WordBeeConfigEditorTabName, FOnSpawnTab::CreateRaw(this, &FWordBeeEditorModule::OnSpawnPluginTab))
				.SetDisplayName(FText::FromString("Wordbee Config Editor"))
				.SetMenuType(ETabSpawnerMenuType::Hidden);
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(WordBeeKeyViewerTabName, FOnSpawnTab::CreateRaw(this, &FWordBeeEditorModule::OnSpawnKeyViewerTab))
				.SetDisplayName(FText::FromString("Wordbee Key Viewer"))
				.SetMenuType(ETabSpawnerMenuType::Hidden);
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(WorkFlowStatusTabName, FOnSpawnTab::CreateRaw(this, &FWordBeeEditorModule::OnSpawnWorkFlowStatusTab))
				.SetDisplayName(FText::FromString("Workflow Status Window"))
				.SetMenuType(ETabSpawnerMenuType::Hidden);

	// Optionally, add a menu entry
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FWordBeeEditorModule::RegisterMenus));


	UE_LOG(LogTemp, Log, TEXT("WordBeeEditor: Successfully started up module!"));
}

void FWordBeeEditorModule::ShutdownModule()
{
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(WordBeeConfigEditorTabName);
}

TSharedRef<SDockTab> FWordBeeEditorModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab)
			.TabRole(ETabRole::NomadTab)
			[
				SNew(SWordBeeEditorConfigWindow)
			];
}
TSharedRef<SDockTab> FWordBeeEditorModule::OnSpawnKeyViewerTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab)
			.TabRole(ETabRole::NomadTab)
			[
				SNew(SKeyViewerWidget)
			];
}
TSharedRef<SDockTab> FWordBeeEditorModule::OnSpawnWorkFlowStatusTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab)
			.TabRole(ETabRole::NomadTab)
			[
				SNew(SWorkFlowStatus)
			];
}

void FWordBeeEditorModule::RegisterMenus()
{
	FToolMenuOwnerScoped OwnerScoped(this);
	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Tools");
	FToolMenuSection& Section = Menu->AddSection("CustomTools", FText::FromString("Custom Tools"));
	// Section.AddMenuEntry(
	// 	"WordBeeConfigEditorTab",
	// 	FText::FromString("Configure"),
	// 	FText::FromString("Open the WordBee Config Editor"),
	// 	FSlateIcon(),
	// 	FUIAction(FExecuteAction::CreateRaw(this, &FWordBeeEditorModule::OnMenuButtonClicked))
	// );

	// Add a submenu called "WordBee Link"
	Section.AddSubMenu(
		"WordBeeLinkSubmenu",  // Unique name for the submenu
		FText::FromString("Wordbee Link"),  // Displayed name in the menu
		FText::FromString("Access WordBee tools"),  // Tooltip for the submenu
		FNewToolMenuDelegate::CreateLambda([this](UToolMenu* InMenu)
		{
			// Inside the submenu, add the "Configure" button
			FToolMenuSection& SubMenuSection = InMenu->AddSection("WordBeeToolsSection", FText::FromString("WordBee Tools"));
			SubMenuSection.AddMenuEntry(
				"WordBeeConfigEditorTab",
				FText::FromString("Configure"),
				FText::FromString("Open the Wordbee Config Editor"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateRaw(this, &FWordBeeEditorModule::OnMenuButtonClicked))
			);
			SubMenuSection.AddMenuEntry(
				"WordBeeKeyViewerTab",
				FText::FromString("Key viewer"),
				FText::FromString("Open to edit metadata"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateRaw(this, &FWordBeeEditorModule::OnKeyViewerClicked))
			);
			SubMenuSection.AddMenuEntry(
				"WorkFlowStatusTab",
				FText::FromString("Workflow status"),
				FText::FromString("Open to view workflow status"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateRaw(this, &FWordBeeEditorModule::OnWorkFlowStatusClick))
			);
		})
	);
}

void FWordBeeEditorModule::OnMenuButtonClicked()
{
	CreateUserDataAssetCommand::Execute();
	CreateConfigDataAssetCommand::CreateConfigDataAsset();
	FGlobalTabmanager::Get()->TryInvokeTab(WordBeeConfigEditorTabName);
}
void FWordBeeEditorModule::OnKeyViewerClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(WordBeeKeyViewerTabName);
}
void FWordBeeEditorModule::OnWorkFlowStatusClick()
{
	FGlobalTabmanager::Get()->TryInvokeTab(WorkFlowStatusTabName);
}
