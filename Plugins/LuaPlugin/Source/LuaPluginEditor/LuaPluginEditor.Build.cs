// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class LuaPluginEditor : ModuleRules
{
	public LuaPluginEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateIncludePaths.Add("LuaPluginEditor/Private");

		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"CoreUObject",
                "UnrealEd", // for FAssetEditorManager
				"LuaPluginRuntime"
			}
			);
	}
}
