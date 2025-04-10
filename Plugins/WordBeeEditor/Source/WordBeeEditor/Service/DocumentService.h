#pragma once

class DocumentService
{
public:
	static void PullDocument(TSharedPtr<TArray<FString>> SelectedLanguages=nullptr, FString src="en");
	static void PushDocument(TSharedPtr<TArray<FString>> SelectedLanguages, bool IsChangesOnly=true);
private:
	static void StoreData(TSharedPtr<TArray<FString>> SelectedLanguages=nullptr, FString src="en");
	static TSharedPtr<SNotificationItem> ShowLoadingNotification(const FString& InitialText);
	static void UpdateNotificationText(TSharedPtr<SNotificationItem> Notification, const FString& NewText, bool bSuccess, bool bExpireAfter);
};
