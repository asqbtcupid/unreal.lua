function Cfg(path)
	return require ("config."..path)
end

-- requirecpp "FPlatformProperties"
-- function GetIniConfigPath(ConfigName)
-- 	return FPaths.ConvertRelativePathToFull(FPaths.GeneratedConfigDir()..FPlatformProperties.PlatformName().."/"..ConfigName..".ini")
-- end

-- requirecpp"FConfigCacheIni"
-- -- exmaple:GConfig:GetString("[/Script/NZFrontEnd.NZLoginUIView]", "CustomServiceUrl", "", GetIniConfigPath("UI"))
-- GConfig = FConfigCacheIni.GConfig()


