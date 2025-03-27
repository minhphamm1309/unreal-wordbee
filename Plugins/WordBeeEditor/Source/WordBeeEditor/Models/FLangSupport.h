#pragma once

#include "CoreMinimal.h"
#include "WordBeeEditor/Utils/ELanguage.h"
#include "FLangSupport.generated.h"

// Unreal Engine version of the C# LangSupport class
USTRUCT(BlueprintType)
struct FLangSupport
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "LangSupport")
	ELanguage languagesSuport;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "LangSupport")
	FString name;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "LangSupport")
	UFont* font;
};
