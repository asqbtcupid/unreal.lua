local LuaArrayHelper = Inherit(CppObjectBase, ULuaArrayHelper)

local __index = LuaArrayHelper.__index
local function __arrIndex(t, k)
	if type(k) == "number" then
		return ULuaArrayHelper.Get(t, k)
	else
		return __index(t, k)
	end
end
rawset(LuaArrayHelper, "__index", __arrIndex)

local __newindex = LuaArrayHelper.__newindex
local function __arrnewindex(t, k, v)
	if type(k) == "number" then
		ULuaArrayHelper.Set(t, k, v)
	else
		__newindex(t, k, v)
	end
end
rawset(LuaArrayHelper, "__newindex", __arrnewindex)

return LuaArrayHelper
