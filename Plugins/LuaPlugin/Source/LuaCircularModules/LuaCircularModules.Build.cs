// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;
 
public class LuaCircularModules : ModuleRules
{          
	 
	public LuaCircularModules(ReadOnlyTargetRules Target) : base(Target)
	{    
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs; 
        bFasterWithoutUnity = true;
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "AIModule", "Slate", "SlateCore", "UMG", "AssetRegistry","LuaPluginRuntime"});

        PublicDependencyModuleNames.AddRange(new string[] {"CatchMe"});

        if (Target.Type == TargetType.Editor)
        {
            PublicDependencyModuleNames.AddRange(new string[] { "LuaDebugger" });
        }
    }

}
