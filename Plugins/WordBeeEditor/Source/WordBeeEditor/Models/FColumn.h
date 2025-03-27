#pragma once

#include "CoreMinimal.h"
#include "FCustomField.h"
#include "FColumn.generated.h"


USTRUCT(BlueprintType)
struct FColumn
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Column")
	FString columnID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Column")
	FString convertedLang;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Column")
	FString text;

	// Custom fields (hidden in Unreal Editor)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Column", meta = (HideInInspector))
	TArray<FCustomField> cfs;

	// Last modified date/time
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Column")
	FString lastModified;
};