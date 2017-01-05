function Init()
    package.path = package.path .. ";".._luadir.."\\?.lua"
    require "logutil"
    require "luaclass"
    require "globalevent"
    require "inputmgr"
    require "luacharacter"
    -- require "actormgr"
    -- require "sqllike"
end

function shutdown()
    
end

function tick(delta)
    
end

function NewActor(v, classpath)
    local function f()
        require "actormgr":Get():NewActor(v, classpath)
    end
    Xpcall(f)
end
