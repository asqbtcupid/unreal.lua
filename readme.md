
## Lua5.1 Plugin
This can be used to generate lua binding code for UE4 Engine.That means you can write game logic by lua.
## How to build
This plugin can be used in UE4 4.16 version.

It's a demo,so right click CatchMe.uproject then Generate visual studio project.

## Communication between C++ and Lua:  

1.call lua function in c++, There are Two way, In both method, No matter how many parameters:

  without return value:

  ```
  UTableUtil::call(function_name, parameter1, parameter2, ...);
  ```

  with return value:

  ```
  UTableUtil::callr<returntype>(function_name, parameter1, parameter2, ...);
```

2.call c++ in lua is similar with Blueprint.You can call c++ class function which with UFUNCTION() and You can get or set c++ member data which with UPROPERTY().

## Communication between Blueprint and Lua:
1.Lua can't not comunicate with Blueprint derectly,I think it's no meaning and slow.

## Export Class, Struct, Enum to Lua

1.config:In the Config/luaconfig.ini, "SupportedModules" mean The UCLASS of this module are exported to Lua, But for Now I only test CoreUobject, Engine, SlateCore, Slate, UMG."SupportedStruct" mean the USTRUCT exported to Lua.Please ignore "NoPropertyStruct" for now.All UENUM are exported.

2.if you want to export your class or struct or enum,you can add "meta=(Lua=1)" to the macro, such as UCLASS(meta=(Lua=1)).
## How to know what function or property of class are exported?
The lua binding code is generate to the folder base on your visual studio project config.If you use Development Editor config,please check your project_name/Intermediate/Build/Win64/UE4Editor/Inc/project_name folder.each class or struct has their corresponding file.Enum are in allEnum.script.h.

## What other type are suppoted?

1.TArray and C array is supported.TSet and TMap is not supported.

2.TWeakObjectPtr is supported,There are some example code in branch 

3.MulticastDelegate is supported.

4.UInterface is supported in branch demo_firstperson_umg.I will merge to master later.

## Featrue

1.Lua Hot Reload, You can change the lua logic during game running.This is my another repositery about lua hot reload:[luahotupdate](https://github.com/asqbtcupid/lua_hotupdate).

2.When lua call c++ function which contains default argument.you dont't have to pass all arguments.For an example,In c++,you declare _void example(int i, int j =1, int k = 2)_.When you call this function in lua,you can just pass one argument:example(0).

3.When lua call c++ function which contains reference parameter, such as _void example(int &i, FVector &v)_,lua can get the reference value after call.In this case, you can write lua code:local i, v = example(argument1, argument2).
## Finally
1.There are some lua code in LuaSource such as luaclass.lua, It imitate the c++ object-oriented.And TimerMgr.lua, It works like a timer.But all of these are not necessary, you can implement better one.  

2.All my work are base on the official plugin ScriptPlugin.https://forums.unrealengine.com/showthread.php?3958-Scripting-Language-extensions-via-plugins

3.If you have any problem,you can leave message in https://forums.unrealengine.com/showthread.php?135440-Lua-5-1-codegenerator-for-UE4.

4.It's an experimental project,If you has any good idea or advise, welcome to fallback.Contact:15244611841@163.com
