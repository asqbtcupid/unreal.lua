function GetClassFunctionNames(LuaClassPath)
	local class = require (LuaClassPath)
	local Names = {}
	while class do
		for k, v in pairs(class) do
			if type(v) == "function" then
				Names[k] = true
			end			
		end
		class = getmetatable(class) 
	end
	return Names
end