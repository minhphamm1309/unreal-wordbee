#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
class UserData: public UDataAsset
{
public:
    static UserData* GetInstance();
    FString Url;
    FString AccountId;
    FString ApiKey;
    FString AuthToken;
    FString ProjectId;
    int32 DocumentId;

private:
    UserData() {} // Private constructor to prevent instantiation

    static UserData* Singleton;
};
