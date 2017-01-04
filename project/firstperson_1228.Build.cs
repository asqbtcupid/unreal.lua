// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
using System.IO;

using UnrealBuildTool;

public class firstperson_1228 : ModuleRules
{
	 private string ModulePath
    {
        get { return ModuleDirectory; }
    }
    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../../Lua/")); }
    }
	public firstperson_1228(TargetInfo Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
		if (UEBuildConfiguration.bBuildEditor == true)
			{

				PublicDependencyModuleNames.AddRange(
					new string[] 
					{
						"UnrealEd", 
					}
				);

			}
		PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "Include"));
        PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "Lib", "Win64", "Release","lua.lib"));
	}
}
