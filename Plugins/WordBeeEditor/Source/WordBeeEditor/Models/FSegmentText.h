#pragma once

#include "CoreMinimal.h"
#include "FCustomField.h"
#include "FSegmentText.generated.h"

USTRUCT(BlueprintType)
struct FSegmentText
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString v;

	UPROPERTY(BlueprintReadWrite)
	int32 st;

	UPROPERTY(BlueprintReadWrite)
	int32 bk;

	UPROPERTY(BlueprintReadWrite)
	int32 ed;

	UPROPERTY(BlueprintReadWrite)
	FString dt;

	UPROPERTY(BlueprintReadWrite)
	TArray<FCustomField> cfs;
};