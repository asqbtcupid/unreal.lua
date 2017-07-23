local function write(str)
    local logfile = io.open(_luadir.."/log.txt", "a")
    if logfile then
        logfile:write(str)
        logfile:flush()
        logfile:close()
        print(str)
    end
end
function Q_(t)
    a_(require "inspect"(t))    
end
function q_(...)
    a_(...)
    a_(debug.traceback())
end
function a_(...)
    local str = ""
    local temp = {...}
    for i = 1, table.maxn(temp) do
        str = str..tostring(temp[i]).."  "
    end
    str = str.."\n"
    ULuautils.log(str)
    write("["..tostring(os.date()).."] "..str)
end

function A_( ... )
    local str = ""
    local temp = {...}
    for i = 1, table.maxn(temp) do
        str = str..tostring(temp[i]).."  "
    end
    -- if G_GameStatics.GameMode then
        UKismetSystemLibrary.PrintString(nil, str)
    -- end
    str = "["..tostring(os.date()).."] "..str
    str = str.."\n"
    ULuautils.log(str)
end

function errhandle(err)
    q_(err)
end

function ErrHandleInLua(err)
    ULuautils.log(err)
    write(err)
end

function Xpcall(f)
    xpcall(f, errhandle)
end