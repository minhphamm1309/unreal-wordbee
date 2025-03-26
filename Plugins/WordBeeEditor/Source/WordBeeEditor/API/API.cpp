#include "API.h"

#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "JsonUtilities.h"
#include "Json.h"
#include "Misc/MessageDialog.h"  // For showing messages in the editor
#include "WordbeeEditor/Models/WordbeeResponse.h"
#include "WordBeeEditor/Utils/APIConstant.h"


const FString UAPI::ROUTER_AUTH = "api/auth/token";
const FString UAPI::ROUTER_DOCUMENTS = "api/apps/wbflex/list";
const FString UAPI::ROUTER_DOCUMENT_PULL = "api/apps/wbflex/documents/{0}/contents/pull";
const FString UAPI::ROUTER_DOCUMENT = "api/apps/wbflex/documents/";
const FString UAPI::ROUTER_POLL = "api/trm/status?requestid={0}";
const FString UAPI::ROUTER_DownloadDocument = "api/media/get/{0}";
const FString UAPI::ROUTER_DOCUMENT_POOLING = "api/trm/status?requestid={0}";
const FString UAPI::ROUTER_PROJECT_LOCALES = "api/projects/{0}/locales";
void UAPI::Authenticate(FString AccountId, FString ApiKey, FString BaseUrl , FOnAuthCompleted callback)
{
	FString URL = ConstructUrl(AccountId, BaseUrl, ROUTER_AUTH);
	this->OnAuthCompleted = callback;
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
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UAPI::OnAuthResponseReceived);

	// Execute the request
	HttpRequest->ProcessRequest();
}

FString UAPI::ConstructUrl(FString AccountId, FString BaseUrl, FString Router)
{
	FString URL = FString::Printf(TEXT("https://%s.%s/%s"), *AccountId, *BaseUrl, *Router);
	return URL;
}

void UAPI::OnAuthResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
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
				FText AuthMessage = FText::FromString(FString::Printf(TEXT("Authenticated successfully! Token: %s"), *AuthToken));
				ResponseAuthToken = AuthToken;
				FMessageDialog::Open(EAppMsgType::Ok, AuthMessage);
			}
		}
		else
		{
			// Handle HTTP errors
			FText ErrorMessage = FText::FromString(FString::Printf(TEXT("Failed to authenticate. HTTP Response Code: %d"), ResponseCode));
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
void UAPI::FetchDocumentById(UUserData* userInfo, const FString& DocumentId, TFunction<void(const FDocumentInfo&)> Callback)
{
	if (userInfo->AccountId.IsEmpty() || userInfo->AuthToken.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid user data. Cannot fetch document."));
		return;
	}

	FString Url = ConstructUrl(userInfo->AccountId, userInfo->Url, ROUTER_DOCUMENT+DocumentId);
	UE_LOG(LogTemp, Log, TEXT("Fetching document info from %s"), *Url);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetVerb(TEXT("GET"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(APIConstant::AuthToken, userInfo->AuthToken);
	Request->SetHeader(APIConstant::AuthAccountID, userInfo->AccountId);

	Request->OnProcessRequestComplete().BindLambda([Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		FString rawBody = Response.IsValid() ? Response->GetContentAsString() : TEXT("Request failed");
		UE_LOG(LogTemp, Warning, TEXT("rawBody: %s"), *rawBody);
		if (!bWasSuccessful || !Response.IsValid())
		{
			UE_LOG(LogTemp, Error, TEXT("Request failed: %s (Code: %d)"), *rawBody, Response.IsValid() ? Response->GetResponseCode() : -1);
			return;
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
void UAPI::PullDocument(UUserData* userInfo, const FString& DocumentId)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	FString url = ConstructUrl(userInfo->AccountId, userInfo->Url, FString::Format(*ROUTER_DOCUMENT_PULL, { DocumentId }));
	Request->SetURL(url);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetContentAsString(TEXT("{ \"includeComments\": true, \"includeCustomFields\": true, \"copySourceToTarget\": false }"));

	Request->OnProcessRequestComplete().BindLambda([userInfo](FHttpRequestPtr Req, FHttpResponsePtr Res, bool bSuccess)
	{
		if (bSuccess && Res.IsValid())
		{
			TSharedPtr<FJsonObject> JsonObject;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Res->GetContentAsString());
			if (FJsonSerializer::Deserialize(Reader, JsonObject))
			{
				int32 RequestId = JsonObject->GetObjectField("trm")->GetIntegerField("requestid");
				CheckStatus(userInfo, RequestId);
			}
		}
	});
	Request->ProcessRequest();
}
void UAPI::CheckStatus(UUserData* userInfo, int32 RequestId, int32 RetryCount)
{
	const int32 MaxRetries = 15; // Stop checking after 15 retries (~30 seconds)
	if (RequestId == 0 || RetryCount >= MaxRetries)
	{
		UE_LOG(LogTemp, Error, TEXT("Status check timed out or invalid request ID."));
		return;
	}
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	FString url = ConstructUrl(userInfo->AccountId, userInfo->Url, FString::Format(*ROUTER_POLL, { RequestId }));
	Request->SetURL(url);
	Request->SetVerb(TEXT("GET"));

	Request->OnProcessRequestComplete().BindLambda([RequestId, RetryCount, userInfo](FHttpRequestPtr Req, FHttpResponsePtr Res, bool bSuccess)
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
					DownloadFile(userInfo, FileToken);
				}
				else
				{
					FPlatformProcess::Sleep(2.0f); // Wait before retrying
					CheckStatus(userInfo, RequestId, RetryCount + 1); // Increment retry count
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
void UAPI::DownloadFile(UUserData* userInfo, const FString& FileToken)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	FString url = ConstructUrl(userInfo->AccountId, userInfo->Url, FString::Format(*ROUTER_DownloadDocument, { FileToken }));
	Request->SetURL(url);
	Request->SetVerb(TEXT("GET"));

	Request->OnProcessRequestComplete().BindLambda([](FHttpRequestPtr Req, FHttpResponsePtr Res, bool bSuccess)
	{
		if (bSuccess && Res.IsValid())
		{
			UE_LOG(LogTemp, Log, TEXT("File downloaded successfully"));
		}
	});
	Request->ProcessRequest();
}
void UAPI::FetchLanguages(UUserData* userInfo, TFunction<void(const TArray<FLanguageInfo>&)> OnSuccess)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	FString url = ConstructUrl(userInfo->AccountId, userInfo->Url, FString::Format(*ROUTER_PROJECT_LOCALES, {3191}));
	Request->SetURL(url);
	UE_LOG(LogTemp, Log, TEXT("Fetching languages with user info:"));
	Request->SetVerb(TEXT("GET"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(APIConstant::AuthToken, userInfo->AuthToken);
	Request->SetHeader(APIConstant::AuthAccountID, userInfo->AccountId);
	Request->OnProcessRequestComplete().BindLambda([OnSuccess](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		if (bWasSuccessful && Response.IsValid() && Response->GetResponseCode() == 200)
		{
			TArray<FLanguageInfo> LanguageList;
			FString ResponseString = Response->GetContentAsString();
			if (FJsonObjectConverter::JsonArrayStringToUStruct(ResponseString, &LanguageList, 0, 0))
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



