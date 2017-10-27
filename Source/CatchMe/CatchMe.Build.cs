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
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "AIModule", "RHI", "RenderCore","Foliage","LuaPluginRuntime" });
        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore", "UMG", "VarWatcher"});

        PublicIncludePaths.Add(Path.Combine(LuaLibPath, "Include"));

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicAdditionalLibraries.Add(Path.Combine(LuaLibPath, "Lib", "Win64", "Release", "lua.lib"));
        }
        else if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            PublicAdditionalLibraries.Add(Path.Combine(LuaLibPath, "Lib", "Mac", "Release", "lua.a"));
        }
        else if(Target.Platform == UnrealTargetPlatform.Android)
        {
            PublicAdditionalLibraries.Add(Path.Combine(LuaLibPath, "Lib", "Android", "Release", "lua.a"));
        }
        else if(Target.Platform == UnrealTargetPlatform.IOS)
        {
            PublicAdditionalLibraries.Add(Path.Combine(LuaLibPath, "Lib", "Ios", "Release", "lua.a"));
        }
    }
}
