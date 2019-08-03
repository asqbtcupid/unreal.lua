ConfigMgr = Inherit(Singleton)
function ConfigMgr:Ctor()
	self.m_CppIns = UNZLuaHelperLibrary.GetConfMgr()
end

local function GetUserId(ins)
	local LoginData = GetCppSingleton(ins, "UNZLoginDataServer")
	local userid = LoginData:GetUin()
	assert(userid ~= nil)
	return userid
end

function ConfigMgr:GetBool(key)
	return self.m_CppIns:GetBoolen(key)
end

function ConfigMgr:SetBool(key, value)
	return self.m_CppIns:SetBoolen(key, value)
end

function ConfigMgr:GetInt(key)
	return self.m_CppIns:GetInt(key)
end

function ConfigMgr:SetInt(key, value)
	return self.m_CppIns:SetInt(key, value)
end

function ConfigMgr:GetBoolUni(ins, key)
	key = key..tostring(GetUserId(ins))
	return self:GetBool(key)
end

function ConfigMgr:SetBoolUni(ins, key, value)
	key = key..tostring(GetUserId(ins))
	return self:SetBool(key, value)
end

function ConfigMgr:GetIntUni(ins, key)
	key = key..tostring(GetUserId(ins))
	return self:GetInt(key)
end

function ConfigMgr:SetIntUni(ins, key)
	key = key..tostring(GetUserId(ins))
	return self:SetInt(key, value)
end

return ConfigMgr