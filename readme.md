
## Lua5.1 Plugin
This can be used to generate lua binding code of UE4 Engine.That means you can write game logic by lua.
## How to build
Checkout branch demo_firstperson or demo_shootergame to see how to implement.

basically:
1. copy all the file and folder of this repository to your project folder.
2. build lua lib manually.
3. add lua include path and libarary path to youproject.build.cs
4. copy Source/TableUtil.h, TableUtil.cpp, Luautils.h, Luautils.cpp to your cpp source folder
5. Edit TableUtil.cpp and Luautils.cpp to include your project header file.

## How to call lua in c++ and how to call c++ in lua?
Checkout branch demo_firstperson or demo_shootergame, there are some example.

1.call lua in c++:

without return value:

```
UTableUtil::call(function_name, parameter1, parameter2, ...);
```

with return value:

```
UTableUtil::callr<returntype>(function_name, parameter1, parameter2, ...);
```










