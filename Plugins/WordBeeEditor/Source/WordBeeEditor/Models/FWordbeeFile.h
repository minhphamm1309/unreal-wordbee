#pragma once

#include "CoreMinimal.h"
#include "FWordbeeFile.generated.h"

USTRUCT(BlueprintType)
struct FSegmentText
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString v;

	UPROPERTY(BlueprintReadWrite)
	int32 st;

	UPROPERTY(BlueprintReadWrite)
	int32 bk;

	UPROPERTY(BlueprintReadWrite)
	int32 ed;

	UPROPERTY(BlueprintReadWrite)
	FString dt;
};

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

