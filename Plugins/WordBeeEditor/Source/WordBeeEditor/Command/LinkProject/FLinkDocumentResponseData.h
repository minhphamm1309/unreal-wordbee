#pragma once

#include "CoreMinimal.h"
#include "FLinkDocumentResponseData.generated.h"

USTRUCT(BlueprintType)
struct FTrmData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int32 RequestId;

	UPROPERTY(BlueprintReadWrite)
	bool IsBatch;

	UPROPERTY(BlueprintReadWrite)
	FString Status;

	UPROPERTY(BlueprintReadWrite)
	FString StatusText;
};

USTRUCT(BlueprintType)
struct FLinkDocumentResponseData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FTrmData Trm;
};

