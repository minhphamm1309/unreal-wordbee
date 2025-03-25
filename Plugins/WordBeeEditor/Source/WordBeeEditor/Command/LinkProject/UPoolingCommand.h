#pragma once
#include "WordBeeEditor/Utils/UserInfo.h"

DECLARE_DELEGATE_TwoParams(FOnPoolingComplete, bool, const FString&);
DECLARE_DELEGATE_OneParam(FOnPoolingFail, const FString&);
class UPoolingCommand
{
public:
	static void Execute(const FUserInfo UserInfo, const int32 RequestId, FOnPoolingComplete callback , FOnPoolingFail failCallback);

private:
	static void DownloadDocument(const FString& FileToken, FOnPoolingComplete callback, FOnPoolingFail failCallback);
	static void Pooling(const int32 RequestId, FOnPoolingComplete callback, FOnPoolingFail failCallback);
};

