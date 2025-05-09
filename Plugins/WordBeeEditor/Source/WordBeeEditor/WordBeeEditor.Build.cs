// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class WordBeeEditor : ModuleRules
{
	public WordBeeEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				"UnrealEd", // Necessary for creating editor windows
				"Slate",
				"SlateCore",
				"EditorStyle",
				"LevelEditor",
				"ToolMenus",
				"Http", 
				"Json", 
				"JsonUtilities",
				"EditorScriptingUtilities",
				"JsonUtilities",
				"AITestSuite", 
				"AssetTools",
				"PropertyEditor",
				"KismetCompiler", 
				"DesktopWidgets", 
				"ToolWidgets",
				"Localization",
				"LocalizationCommandletExecution",
				"LocalizationService",
				"LocalizationDashboard", "SessionServices"
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
	}
}
