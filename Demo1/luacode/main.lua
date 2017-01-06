function Init()
    package.path = package.path .. ";".._luadir.."\\?.lua"
    require "logutil"
    require "luaclass"
    require "globalevent"
    require "inputmgr"
    require "timermgr"
    -- require "luacharacter"
    -- require "actormgr"
    -- require "sqllike"
end

function shutdown()
    
end

function Tick(delta)
    TimerMgr:Get():Tick(delta)
end

function NewActor(v, classpath)
    local function f()
        require "actormgr":Get():NewActor(v, classpath)
    end
    Xpcall(f)
    -- TimerMgr:Get():On(A_, "xixi"):Time(1000):Num(5)
end
