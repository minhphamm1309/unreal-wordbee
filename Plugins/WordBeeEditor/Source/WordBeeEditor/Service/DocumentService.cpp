#include "DocumentService.h"

#include "Algo/AllOf.h"
#include "WordBeeEditor/API/API.h"
#include "WordBeeEditor/Command/LinkProject/ULinkDocumentCommand.h"
#include "WordBeeEditor/Command/StoredLocalize/StoredLocailzationCommand.h"
#include "WordBeeEditor/Models/WordbeeUserData.h"
#include "WordBeeEditor/Utils/FileChangeUtil.h"
#include "WordBeeEditor/Utils/LocalizeUtil.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Internationalization/Text.h"


void DocumentService::PullDocument(TSharedPtr<TArray<FString>> SelectedLanguages, FString src)
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
	  [SlowTask, SelectedLanguages, src](bool bSuccess, const FWordbeeDocument& document)
	  {
		  if (bSuccess)
		  {
			  SlowTask->EnterProgressFrame(1, FText::FromString(TEXT("Storing document...")));
			  FDocumentData documentData = SingletonUtil::GetFromIni<FDocumentData>();
			  ULinkDocumentCommand::SaveDocument(
				  document, documentData.projectId, documentData.projectName,
				  documentData.documentName);
			  SlowTask->EnterProgressFrame(1, FText::FromString(TEXT("Import to Localization...")));
			  StoreData(SelectedLanguages, src);
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

void DocumentService::StoreData(TSharedPtr<TArray<FString>> SelectedLanguages, FString src)
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
		StoredLocailzationCommand::Execute(segments, src);
	}
}

void DocumentService::PushDocument(TSharedPtr<TArray<FString>> SelectedLanguages, bool IsChangesOnly)
{
	LocalizeUtil* localizeUtil = Locate<LocalizeUtil>::Get();
	TArray<FRecord> RecordsToCommit = IsChangesOnly? localizeUtil->RecordsChanged : FileChangeUtil::GetCurrentRecords();
	for (FRecord& Record : RecordsToCommit)
	{
		// Remove columns where columnID does not exist in SelectedLanguages
		Record.columns.RemoveAll([&SelectedLanguages](const FColumn& Column)
		{
			return !SelectedLanguages->Contains(Column.columnID);
		});
	}
	bool bAllEmpty = Algo::AllOf(RecordsToCommit, [](const FRecord& Record)
	{
		return Record.columns.Num() == 0;
	});
	if (bAllEmpty)
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(" No data to push to Wordbee."));
		return;
	}
	FNotificationInfo LoadingInfo(FText::FromString("Pushing data to Wordbee..."));
	LoadingInfo.bFireAndForget = false;
	LoadingInfo.FadeOutDuration = 0.5f;
	LoadingInfo.ExpireDuration = 0.0f;
	LoadingInfo.bUseThrobber = true;
	LoadingInfo.bUseSuccessFailIcons = false;
	TSharedPtr<SNotificationItem> LoadingNotification = FSlateNotificationManager::Get().AddNotification(LoadingInfo);
	if (LoadingNotification.IsValid())
	{
		LoadingNotification->SetCompletionState(SNotificationItem::CS_Pending);
	}
	API::PushRecords(RecordsToCommit, FOnUpdateDocumentComplete::CreateLambda(
	 [LoadingNotification](bool bSuccess, const int32& _, const FString& message)
	 {
	 	if (bSuccess)
	 	{
			 Locate<LocalizeUtil>::Get()->RecordsChanged.Empty();
			 FileChangeUtil::CopyLocalizeToSaved();
	 		if (LoadingNotification.IsValid())
	 		{
				 LoadingNotification->SetText(FText::FromString("Push to Wordbee completed."));
				 LoadingNotification->SetCompletionState(SNotificationItem::CS_Success);
				 LoadingNotification->ExpireAndFadeout();
			 }
	 	}
	 	else
	 	{
	 		if (LoadingNotification.IsValid())
	 		{
				 LoadingNotification->SetText(FText::FromString("Failed to push to Wordbee."));
				 LoadingNotification->SetCompletionState(SNotificationItem::CS_Fail);
				 LoadingNotification->ExpireAndFadeout();
			 }
			 FMessageDialog::Open(EAppMsgType::Ok,
								  FText::FromString("Failed to push data to Wordbee: " + message));
	 	}
	 }));
}
