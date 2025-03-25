#include "URequestExportDocumentCommand.h"

#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "WordBeeEditor/API/API.h"
#include "WordBeeEditor/Command/CreateDataAsset/UserData.h"

class IHttpRequest;
class FHttpModule;

void URequestExportDocumentCommand::Execute(const UUserData* InUserInfo, const FString InDocumentId,
                                            FOnRequestExportDocumentComplete callBack)
{

	FHttpModule* Http = &FHttpModule::Get();

	// Tạo một Http Request
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();

	FString router = UAPI::ROUTER_DOCUMENT_PULL.Replace(TEXT("{0}"), *InDocumentId);

	FString url = UAPI::ConstructUrl(InUserInfo->AccountId, InUserInfo->Url, router);
	// Đặt URL cho request
	Request->SetURL(url);

	// Đặt method cho request (POST, GET, etc.)
	Request->SetVerb("POST");

	// Đặt các Header
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded"));
	Request->SetHeader(TEXT("X-Auth-AccountId"), InUserInfo->AccountId);
	Request->SetHeader(TEXT("X-Auth-Token"), InUserInfo->AuthToken);

	// Tạo một Dictionary JSON cho phần body (sử dụng JsonObject)
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

	// Thêm các key-value vào body
	JsonObject->SetBoolField(TEXT("includeComments"), true);
	JsonObject->SetBoolField(TEXT("includeCustomFields"), true);
	JsonObject->SetBoolField(TEXT("copySourceToTarget"), false);
	JsonObject->SetBoolField(TEXT("excludeTexts"), "none");
	// config.targetSynchronization là một biến boolean bạn định nghĩa trước đó

	// Chuyển đổi JsonObject thành chuỗi JSON
	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	// Đặt body JSON vào Http Request
	Request->SetContentAsString(RequestBody);

	// Gán callback cho khi request hoàn thành
	Request->OnProcessRequestComplete().BindLambda(
		[callBack](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			// Handle the response here
			callBack.ExecuteIfBound(bWasSuccessful, *Response->GetContentAsString());
		});

	// Thực hiện request
	Request->ProcessRequest();
}
