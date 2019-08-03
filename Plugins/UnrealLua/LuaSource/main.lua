if _VERSION == "Lua 5.3" then
    function table.maxn(t)
        return #t
    end

    function unpack( ... )
        return table.unpack(...)
    end

    function loadstring( ... )
        return load(...)
    end
end

function requirecpp(str)
    UTableUtil.require(str) 
end

local function AddMetaToG()
    local function global_index(t, k)
        if type(k) == "string" then
            requirecpp(k)
        end
        return rawget(t, k)
    end

    local global_mt = {}
    global_mt.__index = global_index
    setmetatable(_G, global_mt)
end

function Init(IsMannual)
    AddMetaToG()
    package.path = package.path .. ";".._luadir.."/?.lua"
    -- package.cpath = package.cpath .. ";".._luadir.."/?.dll"
    require "frame.initrequire"
    local function ShowMem()
        collectgarbage("collect")
        -- a_("lua memory: ", collectgarbage("count"))
    end
    -- TimerMgr:Get():On(ShowMem):Time(2)

    InitLuahotupdate()
end

function OpenDebugger()
    if _WITH_EDITOR then
        require "frame.debugger.debuggersetting"
    end
    require "frame.debugger.remotedebugger"
end

function InitLuahotupdate()
    if hasInitHoupdate then
        return
    end
    hasInitHoupdate = true
    local HU = require "luahotupdate"
    local LoadHelper = require "frame.reloadfunction"
    if LoadHelper.NotRun then
        return
    end
    HU.SetFileLoader(LoadHelper.InitFileMap, LoadHelper.LoadStringFunc)
    HU.Init("hotupdatelist", {_luadir}, A_, nil, {requirecpp = true})
    local function HotReloadUpdate()
        if DebuggerPause then
            DebuggerPause()
        end
        if RemoteDebuggerPause then
            RemoteDebuggerPause()
        end
        HU.Update() 
        if DebuggerResume then
            DebuggerResume()
        end
        if RemoteDebuggerResume then
            RemoteDebuggerResume()
        end
    end
    TimerMgr:Get():On(HotReloadUpdate):Time(1):Fire()
end

function Tick(delta)
    local function f()
        TimerMgr:Get():Tick(delta)
    end
    Xpcall(f)
end

--for object orientation
-- execution sequence : basec++ ctor -> baselua ctor -> derivedc++ ctor -> derivedlua ctor
function Ctor(classpath, inscpp, ...)

    ensure(inscpp~=nil, "inscpp is nil")
    
    local class = require(classpath)
    if type(inscpp) == "table" then
        if inscpp._meta_ ~= class then
            setmetatable(inscpp, class)
            if inscpp._meta_:IsChildOf(CppSingleton) then
                inscpp._meta_._ins = nil
            end
            if class:IsChildOf(CppSingleton) then
                class._ins = inscpp
            end
            local CtorFunc = rawget(class, "Ctor")
            if CtorFunc then
                CtorFunc(inscpp, ...)
            end
        end
    else
        local NewLuaIns = class:NewOn(inscpp, ...)
    end
end

function Call(functionName, inscpp, ...)
    if type(inscpp) == "table" and not inscpp._has_destroy_ then
        return inscpp[functionName](inscpp, ...)
    else
        -- ensure(false, "error in Call, No Exist such lua ins or ins has been released")
    end
end
-- *******************************

function GC()
    GlobalEvent.Fire("GC")
    -- collectgarbage("collect")
end

function Shutdown()
    GlobalEvent.Fire("LuaShutdown")
    a_("lua VM shutdown")
end
