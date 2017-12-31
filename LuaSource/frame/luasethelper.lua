local LuaSetHelper = Inherit(CppObjectBase, ULuaSetHelper)

local __index = LuaSetHelper.__index
local function __mapIndex(t, k)
	local f = ULuaSetHelper[k]
	if f then
		return f
	else
		f = CppObjectBase[k]
		if f then
			return f
		else
			return ULuaSetHelper.Get(t, k)
		end
	end
end
rawset(LuaSetHelper, "__index", __mapIndex)

local __newindex = LuaSetHelper.__newindex
local function __mapnewindex(t, k, v)
	ULuaSetHelper.Set(t, k, v)
end
rawset(LuaSetHelper, "__newindex", __mapnewindex)

return LuaSetHelper
