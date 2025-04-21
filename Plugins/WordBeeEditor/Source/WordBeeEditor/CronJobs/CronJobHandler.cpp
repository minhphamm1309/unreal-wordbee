#include "CronJobHandler.h"

#include "WordBeeEditor/EditorWindow/Tabs/SDocumentInfo.h"
#include "WordBeeEditor/Models/FDocumentInfo.h"
#include "WordBeeEditor/Models/WordbeeUserData.h"
#include "WordBeeEditor/Service/DocumentService.h"

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
	Config = wordbee::SingletonUtil<FEditorConfig>::GetFromIni();
	if (!Config.bAutoSyncEnabled) return true;
	DocInfo = SDocumentInfo::CachedDocumentInfo;
	if (!DocInfo.IsValid()) return true;
	if (DocInfo->Id == 0 || DocInfo->Trgs.IsEmpty())
	{
		return true;
	}
	FString src = DocInfo->Src.V;
	TSharedPtr<TArray<FString>> VList = MakeShared<TArray<FString>>();
	VList->Add(src);
	for (const FLanguageInfo& Lang : DocInfo->Trgs)
	{
		VList->Add(Lang.V);  // Add the V value to the array pointed to by VList
	}
	double CurrentTime = FPlatformTime::Seconds();
	if (CurrentTime - LastSyncTime >= Config.SyncIntervalSeconds)
	{
		LastSyncTime = CurrentTime;
		DocumentService::SyncDocument(VList, src, true);
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

