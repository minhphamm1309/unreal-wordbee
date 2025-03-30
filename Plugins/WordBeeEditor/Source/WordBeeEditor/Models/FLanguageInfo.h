#pragma once

#include "CoreMinimal.h"
#include "FLanguageInfo.generated.h"

USTRUCT(BlueprintType)
struct FLanguageInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Language Info")
	FString V;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Language Info")
	FString T;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Language Info")
	FString Src;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Transient, Category = "Language Info")
	bool IsSelected = false;
};
