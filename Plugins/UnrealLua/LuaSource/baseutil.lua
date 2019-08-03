function GetDeltaSeconds(ins)
    if ins == nil then error("") end
	return UGameplayStatics.GetWorldDeltaSeconds(ins)
end

function GetTimeSeconds(ins)
    if ins == nil then error("") end
    return UGameplayStatics.GetTimeSeconds(ins)
end

function Get(t, target, key)
    if key then
        for k,v in pairs(t) do
            if v[key] == target then
                return k, v
            end
        end
    else
        for k,v in pairs(t) do
            if v == target then
                return k, v
            end
        end
    end
end

function CppType(t)
    if type(t) == "table" then
        t = t._cppinstance_ 
    end
    return getmetatable(t).classname
end

function LoadObject(outer, path)
    local ObjectClass = "U"..path:match(".-(%a+).-'.-'")
    if ObjectClass:find("Blueprint") then
        local Object = _G[ObjectClass].LoadObject(outer, path)
        return Object.GeneratedClass
    else
        local Object = _G[ObjectClass].LoadObject(outer, path)
        return Object
    end
end

function LoadClass(outer, path)
    local ObjectClass = path:match(".-(%a+).-'.-'")
    return _G[ObjectClass].LoadClass(outer, path:gsub("'$","_C'"))
end
