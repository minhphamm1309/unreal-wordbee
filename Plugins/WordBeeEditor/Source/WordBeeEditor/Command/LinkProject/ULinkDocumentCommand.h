#pragma once

#include "CoreMinimal.h"
#include "FLinkDocumentResponseData.h"
#include "Interfaces/IHttpRequest.h"
#include "WordBeeEditor/Command/CreateDataAsset/UserData.h"
#include "WordBeeEditor/Models/FWordbeeFile.h"

DECLARE_DELEGATE_OneParam(FOnLinkDocumentComplete, bool);

class ULinkDocumentCommand 
{
public:
	static void Execute(UUserData* UserInfo,  FString DocumentId, FOnLinkDocumentComplete callback);
	static bool ParseJsonToWordbeeFile(const FString& JsonString, FWordbeeFile& OutWordbeeFile);

protected:
	static  FLinkDocumentResponseData ParseJsonResponse(const FString& Response);
};

