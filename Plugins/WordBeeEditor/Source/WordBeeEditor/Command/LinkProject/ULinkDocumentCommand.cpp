#include "ULinkDocumentCommand.h"

#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "UPoolingCommand.h"
#include "URequestExportDocumentCommand.h"
#include "Interfaces/IHttpResponse.h"
#include "WordBeeEditor/API/API.h"
#include "WordBeeEditor/Utils/UserInfo.h"

void ULinkDocumentCommand::Execute(FUserInfo InUserInfo, const FString DocumentId, FOnLinkDocumentComplete callback)
{
	URequestExportDocumentCommand::Execute(InUserInfo, DocumentId, FOnRequestExportDocumentComplete::CreateLambda(
[=](bool success, const FString& response)
       {
			if (success)
			{
				UE_LOG(LogTemp, Warning, TEXT("ULinkDocumentCommand::Execute - %s"), *response);
				FLinkDocumentResponseData ResponseData = ParseJsonResponse(response);
				UPoolingCommand::Execute(InUserInfo, ResponseData.Trm.RequestId
					,FOnPoolingComplete::CreateLambda([=](bool bSuccess, const FString& Response)
					{
						callback.ExecuteIfBound(true);
					})
					, FOnPoolingFail::CreateLambda([=](const FString& Error)
					{
						callback.ExecuteIfBound(false);
					})
					);
			}
		    else
			{
				UE_LOG(LogTemp, Warning, TEXT("ULinkDocumentCommand::Execute - RequestExportDocumentCommand failed"));
			}
       }));
}

FLinkDocumentResponseData ULinkDocumentCommand::ParseJsonResponse(const FString& Response)
{
	FLinkDocumentResponseData ResponseData;

	// Tạo JSON reader để phân tích chuỗi JSON
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response);

	// Tạo đối tượng JsonObject để lưu trữ kết quả phân tích JSON
	TSharedPtr<FJsonObject> JsonObject;
	
	// Chuyển đổi chuỗi JSON thành struct
	if (FJsonObjectConverter::JsonObjectStringToUStruct<FLinkDocumentResponseData>(Response, &ResponseData, 0, 0))
	
	{
		// Chuyển đổi thành công, bạn có thể truy cập các trường của ResponseData
		UE_LOG(LogTemp, Log, TEXT("Request ID: %d"), ResponseData.Trm.RequestId);
		UE_LOG(LogTemp, Log, TEXT("Is Batch: %s"), ResponseData.Trm.IsBatch ? TEXT("true") : TEXT("false"));
		UE_LOG(LogTemp, Log, TEXT("Status: %s"), *ResponseData.Trm.Status);
		UE_LOG(LogTemp, Log, TEXT("Status Text: %s"), *ResponseData.Trm.StatusText);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to convert JSON to struct"));
	}
	return ResponseData;
}

