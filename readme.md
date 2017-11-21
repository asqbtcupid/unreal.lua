
## Lua 5.1 Plugin
This can be used to generate a lua binding for UE4 Engine, allowing you to write game logic in Lua.

## Supported Platforms
Windows, Mac, Android, IOS
(Android tested on Galaxy S7 Edge, and IOS tested on an iPhone 6)

## How to Build
This plugin can be used in UE4 4.17 version. You must use a copy of the engine compiled from source, otherwise you will have to build the plugin yourself.

Right click CatchMe.uproject then Generate visual studio project.

## Communication between C++ and Lua:  

* There are two ways to call a Lua function:

  without a return value,

  ```
  UTableUtil::call(function_name, parameter1, parameter2, ...);
  ```

  and with return value.

  ```
  UTableUtil::callr<returntype>(function_name, parameter1, parameter2, ...);
  ```

* Calling C++ in Lua is similar to how it works for Blueprints. You can call C++ class functions with UFUNCTION(), and you can get/set C++ member data which with UPROPERTY().

## Communication between Blueprint and Lua:
* You can not communicate between Blueprint and Lua.

## Exporting Classes, Structs, and Enums to Lua

* Config: In the Config/luaconfig.ini, "SupportedModules" means the UCLASS of this module is exported to Lua, but for now I have only tested CoreUobject, Engine, SlateCore, Slate, UMG."SupportedStruct" means the USTRUCT exported to Lua. Please ignore "NoPropertyStruct" for now. All UENUM are exported.

* If you want to export your class, struct, or enum you can add "meta=(Lua=1)" to the macro, such as UCLASS(meta=(Lua=1)).

## How to know what function or property of class are exported?
The lua binding code is generated in the base folder on your visual studio project config. If you use the development editor config please check your project_name/Intermediate/Build/Win64/UE4Editor/Inc/project_name folder. Each class or struct has their corresponding file. Enums are in allEnum.script.h.

## What other type are suppoted?

* TArray and C array are supported. TSet and TMap are not supported.

* TWeakObjectPtr is supported. There is some example code in that branch.

* MulticastDelegate is supported.

* UInterface is supported in branch demo_firstperson_umg. I will merge this branch to master at a future date.

## Features

* You can reload the Lua logic at runtime. Check out my other repository for hot reload:[luahotupdate](https://github.com/asqbtcupid/lua_hotupdate).

* When Lua calls a C++ function which has default arguments you dont't have to pass all arguments. For example: in C++ you've declared
```
_void example(int i, int j =1, int k = 2)_
```
When calling this function in Lua you can pass only one argument.
```
example(0)
```

* When Lua calls a C++ function that has reference perameters such as:
```
void example(int &i, FVector &v)
```
You can get the reference values from the call like this:
```
i, v = example(arg1, arg2)
```

## FAQ
* "No filename provided for module LuaglueGenerator" during compile.

 Use a build of UE4 built from source, or try to package the Plugins/LuaPlugin manually. You can find another solution here: https://forums.unrealengine.com/showthread.php?135440-Lua-5-1-codegenerator-for-UE4

* Lots of compile errors in DelegateLuaProxy.h

 In order to get UHT running again make a tiny modification in any .h file in your project, and then compile the project again. The error will disappear.

* Linking error when adding modules to luaconfig.ini

 Add module name to PublicDependencyModuleNames or PrivateDependencyModuleNames to project.build.cs. If the error still occurs then config luaconfig.ini to stop exporting the relevant class or function.
 
## Finally
* There is some lua code in LuaSource. We use luaclass.lua to imitate C++ style object orientation. TimerMgr.lua provides a timer. You do not need to use these, and feel free to implement them yourself.

* All my work is based on the official plugin ScriptPlugin https://forums.unrealengine.com/showthread.php?3958-Scripting-Language-extensions-via-plugins

* If you have any problem you can make a post in https://forums.unrealengine.com/showthread.php?135440-Lua-5-1-codegenerator-for-UE4, or submit an issue.

* This is an experimental project, and if you have any good advice or ideas you can contact me here: 15244611841@163.com
