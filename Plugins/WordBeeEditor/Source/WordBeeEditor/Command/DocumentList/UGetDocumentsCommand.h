#pragma once

#include "CoreMinimal.h"
#include "FDocumentData.h"
#include "Interfaces/IHttpRequest.h"
#include "WordBeeEditor/Utils/UserInfo.h"
#include "UGetDocumentsCommand.generated.h"

DECLARE_DELEGATE_OneParam(FOnHttpRequestComplete, const TArray<FDocumentData>&);

UCLASS()
class UGetDocumentsCommand : public UObject
{
	GENERATED_BODY()

public:
	// Function to execute the HTTP request
	void ExecuteHttpRequest(UserInfo InUserInfo, FOnHttpRequestComplete OnComplete);

private:
	// Callback when the HTTP request completes
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	// Store the delegate to call after the request completes
	FOnHttpRequestComplete OnCompleteDelegate;
	
};
