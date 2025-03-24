#include "UGetDocumentsCommand.h"

#include "FDocumentData.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "WordBeeEditor/API/API.h"
#include "WordBeeEditor/Utils/Constant.h"


void UGetDocumentsCommand::ExecuteHttpRequest(UserInfo InUserInfo, FOnHttpRequestComplete OnComplete)
{
	OnCompleteDelegate = OnComplete;
	FString URL = UAPI::ConstructUrl(InUserInfo.AccountId, InUserInfo.BaseUrl, UAPI::ROUTER_DOCUMENTS);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

	// Set the URL, HTTP method (POST), and headers
	HttpRequest->SetURL(URL);
	HttpRequest->SetVerb("POST");
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded"));
	HttpRequest->SetHeader(TEXT("X-Auth-Token"), InUserInfo.AuthToken);
	HttpRequest->SetHeader(TEXT("X-Auth-AccountId"), InUserInfo.AccountId);              


	// Bind the response handler
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UGetDocumentsCommand::OnResponseReceived);
	
	// Execute the request
	HttpRequest->ProcessRequest();

	//Log header
	UE_LOG(LogTemp, Log, TEXT("URL: %s"), *HttpRequest->GetURL());
	UE_LOG(LogTemp, Log, TEXT("Header: X-Auth-Token: %s"), *HttpRequest->GetHeader(TEXT("X-Auth-Token")));
	UE_LOG(LogTemp, Log, TEXT("Header: X-Auth-AccountId: %s"), *HttpRequest->GetHeader(TEXT("X-Auth-AccountId")));
	

	
}

void UGetDocumentsCommand::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr InResponse, bool bWasSuccessful)
{
	if (bWasSuccessful && InResponse.IsValid())
	{
		TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(InResponse->GetContentAsString());

        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
	        TArray<FDocumentData> DocumentDataArray;
	        // Extract the array "rows"
            const TArray<TSharedPtr<FJsonValue>> Rows = JsonObject->GetArrayField("rows");

            // Iterate through each entry in the "rows" array and create an FDocumentData instance
            for (const TSharedPtr<FJsonValue>& RowValue : Rows)
            {
                TSharedPtr<FJsonObject> RowObject = RowValue->AsObject();

                // Create a new instance of FDocumentData to store this row's data
                FDocumentData DocumentData;
                
                // Extract and assign values to the DocumentData struct
                DocumentData.Id = RowObject->GetStringField("id");
                DocumentData.DtChange = RowObject->GetStringField("dtchange");
                DocumentData.Did = RowObject->GetIntegerField("did");
                DocumentData.Src = RowObject->GetStringField("src");
                DocumentData.SrcT = RowObject->GetStringField("srct");
                DocumentData.Name = RowObject->GetStringField("name");
                DocumentData.Paragraphs = RowObject->GetIntegerField("paragraphs");
                DocumentData.Segments = RowObject->GetIntegerField("segments");
                DocumentData.Pid = RowObject->GetIntegerField("pid");
                DocumentData.Connector = RowObject->GetStringField("connector");
                DocumentData.Preference = RowObject->GetStringField("preference");
                DocumentData.PStatus = RowObject->GetIntegerField("pstatus");
                DocumentData.PStatusT = RowObject->GetStringField("pstatust");
                DocumentData.FilterName = RowObject->GetStringField("filterName");

                // At this point, you have parsed the data and stored it in DocumentData

                // Here you can now store the data, print it, or do further processing
                UE_LOG(LogTemp, Log, TEXT("Document ID: %s, Name: %s, Project: %s"), *DocumentData.Id, *DocumentData.Name, *DocumentData.Preference);

                // You can save this DocumentData instance to an array or process it further
                // For example, you can store it in a TArray of FDocumentData
                DocumentDataArray.Add(DocumentData);
            }
        	
        	OnCompleteDelegate.ExecuteIfBound(DocumentDataArray);
        }
        else
        {
            // Failed to parse JSON
        	OnCompleteDelegate.ExecuteIfBound( TArray<FDocumentData>());
        }
	}
	else
	{
		if (OnCompleteDelegate.IsBound())
		{
			OnCompleteDelegate.Execute( TArray<FDocumentData>());
		}
	}
}
