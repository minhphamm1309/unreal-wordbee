#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/NoExportTypes.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "JsonObjectConverter.h"
#include "FEditorConfig.generated.h"

USTRUCT(BlueprintType)
struct FEditorConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAutoSyncEnabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double SyncIntervalSeconds = 60.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString TargetSynchronization;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SelectedCollectionPath;
	
};
