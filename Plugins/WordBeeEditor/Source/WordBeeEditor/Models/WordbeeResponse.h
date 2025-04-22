#pragma once

#include "CoreMinimal.h"
#include "FWordbeeFile.h"
#include "Http.h"
#include "JsonObjectConverter.h"
#include "WordbeeResponse.generated.h"

struct FCustomField;
// ====================================
// ResponseData
// ====================================
UCLASS(BlueprintType)
class UResponseData : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite)
    FString Url;

    UPROPERTY(BlueprintReadWrite)
    FString PostData;

    UPROPERTY(BlueprintReadWrite)
    int32 StatusCode;

    UPROPERTY(BlueprintReadWrite)
    FString RawBody;

    template <class T>
    bool GetBody(T& OutBody) const
    {
        return FJsonObjectConverter::JsonObjectStringToUStruct<T>(RawBody, &OutBody, 0, 0);
    }
};

// ====================================
// ProjectsList
// ====================================
USTRUCT(BlueprintType)
struct FDocument
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    FString Id;

    UPROPERTY(BlueprintReadWrite)
    FString DtChange;

    UPROPERTY(BlueprintReadWrite)
    FString Name;

    UPROPERTY(BlueprintReadWrite)
    FString Src;

    UPROPERTY(BlueprintReadWrite)
    FString SrcTitle;

    UPROPERTY(BlueprintReadWrite)
    int64 Paragraphs;

    UPROPERTY(BlueprintReadWrite)
    FString Pid;

    UPROPERTY(BlueprintReadWrite)
    FString Preference;
};

USTRUCT(BlueprintType)
struct FProjectsList
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    int64 Total;

    UPROPERTY(BlueprintReadWrite)
    int64 Count;

    UPROPERTY(BlueprintReadWrite)
    TArray<FDocument> Rows;
};

// ====================================
// TRMWrapper
// ====================================
USTRUCT(BlueprintType)
struct FTRM
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    int32 RequestId;

    UPROPERTY(BlueprintReadWrite)
    bool bIsBatch;

    UPROPERTY(BlueprintReadWrite)
    FString Status;

    UPROPERTY(BlueprintReadWrite)
    FString StatusText;

    bool IsFinished() const { return Status.Equals(TEXT("Finished")); }
    bool IsFailed() const { return Status.Equals(TEXT("Failed")); }
};

USTRUCT(BlueprintType)
struct FTRMWrapper
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    FTRM TRM;
};

// ====================================
// Polling
// ====================================
USTRUCT(BlueprintType)
struct FResult
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    TArray<FString> Items;
};


USTRUCT(BlueprintType)
struct FWordbeeDocument
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    FString Type;

    UPROPERTY(BlueprintReadWrite)
    FString Version;

    UPROPERTY(BlueprintReadWrite)
    TOptional<FString> Major;

    UPROPERTY(BlueprintReadWrite)
    TOptional<FString> Minor;

    UPROPERTY(BlueprintReadWrite)
    int32 Did;

    UPROPERTY(BlueprintReadWrite)
    int32 Dsid;

    UPROPERTY(BlueprintReadWrite)
    TArray<FCustomField> CfsConfig;

    UPROPERTY(BlueprintReadWrite)
    TArray<FCustomField> LblsConfig;

    UPROPERTY(BlueprintReadWrite)
    TArray<FSegment> Segments;
};

USTRUCT(BlueprintType)
struct FWorkflowStatus
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite, Category = "Workflow")
    FString Loc;

    UPROPERTY(BlueprintReadWrite, Category = "Workflow")
    FString Loct;

    UPROPERTY(BlueprintReadWrite, Category = "Workflow")
    int32 Status;

    UPROPERTY(BlueprintReadWrite, Category = "Workflow")
    FString Statust;
    FString GetDisplayedStatus() const
    {
        switch (Status)
        {
        case 0:
            return TEXT("In Translation");
        case 1:
            return TEXT("Translation Completed");
        case 2:
            return TEXT("Awaiting Acceptance");
        case 3:
            return TEXT("Pending Assignment");
        default:
            return TEXT("Unknown");
        }
    }
};

