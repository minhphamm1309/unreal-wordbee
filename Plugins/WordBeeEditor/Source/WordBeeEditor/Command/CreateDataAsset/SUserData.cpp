#include "SUserData.h"

UUserData* SUserData::Instance = nullptr;
void SUserData::LoadSettingsIntoUserData(UUserData* UserData)
{
	if (!UserData)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid UUserData instance!"));
		return;
	}
	FString ConfigSection = TEXT("ConnectSettings");
	FString CustomIniPath = FPaths::ProjectSavedDir() + "WordBee/Settings.ini";

	if (!FPaths::FileExists(CustomIniPath))
	{
		UE_LOG(LogTemp, Warning, TEXT("Settings file not found: %s"), *CustomIniPath);
		return;
	}
	FConfigFile ConfigFile;
	ConfigFile.Read(CustomIniPath);
	// Load URL
	FString SUrl;
	if (ConfigFile.GetString(*ConfigSection, TEXT("url"), SUrl))
	{
		UserData->Url = SUrl;
	}
	// Load API Key
	FString SApiKey;
	if (ConfigFile.GetString(*ConfigSection, TEXT("ApiKey"), SApiKey))
	{
		UserData->ApiKey = SApiKey;
	}
	// Load Account ID
	FString SAccountId;
	if (ConfigFile.GetString(*ConfigSection, TEXT("AccountId"), SAccountId))
	{
		UserData->AccountId = SAccountId;
	}
	// Load authtoken
	FString SAuthToken;
	if (ConfigFile.GetString(*ConfigSection, TEXT("AuthToken"), SAuthToken))
	{
		UserData->AuthToken = SAuthToken;
	}
}
UUserData* SUserData::Get()
{
	if (Instance == nullptr)
	{
		Instance = NewObject<UUserData>();
		LoadSettingsIntoUserData(Instance);
	}
	return Instance;
}
