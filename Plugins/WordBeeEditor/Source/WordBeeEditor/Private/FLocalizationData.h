#pragma once

#include "CoreMinimal.h"
#include "FLocalizationData.generated.h"

USTRUCT(BlueprintType)
struct FTextEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Localization")
	FString Text;
};

USTRUCT(BlueprintType)
struct FTranslationEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Localization")
	FTextEntry Source;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Localization")
	FTextEntry Translation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Localization")
	FString Key;
};

USTRUCT(BlueprintType)
struct FSubnamespace
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Localization")
	FString Namespace;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Localization")
	TArray<FTranslationEntry> Children;
};

USTRUCT(BlueprintType)
struct FLocalizationData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Localization")
	int32 FormatVersion;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Localization")
	FString Namespace;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Localization")
	TArray<FSubnamespace> Subnamespaces;
};
