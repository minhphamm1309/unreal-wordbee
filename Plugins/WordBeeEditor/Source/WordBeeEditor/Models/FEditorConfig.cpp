#include "FEditorConfig.h"

bool FEditorConfig::Load()
{
	FString JsonString;
	if (FFileHelper::LoadFileToString(JsonString, *GetConfigFilePath()))
	{
		return FJsonObjectConverter::JsonObjectStringToUStruct(JsonString, this, 0, 0);
	}
	return false;
}

void FEditorConfig::Save()
{
	FString JsonString;
	if (FJsonObjectConverter::UStructToJsonObjectString(*this, JsonString))
	{
		FFileHelper::SaveStringToFile(JsonString, *GetConfigFilePath());
	}
}
