#include "CronJobHandler.h"

#include "WordBeeEditor/Command/LinkProject/ULinkDocumentCommand.h"
#include "WordBeeEditor/Command/StoredLocalize/StoredLocailzationCommand.h"
#include "WordBeeEditor/Models/WordbeeUserData.h"
#include "WordBeeEditor/Utils/FileChangeUtil.h"
#include "WordBeeEditor/Utils/LocalizeUtil.h"

void CronJobHandler::PullDocument(TArray<FString>* SelectedLanguages)
{
	if (SelectedLanguages && SelectedLanguages->Num() == 0)
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("You must select at least one language."));
		return;
	}
	FWordbeeUserData userInfo = SingletonUtil::GetFromIni<FWordbeeUserData>();
	ULinkDocumentCommand::Execute(userInfo, FString::FromInt(userInfo.DocumentId),
  FOnLinkDocumentComplete::CreateLambda(
      [=,this](bool bSuccess, const FWordbeeDocument& document)
      {
          if (bSuccess)
          {
              FDocumentData documentData = SingletonUtil::GetFromIni<FDocumentData>();
              ULinkDocumentCommand::SaveDocument(
                  document, documentData.projectId, documentData.projectName,
                  documentData.documentName);
              StoreData(SelectedLanguages);
              Locate<LocalizeUtil>::Get()->RecordsChanged.Empty();
              FileChangeUtil::CopyLocalizeToSaved();
          }
          else
          {
          	UE_LOG(LogTemp, Error, TEXT("Failed to pull data. Please check your connection and try again."));
          }
      }));
}
void CronJobHandler::StoreData(TArray<FString>* SelectedLanguages)
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
	FString msg = TEXT("Pulling data from Wordbee...");
	if (segments.Num() > 0)
		StoredLocailzationCommand::Execute(segments);
	else
	{
		msg = TEXT("No data to pull from Wordbee.");
	}
}
void CronJobHandler::StartSyncTimer()
{
	LastSyncTime = FPlatformTime::Seconds();
	TickHandle = FTSTicker::GetCoreTicker().AddTicker(
		FTickerDelegate::CreateRaw(this, &CronJobHandler::SyncDataTick),
		1.0f  // Check every 1 second
	);
}
bool CronJobHandler::SyncDataTick(float DeltaTime)
{
	Config = SingletonUtil::GetFromIni<FEditorConfig>();
	if (!Config.bAutoSyncEnabled) return true;
	double CurrentTime = FPlatformTime::Seconds();
	if (CurrentTime - LastSyncTime >= Config.SyncIntervalSeconds)
	{
		LastSyncTime = CurrentTime;
		UE_LOG(LogTemp, Warning, TEXT("CronJob: Pulling Doc"));
		PullDocument();
	}
	return true;  // Keep ticking
}
void CronJobHandler::ShutDownTick()
{
	if (TickHandle.IsValid())
	{
		FTSTicker::GetCoreTicker().RemoveTicker(TickHandle);
	}
}

