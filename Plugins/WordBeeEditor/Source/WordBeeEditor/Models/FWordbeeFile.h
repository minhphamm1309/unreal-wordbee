#pragma once

#include "CoreMinimal.h"
#include "FSegment.h"
#include "FWordbeeFile.generated.h"

USTRUCT(BlueprintType)
struct FWordbeeFile
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString type;

	UPROPERTY(BlueprintReadWrite)
	FString version;

	UPROPERTY(BlueprintReadWrite)
	int32 dsid;

	UPROPERTY(BlueprintReadWrite)
	int32 did;

	UPROPERTY(BlueprintReadWrite)
	TArray<FSegment> segments;
};

