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
	local weakmeta = {__mode = "v"}
	self.m_WeakVars = setmetatable({}, weakmeta)
	self.m_bIsStart = false
	self.m_bIsDebuging = false
	self.m_bPaused = 0

	self.m_TheCoHooking = nil
	self:Timer(self.Tick, self):Time(0.0001)
	self:PullDataToLua()
end

function DebuggerSetting:Tick( )
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
		table.insert(Contents, Content)
		table.insert(FilePaths, FilePath)
		table.insert(Lines, Line)
		table.insert(StackIndexs, i)
	end
	DebuggerSingleton:SetStackData(Contents, Lines, FilePaths, StackIndexs)
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
		if 	IsDebugOperation or BreakPoints[FilePath..tostring(Line)] then

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
		else
			StepInStackCount = StepInStackCount - 1
		end
	elseif bStepOut then
		if Event == "call" then
			StepOutStackCount = StepOutStackCount + 1
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
			debug.sethook()
			self.m_bIsDebuging = false
		end	
	else
		if self:ShouldRunDebug() then
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
	if type(VarValue) == "table" or type(VarValue) == "userdata" then
		local function getname()
			if VarValue.classname then
				name = "("..VarValue.classname..")"
			end
		end
		-- lightweight userdata will error
		pcall(getname)
	end
	return name
end

local function IsContainer(Var)
	local classname = GetClassName(Var)
	if classname == "(ULuaArrayHelper)" or classname == "(ULuaSetHelper)" or classname == "(ULuaMapHelper)" then
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

function DebuggerSetting:AddToWeak(LuaValue)
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
	StackIndex = StackIndex + 2
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
			local NewNode = FDebuggerVarNode.New()
			if MayHaveChildren(value) then
				local WeakIndex = self:AddToWeak(value)
				NewNode.ValueWeakIndex = WeakIndex
			end
			if isupvalue then
				name = name.."(upvalue)"
			end
			NewNode.Name = name
			NewNode.Value = tostring(value)..GetClassName(value)
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
				local WeakIndex = self:AddToWeak(value)
				NewNode.ValueWeakIndex = WeakIndex
			end
			NewNode.Name = name
			NewNode.Value = tostring(value)..GetClassName(value)
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
					if key:find("^LuaGet_") and type(v) == "function" then
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
					if key:find("^LuaGet_") and type(v) == "function" then
						local name = key:match("^LuaGet_(.*)")
						local value = v(Var)
						AddNode(name, value)
					end
				end
			end
		end
	end
	return result
end

function DebuggerSetting:StepOver()
	bStepOver = true	
end

function DebuggerSetting:StepIn()
	bStepIn = true	
end

function DebuggerSetting:StepOut()
	bStepOut = true	
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
end

function DebuggerSetting:Get( )
	return DebuggerSingleton
end

if UDebuggerSetting then
	DebuggerSetting:NewOn(UDebuggerSetting.Get())
	local oldresume = coroutine.resume 
	local oldyield = coroutine.yield 
	local oldwrap  = coroutine.wrap 
	local coroutinestack = {} 
	local function newresume(co, ...)
		DebuggerSingleton:HookCoroutine(co)
		bPauseForMainThread = true
		UTableUtil.SetRunningState(co)
		local result = {oldresume(co, ...)}
		UTableUtil.SetRunningState(nil)
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
