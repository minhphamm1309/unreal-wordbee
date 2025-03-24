#include "ULinkDocumentCommand.h"

#include "HttpModule.h"
#include "WordBeeEditor/API/API.h"
#include "WordBeeEditor/Utils/UserInfo.h"

void ULinkDocumentCommand::Execute(UserInfo InUserInfo,FOnLinkDocumentComplete callback)
{
	FHttpModule* Http = &FHttpModule::Get();

    // Tạo một Http Request
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
    FString url = UAPI::ConstructUrl(InUserInfo.AccountId, InUserInfo.BaseUrl, UAPI::ROUTER_DOCUMENTS);
    // Đặt URL cho request
    Request->SetURL(url);

    // Đặt method cho request (POST, GET, etc.)
    Request->SetVerb("POST");

    // Đặt các Header
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded"));
    Request->SetHeader(TEXT("X-Auth-AccountId"), InUserInfo.AccountId);
    Request->SetHeader(TEXT("X-Auth-Token"), InUserInfo.AuthToken);

    // Tạo một Dictionary JSON cho phần body (sử dụng JsonObject)
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

    // Thêm các key-value vào body
    JsonObject->SetBoolField(TEXT("includeComments"), true);
    JsonObject->SetBoolField(TEXT("includeCustomFields"), true);
    JsonObject->SetBoolField(TEXT("copySourceToTarget"), false);
    JsonObject->SetBoolField(TEXT("excludeTexts"), "none"); // config.targetSynchronization là một biến boolean bạn định nghĩa trước đó

    // Chuyển đổi JsonObject thành chuỗi JSON
    FString RequestBody;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

    // Đặt body JSON vào Http Request
    Request->SetContentAsString(RequestBody);

    // Gán callback cho khi request hoàn thành
    Request->OnProcessRequestComplete().BindLambda([](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
    {
        if (bWasSuccessful && Response.IsValid())
        {
            
        }
        else
        {
            // Xử lý lỗi khi không gửi được request
        }
    });

    // Thực hiện request
    Request->ProcessRequest();
}

void ULinkDocumentCommand::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
}
