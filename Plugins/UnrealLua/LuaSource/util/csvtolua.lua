local function GetWorkingDir()
	if HU.WorkingDir == nil then
	    local p = io.popen("echo %cd%")
	    if p then
	        HU.WorkingDir = p:read("*l").."\\"
	        p:close()
	    end
	end
	return HU.WorkingDir
end

local function Normalize(path)
	path = path:gsub("/","\\") 
	if path:find(":") == nil then
		path = GetWorkingDir()..path 
	end
	local pathLen = #path 
	if path:sub(pathLen, pathLen) == "\\" then
		 path = path:sub(1, pathLen - 1)
	end
	 
    local parts = { }
    for w in path:gmatch("[^\\]+") do
        if     w == ".." and #parts ~=0 then table.remove(parts)
        elseif w ~= "."  then table.insert(parts, w)
        end
    end
    return table.concat(parts, "\\")
end

function InitFileMap(RootPath)
	for _, rootpath in pairs(RootPath) do
		rootpath = Normalize(rootpath)
		local file = io.popen("dir /S/B /A:A "..rootpath)
		io.input(file)
		for line in io.lines() do
	   		local FileName = string.match(line,".*\\(.*)%.lua")
	  	    if FileName ~= nil then
	            if HU.FileMap[FileName] == nil then
	            	HU.FileMap[FileName] = {}
	        	end
	        	local luapath = string.sub(line, #rootpath+2, #line-4)
				luapath = string.gsub(luapath, "\\", ".")
				HU.LuaPathToSysPath[luapath] = SysPath
	        	table.insert(HU.FileMap[FileName], {SysPath = line, LuaPath = luapath})
	    	end
	    end
	    file:close()
	end
end

local function WriteFile(path, data)
	local filehandle = io.open(path, "w")
    filehandle:write(data)
    filehandle:flush()
    filehandle:close()
end

local GetValueWithDot
local function GetValue(csvValue)
	if tonumber(csvValue) then
		return csvValue
	elseif csvValue:lower() == "true" or csvValue:lower() == "false" then
		return csvValue:lower()
	else
		local inner = csvValue:match("\"(.*)\"")
		if inner then
			return GetValue(inner)
		else
			inner = csvValue:match("{(.*)}")
			if inner then
				inner = inner .. "&@#<"
				inner = inner:gsub("(.-)&@#<", GetValueWithDot)
				return "{"..inner.."}"
			else
				csvValue = csvValue:gsub("&@#<", ",")
				csvValue = csvValue:gsub("**!!", "\"")
				return "[=["..csvValue.."]=]"
			end
		end
	end
end
GetValueWithDot = function(csvValue) 
	return GetValue(csvValue)..","
end

local function GetRow(keys, defaults, csvRow)
	local index = 0
	local result
	for value in csvRow:gmatch("[^,]+") do
		index = index + 1
		if index == 1 then
			result = "["..value.."] = { id = "..value..","
		else
			if value == "@default" then
				result = result..keys[index].." = "..defaults[index]..", "
			else				
				result = result..keys[index].." = "..GetValue(value)..", "
			end
		end 
	end
	result = result .. "},"
	return result
end

local function SwapDotInQuota(str)
	return str:gsub("(.-)(,)", "%1&@#<")
end

local function GenerateLuaCode(CsvData)
	CsvData = CsvData:gsub("\"(.-)\"", SwapDotInQuota)
	CsvData = CsvData:gsub(",,", ",@default,")
	CsvData = CsvData:gsub(",\n", ",@default\n")
	local index = 0
	local keys = {}
	local defaults = {}
	local codes = {}
	codes[1] = "local t = {"
	for csvRow in CsvData:gmatch("(.-)\n") do
		index = index + 1
		if index == 1 then
			for key_default in csvRow:gmatch("([^,]+)") do
				local key, default = key_default:match("(.-)%((.*)%)")
				table.insert(keys, key)
				table.insert(defaults, default and GetValue(default) or "0")
			end
		else
			codes[index] = GetRow(keys, defaults, csvRow)
		end
	end
	local result = table.concat(codes, "\n") .. "}\n return t"
	return result
end

local function Run(rootpath)
	rootpath = Normalize(rootpath)
	local file = io.popen("dir /S/B /A:A "..rootpath)
	io.input(file)
	local systempaths = {}
	for line in io.lines() do
   		local FileName = string.match(line,".*\\(.*)%.csv")
  	    if FileName ~= nil then
            table.insert(systempaths, {FileName = FileName, Path = line})
    	end
    end
    local LuaconfigPath = _luadir.."/config"
    for k, v in ipairs(systempaths) do
		io.input(v.Path)
		local CsvData = io.read("*all")
		io.input():close()
		WriteFile(LuaconfigPath.."/"..v.FileName..".lua", GenerateLuaCode(CsvData))
    end
    file:close()
end

return Run

