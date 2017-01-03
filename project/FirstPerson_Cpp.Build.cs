// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
using System.IO;
using UnrealBuildTool;

public class FirstPerson_Cpp : ModuleRules
{
    private string ModulePath
    {
        get { return ModuleDirectory; }
    }
    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../../Lua/")); }
    }
    public FirstPerson_Cpp(TargetInfo Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
        PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "Include"));
        PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "Lib", "Win64", "Release","lua.lib"));
    }
}
