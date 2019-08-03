local m = {}
local WorkingDir
local function GetWorkingDir()
	if WorkingDir == nil then
	    local p = io.popen("echo %cd%")
	    if p then
	        WorkingDir = p:read("*l").."\\"
	        p:close()
	    end
	end
	return WorkingDir
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

function m.run()
	-- local snipfile = io.open(_luadir.."\\b.sublime-snippet", "w")
	local exported = {}
	local filepath = Normalize(_luadir.."/../")
	os.execute("mkdir "..Normalize(_luadir.."/sublimestuff"))
	local file = io.popen("dir /S/B /A:A "..filepath)
	io.input(file)
	for line in io.lines() do
   		local FileName = string.match(line,".*\\(.*)%.script%.h")
   		if FileName then
   			if FileName == "allEnum" then
	   			local EnumName = ""
	   			for code in io.lines(line) do
		   			EnumName = code:match("static const EnumItem ([^_]+)_Enum") or EnumName
		   			local ValueName = code:match("{ \"(.*)\"")
		   			if ValueName then
			   			exported["\""..EnumName.."."..ValueName.."\""] = true
			   		end
		   		end
   			else
	   			local sourcefile = io.open(line, "r")
	   			local source = sourcefile:read("*all")
	   			for class, func in source:gmatch("static int32 ([^_]+)_([^%(]+)") do
					exported["\""..class.."\""] = true
					local property = func:match("Get_(.*)")
					if property then
						exported["\""..property.."\""] = true
					elseif func:match("Set_(.*)") then
					else
						exported["\""..func.."\""] = true
					end
				end
			end
   		end
   	end
   	local code = "{\"completions\":["
   	for key, v in pairs(exported) do
   		-- local temp = "{\"trigger\":"..key..", \"contents\":"..key.."},"
   		local temp = key..","
   		code = code..temp.."\n"
   	end
   	code = code.."]}"
	-- local snipfile = io.open(_luadir.."\\sublimestuff\\".."completions.sublime-completions", "w")
	local snipfile = io.open("C:\\Users\\Administrator\\AppData\\Roaming\\Sublime Text 3\\Packages\\completions.sublime-completions", "w")
	snipfile:write(code)
	snipfile:close()
end

return m