#include "FileChangeUtil.h"

#include "LocalizeUtil.h"
#include "SingletonUtil.h"
#include "WordBeeEditor/Models/FDocumentData.h"

void FileChangeUtil::FileChange(const FString& fileChanged)
{
	FString wordbeeInitPath = FPaths::ProjectSavedDir() / TEXT("Wordbee") / TEXT("Localization") / fileChanged;
	FString LocDir = FPaths::ProjectContentDir() / TEXT("Localization") / fileChanged;
	TArray<FString> Parts;
	FString langCode = fileChanged.ParseIntoArray(Parts, TEXT("/"), true) > 0 ? Parts[1] : TEXT("");

	FLocalizationData fileIni = ParseLocalizationData(wordbeeInitPath);
	FLocalizationData fileLoc = ParseLocalizationData(LocDir);

	FDocumentData document = SingletonUtil::GetFromIni<FDocumentData>();
	
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
							UpdateDocumentData(document, EntryIni.Key, langCode, EntryLoc.Translation.Text);
						}
					}
				}
			}
		}
	}

}

FLocalizationData FileChangeUtil::ParseLocalizationData(const FString& filePath)
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

void FileChangeUtil::UpdateRecord(const FRecord& Record)
{
	LocalizeUtil * localizeUtil = Locate<LocalizeUtil>::Get();

	// Update the record in the localizeUtil->RecordsChanged
	for (int i = 0; i < localizeUtil->RecordsChanged.Num(); i++)
	{
		if (localizeUtil->RecordsChanged[i].recordID.Equals(Record.recordID))
		{
			localizeUtil->RecordsChanged[i] = Record;
			return;
		}
	}
	localizeUtil->RecordsChanged.Add(Record);
}

TArray<FRecord> FileChangeUtil::GetCurrentRecords()
{
	FDocumentData document = SingletonUtil::GetFromIni<FDocumentData>();
	TArray<FRecord> records;
	for (int i = 0; i < document.records.Num(); i++)
	{
		FRecord record = document.records[i];
		records.Add(record);
	}
	return records;
}

void FileChangeUtil::UpdateDocumentData(FDocumentData& Document, const FString& Key, const FString& langCode,
	const FString& Text)
{
	for (int i = 0; i < Document.records.Num(); i++)
	{
		if (Document.records[i].recordID.Equals(Key))
		{
			TArray<FColumn> cols = Document.records[i].columns;
			for (int j = 0; j < cols.Num(); j++)
			{
				if (cols[j].columnID.Equals(langCode))
				{
					cols[j].text = Text;
					Document.records[i].columns = cols;
					UpdateRecord(Document.records[i]);
					break;
				}
			}
			break;
		}
	}

	SingletonUtil::SaveToIni(Document);
	// Update the record in the localizeUtil->RecordsChanged
}

void FileChangeUtil::CopyLocalizeToSaved()
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
}
