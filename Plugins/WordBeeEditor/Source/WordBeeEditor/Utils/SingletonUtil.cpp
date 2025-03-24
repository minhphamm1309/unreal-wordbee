#include "SingletonUtil.h"
#include "Engine/DataAsset.h"
#include "EditorAssetLibrary.h"
#include "WordBeeEditor/Models/UserData.h"
template <typename T>
T* SingletonUtil::GetOrCreateAsset(const FString& AssetPath)
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

// Explicit template instantiation to avoid linker errors
template UserData* SingletonUtil::GetOrCreateAsset<UserData>(const FString&);
