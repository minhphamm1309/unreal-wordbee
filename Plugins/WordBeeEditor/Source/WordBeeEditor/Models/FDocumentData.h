#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FDocumentData.generated.h"

struct FLangSupport;
struct FRecord;
struct FLanguageInfo;
struct FCustomField;

// Unreal Engine version of the C# Document class
USTRUCT(BlueprintType)
struct FDocumentData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Document")
	FString projectId;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Document")
	FString projectName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Document")
	FString documentName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Document")
	int32 did;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Document")
	int32 dsid;

	// CustomFields list
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Document")
	TArray<FCustomField> cfs;

	// Records array
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Document")
	TArray<FRecord> records;

	// Language supports
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Document")
	TArray<FLangSupport> langSupports;
};

