local DebuggerSetting = Inherit(CppObjectBase)

local DebuggerSingleton
local LuaSourceDir 
local BreakPoints
function DebuggerSetting:Ctor()
	LuaSourceDir = self:GetLuaSourceDir()
	DebuggerSingleton = self
	self.m_bIsStart = false
	self.m_bIsDebuging = false
	self:Timer(self.Tick, self):Time(0.0001)
	self:PullDataToLua()
end

function DebuggerSetting:Tick( )
end
local Cached = {}
local function GetFullFilePath(Path)
	local TheCached = Cached[Path]
	if not TheCached then
		local FullPath = string.match(Path, ".*LuaSource(.*%.lua)")
		if FullPath then
			FullPath = string.gsub(FullPath, "\\", "/")
			Cached[Path] = LuaSourceDir..FullPath
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
		local StackInfo = getinfo(i)
		if not StackInfo then break end
		local FilePath = GetFullFilePath(StackInfo.short_src)
		local Line = StackInfo.currentline
		local LuaPath = GetLuaPath(FilePath)
		local Content = LuaPath..":"..tostring(StackInfo.name).." Line"..tostring(Line)
		table.insert(Contents, Content)
		table.insert(FilePaths, FilePath)
		table.insert(Lines, Line)
		table.insert(StackIndexs, i)
	end
	DebuggerSingleton:SetStackData(Contents, Lines, FilePaths, StackIndexs)
end

local function HookCallBack(Event, Line)
	local StackInfo = getinfo(2)
	local FilePath = GetFullFilePath(StackInfo.short_src)
	if IsHitBreakPoint(FilePath, Line) then
		CollectStackData()
		DebuggerSingleton:EnterDebug(FilePath, Line)
	end
end

function DebuggerSetting:CheckToRun()
	local function ShouldRunDebug()
		return self.m_bIsTabOpen and self.m_bIsStart and self:HasAnyBreakPoint()
	end
	if self.m_bIsDebuging then
		if not ShouldRunDebug() then
			debug.sethook()
			self.m_bIsDebuging = false
		end	
	else
		if ShouldRunDebug() then
			debug.sethook(HookCallBack, "l")
			self.m_bIsDebuging = true
		end	
	end
end

function DebuggerSetting:ToggleDebugStart(bIsStart)
	self.m_bIsStart = bIsStart
	self:CheckToRun()
end

function DebuggerSetting:UpdateBreakPoint(BreakPoint)
	BreakPoints = BreakPoint
	self:CheckToRun()
end

function DebuggerSetting:SetTabIsOpen(bIsTabOpen)
	self.m_bIsTabOpen = bIsTabOpen
	self:CheckToRun()
end

function DebuggerSetting:HasAnyBreakPoint( )
	if BreakPoints then
		for FilePath, LineSet in pairs(BreakPoints) do
			for LineNum in pairs(LineSet) do
				return true
			end
		end
	end
	return false
end

function DebuggerSetting:Get( )
	return DebuggerSingleton
end

if UDebuggerSetting then
	DebuggerSetting:NewOn(UDebuggerSetting.Get())
end
return DebuggerSetting
