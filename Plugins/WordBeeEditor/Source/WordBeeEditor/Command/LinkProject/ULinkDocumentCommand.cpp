#include "ULinkDocumentCommand.h"

#include "WordBeeEditor/API/API.h"
#include "WordBeeEditor/Command/StoredLocalize/StoredLocailzationCommand.h"
#include "WordBeeEditor/Models/FDocumentData.h"
#include "WordBeeEditor/Models/FColumn.h"
#include "WordBeeEditor/Models/FLangSupport.h"
#include "WordBeeEditor/Models/FRecord.h"
#include "WordBeeEditor/Models/FSegment.h"
#include "WordBeeEditor/Models/FSegmentText.h"
#include "WordBeeEditor/Utils/SingletonUtil.h"

void ULinkDocumentCommand::Execute(FWordbeeUserData UserInfo, const FString DocumentId,
                                   FOnLinkDocumentComplete callback)
{
	API::PullDocument(UserInfo, DocumentId, FOnPullDocumentComplete::CreateLambda([=](const FString& downloadContent)
	{
		FWordbeeDocument WordbeeFile;
		if (ParseJsonToWordbeeFile(downloadContent, WordbeeFile))
		{
			callback.ExecuteIfBound(true, WordbeeFile);
			UE_LOG(LogTemp, Log, TEXT("Parsed Wordbee file successfully"));
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Parsed Wordbee file failed"));
			callback.ExecuteIfBound(false, WordbeeFile);
		}
	}));
}

bool ULinkDocumentCommand::ParseJsonToWordbeeFile(const FString& JsonString, FWordbeeDocument& OutWordbeeFile)
{
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		// Deserialize basic fields
		OutWordbeeFile.Type = JsonObject->GetStringField("type");
		OutWordbeeFile.Version = JsonObject->GetStringField("version");
		OutWordbeeFile.Dsid = JsonObject->GetIntegerField("dsid");
		OutWordbeeFile.Did = JsonObject->GetIntegerField("did");

		// Parse segments
		const TArray<TSharedPtr<FJsonValue>>* SegmentsArray;
		if (JsonObject->TryGetArrayField("segments", SegmentsArray))
		{
			for (const TSharedPtr<FJsonValue>& SegmentValue : *SegmentsArray)
			{
				TSharedPtr<FJsonObject> SegmentObject = SegmentValue->AsObject();
				FSegment Segment;
				Segment.key = SegmentObject->GetStringField("key");
				Segment.component = SegmentObject->GetStringField("component");
				Segment.dt = SegmentObject->GetStringField("dt");
				Segment.format = SegmentObject->GetStringField("format");
				Segment.bsid = SegmentObject->GetIntegerField("bsid");
				Segment.chmin = SegmentObject->GetIntegerField("chmin");
				Segment.chmax = SegmentObject->GetIntegerField("chmax");

				// Parse texts
				const TSharedPtr<FJsonObject>* TextsObject;
				if (SegmentObject->TryGetObjectField("texts", TextsObject))
				{
					for (const auto& TextPair : (*TextsObject)->Values)
					{
						TSharedPtr<FJsonObject> TextObject = TextPair.Value->AsObject();
						FSegmentText TextSegment;
						TextSegment.v = TextObject->GetStringField("v");
						TextSegment.st = TextObject->GetIntegerField("st");
						TextSegment.bk = TextObject->GetIntegerField("bk");
						TextSegment.ed = TextObject->GetIntegerField("ed");
						TextSegment.dt = TextObject->GetStringField("dt");

						Segment.texts.Add(TextPair.Key, TextSegment);
					}
				}

				OutWordbeeFile.Segments.Add(Segment);
			}
		}

		return true;
	}

	return false;
}

void ULinkDocumentCommand::ResetDocument(FDocumentData& document)
{
	document.projectId = TEXT("");
	document.projectName = TEXT("");
	document.did = 0;
	document.documentName = TEXT("");
	document.dsid = 0;
	document.cfs.Empty();
	document.records.Empty();
	document.langSupports.Empty();
}

ELanguage ULinkDocumentCommand::ConvertStringToEnum(const FString& LangString)
{
	UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("ELanguages"), true);

	if (!EnumPtr)
	{
		return ELanguage::enUS;
	}

	int32 EnumIndex = EnumPtr->GetIndexByName(FName(*LangString));

	if (EnumIndex == INDEX_NONE)
	{
		return ELanguage::enUS;
	}

	return static_cast<ELanguage>(EnumPtr->GetValueByIndex(EnumIndex));
}

void ULinkDocumentCommand::UpdateLanguageSupports(FDocumentData& document)
{
	TArray<FString> LangSupports;

	for (const FRecord& Record : document.records)
	{
		if (Record.columns.Num() > 0)
		{
			for (const FColumn& Column : Record.columns)
			{
				if (!Column.convertedLang.IsEmpty())
				{
					LangSupports.AddUnique(Column.convertedLang);
				}
			}
		}
	}

	document.langSupports.Empty();

	for (const FString& Lang : LangSupports)
	{
		FLangSupport NewLangSupport;

		NewLangSupport.languagesSuport = ConvertStringToEnum(Lang);
		NewLangSupport.name = Lang;

		document.langSupports.Add(NewLangSupport);
	}
}

void ULinkDocumentCommand::UpdateDocument(FDocumentData& document, const FWordbeeDocument& resDocument,
                                          const FString& projectId, const FString& projectName,
                                          const FString& DocumentName)
{
	document.projectId = projectId;
	document.projectName = projectName;
	document.did = resDocument.Did;
	document.documentName = DocumentName;
	document.dsid = resDocument.Dsid;
	document.cfs = resDocument.CfsConfig;

	document.records.Empty();

	TArray<TArray<FString>> FixedLangSupports = CreateFixedLangSupports();

	AddSegmentsToDocument(document, resDocument.Segments, FixedLangSupports);

	UpdateLanguageSupports(document);
}

TArray<TArray<FString>> ULinkDocumentCommand::CreateFixedLangSupports()
{
	TArray<FString> TempLanguages = EnumToStringArray(FindObject<UEnum>(ANY_PACKAGE, TEXT("ELanguage"), true));

	TArray<TArray<FString>> FixedLangSupports;
	for (const FString& TempLang : TempLanguages)
	{
		TArray<FString> Lang;
		Lang.Add(TempLang.Left(2));
		Lang.Add(TempLang.Mid(2, 2));
		FixedLangSupports.Add(Lang);
	}

	return FixedLangSupports;
}

TArray<FString> ULinkDocumentCommand::EnumToStringArray(UEnum* EnumClass)
{
	TArray<FString> EnumNames;

	if (!EnumClass)
	{
		return EnumNames;
	}

	int32 NumEnums = EnumClass->NumEnums();

	for (int32 i = 0; i < NumEnums - 1; i++)
	{
		FString EnumName = EnumClass->GetNameStringByIndex(i);
		EnumNames.Add(EnumName);
	}

	return EnumNames;
}


void ULinkDocumentCommand::AddSegmentsToDocument(FDocumentData& document, const TArray<FSegment>& Segments,
                                                 const TArray<TArray<FString>>& FixedLangSupports)
{
	for (int32 i = 0; i < Segments.Num(); i++)
	{
		const FSegment& segment = Segments[i];

		FRecord NewRecord;
		NewRecord.recordID = segment.key;
		NewRecord.originalRecordID = segment.key;
		NewRecord.columns.Empty();

		for (const TPair<FString, FSegmentText>& kpText : segment.texts)
		{
			FString ConvertedLang = GetConvertedLanguage(kpText.Key, FixedLangSupports);

			FColumn NewColumn(kpText.Key, kpText.Value.v);
			NewColumn.cfs = kpText.Value.cfs;
			NewColumn.convertedLang = ConvertedLang;
			NewColumn.text = kpText.Value.v;
			NewRecord.columns.Add(NewColumn);
		}

		document.records.Add(NewRecord);
	}
}

FString ULinkDocumentCommand::GetConvertedLanguage(const FString& Key, const TArray<TArray<FString>>& FixedLangSupports)
{
	FString ConvertedLang = TEXT("");
	TArray<FString> LangComponents;
	Key.TrimStartAndEnd().ParseIntoArray(LangComponents, TEXT("-"), true);

	for (const TArray<FString>& FixedLang : FixedLangSupports)
	{
		if (LangComponents.Num() == FixedLang.Num() && LangComponents.Num() == 2)
		{
			if (LangComponents[0].ToLower() == FixedLang[0].ToLower() &&
				LangComponents[1].ToLower() == FixedLang[1].ToLower())
			{
				ConvertedLang = FixedLang[0] + FixedLang[1];
				break;
			}
		}

		if (LangComponents.Num() == 1)
		{
			if (LangComponents[0].ToLower() == FixedLang[0].ToLower())
			{
				ConvertedLang = FixedLang[0] + FixedLang[1];
				break;
			}
		}
	}

	return ConvertedLang;
}

void ULinkDocumentCommand::SaveUserData(const FWordbeeUserData& UserData, const FDocumentData& Document,
                                        const FString& String)
{
	SingletonUtil::SaveToIni(UserData);
	SingletonUtil::SaveToIni(Document);
	UE_LOG(LogTemp, Display, TEXT("ULinkDocumentCommand::SaveUserData"));
}

void ULinkDocumentCommand::SaveDocument(const FWordbeeDocument& resDocument, const FString& projectId,
                                        const FString& projectName, const FString& documentName)
{
	FDocumentData document = SingletonUtil::GetFromIni<FDocumentData>();
	FWordbeeUserData userData = SingletonUtil::GetFromIni<FWordbeeUserData>();

	if (resDocument.Type.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("resDocument.Type.IsEmpty"));
		ResetDocument(document);
	}
	else
	{
		UpdateDocument(document, resDocument, projectId, projectName, documentName);
	}

	SaveUserData(userData, document, projectId);

}
