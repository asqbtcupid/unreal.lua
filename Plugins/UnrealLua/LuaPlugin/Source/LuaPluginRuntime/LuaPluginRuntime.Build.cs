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
        PublicIncludePaths.Add("LuaPluginRuntime/Public");
        PrivateIncludePaths.Add("LuaPluginRuntime/Private");
            
        PublicDependencyModuleNames.AddRange(new string[] {  "Core", "CoreUObject", "Engine"});
        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore", "UMG", "AssetRegistry" });

//         PublicDefinitions.Add("LuaDebug");
        PublicDefinitions.Add("STRONG_CHECK_GC_REF");
        
        if (Target.Type == TargetType.Editor)
        {
            PrivateDependencyModuleNames.AddRange(new string[] { "VarWatcher", "UnrealEd", "Slate" });
        } 
        PublicIncludePaths.Add(Path.Combine(LuaLibPath, "Include"));

        // use 5.3 or 5.1
        bool IsUseLua53 = true;

        if (IsUseLua53)
        {
            PublicDependencyModuleNames.Add("Lua53");
            PublicDefinitions.Add("USE_LUA53");
            if (Target.Platform == UnrealTargetPlatform.Win64)
            {
                PublicDefinitions.Add("LUA_BUILD_AS_DLL"); 
//                 Definitions.Add("USE_LUASOURCE");
            }
        }
        else
        {
            if (Target.Platform == UnrealTargetPlatform.Win64)
            {
//                 Definitions.Add("USE_LUASOURCE");
                PublicAdditionalLibraries.Add(Path.Combine(LuaLibPath, "Lib", "Win64", "Debug", "lua.lib"));
//                 PublicAdditionalLibraries.Add(Path.Combine(LuaLibPath, "Lib", "Win64", "Release", "lua.lib"));
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

}
