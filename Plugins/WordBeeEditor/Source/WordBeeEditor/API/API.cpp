#include "API.h"

#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "JsonUtilities.h"
#include "Json.h"
#include "Misc/MessageDialog.h"  // For showing messages in the editor
#include "WordbeeEditor/Models/WordbeeResponse.h"
#include "WordBeeEditor/Utils/APIConstant.h"


const FString API::ROUTER_AUTH = "api/auth/token";
const FString API::ROUTER_DOCUMENTS = "api/apps/wbflex/list";
const FString API::ROUTER_DOCUMENT_PULL = "api/apps/wbflex/documents/{0}/contents/pull";
const FString API::ROUTER_DOCUMENT = "api/apps/wbflex/documents/";
const FString API::ROUTER_POLL = "api/trm/status?requestid={0}";
const FString API::ROUTER_DownloadDocument = "api/media/get/{0}";
const FString API::ROUTER_DOCUMENT_POOLING = "api/trm/status?requestid={0}";
const FString API::ROUTER_PROJECT_LOCALES = "api/projects/{0}/locales";
void API::Authenticate(FString AccountId, FString ApiKey, FString BaseUrl, FOnAuthCompleted callback)
{
	FString URL = ConstructUrl(AccountId, BaseUrl, ROUTER_AUTH);
	// Create the HTTP request
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

	// Set the URL, HTTP method (POST), and headers
	HttpRequest->SetURL(URL);
	HttpRequest->SetVerb("POST");
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	// Create the JSON payload (body)
	TSharedPtr<FJsonObject> RequestJson = MakeShareable(new FJsonObject);
	RequestJson->SetStringField(TEXT("accountid"), AccountId);
	RequestJson->SetStringField(TEXT("key"), ApiKey);

	// Convert JSON object to string
	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(RequestJson.ToSharedRef(), Writer);

	// Set the request content
	HttpRequest->SetContentAsString(RequestBody);

	// Bind the response handler
	HttpRequest->OnProcessRequestComplete().BindLambda(
		[=](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			callback.ExecuteIfBound(Response->GetContentAsString().TrimChar('"'));
		});

	// Execute the request
	HttpRequest->ProcessRequest();
}

FString API::ConstructUrl(FString AccountId, FString BaseUrl, FString Router)
{
	FString URL = FString::Printf(TEXT("https://%s.%s/%s"), *AccountId, *BaseUrl, *Router);
	return URL;
}

void API::OnAuthResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	FString ResponseAuthToken = "";
	if (bWasSuccessful && Response.IsValid())
	{
		int32 ResponseCode = Response->GetResponseCode();

		if (EHttpResponseCodes::IsOk(ResponseCode))
		{
			// If the response is 200 (OK), parse the response body
			FString ResponseContent = Response->GetContentAsString();
			UE_LOG(LogTemp, Log, TEXT("Response: %s"), *ResponseContent);
			ResponseAuthToken = ResponseContent;
			// Parse the JSON response
			TSharedPtr<FJsonObject> JsonResponse;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);

			if (FJsonSerializer::Deserialize(Reader, JsonResponse) && JsonResponse.IsValid())
			{
				// Handle the JSON data
				FString AuthToken = JsonResponse->GetStringField(TEXT("token"));
				UE_LOG(LogTemp, Log, TEXT("Auth Token: %s"), *AuthToken);

				// You can display a message dialog in the editor
				FText AuthMessage = FText::FromString(
					FString::Printf(TEXT("Authenticated successfully! Token: %s"), *AuthToken));
				ResponseAuthToken = AuthToken;
				FMessageDialog::Open(EAppMsgType::Ok, AuthMessage);
			}
		}
		else
		{
			// Handle HTTP errors
			FText ErrorMessage = FText::FromString(
				FString::Printf(TEXT("Failed to authenticate. HTTP Response Code: %d"), ResponseCode));
			FMessageDialog::Open(EAppMsgType::Ok, ErrorMessage);
		}
	}
	else
	{
		// Handle request failure
		FText ErrorMessage = FText::FromString(TEXT("Request failed."));
		FMessageDialog::Open(EAppMsgType::Ok, ErrorMessage);
	}
	if (OnAuthCompleted.IsBound())
	{
		OnAuthCompleted.Execute(ResponseAuthToken);
	}
}

void API::FetchDocumentById(FWordbeeUserData userInfo, const FString& DocumentId,
                            TFunction<void(const FDocumentInfo&)> Callback, bool IsRetry)
{
	if (userInfo.AccountId.IsEmpty() || userInfo.AuthToken.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid user data. Cannot fetch document."));
		return;
	}

	FString Url = ConstructUrl(userInfo.AccountId, userInfo.Url, ROUTER_DOCUMENT + DocumentId);
	UE_LOG(LogTemp, Log, TEXT("Fetching document info from %s"), *Url);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetVerb(TEXT("GET"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(APIConstant::AuthToken, userInfo.AuthToken);
	Request->SetHeader(APIConstant::AuthAccountID, userInfo.AccountId);

	Request->OnProcessRequestComplete().BindLambda(
		[userInfo, DocumentId, Callback, IsRetry](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			FString rawBody = Response.IsValid() ? Response->GetContentAsString() : TEXT("Request failed");
			int32 ResponseCode = Response.IsValid() ? Response->GetResponseCode() : -1;
			UE_LOG(LogTemp, Warning, TEXT("rawBody: %s"), *rawBody);
			if (!bWasSuccessful || !Response.IsValid())
			{
				UE_LOG(LogTemp, Error, TEXT("Request failed: %s (Code: %d)"), *rawBody, ResponseCode);
				return;
			}
			if (ResponseCode == 401 && !IsRetry)
			{
				API::Authenticate(userInfo.AccountId, userInfo.ApiKey, userInfo.Url, 
					FOnAuthCompleted::CreateLambda([userInfo, DocumentId, Callback](FString NewToken) mutable
					{
						// Update userInfo with new token and retry request
						userInfo.AuthToken = NewToken;
						UE_LOG(LogTemp, Log, TEXT("Authentication successful, retrying document fetch..."));
						API::FetchDocumentById(userInfo, DocumentId, Callback, true);
					})
				);
			}
			FDocumentInfo ParsedDocument;
			if (!FJsonObjectConverter::JsonObjectStringToUStruct<FDocumentInfo>(rawBody, &ParsedDocument, 1, 0))
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to parse response"));
				return;
			}
			Callback(ParsedDocument);
		});

	Request->ProcessRequest();
}

void API::PullDocument(FWordbeeUserData userInfo, const FString& DocumentId, FOnPullDocumentComplete callback)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	FString url = ConstructUrl(userInfo.AccountId, userInfo.Url,
	                           FString::Format(*ROUTER_DOCUMENT_PULL, {DocumentId}));
	Request->SetURL(url);
	Request->SetVerb(TEXT("POST"));

	Request->SetHeader(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded"));
	Request->SetHeader(TEXT("X-Auth-AccountId"), userInfo.AccountId);
	Request->SetHeader(TEXT("X-Auth-Token"), userInfo.AuthToken);

	Request->SetContentAsString(
		TEXT("{ \"includeComments\": true, \"includeCustomFields\": true, \"copySourceToTarget\": false }"));

	Request->OnProcessRequestComplete().BindLambda([=](FHttpRequestPtr Req, FHttpResponsePtr Res, bool bSuccess)
	{
		if (bSuccess && Res.IsValid())
		{
			TSharedPtr<FJsonObject> JsonObject;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Res->GetContentAsString());
			if (FJsonSerializer::Deserialize(Reader, JsonObject))
			{
				int32 RequestId = JsonObject->GetObjectField("trm")->GetIntegerField("requestid");
				CheckStatus(userInfo, RequestId, 0, callback);
			}
		}
	});
	Request->ProcessRequest();
}

void API::CheckStatus(FWordbeeUserData userInfo, int32 RequestId, int32 RetryCount, FOnPullDocumentComplete callback)
{
	const int32 MaxRetries = 15; // Stop checking after 15 retries (~30 seconds)
	if (RequestId == 0 || RetryCount >= MaxRetries)
	{
		UE_LOG(LogTemp, Error, TEXT("Status check timed out or invalid request ID."));
		callback.ExecuteIfBound("");
		return;
	}
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	FString url = ConstructUrl(userInfo.AccountId, userInfo.Url, FString::Format(*ROUTER_POLL, {RequestId}));
	Request->SetURL(url);
	Request->SetVerb(TEXT("GET"));

	Request->SetHeader(TEXT("X-Auth-AccountId"), userInfo.AccountId);
	Request->SetHeader(TEXT("X-Auth-Token"), userInfo.AuthToken);
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	Request->OnProcessRequestComplete().BindLambda([=](FHttpRequestPtr Req, FHttpResponsePtr Res, bool bSuccess)
	{
		if (bSuccess && Res.IsValid())
		{
			TSharedPtr<FJsonObject> JsonObject;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Res->GetContentAsString());

			if (FJsonSerializer::Deserialize(Reader, JsonObject))
			{
				FString Status = JsonObject->GetObjectField("trm")->GetStringField("status");
				if (Status == "Finished")
				{
					FString FileToken = JsonObject->GetObjectField("custom")->GetStringField("filetoken");
					DownloadFile(userInfo, FileToken, callback);
				}
				else
				{
					FPlatformProcess::Sleep(2.0f); // Wait before retrying
					CheckStatus(userInfo, RequestId, RetryCount + 1, callback); // Increment retry count
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to get status response."));
		}
	});
	Request->ProcessRequest();
}

void API::DownloadFile(FWordbeeUserData userInfo, const FString& FileToken, FOnPullDocumentComplete callback)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	FString url = ConstructUrl(userInfo.AccountId, userInfo.Url,
	                           FString::Format(*ROUTER_DownloadDocument, {FileToken}));
	Request->SetURL(url);
	Request->SetVerb(TEXT("GET"));

	Request->SetHeader(TEXT("X-Auth-AccountId"), userInfo.AccountId);
	Request->SetHeader(TEXT("X-Auth-Token"), userInfo.AuthToken);
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	Request->OnProcessRequestComplete().BindLambda([=](FHttpRequestPtr Req, FHttpResponsePtr Res, bool bSuccess)
	{
		if (bSuccess && Res.IsValid())
		{
			UE_LOG(LogTemp, Log, TEXT("File downloaded successfully"));
		}
		callback.ExecuteIfBound(*Res->GetContentAsString());
	});
	Request->ProcessRequest();
}
void API::FetchLanguages(FWordbeeUserData userInfo, TFunction<void(const TArray<FLanguageInfo>&)> OnSuccess)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	FString url = ConstructUrl(userInfo.AccountId, userInfo.Url, FString::Format(*ROUTER_PROJECT_LOCALES, {userInfo.ProjectId}));
	Request->SetURL(url);
	UE_LOG(LogTemp, Log, TEXT("Fetching languages with user info: %s"), *url);
	Request->SetVerb(TEXT("GET"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(APIConstant::AuthToken, userInfo.AuthToken);
	Request->SetHeader(APIConstant::AuthAccountID, userInfo.AccountId);
	Request->OnProcessRequestComplete().BindLambda([OnSuccess](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		if (bWasSuccessful && Response.IsValid() && Response->GetResponseCode() == 200)
		{
			TArray<FLanguageInfo> LanguageList;
			FString ResponseString = Response->GetContentAsString();
			if (FJsonObjectConverter::JsonArrayStringToUStruct(ResponseString, &LanguageList, 0, CPF_Transient))
			{
				OnSuccess(LanguageList);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON into FLanguageInfo array"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to fetch languages! Response Code: %d"), Response.IsValid() ? Response->GetResponseCode() : -1);
		}
	});
	Request->ProcessRequest();
}
