#pragma once
#include "WordBeeEditor/Models/FEditorConfig.h"

class CronJobHandler
{
public:
	void ShutDownTick();
	void StartSyncTimer();
private:
	void PullDocument(TArray<FString>* SelectedLanguages = nullptr);
	void StoreData(TArray<FString>* SelectedLanguages);
	bool SyncDataTick(float DeltaTime);
	FTSTicker::FDelegateHandle TickHandle;
	double LastSyncTime;
	FEditorConfig Config;
};
