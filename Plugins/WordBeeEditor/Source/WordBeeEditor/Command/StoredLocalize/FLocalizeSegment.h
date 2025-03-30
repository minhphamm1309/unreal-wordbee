#pragma once

#include "CoreMinimal.h"
#include "FLocalizeSegment.generated.h"

USTRUCT()
struct FLocalizeSegment
{
	GENERATED_BODY()

	UPROPERTY()
	FString Key;
	UPROPERTY()
	FString SourceText;
	UPROPERTY()
	FString TranslationText;
};
