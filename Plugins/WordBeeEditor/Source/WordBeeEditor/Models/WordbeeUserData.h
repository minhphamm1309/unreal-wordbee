#pragma once

#include "CoreMinimal.h"
#include "WordBeeEditor/Utils/ELanguage.h"
#include "WordBeeEditor/Utils/SingletonUtil.h"
#include "WordbeeUserData.generated.h"

USTRUCT(Blueprintable)
struct FWordbeeUserData //: public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UserData")
	ELanguage MainLangEditor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UserData")
	FString Url;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UserData")
	FString AccountId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UserData")
	FString ApiKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UserData")
	FString AuthToken;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UserData")
	FString ProjectId;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UserData")
	int32 DocumentId;
	bool IsValid() const
	{
		return !Url.IsEmpty()
			&& !AccountId.IsEmpty()
			&& !ApiKey.IsEmpty()
			&& !AuthToken.IsEmpty()
			&& DocumentId != 0; // Assuming 0 is invalid
	}
};


