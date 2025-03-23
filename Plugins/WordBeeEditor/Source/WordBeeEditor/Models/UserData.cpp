#include "UserData.h"
#include "WordbeeEditor/Utils/SingletonUtil.h"
#include "WordbeeEditor/Utils/Constants.h"
UserData* UserData::Singleton = nullptr;

UserData* UserData::GetInstance()
{
    if (Singleton == nullptr)
    {
        Singleton = SingletonUtil::GetOrCreateAsset<UserData>(Constants::UserDataPath);
    }
    return Singleton;
}
