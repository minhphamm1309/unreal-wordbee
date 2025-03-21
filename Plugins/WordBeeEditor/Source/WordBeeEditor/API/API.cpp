#include "API.h"

#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "JsonUtilities.h"
#include "Json.h"
#include "Misc/MessageDialog.h"  // For showing messages in the editor


void UAPI::Authenticate(FString AccountId, FString ApiKey, FString BaseUrl , FOnAuthCompleted callback)
{
	FString URL = FString::Printf(TEXT("https://%s.%s/api/auth/token"), *AccountId, *BaseUrl);
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
