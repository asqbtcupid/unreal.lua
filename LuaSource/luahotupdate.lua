local HU = {}

function HU.FailNotify(...)
	if HU.NotifyFunc then HU.NotifyFunc(...) end
end
function HU.DebugNofity(...)
	if HU.DebugNofityFunc then HU.DebugNofityFunc(...) end
end

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

function HU.InitFileMap(RootPath)
	for _, rootpath in pairs(RootPath) do
		rootpath = Normalize(rootpath)
		local file = io.popen("dir /S/B /A:A \""..rootpath.."\"")
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

function HU.InitFakeTable()
	local meta = {}
	HU.Meta = meta
	local function FakeT() return setmetatable({}, meta) end
	local function EmptyFunc() end
	local function pairs() return EmptyFunc end  
	local function setmetatable(t, metaT)
		HU.MetaMap[t] = metaT 
		return t
	end
	local function getmetatable(t, metaT)
		return setmetatable({}, t)
	end
	local function require(LuaPath)
		if not HU.RequireMap[LuaPath] then
			local FakeTable = FakeT()
			HU.RequireMap[LuaPath] = FakeTable
		end
		return HU.RequireMap[LuaPath]
	end
	function meta.__index(t, k)
		if k == "setmetatable" then
			return setmetatable
		elseif k == "pairs" or k == "ipairs" then
			return pairs
		elseif k == "next" then
			return EmptyFunc
		elseif k == "require" then
			return require
		else
			local FakeTable = FakeT()
			rawset(t, k, FakeTable)
			return FakeTable 
		end
	end
	function meta.__newindex(t, k, v) rawset(t, k, v) end
	function meta.__call() return FakeT(), FakeT(), FakeT() end
	function meta.__add() return meta.__call() end
	function meta.__sub() return meta.__call() end
	function meta.__mul() return meta.__call() end
	function meta.__div() return meta.__call() end
	function meta.__mod() return meta.__call() end
	function meta.__pow() return meta.__call() end
	function meta.__unm() return meta.__call() end
	function meta.__concat() return meta.__call() end
	function meta.__eq() return meta.__call() end
	function meta.__lt() return meta.__call() end
	function meta.__le() return meta.__call() end
	function meta.__len() return meta.__call() end
	return FakeT
end

function HU.InitProtection()
	HU.Protection = {}
	HU.Protection[setmetatable] = true
	HU.Protection[pairs] = true
	HU.Protection[ipairs] = true
	HU.Protection[next] = true
	HU.Protection[require] = true
	HU.Protection[HU] = true
	HU.Protection[HU.Meta] = true
	HU.Protection[math] = true
	HU.Protection[string] = true
	HU.Protection[table] = true
end

function HU.AddFileFromHUList()
	package.loaded[HU.UpdateListFile] = nil
	local FileList = require (HU.UpdateListFile)
	HU.ALL = false
	HU.HUMap = {}
	for _, file in pairs(FileList) do
		if file == "_ALL_" then
			HU.ALL = true
			for k, v in pairs(HU.FileMap) do
				for _, path in pairs(v) do
					HU.HUMap[path.LuaPath] = path.SysPath  	
				end
			end
			return
		end
		if HU.FileMap[file] then
			for _, path in pairs(HU.FileMap[file]) do
				HU.HUMap[path.LuaPath] = path.SysPath  	
			end
		else
			HU.FailNotify("HotUpdate can't not find "..file)
		end
	end
end

function HU.ErrorHandle(e)
	HU.FailNotify("HotUpdate Error\n"..tostring(e))
	HU.ErrorHappen = true
end

function HU.BuildNewCode(SysPath, LuaPath)
	io.input(SysPath)
	local NewCode = io.read("*all")
	if HU.ALL and HU.OldCode[SysPath] == nil then
		HU.OldCode[SysPath] = NewCode
		return
	end
	if HU.OldCode[SysPath] == NewCode then
		io.input():close()
		return false
	end
	HU.DebugNofity(SysPath)
	io.input(SysPath)  
	local chunk = "--[["..LuaPath.."]] "
	chunk = chunk..NewCode	
	io.input():close()
	local NewFunction = loadstring(chunk)
	if not NewFunction then 
  		HU.FailNotify(SysPath.." has syntax error.")  	
  		collectgarbage("collect")
  		return false
	else
		HU.FakeENV = HU.FakeT()
		HU.MetaMap = {}
		HU.RequireMap = {}
		setfenv(NewFunction, HU.FakeENV)
		local NewObject
		HU.ErrorHappen = false
		xpcall(function () NewObject = NewFunction() end, HU.ErrorHandle)
		if not HU.ErrorHappen then 
			HU.OldCode[SysPath] = NewCode
			return true, NewObject
		else
	  		collectgarbage("collect")
			return false
		end
	end
end

function HU.Travel_G()
	local visited = {}
	visited[HU] = true
	local function f(t)
		if (type(t) ~= "function" and type(t) ~= "table") or visited[t] or HU.Protection[t] then return end
		visited[t] = true
		if type(t) == "function" then
		  	for i = 1, math.huge do
				local name, value = debug.getupvalue(t, i)
				if not name then break end
				if type(value) == "function" then
					for _, funcs in ipairs(HU.ChangedFuncList) do
						if value == funcs[1] then
							debug.setupvalue(t, i, funcs[2])
						end
					end
				end
				f(value)
			end
		elseif type(t) == "table" then
			f(debug.getmetatable(t))
			local changeIndexs = {}
			for k,v in pairs(t) do
				f(k); f(v);
				if type(v) == "function" then
					for _, funcs in ipairs(HU.ChangedFuncList) do
						if v == funcs[1] then t[k] = funcs[2] end
					end
				end
				if type(k) == "function" then
					for index, funcs in ipairs(HU.ChangedFuncList) do
						if k == funcs[1] then changeIndexs[#changeIndexs+1] = index end
					end
				end
			end
			for _, index in ipairs(changeIndexs) do
				local funcs = HU.ChangedFuncList[index]
				t[funcs[2]] = t[funcs[1]] 
				t[funcs[1]] = nil
			end
		end
	end
	
	f(_G)
	local registryTable = debug.getregistry()
	f(registryTable)
	
	for _, funcs in ipairs(HU.ChangedFuncList) do
		if funcs[3] == "HUDebug" then funcs[4]:HUDebug() end
	end
end

function HU.ReplaceOld(OldObject, NewObject, LuaPath, From, Deepth)
	if type(OldObject) == type(NewObject) then
		if type(NewObject) == "table" then
			HU.UpdateAllFunction(OldObject, NewObject, LuaPath, From, "") 
		elseif type(NewObject) == "function" then
			HU.UpdateOneFunction(OldObject, NewObject, LuaPath, nil, From, "")
		end
	end
end

function HU.HotUpdateCode(LuaPath, SysPath)
	local OldObject = package.loaded[LuaPath]
	if OldObject ~= nil then
		HU.VisitedSig = {}
		HU.ChangedFuncList = {}
		local Success, NewObject = HU.BuildNewCode(SysPath, LuaPath)
		if Success then
			HU.ReplaceOld(OldObject, NewObject, LuaPath, "Main", "")
			for LuaPath, NewObject in pairs(HU.RequireMap) do
				local OldObject = package.loaded[LuaPath]
				HU.ReplaceOld(OldObject, NewObject, LuaPath, "Main_require", "")
			end
			setmetatable(HU.FakeENV, nil)
			HU.UpdateAllFunction(HU.ENV, HU.FakeENV, " ENV ", "Main", "")
			if #HU.ChangedFuncList > 0 then
				HU.Travel_G()
			end
			collectgarbage("collect")
		end
	elseif HU.OldCode[SysPath] == nil then 
		io.input(SysPath)
		HU.OldCode[SysPath] = io.read("*all")
		io.input():close()
	end
end

function HU.ResetENV(object, name, From, Deepth)
	local visited = {}
	local function f(object, name)
		if not object or visited[object] then return end
		visited[object] = true
		if type(object) == "function" then
			HU.DebugNofity(Deepth.."HU.ResetENV", name, "  from:"..From)
			xpcall(function () setfenv(object, HU.ENV) end, HU.FailNotify)
		elseif type(object) == "table" then
			HU.DebugNofity(Deepth.."HU.ResetENV", name, "  from:"..From)
			for k, v in pairs(object) do
				f(k, tostring(k).."__key", " HU.ResetENV ", Deepth.."    " )
				f(v, tostring(k), " HU.ResetENV ", Deepth.."    ")
			end
		end
	end
	f(object, name)
end

function HU.UpdateUpvalue(OldFunction, NewFunction, Name, From, Deepth)
	HU.DebugNofity(Deepth.."HU.UpdateUpvalue", Name, "  from:"..From)
	local OldUpvalueMap = {}
	local OldExistName = {}
	for i = 1, math.huge do
		local name, value = debug.getupvalue(OldFunction, i)
		if not name then break end
		OldUpvalueMap[name] = value
		OldExistName[name] = true
	end
	for i = 1, math.huge do
		local name, value = debug.getupvalue(NewFunction, i)
		if not name then break end
		if OldExistName[name] then
			local OldValue = OldUpvalueMap[name]
			if type(OldValue) ~= type(value) then
				debug.setupvalue(NewFunction, i, OldValue)
			elseif type(OldValue) == "function" then
				HU.UpdateOneFunction(OldValue, value, name, nil, "HU.UpdateUpvalue", Deepth.."    ")
			elseif type(OldValue) == "table" then
				HU.UpdateAllFunction(OldValue, value, name, "HU.UpdateUpvalue", Deepth.."    ")
				debug.setupvalue(NewFunction, i, OldValue)
			else
				debug.setupvalue(NewFunction, i, OldValue)
			end
		else
			HU.ResetENV(value, name, "HU.UpdateUpvalue", Deepth.."    ")
		end
	end
end 

function HU.UpdateOneFunction(OldObject, NewObject, FuncName, OldTable, From, Deepth)
	if HU.Protection[OldObject] or HU.Protection[NewObject] then return end
	if OldObject == NewObject then return end
	local signature = tostring(OldObject)..tostring(NewObject)
	if HU.VisitedSig[signature] then return end
	HU.VisitedSig[signature] = true
	HU.DebugNofity(Deepth.."HU.UpdateOneFunction "..FuncName.."  from:"..From)
	if pcall(debug.setfenv, NewObject, getfenv(OldObject)) then
		HU.UpdateUpvalue(OldObject, NewObject, FuncName, "HU.UpdateOneFunction", Deepth.."    ")
		HU.ChangedFuncList[#HU.ChangedFuncList + 1] = {OldObject, NewObject, FuncName, OldTable}
	end
end

function HU.UpdateAllFunction(OldTable, NewTable, Name, From, Deepth)
	if HU.Protection[OldTable] or HU.Protection[NewTable] then return end
	if OldTable == NewTable then return end
	local signature = tostring(OldTable)..tostring(NewTable)
	if HU.VisitedSig[signature] then return end
	HU.VisitedSig[signature] = true
	HU.DebugNofity(Deepth.."HU.UpdateAllFunction "..Name.."  from:"..From)
	for ElementName, Element in pairs(NewTable) do
		local OldElement = OldTable[ElementName]
		if type(Element) == type(OldElement) then
			if type(Element) == "function" then
				HU.UpdateOneFunction(OldElement, Element, ElementName, OldTable, "HU.UpdateAllFunction", Deepth.."    ")
			elseif type(Element) == "table" then
				HU.UpdateAllFunction(OldElement, Element, ElementName, "HU.UpdateAllFunction", Deepth.."    ")
			end
		elseif OldElement == nil and type(Element) == "function" then
			if pcall(setfenv, Element, HU.ENV) then
				OldTable[ElementName] = Element
			end
		end
	end
	local OldMeta = debug.getmetatable(OldTable)  
	local NewMeta = HU.MetaMap[NewTable]
	if type(OldMeta) == "table" and type(NewMeta) == "table" then
		HU.UpdateAllFunction(OldMeta, NewMeta, Name.."'s Meta", "HU.UpdateAllFunction", Deepth.."    ")
	end
end

function HU.Init(UpdateListFile, RootPath, FailNotify, ENV)
	HU.UpdateListFile = UpdateListFile
	HU.HUMap = {}
	HU.FileMap = {}
	HU.NotifyFunc = FailNotify
	HU.OldCode = {}
	HU.ChangedFuncList = {}
	HU.VisitedSig = {}
	HU.FakeENV = nil
	HU.ENV = ENV or _G
	HU.LuaPathToSysPath = {}
	HU.InitFileMap(RootPath)
	HU.FakeT = HU.InitFakeTable()
	HU.InitProtection()
	HU.ALL = false
end

function HU.Update()
	HU.AddFileFromHUList()
	for LuaPath, SysPath in pairs(HU.HUMap) do
		HU.HotUpdateCode(LuaPath, SysPath)
	end
end

return HU
