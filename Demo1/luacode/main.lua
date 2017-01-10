function Shutdown()
end
function Init()
    package.path = package.path .. ";".._luadir.."\\?.lua"
    require "structutil"
    require "logutil"
    require "luaclass"
    require "globalevent"
    require "inputmgr"
    require "timermgr"
    require "actormgr"
    local function ShowMem()
        collectgarbage("collect")
        A_("lua memory: ", collectgarbage("count"))
    end
    InitLuahotupdate()
    TimerMgr:Get():On(ShowMem):Time(5000):Fire()
end

function InitLuahotupdate()
    if hasInitHoupdate then
        return
    end
    hasInitHoupdate = true
    local HU = require "luahotupdate"
    HU.Init("hotupdatelist", {_luadir}, A_)
    TimerMgr:Get():On(HU.Update):Time(1000)
end

function Tick(delta)
    TimerMgr:Get():Tick(delta)
end

function CtorCpp(inscpp, classpath, ...)
    local function f()
        ActorMgr:Get():CtorCpp(inscpp, classpath)
    end
    Xpcall(f)
end

function NewActor(inscpp, classpath, ...)
    local function f()
        ActorMgr:Get():BindActor(inscpp, classpath)
    end
    Xpcall(f)
end

function CppCallBack(classpath, functionName, inscpp, ...)
    ActorMgr:Get():CallLuaInsFunc(inscpp, classpath, functionName, ...)
end