#include "CronJobHandler.h"

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
	Config = SingletonUtil::GetFromIni<FEditorConfig>();
	if (!Config.bAutoSyncEnabled) return true;
	double CurrentTime = FPlatformTime::Seconds();
	if (CurrentTime - LastSyncTime >= Config.SyncIntervalSeconds)
	{
		LastSyncTime = CurrentTime;
		UE_LOG(LogTemp, Warning, TEXT("CronJob: Pulling Doc"));
		DocumentService::PullDocument();
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

