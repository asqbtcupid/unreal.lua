
# unreal.lua
UE4 is not good enough.
    
Lua make UE4 perfect, hahaha~~
    
## What is this?
  This is a framework for binding UE4 to lua.
## Platform & Version

Windows, Mac, Android, IOS

UE4 : 4.20 4.21
Lua : 5.3

## How to build
for this repository:

    Right click CatchMe.uproject then Generate project.

for your project:
    
    1.copy Plugins to your project.
    
    2.If you want to use Lua in c++, modify your modulesname.build.cs, add "InputCore","Lua53","LuaPluginRuntime","LuaManuallyGlue" to your PublicDependencyModuleNames.Take a look at CatchMe.build.cs.
  
  drag Plugins/UnrealLua/LuaSource folder to editor content to generate UE4 asset before package.

## Feature

   1.Full Aceess to blueprint API.

   2.[Lua Hot Reload](https://github.com/asqbtcupid/lua_hotupdate)
    
   3.[Variable Watcher](https://github.com/asqbtcupid/unreal.lua/wiki/var-watcher)
    
   4.[Debugger](https://github.com/asqbtcupid/unreal.lua/wiki/debugger)
  
   5.extremely easy for c++ binding. [binding](https://github.com/asqbtcupid/unreal.lua/wiki/Bindding),It's the most powerful lua binding framework in the world.

   6.write slate by lua(expertimental),[example](https://github.com/asqbtcupid/unreal.lua/wiki/slate_lua)

   7.extremely high performance.
## Documentation

[wiki](https://github.com/asqbtcupid/unreal.lua/wiki)

## FAQ
todo
## Finally
Contact:15244611841@163.com

support:ETH 0x3b678916bc9d9f9a18717722e90b3afab70c344b
