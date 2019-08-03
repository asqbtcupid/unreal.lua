if _VERSION == "Lua 5.3" then
	function getfenv(f)
		if type(f) == "function" then
			local name, value = debug.getupvalue(f, 1)
			if name == "_ENV" then
				return value
			else
				return _ENV
			end		
		end
	end

	function setfenv(f, Env)
		if type(f) == "function" then
			local name, value = debug.getupvalue(f, 1)
			if name == "_ENV" then
				debug.setupvalue(f, 1, Env)
			end		
		end
	end
	debug = debug or {}
	debug.setfenv = setfenv

	function loadstring( ... )
        return load(...)
    end
end

local StackOffset

if _VERSION == "Lua 5.3" then
	StackOffset = 1
else
	StackOffset = 2
end

requirecpp "UDebuggerSetting"
requirecpp "FDebuggerVarNode"
local DebuggerSetting = Inherit(CppObjectBase)

local DebuggerSingleton
local LuaSourceDir 
local BreakPoints = {}
local BreakLines = {}
local bStepOver = false
local bStepIn = false
local bStepOut = false
local StepInStackCount = 0
local StepOutStackCount = 0
local bPauseForMainThread = false

function DebuggerSetting:Ctor()
	LuaSourceDir = self:GetLuaSourceDir()
	DebuggerSingleton = self
	self.m_VarsIndex = 0
	-- local weakmeta = {__mode = "v"}
	-- self.m_WeakVars = setmetatable({}, weakmeta)
	self.m_WeakVars = {}
	self.m_bIsStart = false
	self.m_bIsDebuging = false
	self.m_bPaused = 0

	self.m_TheCoHooking = nil
	self:Timer(self.Tick, self):Time(1)
	self:PullDataToLua()
end

function DebuggerSetting:Tick( )
	self.m_WeakVars = {}
end

local Cached = {}
local function GetFullFilePath(Path)
	local TheCached = Cached[Path]
	if not TheCached then
		local FullPath = string.match(Path, "@.*LuaSource(.*%.lua)")
		if FullPath then
			FullPath = string.gsub(FullPath, "\\", "/")
			FullPath = LuaSourceDir..FullPath
			Cached[Path] = FullPath
			return FullPath
		end
		local LuaPath = string.match(Path, "%-%-%[%[(.-)%]%]")
		if LuaPath then
			LuaPath = string.gsub(LuaPath, "%.", "/")
			LuaPath = LuaSourceDir .. "/"..LuaPath .. ".lua"
			Cached[Path] = LuaPath
			return LuaPath
		end
		Cached[Path] = Path
		return Path
	else
		return TheCached
	end
end
local LuaPathCached = {}
local function GetLuaPath(Path)
	local TheCached = LuaPathCached[Path]
	if not TheCached then
		local FullPath = string.match(Path, ".*LuaSource/(.*)%.lua")
		if FullPath == nil then
			FullPath = Path
		end
		FullPath = string.gsub(FullPath, "/", ".")
		LuaPathCached[Path] = FullPath
		return FullPath
	else
		return TheCached
	end
end

local function IsHitBreakPoint(FilePath, Line)
	local Set = BreakPoints[FilePath]
	if Set then
		return Set[Line] == true
	else
		return false
	end
end

local getinfo = debug.getinfo
local function CollectStackData()
	local Contents={}
	local FilePaths={}
	local Lines={}
	local StackIndexs={}
	local FuncInfos={}
	for i = 3, math.huge do
		local StackInfo
		local RunningCoroutine = coroutine.running()
		if RunningCoroutine then
			StackInfo = getinfo(RunningCoroutine, i)
		else
			StackInfo = getinfo(i)
		end
		if not StackInfo then break end
		local FilePath = GetFullFilePath(StackInfo.source)
		local Line = StackInfo.currentline
		local LuaPath = GetLuaPath(FilePath)
		local Content = LuaPath.." @ "..tostring(StackInfo.name)..":Line "..tostring(Line)
		local FuncInfo = tostring(StackInfo.func)
		table.insert(Contents, Content)
		table.insert(FilePaths, FilePath)
		table.insert(Lines, Line)
		table.insert(StackIndexs, i)
		table.insert(FuncInfos, FuncInfo)
	end
	DebuggerSingleton:SetStackData(Contents, Lines, FilePaths, StackIndexs, FuncInfos)
end

local function GetExecuteContext(StackIndex)
	StackIndex = StackIndex + 1
	local ExecuteContext = {}
	local StackInfo
	local RunningCoroutine = coroutine.running()
	if RunningCoroutine then
		StackInfo = getinfo(RunningCoroutine, StackIndex, "f")
	else
		StackInfo = getinfo(StackIndex, "f")
	end
	local func = StackInfo and StackInfo.func
	if func then
		for i = 1, math.huge do
			local name, value = debug.getupvalue(func, i)
			if not name then break end
			ExecuteContext[name] = value
		end
		for i = 1, math.huge do
			local name, value
			if RunningCoroutine then
				name, value = debug.getlocal(RunningCoroutine, StackIndex, i)
			else
				name, value = debug.getlocal(StackIndex, i)
			end			
			if not name then break end
			ExecuteContext[name] = value
		end
	end
	local meta = {}
	function meta.__index(t, k)
		return _G[k]
	end
	return setmetatable(ExecuteContext, meta)
end

local function PassCondition(FilePath, Line)
	local ConditionText = DebuggerSingleton:GetBreakPointHitConditionText(FilePath, Line)
	ConditionText = tostring(ConditionText)
	if ConditionText == "" then
		return true
	end
	
	local tempfuncstr = "return "..ConditionText
	local ExeFunc = loadstring(tempfuncstr)
	if ExeFunc then
		local ConditionExeContext = GetExecuteContext(3)
		setfenv(ExeFunc, ConditionExeContext)
		local IsSucesss, ErrorStr = pcall(ExeFunc)
		if IsSucesss then
			return ExeFunc()
		else
			return false
		end
	end
	return false
end

local function HookCallBack(Event, Line)
	if Line then
		local IsDebugOperation = (bStepOut and StepOutStackCount<=-1)
								or (bStepOver and StepInStackCount<=0) 
								or bStepIn 
		if not IsDebugOperation then
			if not BreakLines[Line] then
				return 
			end
		end				
		local RunningCoroutine = coroutine.running()
		if RunningCoroutine == nil and bPauseForMainThread then
			return
		end	
		local StackInfo
		if RunningCoroutine then
			StackInfo = getinfo(RunningCoroutine, 2, "S")
		else
			StackInfo = getinfo(2, "S")
		end
		local FilePath = GetFullFilePath(StackInfo.source)
		if 	IsDebugOperation or (BreakPoints[FilePath..tostring(Line)] and PassCondition(FilePath, Line)) then

			StepInStackCount = 0
			StepOutStackCount = 0
			bStepIn = false
			bStepOver = false
			bStepOut = false
			CollectStackData()
			DebuggerSingleton:EnterDebug(FilePath, Line)
		end
	elseif bStepOver then
		if Event == "call" then
			StepInStackCount = StepInStackCount + 1
		elseif Event == "tail call" then

		else
			StepInStackCount = StepInStackCount - 1
		end
	elseif bStepOut then
		if Event == "call" then
			StepOutStackCount = StepOutStackCount + 1
		elseif Event == "tail call" then
			
		else
			StepOutStackCount = StepOutStackCount - 1
		end 
		if StepOutStackCount <= -1 then
			bStepOut = false
			bStepIn = true
		end
	end
end

function DebuggerSetting:ShouldRunDebug()
	return self.m_bIsTabOpen and self.m_bIsStart and self:HasAnyBreakPoint() and self.m_bPaused == 0
end

function DebuggerSetting:HookCoroutine(co)
	if self:ShouldRunDebug() then
		debug.sethook(co, HookCallBack, "lcr")
	end
end

function DebuggerSetting:UnHookCoroutine(co)
	debug.sethook(co)
end

function DebuggerSetting:CheckToRun()
	if self.m_bIsDebuging then
		if not self:ShouldRunDebug() then
			-- a_("lalala1")
			debug.sethook()
			self.m_WeakVars = {}
			collectgarbage("collect")
			self.m_bIsDebuging = false
		end	
	else
		if self:ShouldRunDebug() then
			-- a_("lalala2")
			debug.sethook(HookCallBack, "lcr")
			self.m_bIsDebuging = true
		end	
	end
end

function DebuggerSetting:ToggleDebugStart(bIsStart)
	self.m_bIsStart = bIsStart
	self:CheckToRun()
end

function DebuggerSetting:UpdateBreakPoint(InBreakPoints)
	BreakPoints = {}
	BreakLines = {}
	for FilePath, LineSet in pairs(InBreakPoints) do
		for LineNum in pairs(LineSet) do
			BreakPoints[FilePath..tostring(LineNum)] = true
			BreakLines[LineNum] = true
		end
	end
	self:CheckToRun()
end

function DebuggerSetting:SetTabIsOpen(bIsTabOpen)
	self.m_bIsTabOpen = bIsTabOpen
	self:CheckToRun()
end

function DebuggerSetting:HasAnyBreakPoint( )
	if BreakPoints then
		for a,b in pairs(BreakPoints) do
			return true
		end
	end
	return false
end

local function GetClassName(VarValue)
	local name = ""
	if type(VarValue) == "table" then
		local function getname()
			if rawget(VarValue, "classname") ~= nil then
				name = "("..rawget(VarValue, "classname")..")"
			end
		end
		-- lightweight userdata will error
		pcall(getname)
	elseif type(VarValue) == "userdata" and getmetatable(VarValue) then
		return GetClassName(getmetatable(VarValue))
	end
	return name
end

local function IsContainer(Var)
	local classname = GetClassName(Var)
	if type(Var) == "userdata" and (classname == "(ULuaArrayHelper)" or classname == "(ULuaSetHelper)" or classname == "(ULuaMapHelper)") then
		return true
	elseif type(Var) == "userdata" and (string.find(classname, "^%(TArray_") or string.find(classname, "^%(TMap_")) then
		return true
	elseif type(Var) == "userdata" and string.find(classname, "^%(TShared") then
		return true
	else
		return false
	end
end

local function MayHaveChildren(Var)
	local TheType = type(Var)
	if TheType == "table" or TheType == "userdata" then
		return true
	else
		return false
	end
end

function DebuggerSetting:AddToWeak(LuaValue, name)
	local index = self.m_VarsIndex +1
	self.m_VarsIndex = index
	if IsContainer(LuaValue) then
		LuaValue = LuaValue:Table()
	end
	self.m_WeakVars[index] = LuaValue
	return index
end

function DebuggerSetting:GetStackVars(StackIndex)
	local result = {}
	StackIndex = StackIndex + StackOffset
	local StackInfo
	local RunningCoroutine = coroutine.running()

	if RunningCoroutine then
		StackInfo = getinfo(RunningCoroutine, StackIndex, "f")
	else
		StackInfo = getinfo(StackIndex, "f")
	end
	local func = StackInfo and StackInfo.func
	if func then
		local function AddNode(name, value, isupvalue)
			if name == "(*temporary)" then
				return
			end
			local NewNode = FDebuggerVarNode.New()
			if MayHaveChildren(value) then
				local WeakIndex = self:AddToWeak(value, name)
				NewNode.ValueWeakIndex = WeakIndex
			end
			if isupvalue then
				name = name.."(upvalue)"
			end
			NewNode.Name = tostring(name)
			if type(value) == "string" then
				if tonumber(value) then
					NewNode.Value = value.."(str)"	
				else
					NewNode.Value = value
				end
			else
				NewNode.Value = tostring(value)..tostring(GetClassName(value))
			end
			table.insert(result, NewNode)
		end
		for i = 1, math.huge do
			local name, value = debug.getupvalue(func, i)
			if not name then break end
			AddNode(name, value, true)
		end
		for i = 1, math.huge do
			local name, value
			if RunningCoroutine then
				name, value = debug.getlocal(RunningCoroutine, StackIndex, i)
			else
				name, value = debug.getlocal(StackIndex, i)
			end			
			if not name then break end
			AddNode(name,value)
		end
	end
	return result
end

function DebuggerSetting:GetVarNodeChildren(ParentNode)
	local result = {}
	local Var = self.m_WeakVars[ParentNode.ValueWeakIndex]
	if Var then	
		local function AddNode(name, value)
			local NewNode = FDebuggerVarNode.New()
			if MayHaveChildren(value) then
				local WeakIndex = self:AddToWeak(value, name)
				NewNode.ValueWeakIndex = WeakIndex
			end
			NewNode.Name = tostring(name)
			if type(value) == "string" then
				if tonumber(value) then
					NewNode.Value = value.."(str)"	
				else
					NewNode.Value = value
				end

			else
				NewNode.Value = tostring(value)..tostring(GetClassName(value))
			end
			table.insert(result, NewNode)
		end
		if type(Var) == "table" then
			local meta = getmetatable(Var)
			if meta then
				AddNode("$meta$", meta)
			end
			for name, value in pairs(Var) do 
				AddNode(name, value)
			end
			if meta then
				for key, v in pairs(meta) do
					if type(key) == "string" and key:find("^LuaGet_") and type(v) == "function" then
						local name = key:match("^LuaGet_(.*)")
						local value = v(Var)
						AddNode(name, value)
					end
				end
			end
		elseif type(Var) == "userdata" then 
			local meta = getmetatable(Var)
			if meta then
				AddNode("$meta$", meta)
				for key, v in pairs(meta) do
		 			if type(key) == "string" and key:find("^LuaGet_") and type(v) == "function" then
						local name = key:match("^LuaGet_(.*)")
						local value = v(Var)
						AddNode(name, value)
					end
				end
				if meta.GetAllProperty then
					local AllProperty = Var:GetAllProperty()
					for key, v in pairs(AllProperty) do
						AddNode(key, v)
					end
				end
			end
		end
	end
	return result
end

function DebuggerSetting:StepOver()
	collectgarbage("collect")
	bStepOver = true	
end

function DebuggerSetting:StepIn()
	bStepIn = true	
end

function DebuggerSetting:StepOut()
	bStepOut = true	
end

function DebuggerSetting:BreakConditionChange()
end

function DebuggerPause( )
	if DebuggerSingleton then
		DebuggerSingleton.m_bPaused = DebuggerSingleton.m_bPaused + 1
		DebuggerSingleton:CheckToRun()
	end
end

function DebuggerResume( )
	if DebuggerSingleton then
		DebuggerSingleton.m_bPaused = math.max(DebuggerSingleton.m_bPaused - 1, 0)
		DebuggerSingleton:CheckToRun()
	end
	if RemoteDebuggerSetting then
		RemoteDebuggerSetting:Get():DebuggerResume()
	end
end

function DebuggerSetting:Get( )
	return DebuggerSingleton
end

if UDebuggerSetting then
	DebuggerSetting:NewOn(UDebuggerSetting.Get(false))
	local oldresume = coroutine.resume 
	local oldyield = coroutine.yield 
	local oldwrap  = coroutine.wrap 
	local coroutinestack = {} 
	local function newresume(co, ...)
		DebuggerSingleton:HookCoroutine(co)
		bPauseForMainThread = true
		-- UTableUtil.SetRunningState(co)
		local result = {oldresume(co, ...)}
		-- UTableUtil.SetRunningState(nil)
		bPauseForMainThread = false
		if coroutine.status(co) == "dead" then
			DebuggerSingleton:UnHookCoroutine(co)
		end
		return unpack(result, 1, 10)
	end 
	local function newyield(...)
		DebuggerSingleton:UnHookCoroutine(coroutine.running())
		return oldyield(...)
	end
	local function newwrap(func)
		local co = coroutine.create(func)
		local function f(...)
			newresume(co, ...)
		end
		return f
	end
	coroutine.resume = newresume
	coroutine.yield = newyield
	coroutine.wrap = newwrap
end

return DebuggerSetting
