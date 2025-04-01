#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "UObject/NoExportTypes.h"
#include "WordBeeEditor/Models/WordbeeUserData.h"
#include "WordBeeEditor/Models/WordbeeResponse.h"
#include "WordbeeEditor/Models/FDocumentInfo.h"

DECLARE_DELEGATE_OneParam(FOnAuthCompleted, FString);
DECLARE_DELEGATE_OneParam(FOnPullDocumentComplete, FString);
class API
{
public :
	static const FString ROUTER_AUTH ;
	static const FString ROUTER_DOCUMENTS ;
	static const FString ROUTER_DOCUMENT_PULL ;
	static const FString ROUTER_DOCUMENT;
	static const FString ROUTER_POLL;
	static const FString ROUTER_DownloadDocument;
	static const FString ROUTER_DOCUMENT_POOLING ;
	static const FString ROUTER_PROJECT_LOCALES;
	static const FString ROUTER_WORKFLOW;
public:
	static  void Authenticate(FString AccountId, FString ApiKey, FString BaseUrl, FOnAuthCompleted callback);
	static FString ConstructUrl(FString AccountId, FString BaseUrl, FString Router);
	static void FetchDocumentById(FWordbeeUserData userInfo, const FString& DocumentId, TFunction<void(const FDocumentInfo&)> Callback, bool IsRetry=false);
	static void PullDocument(FWordbeeUserData userInfo, const FString& DocumentId , FOnPullDocumentComplete callback, const TArray<FString>& Keys = {});
	static  void CheckStatus(FWordbeeUserData userInfo, int32 RequestId, int32 RetryCount = 0, FOnPullDocumentComplete callback = nullptr);
	static void DownloadFile(FWordbeeUserData userInfo, const FString& FileToken, FOnPullDocumentComplete callback = nullptr);
	static void FetchLanguages(FWordbeeUserData userInfo, TFunction<void(const TArray<FLanguageInfo>&)> OnSuccess, TFunction<void(const FString&)> OnError = nullptr, bool IsRetry=false);
	static void FetchWorkflow(FWordbeeUserData userInfo, const FString& DocumentId,
						TFunction<void(const TArray<FWorkflowStatus>&)> onSuccess,
						TFunction<void(const FString&)> onError=nullptr, bool IsRetry=false);
private:
	FOnAuthCompleted OnAuthCompleted;
	void OnAuthResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
