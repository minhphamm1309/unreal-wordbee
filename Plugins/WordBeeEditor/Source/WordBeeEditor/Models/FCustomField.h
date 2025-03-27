#pragma once

#include "CoreMinimal.h"
#include "FCustomField.generated.h"

USTRUCT(BlueprintType)
struct FCustomField
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString Id;

	UPROPERTY(BlueprintReadWrite)
	FString Title;
};