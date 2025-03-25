#pragma once

#include "CoreMinimal.h"
#include "FDocumentData.h"
#include "Interfaces/IHttpRequest.h"
#include "WordBeeEditor/Command/CreateDataAsset/UserData.h"

DECLARE_DELEGATE_OneParam(FOnHttpRequestComplete, const TArray<FDocumentData>&);

class UGetDocumentsCommand 
{
public:
	// Function to execute the HTTP request
	static void ExecuteHttpRequest(UUserData* UserInfo,  FOnHttpRequestComplete OnComplete);

private:
	// Store the delegate to call after the request completes
	FOnHttpRequestComplete OnCompleteDelegate;
	
};
