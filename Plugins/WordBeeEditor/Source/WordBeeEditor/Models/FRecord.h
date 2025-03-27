#pragma once

#include "CoreMinimal.h"
#include "FColumn.h"
#include "FRecord.generated.h"

USTRUCT(BlueprintType)
struct FRecord
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Record")
	FString recordID;

	// Array of columns
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Record")
	TArray<FColumn> columns;

	// Original record ID (hidden in Unreal Editor)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Record", meta = (HideInInspector))
	FString originalRecordID;
};