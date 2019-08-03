// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Lua53 : ModuleRules
{
	public Lua53(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.NoSharedPCHs;

        //         Type = ModuleType.External;
        bEnableUndefinedIdentifierWarnings = false;
        PublicIncludePaths.AddRange(
			new string[] {
				"Lua53/Public"
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"Lua53/Private",
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"Projects"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicDefinitions.Add("LUA_BUILD_AS_DLL");
            PublicDefinitions.Add("LUA_LIB");
        }
	}
}
