#pragma once

#include "CoreMinimal.h"
#include "WordBeeEditor/Utils/UserInfo.h"

DECLARE_DELEGATE_TwoParams(FOnRequestExportDocumentComplete, bool, const FString&);

class URequestExportDocumentCommand 
{
public:
	static void Execute(const FUserInfo InUserInfo, const FString InDocumentId,FOnRequestExportDocumentComplete callBack);
private :
};

