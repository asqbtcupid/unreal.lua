function Shutdown()
end
function Init()
    package.path = package.path .. ";".._luadir.."\\?.lua"
    require "structutil"
    require "logutil"
    require "luaclass"
    require "globalevent"
    require "timermgr"
    require "actormgr"
    require "baseutil"
    local function ShowMem()
        collectgarbage("collect")
        A_("lua memory: ", collectgarbage("count"))
    end
    InitLuahotupdate()
    -- TimerMgr:Get():On(ShowMem):Time(1000)
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
    local function f()
        TimerMgr:Get():Tick(delta)
    end
    Xpcall(f)
end

function CtorCpp(classpath, inscpp, ...)
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
    return ActorMgr:Get():CallLuaInsFunc(inscpp, classpath, functionName, ...)
end
