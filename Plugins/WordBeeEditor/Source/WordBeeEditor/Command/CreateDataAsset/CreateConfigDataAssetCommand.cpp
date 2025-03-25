#include "CreateConfigDataAssetCommand.h"

#include "AssetToolsModule.h"
#include "ConfigurationData.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "ContentBrowserModule.h"
#include "EditorAssetLibrary.h"
#include "IContentBrowserSingleton.h"
#include "ObjectTools.h"
#include "Factories/Factory.h"
#include "Kismet2/KismetEditorUtilities.h"

void CreateConfigDataAssetCommand::CreateConfigDataAsset()
{
#if WITH_EDITOR
	FString Name = FString(TEXT("ConfigurationData"));
	FString PackagePath = FString(TEXT("/Game/WordBee")); 
	FString AssetName = FString::Printf(TEXT("%s/%s"), *PackagePath, *Name);
	UPackage* Package = CreatePackage(*AssetName);

	
	// Check if the asset already exists
	if (UEditorAssetLibrary::DoesAssetExist(AssetName))
	{
		UE_LOG(LogTemp, Warning, TEXT("Asset already exists at %s"), *AssetName);
		return;
	}

	UBlueprint* Blueprint = FKismetEditorUtilities::CreateBlueprint(
		UConfigurationData::StaticClass(), 
		Package,                            
		FName(*Name),                       
		EBlueprintType::BPTYPE_Normal,      
		UBlueprint::StaticClass(),          
		UBlueprintGeneratedClass::StaticClass() 
	);

	if (Blueprint)
	{
		FAssetRegistryModule::AssetCreated(Blueprint);
		Package->MarkPackageDirty();
		FString PackageFileName = FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension());
		bool bSuccess = UPackage::SavePackage(Package, Blueprint, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *PackageFileName);
        
		if (bSuccess)
		{
			UE_LOG(LogTemp, Log, TEXT("Blueprint created successfully at %s"), *PackageFileName);
			FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
			ContentBrowserModule.Get().SyncBrowserToAssets(TArray<FAssetData> { FAssetData(Blueprint) });
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to save Blueprint package"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create Blueprint"));
	}
#endif
}
