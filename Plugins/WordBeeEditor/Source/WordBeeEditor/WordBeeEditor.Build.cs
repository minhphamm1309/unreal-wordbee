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
				"AITestSuite", 
				"EditorScriptingUtilities",
				"AssetTools",
				"KismetCompiler"

				// ... add private dependencies that you statically link with here ...	
			}
			);
		
	}
}
