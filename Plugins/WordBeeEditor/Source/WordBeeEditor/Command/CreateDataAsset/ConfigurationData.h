#pragma once

#include "CoreMinimal.h"
#include "ConfigurationData.generated.h"

UCLASS(BlueprintType)
class WORDBEEEDITOR_API UConfigurationData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString targetSynchronization;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> wordbeeColumns;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> sheetColumns;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool autoSyncEnabled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float syncIntervalSeconds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString selectedCollectionPath;
};

// public string targetSynchronization;
// public string[] wordbeeColumns;
// public string[] sheetColumns;
// public bool autoSyncEnabled;
// public double syncIntervalSeconds;
// public string selectedCollectionPath;
