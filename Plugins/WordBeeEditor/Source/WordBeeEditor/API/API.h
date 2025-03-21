#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "UObject/NoExportTypes.h"
#include "API.generated.h"

UCLASS()
class UAPI : public UObject
{
	GENERATED_BODY()
public:
	void Authenticate(FString AccountId, FString ApiKey, FString BaseUrl);
private:
	void OnAuthResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
