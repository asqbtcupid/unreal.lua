local LuaMapHelper = Inherit(CppObjectBase)

local __index = LuaMapHelper.__index
local function __mapIndex(t, k)
	local f = ULuaMapHelper[k]
	if f then
		return f
	else
		f = CppObjectBase[k]
		if f then
			return f
		else
			return ULuaMapHelper.Get(t, k)
		end
	end
end
rawset(LuaMapHelper, "__index", __mapIndex)

local __newindex = LuaMapHelper.__newindex
local function __mapnewindex(t, k, v)
	ULuaMapHelper.Set(t, k, v)
end
rawset(LuaMapHelper, "__newindex", __mapnewindex)

return LuaMapHelper
