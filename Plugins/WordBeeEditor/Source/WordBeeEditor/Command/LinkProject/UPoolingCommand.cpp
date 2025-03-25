#include "UPoolingCommand.h"

#include "AITestsCommon.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "WordBeeEditor/API/API.h"

void UPoolingCommand::Execute(const FUserInfo UserInfo, const int32 RequestId, FOnPoolingComplete OnSuccess, FOnPoolingFail OnError)
{
	constexpr float TimeOut = 10.f;
    constexpr int32 DelayCall = 1000; // ms
    const double StartTime = FPlatformTime::Seconds();
    FString FileToken = TEXT("");
    bool bIsTimeOut = false;

    // Lambda function for handling HTTP response
    auto PollingRequest = [=]() mutable
    {
        if (FileToken.IsEmpty())
        {
            // Check for timeout
            bIsTimeOut = FPlatformTime::Seconds() - StartTime >= TimeOut;
            if (bIsTimeOut)
            {
                OnError.ExecuteIfBound(TEXT("Polling - Timeout Error"));
                return;
            }

            TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
    
            FString Router = UAPI::ROUTER_DOCUMENT_POOLING.Replace(TEXT("{0}"), *FString::FromInt(RequestId));
            FString url = UAPI::ConstructUrl(UserInfo.AccountId, UserInfo.BaseUrl, Router);
            // Thiết lập URL
            HttpRequest->SetURL(url);

            // Thiết lập phương thức HTTP là GET
            HttpRequest->SetVerb(TEXT("GET"));
    
            // Thiết lập các header
            HttpRequest->SetHeader(TEXT("X-Auth-AccountId"), UserInfo.AccountId);
            HttpRequest->SetHeader(TEXT("X-Auth-Token"), UserInfo.AuthToken);
            HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
            
            HttpRequest->OnProcessRequestComplete().BindLambda([=](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
            {
                if (!bWasSuccessful || !Response.IsValid() || Response->GetResponseCode() != EHttpResponseCodes::Ok)
                {
                    OnError.ExecuteIfBound(TEXT("Polling - HTTP Error"));
                    return;
                }
                UE_LOG(LogTemp, Display, TEXT("Polling - Http Request Completed : %s"), *Response->GetContentAsString());
                // // Parse response
                // FPollingCustomData_PullingDocument PollingData;
                // // Assuming we have a function to parse the response to our custom data format
                // if (!ParsePollingResponse(Response, PollingData))
                // {
                //     failCallback.ExecuteIfBound(TEXT("Polling - Failed to parse response"));
                //     return;
                // }
                //
                // // Check if the polling result is failed
                // if (PollingData.Trm.IsFailed())
                // {
                //     failCallback.ExecuteIfBound(TEXT("Polling - trm.status Failed"));
                //     return;
                // }
                //
                // // Check if the polling result is finished and we got the file token
                // if (PollingData.Trm.IsFinished() && !PollingData.FileToken.IsEmpty())
                // {
                //     FileToken = PollingData.FileToken;
                //     DownloadDocument(FileToken, callback, failCallback);  // DownloadDocument needs to be defined separately
                // }
                // else
                // {
                //     // If not finished, set a delay to poll again
                //     FTimerHandle TimerHandle;
                //     GetWorld()->GetTimerManager().SetTimer(TimerHandle, PollingRequest, DelayCall / 1000.f, false);
                // }
            });
            HttpRequest->ProcessRequest();
        }
    };

    // Start the initial polling request
    PollingRequest();
}

void UPoolingCommand::DownloadDocument(const FString& FileToken, FOnPoolingComplete callback,
    FOnPoolingFail failCallback)
{
}

void UPoolingCommand::Pooling(const int32 RequestId, FOnPoolingComplete callback, FOnPoolingFail failCallback)
{
}
