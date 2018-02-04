
# unreal.lua
UE4 is not good enough.
    
Lua make UE4 perfect, hahaha~~
    

## Platform & Version

Windows, Mac, Android, IOS

4.16, 4.17, 4.18

## How to build
for this repository:

    Right click CatchMe.uproject then Generate project.

    If you use launcher engine and is under 4.18, try CatchMe_for4.17launcherengine.uproject.

for your project:
    
    1.copy LuaSource and Plugins to your project.
    
    2.modify luaconfig.ini, Replace all "CatchMe" with your GameModule's name."CatchMe" belong to me,haha~
    
    3.modify LuaPluginRuntime.build.cs, Replace "CatchMe" with your GameModule's name.
    
    4.modify LuaCircularModules.build.cs, Replace "CatchMe" with your GameModule's name.
    
    5.If you want to use Lua in c++, modify your project.build.cs, add "LuaPluginRuntime" to your PublicDependencyModuleNames.
### Attention
    You will probably meet link error for the first time compile,No need to panic.
    
    Just make a little change to you project.build.cs, such as an empty space.Then complie again.
    
    If error still exist,see FAQ below.
    
## Feature

    1.Full Aceess Engine API, include blueprint.

    2.Lua Hot Reload, [luahotupdate](https://github.com/asqbtcupid/lua_hotupdate)
    
    3.Variable Watcher
    
    4.Debugger
    
## Documentation

[wiki](https://github.com/asqbtcupid/unreal.lua/wiki)

## FAQ
todo
## Finally
Contact:15244611841@163.com

support:ETH 0x3b678916bc9d9f9a18717722e90b3afab70c344b
