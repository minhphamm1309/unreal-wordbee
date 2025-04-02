#include "API.h"

#include <__ranges/elements_view.h>

#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "JsonUtilities.h"
#include "Json.h"
#include "Misc/MessageDialog.h"  // For showing messages in the editor
#include "WordBeeEditor/Models/FDocumentData.h"
#include "WordbeeEditor/Models/WordbeeResponse.h"
#include "WordBeeEditor/Utils/APIConstant.h"
#include "WordBeeEditor/Utils/SingletonUtil.h"


const FString API::ROUTER_AUTH = "api/auth/token";
const FString API::ROUTER_DOCUMENTS = "api/apps/wbflex/list";
const FString API::ROUTER_DOCUMENT_PULL = "api/apps/wbflex/documents/{0}/contents/pull";
const FString API::ROUTER_DOCUMENT = "api/apps/wbflex/documents/";
const FString API::ROUTER_POLL = "api/trm/status?requestid={0}";
const FString API::ROUTER_DownloadDocument = "api/media/get/{0}";
const FString API::ROUTER_DOCUMENT_POOLING = "api/trm/status?requestid={0}";
const FString API::ROUTER_PROJECT_LOCALES = "api/projects/{0}/locales";
const FString API::ROUTER_WORKFLOW = "api/apps/wbflex/documents/{0}/workflow/status";
const FString API::ROUTER_DOCUMENT_UPDATE = "api/apps/wbflex/documents/{0}/contents/push";

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
		[userInfo, DocumentId, Callback, IsRetry](FHttpRequestPtr Request, FHttpResponsePtr Response,
		                                          bool bWasSuccessful)
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
				                  FOnAuthCompleted::CreateLambda(
					                  [userInfo, DocumentId, Callback](FString NewToken) mutable
					                  {
						                  // Update userInfo with new token and retry request
						                  userInfo.AuthToken = NewToken;
						                  UE_LOG(LogTemp, Log,
						                         TEXT("Authentication successful, retrying document fetch..."));
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

void API::PullDocument(FWordbeeUserData userInfo, const FString& DocumentId, FOnPullDocumentComplete callback, const TArray<FString>& Keys)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	FString url = ConstructUrl(userInfo.AccountId, userInfo.Url,
	                           FString::Format(*ROUTER_DOCUMENT_PULL, {DocumentId}));
	Request->SetURL(url);
	Request->SetVerb(TEXT("POST"));

	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("X-Auth-AccountId"), userInfo.AccountId);
	Request->SetHeader(TEXT("X-Auth-Token"), userInfo.AuthToken);

	// Create JSON body
	TSharedPtr<FJsonObject> RequestJson = MakeShareable(new FJsonObject());
	RequestJson->SetBoolField("includeComments", true);
	RequestJson->SetBoolField("includeCustomFields", true);
	RequestJson->SetBoolField("copySourceToTarget", false);

	// Convert the Keys array to a JSON array
	if (Keys.Num() > 0)
	{
		TArray<TSharedPtr<FJsonValue>> JsonKeys;
		for (const FString& Key : Keys)
		{
			JsonKeys.Add(MakeShareable(new FJsonValueString(Key)));
		}
		RequestJson->SetArrayField("keys", JsonKeys);	
	}
	// Serialize JSON object to string
	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(RequestJson.ToSharedRef(), Writer);

	Request->SetContentAsString(RequestBody);

	Request->OnProcessRequestComplete().BindLambda([=](FHttpRequestPtr Req, FHttpResponsePtr Res, bool bSuccess)
	{
		if (Res->GetResponseCode() == 401)
		{
			FWordbeeUserData userInfo = SingletonUtil::GetFromIni<FWordbeeUserData>();
			Authenticate(userInfo.AccountId, userInfo.ApiKey, userInfo.Url,
			             FOnAuthCompleted::CreateLambda(
				             [=](
				             FString NewToken) mutable
				             {
					             // Update userInfo with new token and retry request
					             userInfo.AuthToken = NewToken;
					             UE_LOG(LogTemp, Log,
					                    TEXT("Authentication successful, retrying document fetch..."));
					             SingletonUtil::SaveToIni<FWordbeeUserData>(userInfo);
					             PullDocument(userInfo, DocumentId, callback);
				             })
			);
		}
		else if (bSuccess && Res.IsValid())
		{
			TSharedPtr<FJsonObject> JsonObject;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Res->GetContentAsString());
			if (FJsonSerializer::Deserialize(Reader, JsonObject))
			{
				int32 RequestId = JsonObject->GetObjectField("trm")->GetIntegerField("requestid");
				CheckStatus(userInfo, RequestId, 0, callback);
			}
		}
		else
		{
			FMessageDialog::Open(EAppMsgType::Ok,
			                     FText::FromString(
				                     "Failed to PullDocument: " + FString::FromInt(Res->GetResponseCode())));
		}
	});
	Request->ProcessRequest();
}

void API::ExportRecords(const int32& documentId, TArray<FRecord> Records, FOnUpdateDocumentComplete onCompleted,
                        FRecord* TargetRecord, FColumn* TargetCol, bool bIsRetry)
{
	FWordbeeUserData userInfo = SingletonUtil::GetFromIni<FWordbeeUserData>();
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	FString url = ConstructUrl(userInfo.AccountId, userInfo.Url,
	                           FString::Format(*ROUTER_DOCUMENT_UPDATE, {documentId}));
	Request->SetURL(url);
	Request->SetVerb(TEXT("POST"));

	Request->SetHeader(TEXT("X-Auth-AccountId"), userInfo.AccountId);
	Request->SetHeader(TEXT("X-Auth-Token"), userInfo.AuthToken);
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	TArray<TSharedPtr<FJsonValue>> SegmentsArray;

	for (const FRecord& Record : Records)
	{
		bool bIsCreateNewRecord = Record.originalRecordID.IsEmpty();
		FString RecordID = bIsCreateNewRecord ? Record.recordID : Record.originalRecordID;

		if (TargetRecord)
		{
			FString TargetRecordID = bIsCreateNewRecord ? TargetRecord->recordID : TargetRecord->originalRecordID;
			if (RecordID != TargetRecordID)
			{
				continue;
			}
		}

		TSharedPtr<FJsonObject> TextsData = MakeShareable(new FJsonObject);

		for (const FColumn& Col : Record.columns)
		{
			if (TargetCol && Col.columnID != TargetCol->columnID)
			{
				continue;
			}

			if (Col.text.IsEmpty())
			{
				continue;
			}

			TSharedPtr<FJsonObject> ColDict = MakeShareable(new FJsonObject);
			ColDict->SetStringField(TEXT("v"), Col.text);

			if (Col.cfs.Num() > 0)
			{
				TArray<TSharedPtr<FJsonValue>> CfsArray;
				for (const FCustomField& Cfs : Col.cfs)
				{
					TSharedPtr<FJsonObject> CfsDict = MakeShareable(new FJsonObject);
					CfsDict->SetStringField(TEXT("t"), Cfs.Title);
					CfsDict->SetStringField(TEXT("id"), Cfs.Id);
					CfsDict->SetStringField(TEXT("v"), Cfs.V);

					CfsArray.Add(MakeShareable(new FJsonValueObject(CfsDict)));
				}

				ColDict->SetArrayField(TEXT("cfs"), CfsArray);
			}

			TextsData->SetObjectField(Col.columnID, ColDict);
		}

		TSharedPtr<FJsonObject> RecordData = MakeShareable(new FJsonObject);
		RecordData->SetStringField(TEXT("key"), RecordID);
		RecordData->SetObjectField(TEXT("texts"), TextsData);

		SegmentsArray.Add(MakeShareable(new FJsonValueObject(RecordData)));
	}

	TSharedPtr<FJsonObject> Mode = MakeShareable(new FJsonObject);
	Mode->SetStringField(TEXT("mode"), TEXT("partial"));

	TSharedPtr<FJsonObject> Body = MakeShareable(new FJsonObject);
	Body->SetObjectField(TEXT("header"), Mode);
	Body->SetArrayField(TEXT("segments"), SegmentsArray);

	FString JsonBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonBody);
	FJsonSerializer::Serialize(Body.ToSharedRef(), Writer);

	Request->SetContentAsString(JsonBody);
	Request->OnProcessRequestComplete().BindLambda(
		[documentId, Records, TargetRecord, TargetCol, bIsRetry,onCompleted](
		FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			if (Response->GetResponseCode() == 401)
			{
				FWordbeeUserData userInfo = SingletonUtil::GetFromIni<FWordbeeUserData>();
				Authenticate(userInfo.AccountId, userInfo.ApiKey, userInfo.Url,
				             FOnAuthCompleted::CreateLambda(
					             [=](
					             FString NewToken) mutable
					             {
						             // Update userInfo with new token and retry request
						             userInfo.AuthToken = NewToken;
						             UE_LOG(LogTemp, Log,
						                    TEXT("Authentication successful, retrying document fetch..."));
						             SingletonUtil::SaveToIni<FWordbeeUserData>(userInfo);

						             API::ExportRecords(documentId, Records, onCompleted, TargetRecord, TargetCol,
						                                true);
					             })
				);
			}
			else if (bWasSuccessful)
			{
				TSharedPtr<FJsonObject> JsonObject;
				TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*Response->GetContentAsString());
				if (FJsonSerializer::Deserialize(Reader, JsonObject))
				{
					int32 RequestId = JsonObject->GetObjectField("trm")->GetIntegerField("requestid");
					onCompleted.ExecuteIfBound(true, RequestId, "Done");
				}
			}
			else if (!bWasSuccessful) // Unauthorized
			{
				if (!bIsRetry)
				{
					// Retry with authentication
					ExportRecords(documentId, Records, onCompleted, TargetRecord, TargetCol, true);
				}
				if (bIsRetry)
				{
					onCompleted.ExecuteIfBound(
						false, 0, "ERROR " + FString::FromInt(Response->GetResponseCode()) + "\n please try again");
				}
			}
		});

	Request->ProcessRequest();
}

void API::PushRecords(TArray<FRecord> Records, FOnUpdateDocumentComplete onCompleted)
{
	FDocumentData document = SingletonUtil::GetFromIni<FDocumentData>();
	ExportRecords(document.did, Records, FOnUpdateDocumentComplete::CreateLambda(
		              [=](bool bSuccess, const int32& RequestId, const FString& message)
		              {
			              if (bSuccess)
			              {
				              CheckStatus(RequestId, 0, FOnCheckStatusComplete::CreateLambda([=](bool bSuccess)
				              {
					              onCompleted.ExecuteIfBound(bSuccess, RequestId, message);
				              }));
			              }
			              else
			              {
				              onCompleted.ExecuteIfBound(bSuccess, RequestId, message);
			              }
		              }));
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

void API::CheckStatus(int32 RequestId, int32 RetryCount, FOnCheckStatusComplete callback)
{
	const int32 MaxRetries = 15; // Stop checking after 15 retries (~30 seconds)
	if (RequestId == 0 || RetryCount >= MaxRetries)
	{
		UE_LOG(LogTemp, Error, TEXT("Status check timed out or invalid request ID."));
		callback.ExecuteIfBound("");
		return;
	}
	FWordbeeUserData userInfo = SingletonUtil::GetFromIni<FWordbeeUserData>();
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
					callback.ExecuteIfBound(true);
				}
				else
				{
					FPlatformProcess::Sleep(2.0f); // Wait before retrying
					CheckStatus(RequestId, RetryCount + 1, callback); // Increment retry count
				}
			}
		}
		else
		{
			callback.ExecuteIfBound(false);
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

void API::FetchLanguages(FWordbeeUserData userInfo, 
                         TFunction<void(const TArray<FLanguageInfo>&)> OnSuccess, 
                         TFunction<void(const FString&)> OnError, 
                         bool IsRetry)
{
	if (userInfo.AccountId.IsEmpty() || userInfo.AuthToken.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid user data. Cannot fetch languages."));
		if (OnError) OnError(TEXT("Invalid user data"));
		return;
	}

	FString Url = ConstructUrl(userInfo.AccountId, userInfo.Url, FString::Format(*ROUTER_PROJECT_LOCALES, { userInfo.ProjectId }));
	UE_LOG(LogTemp, Log, TEXT("Fetching languages with user info: %s"), *Url);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	UE_LOG(LogTemp, Log, TEXT("Fetching languages with user info: %s"), *url);
	Request->SetVerb(TEXT("GET"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(APIConstant::AuthToken, userInfo.AuthToken);
	Request->SetHeader(APIConstant::AuthAccountID, userInfo.AccountId);

	Request->OnProcessRequestComplete().BindLambda(
		[userInfo, OnSuccess, OnError, IsRetry](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			FString RawBody = Response.IsValid() ? Response->GetContentAsString() : TEXT("Request failed");
			int32 ResponseCode = Response.IsValid() ? Response->GetResponseCode() : -1;
			UE_LOG(LogTemp, Warning, TEXT("rawBody: %s"), *RawBody);

			if (!bWasSuccessful || !Response.IsValid())
			{
				UE_LOG(LogTemp, Error, TEXT("Request failed: %s (Code: %d)"), *RawBody, ResponseCode);
				if (OnError) OnError(FString::Printf(TEXT("Request failed with code %d"), ResponseCode));
				return;
			}

			if (ResponseCode == 401 && !IsRetry)
			{
				API::Authenticate(userInfo.AccountId, userInfo.ApiKey, userInfo.Url,
					FOnAuthCompleted::CreateLambda([userInfo, OnSuccess, OnError](FString NewToken) mutable
					{
						// Update userInfo with new token and retry request
						userInfo.AuthToken = NewToken;
						UE_LOG(LogTemp, Log, TEXT("Authentication successful, retrying language fetch..."));
						API::FetchLanguages(userInfo, OnSuccess, OnError, true);
					})
				);
				return;
			}

			if (ResponseCode == 200)
			{
				TArray<FLanguageInfo> LanguageList;
				if (FJsonObjectConverter::JsonArrayStringToUStruct(RawBody, &LanguageList, 0, CPF_Transient))
				{
					OnSuccess(LanguageList);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON into FLanguageInfo array"));
					if (OnError) OnError(TEXT("Failed to parse JSON response"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to fetch languages! Response Code: %d"), ResponseCode);
				if (OnError) OnError(FString::Printf(TEXT("Failed to fetch languages, response code %d"), ResponseCode));
			}
		});


	Request->ProcessRequest();
}

void API::FetchWorkflow(FWordbeeUserData userInfo, const FString& DocumentId,
                        TFunction<void(const TArray<FWorkflowStatus>&)> onSuccess,
                        TFunction<void(const FString&)> onError, bool IsRetry)
{
	if (userInfo.AccountId.IsEmpty() || userInfo.AuthToken.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid user data. Cannot fetch workflow."));
		onError(TEXT("Invalid user data"));
		return;
	}
	FString Url = ConstructUrl(userInfo.AccountId, userInfo.Url, FString::Format(*ROUTER_WORKFLOW, { userInfo.DocumentId }));
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetVerb(TEXT("GET"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(APIConstant::AuthToken, userInfo.AuthToken);
	Request->SetHeader(APIConstant::AuthAccountID, userInfo.AccountId);

	Request->OnProcessRequestComplete().BindLambda(
		[userInfo, DocumentId, onSuccess, onError, IsRetry](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			FString RawBody = Response.IsValid() ? Response->GetContentAsString() : TEXT("Request failed");
			int32 ResponseCode = Response.IsValid() ? Response->GetResponseCode() : -1;
			UE_LOG(LogTemp, Warning, TEXT("Response: %s"), *RawBody);

			if (!bWasSuccessful || !Response.IsValid())
			{
				UE_LOG(LogTemp, Error, TEXT("Request failed: %s (Code: %d)"), *RawBody, ResponseCode);
				onError(FString::Printf(TEXT("Request failed: %s (Code: %d)"), *RawBody, ResponseCode));
				return;
			}

			// Handle authentication failure and retry if necessary
			if (ResponseCode == 401 && !IsRetry)
			{
				API::Authenticate(userInfo.AccountId, userInfo.ApiKey, userInfo.Url,
					FOnAuthCompleted::CreateLambda([userInfo, DocumentId, onSuccess, onError](FString NewToken) mutable
					{
						// Update userInfo with new token and retry request
						userInfo.AuthToken = NewToken;
						UE_LOG(LogTemp, Log, TEXT("Authentication successful, retrying workflow fetch..."));
						API::FetchWorkflow(userInfo, DocumentId, onSuccess, onError, true);
					})
				);
				return;
			}

			// Deserialize JSON into an array of workflow statuses
			TArray<FWorkflowStatus> ParsedWorkflows;
			if (!FJsonObjectConverter::JsonArrayStringToUStruct<FWorkflowStatus>(RawBody, &ParsedWorkflows, 0, 0))
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to parse response"));
				onError(TEXT("Failed to parse response"));
				return;
			}

			// Call success callback with the parsed data
			onSuccess(ParsedWorkflows);
		});

	Request->ProcessRequest();
}



