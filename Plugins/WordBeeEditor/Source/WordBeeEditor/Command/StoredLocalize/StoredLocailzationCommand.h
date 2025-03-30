#pragma once
#include "FLocalizeSegment.h"
#include "WordBeeEditor/Models/FSegment.h"

// Define the structure classes first
struct FLocalizationKeyEntryM
{
	FString Key;
	FString Path;
    
	TSharedPtr<FJsonObject> ToJson() const
	{
		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
		JsonObject->SetStringField("Key", Key);
		JsonObject->SetStringField("Path", Path);
		return JsonObject;
	}
};

struct FLocalizationTextEntryM
{
	FString Text;
	TArray<FLocalizationKeyEntryM> Keys;
    
	TSharedPtr<FJsonObject> ToJson() const
	{
		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
        
		// Source text
		TSharedPtr<FJsonObject> SourceObj = MakeShareable(new FJsonObject);
		SourceObj->SetStringField("Text", Text);
		JsonObject->SetObjectField("Source", SourceObj);
        
		// Keys array
		TArray<TSharedPtr<FJsonValue>> KeyValues;
		for (const FLocalizationKeyEntryM& Key : Keys)
		{
			KeyValues.Add(MakeShareable(new FJsonValueObject(Key.ToJson())));
		}
		JsonObject->SetArrayField("Keys", KeyValues);
        
		return JsonObject;
	}
};

struct FLocalizationNamespaceM
{
	FString Namespace;
	TArray<FLocalizationTextEntryM> Children;
    
	TSharedPtr<FJsonObject> ToJson() const
	{
		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
		JsonObject->SetStringField("Namespace", Namespace);
        
		TArray<TSharedPtr<FJsonValue>> ChildValues;
		for (const FLocalizationTextEntryM& Child : Children)
		{
			ChildValues.Add(MakeShareable(new FJsonValueObject(Child.ToJson())));
		}
		JsonObject->SetArrayField("Children", ChildValues);
        
		return JsonObject;
	}
};

class StoredLocailzationCommand
{
public:
	static void Execute(const TArray<FSegment>& Segments);
	static bool AddCultureIfNotSupported(const FString& CultureCode);
	static  void AddLocalizationEntry(const TArray<FLocalizeSegment>& LocalizeSegments, FString CultureCode);
	static FLocalizationTextEntryM ParseToManifest(const FString& Key, const FString& String);
	static void GenerateLocalizationManifest(TArray<FLocalizationNamespaceM> Array);
	static void CreateNewLocalizationTarget(const FString& Key);

};
