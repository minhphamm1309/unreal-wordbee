#pragma once

#include "CoreMinimal.h"
#include "DocumentDataAsset.generated.h"

UCLASS(BlueprintType)
class WORDBEEEDITOR_API UDocumentDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Document")
	FString ProjectId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Document")
	FString ProjectName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Document")
	FString DocumentName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Document")
	FString DId;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Document")
	FString DSId;
};
