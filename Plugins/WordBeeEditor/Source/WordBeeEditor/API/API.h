#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "UObject/NoExportTypes.h"
#include "WordBeeEditor/Command/CreateDataAsset/UserData.h"
#include "WordBeeEditor/Models/WordbeeResponse.h"
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
	static const FString ROUTER_DOCUMENT_POOLING ;
	
public:
	void Authenticate(FString AccountId, FString ApiKey, FString BaseUrl, FOnAuthCompleted callback);
	static FString ConstructUrl(FString AccountId, FString BaseUrl, FString Router);
	static void FetchDocumentById(UUserData* userInfo, const FString& DocumentId, TFunction<void(const FDocumentInfo&)> Callback);
	static void PullDocument(UUserData* userInfo, const FString& DocumentId);
	static void CheckStatus(UUserData* userInfo, int32 RequestId, int32 RetryCount = 0);
	static void DownloadFile(UUserData* userInfo, const FString& FileToken);
private:
	FOnAuthCompleted OnAuthCompleted;
	void OnAuthResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
