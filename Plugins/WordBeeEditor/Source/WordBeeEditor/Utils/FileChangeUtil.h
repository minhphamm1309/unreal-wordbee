#pragma once

#include "FLocalizationData.h"
#include "WordBeeEditor/Models/FDocumentData.h"
#include "WordBeeEditor/Models/FRecord.h"

class FileChangeUtil
{
public:
	static void FileChange(const FString& fileName);

	static FLocalizationData ParseLocalizationData(const FString& filePath);
	static void UpdateRecord(const FRecord& Record);
	static void UpdateDocumentData(FDocumentData& Document, const FString& Key, const FString& langCode, const FString& Text);
	static void CopyLocalizeToSaved();
};
