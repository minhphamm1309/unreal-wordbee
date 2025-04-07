#pragma once

#include "CoreMinimal.h"
#include "FLinkDocumentResponseData.h"
#include "Interfaces/IHttpRequest.h"
#include "WordBeeEditor/Models/FDocumentData.h"
#include "WordBeeEditor/Models/WordbeeUserData.h"
#include "WordBeeEditor/Models/FWordbeeFile.h"

struct FWordbeeDocument;
DECLARE_DELEGATE_TwoParams(FOnLinkDocumentComplete, bool, const FWordbeeDocument&);

class ULinkDocumentCommand 
{
public:
	static void Execute(FWordbeeUserData UserInfo,  FString DocumentId, FOnLinkDocumentComplete callback);
	static bool ParseJsonToWordbeeFile(const FString& JsonString, FWordbeeDocument& OutWordbeeFile);
	static  void ResetDocument(FDocumentData& Document);
	static ELanguage ConvertStringToEnum(const FString& Lang);
	static void UpdateLanguageSupports(FDocumentData& Document);
	static void UpdateDocument(FDocumentData& document, const FWordbeeDocument& WordbeeFile, const FString& String, const FString& ProjectName, const
	                           FString& DocumentName);
	static void SaveDocument(const FWordbeeDocument& resDocument, const FString& projectId, const FString& projectName,
	                  const FString& documentName);
	static  TArray<TArray<FString>> CreateFixedLangSupports();
	static void AddSegmentsToDocument(FDocumentData& document, const TArray<FSegment>& Segments, const TArray<TArray<FString>>& FixedLangSupports);
	static FString GetConvertedLanguage(const FString& Key, const TArray<TArray<FString>>& FixedLangSupports);
	static TArray<FString> EnumToStringArray(UEnum* EnumClass);
};

