// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class LuaPluginEditor : ModuleRules
{
	public LuaPluginEditor(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateIncludePaths.Add("LuaPluginEditor/Private");
        PublicIncludePaths.Add("LuaPluginEditor/Public");
        PrivateDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"CoreUObject",
                "UnrealEd", // for FAssetEditorManager
				"LuaPluginRuntime",
                "Projects",
                "InputCore",
                "LevelEditor",
                "Engine",
                "Slate",
                "SlateCore",
                "BlueprintGraph",
            }
			);
	}
}
