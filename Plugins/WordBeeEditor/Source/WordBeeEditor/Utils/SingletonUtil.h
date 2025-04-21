#pragma once

#ifndef SINGLE_TON
#define SINGLE_TON

#include "JsonObjectConverter.h"

namespace wordbee {

    template <typename T>
    class SingletonUtil
    {
    public:
        static T GetFromIni()
        {
            FString key;
            if (T::StaticStruct())
            {
                key = T::StaticStruct()->GetName();
            }

            FString ConfigSection = TEXT("ConnectSettings");
            FString CustomIniPath = FPaths::ProjectSavedDir() + "Wordbee/" + key + TEXT(".ini");

            if (!FPaths::FileExists(CustomIniPath))
                return T();

            FConfigFile ConfigFile;
            ConfigFile.Read(CustomIniPath);

            FString StrJsonObject;
            if (ConfigFile.GetString(*ConfigSection, *key, StrJsonObject))
            {
                T OutObject;
                FJsonObjectConverter::JsonObjectStringToUStruct(StrJsonObject, &OutObject, 0, 0);
                return OutObject;
            }

            return T();
        }

        static void SaveToIni(const T& Object)
        {
            FString key;
            if (T::StaticStruct())
            {
                key = T::StaticStruct()->GetName();
            }

            FString ConfigSection = TEXT("ConnectSettings");
            FString CustomIniPath = FPaths::ProjectSavedDir() + "Wordbee/" + key + TEXT(".ini");

            IFileManager::Get().MakeDirectory(*FPaths::GetPath(CustomIniPath), true);

            if (!FPaths::FileExists(CustomIniPath))
            {
                FFileHelper::SaveStringToFile(TEXT(""), *CustomIniPath);
            }

            FConfigFile ConfigFile;
            ConfigFile.Read(CustomIniPath);

            FString StrJsonObject;
            FJsonObjectConverter::UStructToJsonObjectString(Object, StrJsonObject, 0, 0, 0, nullptr, true);
            ConfigFile.SetString(*ConfigSection, *key, *StrJsonObject);
            ConfigFile.Write(CustomIniPath);
        }

        static void ClearFromIni()
        {
            FString key;
            if (T::StaticStruct())
            {
                key = T::StaticStruct()->GetName();
            }

            FString ConfigSection = TEXT("ConnectSettings");
            FString CustomIniPath = FPaths::ProjectSavedDir() + "Wordbee/" + key + TEXT(".ini");

            if (!FPaths::FileExists(CustomIniPath))
                return;

            FConfigFile ConfigFile;
            ConfigFile.Read(CustomIniPath);

            if (ConfigFile.Contains(ConfigSection))
            {
                FConfigSection* Section = ConfigFile.Find(ConfigSection);
                if (Section && Section->Remove(*key) > 0)
                {
                    ConfigFile.Write(CustomIniPath);
                }
            }
        }
    };
}
#endif


