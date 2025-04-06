#pragma once
#include "WordBeeEditor/Models/FEditorConfig.h"

class CronJobHandler
{
public:
	void ShutDownTick();
	void StartSyncTimer();
private:
	bool SyncDataTick(float DeltaTime);
	FTSTicker::FDelegateHandle TickHandle;
	double LastSyncTime;
	FEditorConfig Config;
};
