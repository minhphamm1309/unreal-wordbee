#include "SUserData.h"

UUserData* SUserData::Instance = nullptr;

UUserData* SUserData::Get()
{
	if (Instance == nullptr)
	{
		FString Path = TEXT("/Game/WordBee/UserData");
		Instance = Cast<UUserData>(StaticLoadObject(UUserData::StaticClass(), Instance, *Path));

		if (Instance == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to create Blueprint"));
			return nullptr;
		}
	}
	return Instance;
}
