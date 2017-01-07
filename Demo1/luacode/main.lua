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
    TimerMgr:Get():On(ShowMem):Time(5000):Fire()
end

function Tick(delta)
    TimerMgr:Get():Tick(delta)
end

function CtorCpp(this, classpath, ...)
    local function f()
        ActorMgr:Get():CtorCpp(this, classpath)
    end
    Xpcall(f)
end

function ShowMem()
    collectgarbage("collect")
    A_("lua memory: ", collectgarbage("count"))
end


function NewActor(v, classpath)
    local function f()
        ActorMgr:Get():BindActor(v, classpath)
    end
    Xpcall(f)
end
