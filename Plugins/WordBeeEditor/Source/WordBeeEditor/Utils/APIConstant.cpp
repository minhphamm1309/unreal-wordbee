#include "APIConstant.h"

const FString APIConstant::AuthToken = TEXT("X-Auth-Token");
const FString APIConstant::AuthAccountID = TEXT("X-Auth-AccountId");

const FString APIConstant::Auth = TEXT("api/auth/token");
const FString APIConstant::Document = TEXT("api/apps/wbflex/documents/{0}"); // {0} document ID
const FString APIConstant::DocumentFields = TEXT("api/apps/wbflex/documents/{0}/fields"); // {0} document ID
const FString APIConstant::DocumentsList = TEXT("api/apps/wbflex/list");
const FString APIConstant::RequestExportDocument = TEXT("api/apps/wbflex/documents/{0}/contents/pull"); // {0} document ID
const FString APIConstant::PushMetaData = TEXT("api/resources/segments/view/actions/batch");
const FString APIConstant::PollingDocument = TEXT("api/trm/status?requestid={0}"); // {0} request ID
const FString APIConstant::DownloadDocument = TEXT("api/media/get/{0}"); // {0} fileToken
const FString APIConstant::UpdateRecord = TEXT("api/apps/wbflex/documents/{0}/contents/push"); // {0} document ID
