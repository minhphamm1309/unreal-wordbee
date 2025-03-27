#pragma once

#include "CoreMinimal.h"
#include "FDocumentDataResponse.h"
#include "Interfaces/IHttpRequest.h"
#include "WordBeeEditor/Models/WordbeeUserData.h"

DECLARE_DELEGATE_OneParam(FOnHttpRequestComplete, const TArray<FDocumentDataResponse>&);

class UGetDocumentsCommand 
{
public:
	// Function to execute the HTTP request
	static void ExecuteHttpRequest(FWordbeeUserData UserInfo,  FOnHttpRequestComplete OnComplete);

private:
	// Store the delegate to call after the request completes
	FOnHttpRequestComplete OnCompleteDelegate;
	
};
