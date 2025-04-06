#pragma once

class DocumentService
{
public:
	static void PullDocument(TSharedPtr<TArray<FString>> SelectedLanguages=nullptr);
private:
	static void StoreData(TSharedPtr<TArray<FString>> SelectedLanguages=nullptr);
};
