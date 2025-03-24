#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "WordBeeEditor/Utils/UserInfo.h"
#include "ULinkDocumentCommand.generated.h"

DECLARE_DELEGATE_OneParam(FOnLinkDocumentComplete, bool);

UCLASS()
class ULinkDocumentCommand : public UObject
{
	GENERATED_BODY()
public:
	void Execute(UserInfo InUserInfo, FOnLinkDocumentComplete callback);

protected:
	// Callback when the HTTP request completes
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	FOnLinkDocumentComplete OnCompleteDelegate;
};
