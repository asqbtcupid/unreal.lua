// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;
 
public class LuaPluginRuntime : ModuleRules
{          
	private string LuaLibPath
    {
        get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "../../Lua/")); }
    }
	public LuaPluginRuntime(ReadOnlyTargetRules Target) : base(Target)
	{    
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs; 
        bFasterWithoutUnity = true;
        PrivateIncludePaths.Add("LuaPluginRuntime/Private");
           
        PublicDependencyModuleNames.AddRange(new string[] {  "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "AIModule", "RHI", "RenderCore", "Foliage", "GameplayTasks" });
        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore", "UMG", "AssetRegistry" });
            
        PrivateDependencyModuleNames.AddRange(new string[] { "CatchMe" });
         
        Definitions.Add("LuaDebug");
        Definitions.Add("STRONG_CHECK_GC_REF");

        if (Target.Type == TargetType.Editor)
        {
            PrivateDependencyModuleNames.AddRange(new string[] { "VarWatcher", "UnrealEd" });
        }
        PublicIncludePaths.Add(Path.Combine(LuaLibPath, "Include"));
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            Definitions.Add("USE_LUASOURCE");
            PublicAdditionalLibraries.Add(Path.Combine(LuaLibPath, "Lib", "Win64", "Debug", "lua.lib"));
//             PublicAdditionalLibraries.Add(Path.Combine(LuaLibPath, "Lib", "Win64", "Release", "lua.lib"));
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
