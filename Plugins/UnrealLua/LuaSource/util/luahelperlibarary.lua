function IsValid(p)
	if type(p) == "table" or type(p) == "userdata" then
		return UNZLuaHelperLibrary.IsObjectValid(p)
	end
	return false
end

function DiscardOldExistTable()
  	local registry = debug.getregistry()["_existuserdata"]
    local newmeta = {__mode = "kv"}
    local newexist = {}
    for k, v in pairs(registry) do
        newexist[k] = v
    end
    setmetatable(newexist, newmeta)
    debug.getregistry()["_existuserdata"] = newexist
end

function GetCppSingleton(Ins, DataClassName)
    requirecpp(DataClassName)
    local GameIns = UGameplayStatics.GetGameInstance(Ins)
    assert(GameIns ~= nil)
    local Mgr = UNZPIESingletonManager.Get(GameIns)
    local Data = Mgr:GetNZPIESingleton(_G[DataClassName].Class())
    return Data
end