#pragma once

#include "CoreMinimal.h"
#include "EditorAssetLibrary.h"
#include "JsonObjectConverter.h"
#include "Reflection/FunctionUtilsPrivate.h"
#include "UObject/NoExportTypes.h"
#include "UObject/UnrealType.h"

class SingletonUtil
{
public:
    template <typename T>
    static T* GetOrCreateAsset(const FString& AssetPath)
    {
        if (!T::StaticClass()->IsChildOf(UDataAsset::StaticClass()))
        {
            UE_LOG(LogTemp, Error, TEXT("GetOrCreateAsset: %s is not a UDataAsset!"), *T::StaticClass()->GetName());
            return nullptr;
        }
        // Try to load the asset
        T* Asset = Cast<T>(UEditorAssetLibrary::LoadAsset(AssetPath));

        if (!Asset)
        {
            // Create new asset if it does not exist
            Asset = NewObject<T>();
        
            // Save to disk
            UEditorAssetLibrary::SaveAsset(AssetPath, false);
        }

        return Asset;
    }

    template <typename T>
    static T GetFromIni()
    {
        static_assert(UE::Private::TIsUHTUStruct<T>::value, "The template parameter must be a USTRUCT, not a UCLASS!");
        FString key;
        if (T::StaticStruct())
        {
            key = T::StaticStruct()->GetName();
        }

        FString ConfigSection = TEXT("ConnectSettings");
        FString CustomIniPath = FPaths::ProjectSavedDir() + "WordBee/Settings.ini";

        if (!FPaths::FileExists(CustomIniPath)) return T();

        // Create a new FConfigFile to read from the custom INI file
        FConfigFile ConfigFile;

        // Read the config file from disk
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

    template <typename T>
    static void SaveToIni(const T& Object)
    {
        FString key;
        if (T::StaticStruct())
        {
            key = T::StaticStruct()->GetName();
        }

        FString ConfigSection = TEXT("ConnectSettings");
        FString CustomIniPath = FPaths::ProjectSavedDir() + "WordBee/Settings.ini";

        if (!FPaths::FileExists(CustomIniPath)) return;

        // Create a new FConfigFile to read from the custom INI file
        FConfigFile ConfigFile;

        // Read the config file from disk
        ConfigFile.Read(CustomIniPath);

        FString StrJsonObject;
        FJsonObjectConverter::UStructToJsonObjectString(Object, StrJsonObject, 0, 0, 0, nullptr, true);

        ConfigFile.SetString(*ConfigSection, *key, *StrJsonObject);
        ConfigFile.Write(CustomIniPath);
    }
};

template <typename T>
class Locate {
private:
    static std::unique_ptr<T> instance;  

public:
    static T* Get() {
        return instance.get();
    }

    static void Set(T* newInstance) {
        instance.reset(newInstance); 
    }

    // Xóa instance
    static void Clear() {
        instance.reset();
    }
};

template <typename T>
std::unique_ptr<T> Locate<T>::instance;
