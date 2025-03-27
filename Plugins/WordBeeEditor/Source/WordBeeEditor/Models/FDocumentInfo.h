#pragma once

#include "CoreMinimal.h"
#include "FLanguageInfo.h"
#include "FDocumentInfo.generated.h"


USTRUCT(BlueprintType)
struct FDocumentInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Document Info")
	int32 Id;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Document Info")
	FString Name;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Document Info")
	FString DtChange;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Document Info")
	FLanguageInfo Src;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Document Info")
	TArray<FLanguageInfo> Trgs;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Document Info")
	FString FilterName;
};
