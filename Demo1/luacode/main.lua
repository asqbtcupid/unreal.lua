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
    TimerMgr:Get():On(ShowMem):Time(5000):Fire()
end

function Tick(delta)
    TimerMgr:Get():Tick(delta)
end

function CtorCpp(inscpp, classpath, ...)
    ActorMgr:Get():CtorCpp(inscpp, classpath, ...)
end

function NewActor(inscpp, classpath, ...)
    local function f()
        ActorMgr:Get():BindActor(inscpp, classpath)
    end
    Xpcall(f)
end

function CppCallBack(inscpp, classpath, functionName, ...)
    A_("lalala", inscpp, classpath, functionName, ...)
    ActorMgr:Get():CallLuaInsFunc(inscpp, classpath, functionName, ...)
end