// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;
 
public class CatchMe : ModuleRules
{
    public CatchMe(ReadOnlyTargetRules Target) : base(Target)
    { 
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
           
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "AIModule", "RHI", "RenderCore","Foliage","LuaPluginRuntime", "GameplayTasks" });
        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore", "UMG"});

        CircularlyReferencedDependentModules.Add("LuaPluginRuntime");
    }
}                 
                                                                                                                            