#pragma once

#include "CoreMinimal.h"
#include "FSegmentText.h"
#include "FSegment.generated.h"

USTRUCT(BlueprintType)
struct FSegment 
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite)
	FString key;

	UPROPERTY(BlueprintReadWrite)
	FString component;

	UPROPERTY(BlueprintReadWrite)
	FString dt;

	UPROPERTY(BlueprintReadWrite)
	FString format;

	UPROPERTY(BlueprintReadWrite)
	int32 bsid;

	UPROPERTY(BlueprintReadWrite)
	int32 chmin;

	UPROPERTY(BlueprintReadWrite)
	int32 chmax;

	UPROPERTY(BlueprintReadWrite)
	TMap<FString, FSegmentText> texts;
};