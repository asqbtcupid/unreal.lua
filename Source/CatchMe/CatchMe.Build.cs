// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class CatchMe : ModuleRules
{
    private string LuaLibPath
    {
        get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "../../Lua/")); }
    }
    public CatchMe(ReadOnlyTargetRules Target) : base(Target)
    {
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "AIModule", "RHI", "RenderCore","Foliage" });
        PrivateDependencyModuleNames.AddRange(new string[] {"Slate", "SlateCore", "UMG" });
        PublicIncludePaths.Add(Path.Combine(LuaLibPath, "Include"));
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicAdditionalLibraries.Add(Path.Combine(LuaLibPath, "Lib", "Win64", "Release", "lua.lib"));
        }
        else if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            PublicAdditionalLibraries.Add(Path.Combine(LuaLibPath, "Lib", "Mac", "Release", "lua.a"));
        }
    }
}
