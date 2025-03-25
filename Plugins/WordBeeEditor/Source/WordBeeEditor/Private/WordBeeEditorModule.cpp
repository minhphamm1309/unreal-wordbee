#include "WordBeeEditorModule.h"

#include "WordBeeEditor/Command/CreateDataAsset/CreateConfigDataAssetCommand.h"
#include "WordBeeEditor/Command/CreateDataAsset/CreateUserDataAssetCommand.h"

void FWordBeeEditorModule::StartupModule()
{
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(WordBeeConfigEditorTabName, FOnSpawnTab::CreateRaw(this, &FWordBeeEditorModule::OnSpawnPluginTab))
				.SetDisplayName(FText::FromString("WordBee Config Editor"))
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
		FText::FromString("WordBee Link"),  // Displayed name in the menu
		FText::FromString("Access WordBee tools"),  // Tooltip for the submenu
		FNewToolMenuDelegate::CreateLambda([this](UToolMenu* InMenu)
		{
			// Inside the submenu, add the "Configure" button
			FToolMenuSection& SubMenuSection = InMenu->AddSection("WordBeeToolsSection", FText::FromString("WordBee Tools"));
			SubMenuSection.AddMenuEntry(
				"WordBeeConfigEditorTab",
				FText::FromString("Configure"),
				FText::FromString("Open the WordBee Config Editor"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateRaw(this, &FWordBeeEditorModule::OnMenuButtonClicked))
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
