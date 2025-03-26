#pragma once

#include "CoreMinimal.h"
#include "EditorAssetLibrary.h"

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
};
