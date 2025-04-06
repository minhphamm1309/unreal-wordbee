#include "DocumentService.h"

#include "WordBeeEditor/Command/LinkProject/ULinkDocumentCommand.h"
#include "WordBeeEditor/Command/StoredLocalize/StoredLocailzationCommand.h"
#include "WordBeeEditor/Models/WordbeeUserData.h"
#include "WordBeeEditor/Utils/FileChangeUtil.h"
#include "WordBeeEditor/Utils/LocalizeUtil.h"

void DocumentService::PullDocument(TSharedPtr<TArray<FString>> SelectedLanguages)
{
	if (SelectedLanguages && SelectedLanguages->Num() == 0)
	{
		return;
	}
	TSharedPtr<FScopedSlowTask> SlowTask = MakeShared<FScopedSlowTask>(3, FText::FromString(TEXT("Processing... Please wait.")));
	SlowTask->MakeDialog();
	FWordbeeUserData userInfo = SingletonUtil::GetFromIni<FWordbeeUserData>();
	ULinkDocumentCommand::Execute(userInfo, FString::FromInt(userInfo.DocumentId),
  FOnLinkDocumentComplete::CreateLambda(
	  [SlowTask, SelectedLanguages](bool bSuccess, const FWordbeeDocument& document)
	  {
		  if (bSuccess)
		  {
			  SlowTask->EnterProgressFrame(1, FText::FromString(TEXT("Storing document...")));
			  FDocumentData documentData = SingletonUtil::GetFromIni<FDocumentData>();
			  ULinkDocumentCommand::SaveDocument(
				  document, documentData.projectId, documentData.projectName,
				  documentData.documentName);
			  SlowTask->EnterProgressFrame(1, FText::FromString(TEXT("Import to Localization...")));
			  StoreData(SelectedLanguages);
			  Locate<LocalizeUtil>::Get()->RecordsChanged.Empty();
			  FileChangeUtil::CopyLocalizeToSaved();
			  SlowTask->EnterProgressFrame(1, FText::FromString(TEXT("Finished!")));
		  }
		  else
		  {
			  UE_LOG(LogTemp, Error, TEXT("Failed to pull data. Please check your connection and try again."));
		  }
	  }));
}

void DocumentService::StoreData(TSharedPtr<TArray<FString>> SelectedLanguages)
{
	FDocumentData document = SingletonUtil::GetFromIni<FDocumentData>();
	TArray<FSegment> segments;
	// convert all document.Records to TArray<FSegment>
	for (const FRecord& record : document.records)
	{
		FSegment segment;
		segment.key = record.recordID;

		for (const auto& text : record.columns)
		{
			if (SelectedLanguages && !SelectedLanguages->Contains(text.columnID)) continue;
			FSegmentText textSegment;
			textSegment.v = text.text;
			segment.texts.Add(text.columnID, textSegment);
		}
		segments.Add(segment);
	}
	if (segments.Num() > 0)
	{
		StoredLocailzationCommand::Execute(segments);
	}
}
