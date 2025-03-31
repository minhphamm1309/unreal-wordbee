#include "WordBeeEditorModule.h"

#include "DirectoryWatcherModule.h"
#include "IDirectoryWatcher.h"
#include "JsonObjectConverter.h"
#include "WordBeeEditor/Command/CreateDataAsset/CreateConfigDataAssetCommand.h"
#include "WordBeeEditor/Command/CreateDataAsset/CreateUserDataAssetCommand.h"
#include "WordBeeEditor/EditorWindow/SKeyViewerWidget.h"
#include "WordBeeEditor/Models/FDocumentData.h"
#include "WordBeeEditor/Models/WordbeeResponse.h"
#include "WordBeeEditor/Models/WordbeeUserData.h"
#include "WordBeeEditor/Utils/SingletonUtil.h"


void FWordBeeEditorModule::StartupModule()
{
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(WordBeeConfigEditorTabName, FOnSpawnTab::CreateRaw(this, &FWordBeeEditorModule::OnSpawnPluginTab))
				.SetDisplayName(FText::FromString("Wordbee Config Editor"))
				.SetMenuType(ETabSpawnerMenuType::Hidden);
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(WordBeeKeyViewerTabName, FOnSpawnTab::CreateRaw(this, &FWordBeeEditorModule::OnSpawnKeyViewerTab))
				.SetDisplayName(FText::FromString("Wordbee Key Viewer"))
				.SetMenuType(ETabSpawnerMenuType::Hidden);

	// Optionally, add a menu entry
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FWordBeeEditorModule::RegisterMenus));

	StartWatcherLocalization();
	UE_LOG(LogTemp, Log, TEXT("WordBeeEditor: Successfully started up module!"));
}

void FWordBeeEditorModule::StartWatcherLocalization()
{
	FString wordbeeInitPath = FPaths::ProjectSavedDir() / TEXT("Wordbee") / TEXT("Localization");
	if (!FPaths::DirectoryExists(wordbeeInitPath))
	{
		FPlatformFileManager::Get().GetPlatformFile().CreateDirectory(*wordbeeInitPath);
	}
	FString LocDir = FPaths::ProjectContentDir() / TEXT("Localization");

	//copy the localization files to the new wordbeeInitPath
	IFileManager& FileManager = IFileManager::Get();
	TArray<FString> Files;
	FileManager.FindFilesRecursive(Files, *LocDir, TEXT("*.*"), true, false, false);
	for (const FString& File : Files)
	{
		FString DestFile = File.Replace(*LocDir, *wordbeeInitPath);
		FileManager.Copy(*DestFile, *File);
	}
	
	IDirectoryWatcher* DirectoryWatcher = FModuleManager::LoadModuleChecked<FDirectoryWatcherModule>("DirectoryWatcher").Get();
	if (DirectoryWatcher)
	{
		DirectoryWatcher->RegisterDirectoryChangedCallback_Handle(
			LocDir,
			IDirectoryWatcher::FDirectoryChanged::CreateLambda([this](const TArray<FFileChangeData>& FileChanges)
			{
				FString LocDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir() / TEXT("Localization"));
				for (const FFileChangeData& Change : FileChanges)
				{
					FString fileChangedPath = Change.Filename.Replace(*LocDir, TEXT(""));
					UE_LOG(LogTemp, Warning, TEXT("Localization file changed: %s"), *fileChangedPath);
					SyncLocalizationFileChanged(fileChangedPath);
				}
			}),
			WatcherHandle
		);
	}
	
}

void FWordBeeEditorModule::StopWatcherLocalization()
{
	FString LocalizationDir = FPaths::ProjectContentDir() / TEXT("Localization");

	IDirectoryWatcher* DirectoryWatcher = FModuleManager::LoadModuleChecked<FDirectoryWatcherModule>("DirectoryWatcher").Get();
	if (DirectoryWatcher && WatcherHandle.IsValid())
	{
		DirectoryWatcher->UnregisterDirectoryChangedCallback_Handle(LocalizationDir, WatcherHandle);
		WatcherHandle.Reset();
		UE_LOG(LogTemp, Log, TEXT("Stopped watching localization files."));
	}
}

void FWordBeeEditorModule::ShutdownModule()
{
	StopWatcherLocalization();
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

void FWordBeeEditorModule::UpdateDocumentData(const FDocumentData& Document, const FString& Key,const FString& langCode, const FString& Text,
	TArray<FRecord> Array)
{
	for (auto record : Document.records)
	{
		if (record.recordID.Equals(Key))
		{
			for (auto Column : record.columns)
			{
				if (Column.columnID.Equals(langCode))
				{
					Column.text = Text;
					Array.Add(record);
					break;
				}
			}
			break;
		}
	}
}

void FWordBeeEditorModule::SyncLocalizationFileChanged(const FString& fileChanged)
{
	FString wordbeeInitPath = FPaths::ProjectSavedDir() / TEXT("Wordbee") / TEXT("Localization") / fileChanged;
	FString LocDir = FPaths::ProjectContentDir() / TEXT("Localization") / fileChanged;
	TArray<FString> Parts;
	FString langCode = fileChanged.ParseIntoArray(Parts, TEXT("/"), true) > 0 ? Parts[1] : TEXT("");

	FLocalizationData fileIni = ParseLocalizationData(wordbeeInitPath);
	FLocalizationData fileLoc = ParseLocalizationData(LocDir);

	FDocumentData document = SingletonUtil::GetFromIni<FDocumentData>();
	TArray<FRecord> Records;
	for (const FSubnamespace& SubnamespaceIni : fileIni.Subnamespaces)
	{
		for (const FSubnamespace& SubnamespaceLoc : fileLoc.Subnamespaces)
		{
			if (SubnamespaceIni.Namespace == SubnamespaceLoc.Namespace)
			{
				for (const FTranslationEntry& EntryIni : SubnamespaceIni.Children)
				{
					for (const FTranslationEntry& EntryLoc : SubnamespaceLoc.Children)
					{
						if (EntryIni.Key == EntryLoc.Key && EntryIni.Translation.Text != EntryLoc.Translation.Text)
						{
							UE_LOG(LogTemp, Warning, TEXT("Key: %s - Old Translation: %s - New Translation: %s"),
								*EntryIni.Key, *EntryIni.Translation.Text, *EntryLoc.Translation.Text);
							UpdateDocumentData(document, EntryIni.Key, langCode, EntryLoc.Translation.Text, Records);
						}
					}
				}
			}
		}
	}
	
}

FLocalizationData FWordBeeEditorModule::ParseLocalizationData(const FString& filePath)
{
	FLocalizationData LocalizationData;

	FString JsonString;
	if (FFileHelper::LoadFileToString(JsonString, *filePath))
	{
		if (!FJsonObjectConverter::JsonObjectStringToUStruct(JsonString, &LocalizationData, 0, 0))
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to read file: %s"), *filePath);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to read file: %s"), *filePath);
	}

	return LocalizationData;
}
