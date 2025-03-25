#pragma once

#include "CoreMinimal.h"
#include "WordBeeEditor/Command/CreateDataAsset/UserData.h"

DECLARE_DELEGATE_TwoParams(FOnRequestExportDocumentComplete, bool, const FString&);

class URequestExportDocumentCommand 
{
public:
	static void Execute(const UUserData* InUserInfo, const FString InDocumentId,FOnRequestExportDocumentComplete callBack);
private :
};

