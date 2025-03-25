#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "UObject/NoExportTypes.h"
#include "WordBeeEditor/Models/WordbeeResponse.h"
#include "WordBeeEditor/Utils/UserInfo.h"
#include "WordbeeEditor/Models/FDocumentInfo.h"
#include "API.generated.h"

DECLARE_DELEGATE_OneParam(FOnAuthCompleted, FString);

UCLASS()
class UAPI : public UObject
{
	GENERATED_BODY()
public :
	static const FString ROUTER_AUTH ;
	static const FString ROUTER_DOCUMENTS ;
	static const FString ROUTER_DOCUMENT_PULL ;
	static const FString ROUTER_DOCUMENT;
	static const FString ROUTER_POLL;
	static const FString ROUTER_DownloadDocument;
public:
	void Authenticate(FString AccountId, FString ApiKey, FString BaseUrl, FOnAuthCompleted callback);
	static FString ConstructUrl(FString AccountId, FString BaseUrl, FString Router);
	static void FetchDocumentById(const UserInfo& userInfo, const FString& DocumentId, TFunction<void(const FDocumentInfo&)> Callback);
	static void PullDocument(UserInfo& userInfo, const FString& DocumentId);
	static void CheckStatus(UserInfo& userInfo, int32 RequestId, int32 RetryCount = 0);
	static void DownloadFile(UserInfo& userInfo, const FString& FileToken);
private:
	FOnAuthCompleted OnAuthCompleted;
	void OnAuthResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
