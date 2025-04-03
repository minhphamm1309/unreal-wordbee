#include "StoredLocailzationCommand.h"

#include "Internationalization/Internationalization.h"
#include "Internationalization/TextLocalizationManager.h"
#include "Misc/ConfigCacheIni.h"
#include "LocalizationSettings.h"
#include "GeometryCollection/Facades/CollectionPositionTargetFacade.h"
#include "HAL/PlatformFilemanager.h"

void StoredLocailzationCommand::Execute( const TArray<FSegment>& Segments)
{
	// Get Locate codes
	TArray<FString> languagesCodeUnique;
	for (auto segment : Segments)
	{
		for (auto localizeSegment : segment.texts)
		{
			languagesCodeUnique.AddUnique(localizeSegment.Key);
		}
	}

	// init data
	TArray<FLocalizationNamespaceM> Namespaces;
    
	FLocalizationNamespaceM StringTableNamespace;
	StringTableNamespace.Namespace = "Wordbee";
	
	TMap<FString, TArray<FLocalizeSegment>> LocalizeSegments;
	for (const FSegment& Segment : Segments)
	{
		FSegmentText sourceEn = Segment.texts.FindChecked("en");
		FString sourceText = sourceEn.v;
		StringTableNamespace.Children.Add(ParseToManifest(Segment.key, sourceText));
		
		for (const auto& LocalizeSegment : Segment.texts)
		{
			FLocalizeSegment LocalizeSegmentData;
			LocalizeSegmentData.Key = Segment.key;
			// Set the source text from "en"
			LocalizeSegmentData.SourceText = sourceText;
			LocalizeSegmentData.TranslationText = LocalizeSegment.Value.v;
			LocalizeSegments.FindOrAdd(LocalizeSegment.Key).Add(LocalizeSegmentData);
		}
	}
	Namespaces.Add(StringTableNamespace);

	CreateNewLocalizationTarget(TEXT("Wordbee"));

	// Create the localization manifest
	GenerateLocalizationManifest(Namespaces);
	
	// Create the localization settings
	for (const auto& LanguageCode : languagesCodeUnique)
	{
		AddCultureIfNotSupported(LanguageCode);
		TArray<FLocalizeSegment> LocalizeSegmentsData = LocalizeSegments.FindChecked(LanguageCode);
		AddLocalizationEntry(LocalizeSegmentsData, LanguageCode);
	}
	
}

bool StoredLocailzationCommand::AddCultureIfNotSupported(const FString& CultureCode)
{
    // Get the internationalization subsystem
    FInternationalization& I18N = FInternationalization::Get();
    
    // Verify the culture exists
    TArray<FString> AvailableCultureNames;
    I18N.GetCultureNames(AvailableCultureNames);
    
    if (!AvailableCultureNames.Contains(CultureCode))
    {
        UE_LOG(LogTemp, Warning, TEXT("Culture code %s is not available"), *CultureCode);
        return false;
    }

	// 2. Get the localization settings
	ULocalizationSettings* const LocalizationSettings = GetMutableDefault<ULocalizationSettings>();
	if (!LocalizationSettings)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to access localization settings"));
		return false;
	}

	// 3. Add to engine configuration (corrected FConfigValue handling)
	FConfigFile* ConfigFile = GConfig->FindConfigFile(GEditorIni);
	if (ConfigFile)
	{
		FConfigSection* Section = ConfigFile->FindOrAddSection(TEXT("Internationalization"));
		FConfigValue* SupportedCulturesValue = Section->Find(TEXT("LocalizationPaths"));

		if (SupportedCulturesValue)
		{
			FString SupportedCulturesString = SupportedCulturesValue->GetValue();
			TArray<FString> CulturesList;
			SupportedCulturesString.ParseIntoArray(CulturesList, TEXT(";"), true);

			if (!CulturesList.Contains(CultureCode))
			{
				// Modify the FConfigValue directly
				*SupportedCulturesValue = FConfigValue(SupportedCulturesString + TEXT(";") + CultureCode);
				ConfigFile->Dirty = true;
				ConfigFile->Write(GEngineIni);
			}
		}
		else
		{
			// If the key doesn't exist, add it
			Section->Add(TEXT("LocalizationPaths"), CultureCode);
			ConfigFile->Dirty = true;
			ConfigFile->Write(GEngineIni);
		}
	}

	// 4. Add to active cultures
	// TArray<FString> CurrentActiveCultures;
	// I18N.GetCurrentCulture()->GetCul(CurrentActiveCultures);
 //    
	// if (!CurrentActiveCultures.Contains(CultureCode))
	// {
	// 	CurrentActiveCultures.Add(CultureCode);
	// 	I18N.SetCurrentCulture(CurrentActiveCultures);
	// }

	// 5. Add to all localization targets (using correct UE5 method)
	bool bModifiedAnyTarget = false;
    
	// Get the target set from settings
	ULocalizationTargetSet* EngineTargetSet = LocalizationSettings->GetEngineTargetSet();
	ULocalizationTargetSet* GameTargetSet = LocalizationSettings->GetGameTargetSet();

	// Helper lambda to add culture to a target set
	auto AddCultureToTargetSet = [&](ULocalizationTargetSet* TargetSet)
	{
		if (TargetSet)
		{
			for (ULocalizationTarget* Target : TargetSet->TargetObjects)
			{
				FCultureStatistics CultureStatistics;
				CultureStatistics.CultureName = CultureCode;
				if (Target && !Target->Settings.SupportedCulturesStatistics.ContainsByPredicate([&](const FCultureStatistics& ExistingCulture)
				{
					return ExistingCulture.CultureName == CultureCode;
				}))
				{
					Target->Settings.SupportedCulturesStatistics.Add(CultureStatistics);
					Target->MarkPackageDirty();
					bModifiedAnyTarget = true;
				}
			}
		}
	};

	// Process both engine and game target sets
	AddCultureToTargetSet(EngineTargetSet);
	AddCultureToTargetSet(GameTargetSet);
	
	FTextLocalizationManager::Get().RefreshResources();
	//FLocalizationConfigurationScript::UpdateConfigFilesFromSettings();
	//FLocalizationConfigurationScript::Gen();
	return true;
}

void StoredLocailzationCommand::AddCultureToSupportedList(FString& InputString, const FString& NewCulture)
{
	FString StartPattern = TEXT("SupportedCulturesStatistics=(");
	FString EndPattern = TEXT("))");

	int32 StartIndex = InputString.Find(StartPattern) + StartPattern.Len();
	int32 EndIndex = InputString.Find(EndPattern, ESearchCase::IgnoreCase, ESearchDir::FromStart, StartIndex);

	if (StartIndex != INDEX_NONE && EndIndex != INDEX_NONE)
	{
		FString CultureSection = InputString.Mid(StartIndex, EndIndex - StartIndex + 1);

		if (!CultureSection.Contains(FString::Printf(TEXT("CultureName=\"%s\""), *NewCulture)))
		{
			if (CultureSection.IsEmpty())
			{
				CultureSection = FString::Printf(TEXT("(CultureName=\"%s\")"), *NewCulture);
			}
			else
			{
				CultureSection += FString::Printf(TEXT(",(CultureName=\"%s\")"), *NewCulture);
			}

			InputString = InputString.Left(StartIndex) + CultureSection + InputString.RightChop(EndIndex + 1);
		}
	}
}

void StoredLocailzationCommand::AddLocalizationEntryIni(const FString& CultureCode)
{
	FString ConfigSection = TEXT("/Script/Localization.LocalizationSettings");
	// Create a new FConfigFile to read from the custom INI file
	FConfigFile ConfigFile;
	const FString defaultEditorIni = FPaths::ProjectConfigDir() / TEXT("DefaultEditor.ini");
	// Read the config file from disk
	ConfigFile.Read(defaultEditorIni);

	FString StrJsonObject;
	TArray<FString> settings;
	FString key = TEXT("+GameTargetsSettings");
	
	if (ConfigFile.GetString(*ConfigSection, *key, StrJsonObject))
	{
		AddCultureToSupportedList(StrJsonObject , CultureCode);
		ConfigFile.SetString(*ConfigSection, *key, *StrJsonObject);
		ConfigFile.Write(defaultEditorIni);
		UE_LOG(LogTemp, Warning, TEXT("Add %s Done"), *CultureCode);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Fail %s"), *defaultEditorIni);
	}
}

void StoredLocailzationCommand::AddLocalizationEntry(const TArray<FLocalizeSegment>& LocalizeSegments, FString CultureCode)
{
	FString OutputPath = FPaths::ProjectContentDir() / TEXT("Localization")/ TEXT("Wordbee") / CultureCode;
	// make sure the directory exists
	if (!FPaths::DirectoryExists(OutputPath))
	{
		FPlatformFileManager::Get().GetPlatformFile().CreateDirectory(*OutputPath);
	}

	AddLocalizationEntryIni(CultureCode);
	
	TSharedPtr<FJsonObject> RootObject = MakeShareable(new FJsonObject);
    RootObject->SetNumberField("FormatVersion", 2);
    RootObject->SetStringField("Namespace", "");
    
    TArray<TSharedPtr<FJsonValue>> Subnamespaces;
    
    {
        TSharedPtr<FJsonObject> NamespaceObj = MakeShareable(new FJsonObject);
        NamespaceObj->SetStringField("Namespace", "Wordbee");
        
        TArray<TSharedPtr<FJsonValue>> Children;
        
        for (const FLocalizeSegment& Entry : LocalizeSegments)
        {
            TSharedPtr<FJsonObject> EntryObj = MakeShareable(new FJsonObject);
            
            // Source
            TSharedPtr<FJsonObject> SourceObj = MakeShareable(new FJsonObject);
            SourceObj->SetStringField("Text", Entry.SourceText);
            EntryObj->SetObjectField("Source", SourceObj);
            
            // Translation (empty)
            TSharedPtr<FJsonObject> TranslationObj = MakeShareable(new FJsonObject);
            TranslationObj->SetStringField("Text", Entry.TranslationText);
            EntryObj->SetObjectField("Translation", TranslationObj);
            
            // Key
            EntryObj->SetStringField("Key", Entry.Key);
            
            Children.Add(MakeShareable(new FJsonValueObject(EntryObj)));
        }
        
        NamespaceObj->SetArrayField("Children", Children);
        Subnamespaces.Add(MakeShareable(new FJsonValueObject(NamespaceObj)));
    }
    
    RootObject->SetArrayField("Subnamespaces", Subnamespaces);
    
    // Write to file
    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(RootObject.ToSharedRef(), Writer);
	
    FString OutputFilePath = OutputPath / TEXT("Wordbee.archive");
    FFileHelper::SaveStringToFile(OutputString, *OutputFilePath);
}

FLocalizationTextEntryM StoredLocailzationCommand::ParseToManifest(const FString& Key, const FString& sourceText)
{
	// Add Settings entry
	FLocalizationTextEntryM SettingsEntry;
	SettingsEntry.Text = sourceText;
	FLocalizationKeyEntryM SettingsKey;
	SettingsKey.Key = Key;
	SettingsKey.Path = "";
	SettingsEntry.Keys.Add(SettingsKey);
	return SettingsEntry;
}

void StoredLocailzationCommand::GenerateLocalizationManifest(TArray<FLocalizationNamespaceM> Namespaces)
{
	FString OutputPath = FPaths::ProjectContentDir() / TEXT("Localization")/ TEXT("Wordbee");
	// make sure the directory exists
	if (!FPaths::DirectoryExists(OutputPath))
	{
		FPlatformFileManager::Get().GetPlatformFile().CreateDirectory(*OutputPath);
	}
	
	// Create root object
	TSharedPtr<FJsonObject> RootObject = MakeShareable(new FJsonObject);
	RootObject->SetNumberField("FormatVersion", 1);
	RootObject->SetStringField("Namespace", "");
    
	// Add subnamespaces
	TArray<TSharedPtr<FJsonValue>> SubnamespaceValues;
	for (const FLocalizationNamespaceM& Namespace : Namespaces)
	{
		SubnamespaceValues.Add(MakeShareable(new FJsonValueObject(Namespace.ToJson())));
	}
	RootObject->SetArrayField("Subnamespaces", SubnamespaceValues);
    
	// Write to file
	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(RootObject.ToSharedRef(), Writer);
	
	FString OutputFilePath = OutputPath / TEXT("Wordbee.manifest");
	FFileHelper::SaveStringToFile(OutputString, *OutputFilePath);

	FString OutputFileConflictPath = OutputPath / TEXT("Wordbee_Conflicts.txt");
	FFileHelper::SaveStringToFile(TEXT(""), *OutputFileConflictPath);
}

void StoredLocailzationCommand::CreateNewLocalizationTarget(const FString& Key)
{
	
}





