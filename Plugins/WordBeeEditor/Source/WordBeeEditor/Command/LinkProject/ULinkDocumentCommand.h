#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "WordBeeEditor/Utils/UserInfo.h"

DECLARE_DELEGATE_OneParam(FOnLinkDocumentComplete, bool);

struct FTrmData
{
	int32 RequestId;

	bool IsBatch;

	FString Status;

	FString StatusText;
};

struct FResponseData
{
	FTrmData Trm;
};

class ULinkDocumentCommand 
{
public:
	static void Execute(FUserInfo InUserInfo, FString DocumentId, FOnLinkDocumentComplete callback);

protected:
	static  FResponseData ConvertResponseData(const FString& Response);
};

