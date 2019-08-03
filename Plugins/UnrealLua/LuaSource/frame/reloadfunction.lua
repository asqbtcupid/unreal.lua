local Helper = {}

local function InitFileMap(BaseDirPath)
	local IFile = IFileManager.Get()
	local Files = TArray_FString.New()
	IFile:FindFilesRecursive(Files, BaseDirPath, "*.lua", true, false, false)
	local Files = Files:Table()
	local FileMap = {}
	for i, SysPath in ipairs(Files) do
		local luapath = string.sub(SysPath, #BaseDirPath+1, #SysPath-4)
		luapath = string.gsub(luapath, "/", ".")

		local FileName = string.match(SysPath,".*/(.*)%.lua")
		FileMap[FileName] = FileMap[FileName] or {}
		table.insert(FileMap[FileName], {SysPath = SysPath, LuaPath = luapath})
	end
	return FileMap
end

local function WindowsInitFileMap(RootPath)
	return InitFileMap(FPaths.ProjectPluginsDir().."UnrealLua/LuaSource/")
end

local function LoadStringFunc(FileSysPath)
	local IFile = IFileManager.Get()
	if IFile:FileExists(FileSysPath) then
		local IsTrue, String = FFileHelper.LoadFileToString("", FileSysPath, 0)
		return String
	end
	return ""
end

local function AndroidInitFileMap(RootPath)
	return InitFileMap(FPaths.ProjectSavedDir().."LuaSource/")
end

Helper.LoadStringFunc = LoadStringFunc
local PlatFormName = UGameplayStatics.GetPlatformName()
if PlatFormName == "Windows" then
	Helper.InitFileMap = WindowsInitFileMap
elseif PlatFormName == "Android" then
	Helper.InitFileMap = AndroidInitFileMap
else
	Helper.NotRun = true
end
Helper.RawFindFile = InitFileMap

return Helper