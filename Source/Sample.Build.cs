using UnrealBuildTool;
using System.IO;

public class FirstPerson : ModuleRules
{
    //add lualibpath
    private string LuaLibPath
    {
        get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "../../Lua/")); }
    }

    public FirstPerson(TargetInfo Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
        
        //add lualibpath
        PublicIncludePaths.Add(Path.Combine(LuaLibPath, "Include"));
        PublicAdditionalLibraries.Add(Path.Combine(LuaLibPath, "Lib", "Win64", "Release", "lua.lib"));
    }
}
