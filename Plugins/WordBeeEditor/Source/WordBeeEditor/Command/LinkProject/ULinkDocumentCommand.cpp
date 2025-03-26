#include "ULinkDocumentCommand.h"

#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "Interfaces/IHttpResponse.h"
#include "WordBeeEditor/API/API.h"
#include "WordBeeEditor/Models/FWordbeeFile.h"

void ULinkDocumentCommand::Execute(UUserData* UserInfo,  const FString DocumentId, FOnLinkDocumentComplete callback)
{
	API::PullDocument(UserInfo, DocumentId, FOnPullDocumentComplete::CreateLambda([=](const FString& downloadContent)
	{

	    FWordbeeFile WordbeeFile;

	    if (ParseJsonToWordbeeFile(downloadContent, WordbeeFile))
	    {
	        UE_LOG(LogTemp, Log, TEXT("Parsed Wordbee file successfully"));
	    }
	    else
	    {
		    UE_LOG(LogTemp, Log, TEXT("Parsed Wordbee file failed"));
	    }
	}));

}

bool ULinkDocumentCommand::ParseJsonToWordbeeFile(const FString& JsonString, FWordbeeFile& OutWordbeeFile)
{
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

    if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
    {
        // Deserialize basic fields
        OutWordbeeFile.type = JsonObject->GetStringField("type");
        OutWordbeeFile.version = JsonObject->GetStringField("version");
        OutWordbeeFile.dsid = JsonObject->GetIntegerField("dsid");
        OutWordbeeFile.did = JsonObject->GetIntegerField("did");

        // Parse segments
        const TArray<TSharedPtr<FJsonValue>>* SegmentsArray;
        if (JsonObject->TryGetArrayField("segments", SegmentsArray))
        {
            for (const TSharedPtr<FJsonValue>& SegmentValue : *SegmentsArray)
            {
                TSharedPtr<FJsonObject> SegmentObject = SegmentValue->AsObject();
                FSegment Segment;
                Segment.key = SegmentObject->GetStringField("key");
                Segment.component = SegmentObject->GetStringField("component");
                Segment.dt = SegmentObject->GetStringField("dt");
                Segment.format = SegmentObject->GetStringField("format");
                Segment.bsid = SegmentObject->GetIntegerField("bsid");
                Segment.chmin = SegmentObject->GetIntegerField("chmin");
                Segment.chmax = SegmentObject->GetIntegerField("chmax");

                // Parse texts
                const TSharedPtr<FJsonObject>* TextsObject;
                if (SegmentObject->TryGetObjectField("texts", TextsObject))
                {
                    for (const auto& TextPair : (*TextsObject)->Values)
                    {
                        TSharedPtr<FJsonObject> TextObject = TextPair.Value->AsObject();
                        FSegmentText TextSegment;
                        TextSegment.v = TextObject->GetStringField("v");
                        TextSegment.st = TextObject->GetIntegerField("st");
                        TextSegment.bk = TextObject->GetIntegerField("bk");
                        TextSegment.ed = TextObject->GetIntegerField("ed");
                        TextSegment.dt = TextObject->GetStringField("dt");

                        Segment.texts.Add(TextPair.Key, TextSegment);
                    }
                }

                OutWordbeeFile.segments.Add(Segment);
            }
        }

        return true;
    }

    return false;
}

