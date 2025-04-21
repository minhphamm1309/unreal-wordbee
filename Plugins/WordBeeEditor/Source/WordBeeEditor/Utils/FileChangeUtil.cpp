#include "FileChangeUtil.h"

#include "LocalizeUtil.h"
#include "Locate.h"
#include "SingletonUtil.h"
#include "WordBeeEditor/Models/FDocumentData.h"

void FileChangeUtil::FileChange(const FString& fileChanged)
{
	FString wordbeeInitPath = FPaths::ProjectSavedDir() / TEXT("Wordbee") / TEXT("Localization") / fileChanged;
	FString LocDir = FPaths::ProjectContentDir() / TEXT("Localization") / fileChanged;
	FString NormalizedPath = fileChanged;
	FPaths::NormalizeFilename(NormalizedPath);  // Converts all \ to /
	FString PathOnly = FPaths::GetPath(NormalizedPath);
	TArray<FString> Parts;
	PathOnly.ParseIntoArray(Parts, TEXT("/"), true);
	FString langCode = Parts.Num() > 0 ? Parts.Last() : TEXT("");

	FLocalizationData fileIni = ParseLocalizationData(wordbeeInitPath);
	FLocalizationData fileLoc = ParseLocalizationData(LocDir);

	FDocumentData document = wordbee::SingletonUtil<FDocumentData>::GetFromIni();
	// Map for quick lookup of existing records in Document
	TMap<FString, FRecord*> DocumentRecordMap;
	for (FRecord& Record : document.records)
	{
		DocumentRecordMap.Add(Record.recordID, &Record);
	}

	// Loop through loc (new data)
	for (const FSubnamespace& SubnamespaceLoc : fileLoc.Subnamespaces)
	{
		// Try to find matching subnamespace in ini (old)
		const FSubnamespace* MatchingIni = fileIni.Subnamespaces.FindByPredicate(
			[&](const FSubnamespace& IniSub) { return IniSub.Namespace == SubnamespaceLoc.Namespace; }
		);

		for (const FTranslationEntry& EntryLoc : SubnamespaceLoc.Children)
		{
			bool bIsExistingKey = false;
			FString OldText = TEXT("");

			if (MatchingIni)
			{
				const FTranslationEntry* EntryIni = MatchingIni->Children.FindByPredicate(
					[&](const FTranslationEntry& Entry) { return Entry.Key == EntryLoc.Key; }
				);

				if (EntryIni)
				{
					bIsExistingKey = true;
					OldText = EntryIni->Translation.Text;
				}
			}

			// Update if the text differs
			if ((bIsExistingKey && OldText != EntryLoc.Translation.Text) || !bIsExistingKey)
			{
				UpdateDocumentData(document, EntryLoc.Key, langCode, EntryLoc.Translation.Text);
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
	FDocumentData document = wordbee::SingletonUtil<FDocumentData>::GetFromIni();
	TArray<FRecord> records;
	for (int i = 0; i < document.records.Num(); i++)
	{
		FRecord record = document.records[i];
		records.Add(record);
	}
	return records;
}

void FileChangeUtil::UpdateDocumentData(FDocumentData& Document, const FString& Key, const FString& langCode, const FString& Text)
{
	bool bRecordFound = false;

	for (int i = 0; i < Document.records.Num(); i++)
	{
		if (Document.records[i].recordID.Equals(Key))
		{
			bRecordFound = true;
			bool bColumnFound = false;

			for (int j = 0; j < Document.records[i].columns.Num(); j++)
			{
				if (Document.records[i].columns[j].columnID.Equals(langCode))
				{
					Document.records[i].columns[j].text = Text;
					UpdateRecord(Document.records[i]);
					wordbee::SingletonUtil<FDocumentData>::SaveToIni(Document);
					return;
				}
			}

			// If the column doesn't exist, add it
			if (!bColumnFound)
			{
				FColumn NewColumn;
				NewColumn.columnID = langCode;
				NewColumn.text = Text;
				Document.records[i].columns.Add(NewColumn);
				UpdateRecord(Document.records[i]);
				wordbee::SingletonUtil<FDocumentData>::SaveToIni(Document);
				return;
			}
		}
	}

	// If the record doesn't exist, create it
	if (!bRecordFound)
	{
		FRecord NewRecord;
		NewRecord.recordID = Key;

		FColumn NewColumn;
		NewColumn.columnID = langCode;
		NewColumn.text = Text;

		NewRecord.columns.Add(NewColumn);
		Document.records.Add(NewRecord);
		UpdateRecord(NewRecord);
	}
	wordbee::SingletonUtil<FDocumentData>::SaveToIni(Document);
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
