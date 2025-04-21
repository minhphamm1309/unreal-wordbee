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
#include "WordBeeEditor/Utils/Locate.h"

void DocumentService::SyncDocument(TSharedPtr<TArray<FString>> SelectedLanguages, FString src, bool IsChangesOnly, TFunction<void(bool)> OnFinish)
{
	PullDocument(SelectedLanguages, src, [=](bool bPullSuccess)
	{
		if (bPullSuccess)
		{
			PushDocument(SelectedLanguages, IsChangesOnly, [=](bool bPushSuccess)
			{
				if (OnFinish) OnFinish(bPushSuccess);
			});
		}
		else
		{
			if (OnFinish) OnFinish(false);
		}
	});
}


void DocumentService::PullDocument(TSharedPtr<TArray<FString>> SelectedLanguages, FString src, TFunction<void(bool)> OnFinish)
{
	if (SelectedLanguages && SelectedLanguages->Num() == 0)
	{
		if (OnFinish) OnFinish(false);
		return;
	}
	TSharedPtr<SNotificationItem> LoadingNotification = ShowLoadingNotification("Pulling data from Wordbee...");
	FWordbeeUserData userInfo = wordbee::SingletonUtil<FWordbeeUserData>::GetFromIni();
	ULinkDocumentCommand::Execute(userInfo, FString::FromInt(userInfo.DocumentId),
  FOnLinkDocumentComplete::CreateLambda(
	  [LoadingNotification, SelectedLanguages, src, OnFinish](bool bSuccess, const FWordbeeDocument& document)
	  {
		  if (bSuccess)
		  {
			  FDocumentData documentData = wordbee::SingletonUtil<FDocumentData>::GetFromIni();
			  ULinkDocumentCommand::SaveDocument(
				  document, documentData.projectId, documentData.projectName,
				  documentData.documentName);
			  StoreData(SelectedLanguages, src);
			  Locate<LocalizeUtil>::Get()->RecordsChanged.Empty();
			  FileChangeUtil::CopyLocalizeToSaved();
		  	UpdateNotificationText(LoadingNotification, "Pull document completed.", true, true);
		  	if (OnFinish) OnFinish(true);
		  }
		  else
		  {
			  UE_LOG(LogTemp, Error, TEXT("Failed to pull data. Please check your connection and try again."));
		  	UpdateNotificationText(LoadingNotification, "Failed to pull document.", false, true);
		  	if (OnFinish) OnFinish(false);
		  }
	  }));
}

void DocumentService::StoreData(TSharedPtr<TArray<FString>> SelectedLanguages, FString src)
{
	FDocumentData document = wordbee::SingletonUtil<FDocumentData>::GetFromIni();
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

void DocumentService::PushDocument(TSharedPtr<TArray<FString>> SelectedLanguages, bool IsChangesOnly, TFunction<void(bool)> OnFinish)
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
		if (OnFinish) OnFinish(false);
		return;
	}
	TSharedPtr<SNotificationItem> LoadingNotification = ShowLoadingNotification("Pushing data to Wordbee...");
	FWordbeeUserData userInfo = wordbee::SingletonUtil<FWordbeeUserData>::GetFromIni();
	API::PushRecords(userInfo, RecordsToCommit, FOnUpdateDocumentComplete::CreateLambda(
	 [LoadingNotification, OnFinish](bool bSuccess, const int32& _, const FString& message)
	 {
	 	if (bSuccess)
	 	{
			 Locate<LocalizeUtil>::Get()->RecordsChanged.Empty();
			 FileChangeUtil::CopyLocalizeToSaved();
	 		UpdateNotificationText(LoadingNotification, "Push to Wordbee completed.", true, true);
	 		if (OnFinish) OnFinish(true);
	 	}
	 	else
	 	{
	 		UpdateNotificationText(LoadingNotification, "Failed to push to Wordbee.", false, true);
	 		if (OnFinish) OnFinish(false);
	 	}
	 }));
}
TSharedPtr<SNotificationItem> DocumentService::ShowLoadingNotification(const FString& InitialText)
{
	FNotificationInfo LoadingInfo(FText::FromString(InitialText));
	LoadingInfo.bFireAndForget = false;
	LoadingInfo.FadeOutDuration = 0.5f;
	LoadingInfo.ExpireDuration = 3.0f;
	LoadingInfo.bUseThrobber = true;
	LoadingInfo.bUseSuccessFailIcons = false;

	TSharedPtr<SNotificationItem> LoadingNotification = FSlateNotificationManager::Get().AddNotification(LoadingInfo);
	if (LoadingNotification.IsValid())
	{
		LoadingNotification->SetCompletionState(SNotificationItem::CS_Pending);
	}
	return LoadingNotification;
}

void DocumentService::UpdateNotificationText(TSharedPtr<SNotificationItem> Notification, const FString& NewText, bool bSuccess, bool bExpireAfter)
{
	if (Notification.IsValid())
	{
		Notification->SetText(FText::FromString(NewText));

		if (bSuccess)
		{
			Notification->SetCompletionState(SNotificationItem::CS_Success);
		}
		else
		{
			Notification->SetCompletionState(SNotificationItem::CS_Fail);
		}

		if (bExpireAfter)
		{
			Notification->ExpireAndFadeout();
		}
	}
}

