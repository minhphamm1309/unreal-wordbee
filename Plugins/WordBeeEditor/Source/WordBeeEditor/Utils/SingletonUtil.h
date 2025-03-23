#pragma once

#include "CoreMinimal.h"

class SingletonUtil
{
public:
    template <typename T>
    static T* GetOrCreateAsset(const FString& AssetPath);
};