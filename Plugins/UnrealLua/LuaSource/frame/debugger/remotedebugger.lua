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
	StackOffset = 2
else
	StackOffset = 3
end

-- 游戏端require 这个文件，调用 CreateDebuggerSever()会创建63000这个端口的TCP
-- 如果是android，usb连接后需要adb forward tcp:63000 tcp:63000
-- 然后编辑器调式器启动remote模式，需要编辑器里也打开游戏，会自动连接这个端口，之后就可以断点了

local ServerPort = 63000

local DebuggerSetting = require "frame.debugger.debuggersetting"
local SocketMgr = require "frame.debugger.debugsocketmgr"
RemoteDebuggerSetting_Game = Inherit(Singleton)
RemoteDebuggerSetting_Editor = Inherit(CppObjectBase)


local DebuggerSingleton
local DebuggerSingletonEditor
local LuaSourceDir 
local BreakPoints = {}
local BreakLines = {}
-- file..line to condition
local HitConditions = {}
local bStepOver = false
local bStepIn = false
local bStepOut = false
local StepInStackCount = 0
local StepOutStackCount = 0
local bPauseForMainThread = false

function RemoteDebuggerSetting_Game:Ctor()
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
end

function RemoteDebuggerSetting_Game:Tick()
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
	for i = 4, math.huge do
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
	return {Contents=Contents, FilePaths=FilePaths, Lines=Lines, StackIndexs=StackIndexs, FuncInfos=FuncInfos}
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
	local HitConditionOfFile = HitConditions[FilePath]
	if HitConditionOfFile then
		local HitConditionOfLine = HitConditionOfFile[Line] 
		if HitConditionOfLine == nil or HitConditionOfLine == "" then
			return true
		end

		local tempfuncstr = "return "..HitConditionOfLine
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
	else
		return true
	end
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
			-- CollectStackData()
			RemoteDebuggerSetting_Game:Get():EnterDebug(FilePath, Line)
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

function RemoteDebuggerSetting_Game:EnterDebug(FilePath, Line)
	self.m_IsWaittingContinue = true
	self.m_ServerSocket:Call("EnterDebug", FilePath, Line, CollectStackData())
	while self.m_IsWaittingContinue do
		-- local function f()
			self.m_ServerSocket:Tick()
		-- end
		-- Xpcall(f)
	end
end

function RemoteDebuggerSetting_Game:DebugContinue( )
	self.m_IsWaittingContinue = false
	self.m_WeakVars = {}
end

function RemoteDebuggerSetting_Game:ShouldRunDebug()
	return self.m_bIsTabOpen 
	and self.m_bIsStart
	and self:HasAnyBreakPoint() 
	and LuaSourceDir ~= nil 
	and self.m_bPaused == 0
end

function RemoteDebuggerSetting_Game:HookCoroutine(co)
	if self:ShouldRunDebug() then
		debug.sethook(co, HookCallBack, "lcr")
	end
end

function RemoteDebuggerSetting_Game:UnHookCoroutine(co)
	debug.sethook(co)
end

function RemoteDebuggerSetting_Game:CheckToRun()
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


function RemoteDebuggerSetting_Game:SyncState(InbIsTabOpen, InbIsStart, InBreakPoints, InBreakLines, InLuaSourceDir, InHitConditions)
	BreakPoints = InBreakPoints
	BreakLines = InBreakLines
	HitConditions = InHitConditions
	self.m_bIsTabOpen = InbIsTabOpen
	self.m_bIsStart = InbIsStart
	LuaSourceDir = InLuaSourceDir
	self:CheckToRun()
end

function CreateDebuggerSever( )
	RemoteDebuggerSetting_Game:Get()
	if DebuggerSingleton then
		DebuggerSingleton:CreateServer()
	end
end

function RemoteDebuggerSetting_Game:CreateServer( )
	if self.m_ServerSocket then
		return
	end
	local mgr = UnrealLuaSocketMgr:Get()
	self.m_ServerSocket = mgr:Listen(ServerPort)
	self.m_ServerSocket:SetRpcIns(self)
end

local function GetClassName(VarValue)
	local name = ""
	if type(VarValue) == "table" then
		local function getname()
			if VarValue.classname then
				name = "("..VarValue.classname..")"
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

function RemoteDebuggerSetting_Game:AddToWeak(LuaValue, name)
	local index = self.m_VarsIndex +1
	self.m_VarsIndex = index
	if IsContainer(LuaValue) then
		LuaValue = LuaValue:Table()
	end
	self.m_WeakVars[index] = LuaValue
	return index
end

function RemoteDebuggerSetting_Game:GetStackVars(StackIndex)
	StackOffset = 2
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
			local NewNode = {}
			if MayHaveChildren(value) then
				local WeakIndex = self:AddToWeak(value, name)
				NewNode.ValueWeakIndex = WeakIndex
			end
			if isupvalue then
				name = name.."(upvalue)"
			end
			NewNode.Name = tostring(name)
			NewNode.Value = tostring(value)..tostring(GetClassName(value))
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

function RemoteDebuggerSetting_Game:GetVarNodeChildren(ParentNode)
	local result = {}
	ParentNode.ValueWeakIndex = tonumber(ParentNode.ValueWeakIndex)
	local Var = self.m_WeakVars[ParentNode.ValueWeakIndex]
	if Var then	
		local function AddNode(name, value)
			local NewNode = {}
			if MayHaveChildren(value) then
				local WeakIndex = self:AddToWeak(value, name)
				NewNode.ValueWeakIndex = WeakIndex
			end
			NewNode.Name = tostring(name)
			NewNode.Value = tostring(value)..tostring(GetClassName(value))
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

function RemoteDebuggerSetting_Game:HasAnyBreakPoint()
	if BreakPoints then
		for a,b in pairs(BreakPoints) do
			return true
		end
	end
	return false
end

function RemoteDebuggerSetting_Game:StepOver()
	self.m_IsWaittingContinue = false
	bStepOver = true	
end

function RemoteDebuggerSetting_Game:StepIn()
	self.m_IsWaittingContinue = false
	bStepIn = true	
end

function RemoteDebuggerSetting_Game:StepOut()
	self.m_IsWaittingContinue = false
	bStepOut = true	
end

function RemoteDebuggerSetting_Game:DebuggerPause( )
	self.m_bPaused = self.m_bPaused + 1
	self:CheckToRun()
end

function RemoteDebuggerSetting_Game:DebuggerResume( )
	self.m_bPaused = math.max(self.m_bPaused - 1, 0)
	self:CheckToRun()
end

function RemoteDebuggerPause()
	RemoteDebuggerSetting_Game:Get():DebuggerPause()
end

function RemoteDebuggerResume()
	RemoteDebuggerSetting_Game:Get():DebuggerResume()
end

function RemoteDebuggerSetting_Editor:EnterDebug(InFilePath, InLine, InStackData)
	self:SetStackData(InStackData.Contents, InStackData.Lines, InStackData.FilePaths, InStackData.StackIndexs, InStackData.FuncInfos)
	URemoteDebuggerSetting.EnterDebug(self, InFilePath, InLine)
end

function RemoteDebuggerSetting_Editor:ShouldRunDebug()
	return self.m_bIsTabOpen and self.m_bIsStart
end

function RemoteDebuggerSetting_Editor:TryConnectGame( )
	if self:ShouldRunDebug() then
		
		if not self.m_ClientSocket then
			self.m_ClientSocket = UnrealLuaSocketMgr:Get():CreateClient(62233)
		end
		-- a_("connect:", self.m_HasConnectedToServer)
		if not self.m_HasConnectedToServer and ServerPort then
			self.m_HasConnectedToServer = self.m_ClientSocket:TryToConnect("127.0.0.1", ServerPort)
			if self.m_HasConnectedToServer then
				self.m_ClientSocket:SetRpcIns(self)
				
			end
		end
	end
end

function RemoteDebuggerSetting_Editor:CallGame(...)
	self:TryConnectGame()
	if self.m_HasConnectedToServer then
		self.m_ClientSocket:Call(...)
	end
end

function RemoteDebuggerSetting_Editor:CallGame_r(...)
	self:TryConnectGame()
	if self.m_HasConnectedToServer then
		return self.m_ClientSocket:Callr(...)
	end
end

function RemoteDebuggerSetting_Editor:SyncState()
	LuaSourceDir = LuaSourceDir or self:GetLuaSourceDir()
	self:CallGame("SyncState", self.m_bIsTabOpen, self.m_bIsStart, BreakPoints, BreakLines, LuaSourceDir, HitConditions)
end

function RemoteDebuggerSetting_Editor:DebugContinue()
	self:CallGame("DebugContinue")
end

function RemoteDebuggerSetting_Editor:ToggleDebugStart(bIsStart)
	self.m_bIsStart = bIsStart
	self:SyncState()
end


function RemoteDebuggerSetting_Editor:GetHitCondition()
	if self.m_BreakPointFromEditor then
		HitConditions = {}
		for FilePath, LineSet in pairs(self.m_BreakPointFromEditor) do
			for LineNum in pairs(LineSet) do
				local HitCondition = tostring(self:GetBreakPointHitConditionText(FilePath, LineNum))
				if HitCondition ~= "" then
					HitConditions[FilePath] = HitConditions[FilePath] or {}
					HitConditions[FilePath][LineNum] = HitCondition
				end
			end
		end
	end
end

function RemoteDebuggerSetting_Editor:UpdateBreakPoint(InBreakPoints)
	BreakPoints = {}
	BreakLines = {}
	self.m_BreakPointFromEditor = InBreakPoints
	for FilePath, LineSet in pairs(InBreakPoints) do
		for LineNum in pairs(LineSet) do
			BreakPoints[FilePath..tostring(LineNum)] = true
			BreakLines[LineNum] = true
		end
	end
	self:GetHitCondition()
	self:SyncState()
end

function RemoteDebuggerSetting_Editor:SetTabIsOpen(bIsTabOpen)
	self.m_bIsTabOpen = bIsTabOpen
	self:SyncState()
end

function RemoteDebuggerSetting_Editor:Ctor( )
	DebuggerSingletonEditor = self
	self:PullDataToLua()
end

function RemoteDebuggerSetting_Editor:StepOver()
	self:CallGame("StepOver")
end

function RemoteDebuggerSetting_Editor:StepIn()
	self:CallGame("StepIn")
end

function RemoteDebuggerSetting_Editor:StepOut()
	self:CallGame("StepOut")
end

function RemoteDebuggerSetting_Editor:BreakConditionChange()
	self:GetHitCondition()
	self:SyncState()
end

function RemoteDebuggerSetting_Editor:GetStackVars(StackIndex)
	local ResultFromGame = self:CallGame_r("GetStackVars", StackIndex)
	local Result = {}
	for i, v in ipairs(ResultFromGame) do
		local NewNode = FDebuggerVarNode.New()
		if v.ValueWeakIndex then
			NewNode.ValueWeakIndex = v.ValueWeakIndex
		end
		NewNode.Name = v.Name
		NewNode.Value = v.Value
		table.insert(Result, NewNode)
	end
	return Result
end

function RemoteDebuggerSetting_Editor:GetVarNodeChildren(ParentNode)
	local ParentNodeGame = {} 
	ParentNodeGame.ValueWeakIndex = ParentNode.ValueWeakIndex
	ParentNodeGame.Name = tostring(ParentNode.Name)
	ParentNodeGame.Value = tostring(ParentNode.Value)
	local ResultFromGame = self:CallGame_r("GetVarNodeChildren", ParentNodeGame)
	local Result = {}
	if ResultFromGame then
		for i, v in ipairs(ResultFromGame) do
			local NewNode = FDebuggerVarNode.New()
			if v.ValueWeakIndex then
				NewNode.ValueWeakIndex = v.ValueWeakIndex
			end
			NewNode.Name = v.Name
			NewNode.Value = v.Value
			table.insert(Result, NewNode)
		end
	end
	return Result
end

if URemoteDebuggerSetting then
	RemoteDebuggerSetting_Editor:NewOn(URemoteDebuggerSetting.Get(true))
	-- local oldresume = coroutine.resume 
	-- local oldyield = coroutine.yield 
	-- local oldwrap  = coroutine.wrap 
	-- local coroutinestack = {} 
	-- local function newresume(co, ...)
	-- 	DebuggerSingleton:HookCoroutine(co)
	-- 	bPauseForMainThread = true
	-- 	UTableUtil.SetRunningState(co)
	-- 	local result = {oldresume(co, ...)}
	-- 	UTableUtil.SetRunningState(nil)
	-- 	bPauseForMainThread = false
	-- 	if coroutine.status(co) == "dead" then
	-- 		DebuggerSingleton:UnHookCoroutine(co)
	-- 	end
	-- 	return unpack(result, 1, 10)
	-- end 
	-- local function newyield(...)
	-- 	DebuggerSingleton:UnHookCoroutine(coroutine.running())
	-- 	return oldyield(...)
	-- end
	-- local function newwrap(func)
	-- 	local co = coroutine.create(func)
	-- 	local function f(...)
	-- 		newresume(co, ...)
	-- 	end
	-- 	return f
	-- end
	-- coroutine.resume = newresume
	-- coroutine.yield = newyield
	-- coroutine.wrap = newwrap
else
	if not _WITH_EDITOR then
		CreateDebuggerSever()
	end
end