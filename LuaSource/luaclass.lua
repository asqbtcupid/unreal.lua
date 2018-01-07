function clone(t, cache)
	if type(t) ~= "table" then
		return t
	end
	cache = cache or {}
	local result = {}	
	cache[t] = result
	for k, v in pairs(t) do
		if type(v) == "table" then
			if not cache[v] then cache[v] = clone(v, cache) end
			result[k] = cache[v]
		else
			result[k] = v
		end
	end
	return result
end

function addfunc(des_t, src_t)
	for k,v in pairs(src_t) do
		if type(v) == "function" and k ~= "Ctor" and k ~= "Destroy" and not des_t[k] then
			des_t[k] = v
		end
	end
end

Object = {}
Object.__index = Object
Object._meta_ = Object
function Object:Ctor(...)
end

function Object:Destroy()
end

function Object:IsChildOf(parent)
	if rawget(self, "_meta_") == nil then
		error("not class")
	end
	local direct_parent = self:Super()
	if direct_parent == parent then
		return true
	elseif direct_parent then
		return direct_parent:IsChildOf(parent)
	else
		return false
	end
end

function Object:Super()
	local meta = self._meta_
	return meta and meta._parentclass
end

function Object:Ins()
	if rawget(self, "_meta_") == nil then
		error("not class")
	end
	local newIns = setmetatable({}, self)
	return newIns
end

function Object:NewIns(...)
	local ins = self:Ins()
	ins:Initialize(...)
	return ins
end

local CtorStr = "Ctor"
local function CtorRecursively(theclass, ins, ...)
	if theclass == nil then return end
	local super = theclass:Super()
	if super then
		CtorRecursively(super, ins, ...)
	end
	local Ctor = rawget(theclass, CtorStr)
	if Ctor then Ctor(ins, ...) end
end

function Object:Initialize(...)
	if rawget(self, "_meta_") then
		error("not ins")
	end
	CtorRecursively(self._meta_, self, ...)
end

local function InternDestroy(theclass, ins, ...)
	if theclass == nil then return end
	local Destroy = rawget(theclass, "Destroy")
	if  Destroy then
		Destroy(ins, ...)
	end
	local super = theclass.Super and theclass:Super()
	if super then
		InternDestroy(super, ins, ...)
	end
end

function Object:Release(...)
	if rawget(self, "_meta_") then
		error("not ins")
	end
	if self._has_destroy_ then
		return
	end
	InternDestroy(self._meta_, self, ...)
	self._has_destroy_ = true
end

local _parentclass = "_parentclass"
local GetPrefix = "LuaGet_"
local _cppinstance_str = "_cppinstance_"
local function __indexcpp(t, k)
	local GetFuncKey = GetPrefix..tostring(k)
	local getfunc = rawget(t, GetFuncKey)
	if getfunc then return getfunc(t) end

	local class = getmetatable(t)
	while class do
		local v = rawget(class, k)
		if v then 
			if type(v) == "function" then
				rawset(t, k, v)
			end
			return v 
		end 

		v = rawget(class, GetFuncKey)
		if type(v) == "function" then
			rawset(t, GetFuncKey, v)
			return v(t)
		end

		class = getmetatable(class) 
	end

	local _cppinstance_ = rawget(t, _cppinstance_str)
	if _cppinstance_ then
		local v = _cppinstance_[k] 
		if v then 
			if type(v) == "function" then
				rawset(t, k, v)
			end
			return v 
		end 

		v = _cppinstance_[GetFuncKey]
		if type(v) == "function" then
			rawset(t, GetFuncKey, v)
			return v(t)
		end
	end

end

local _cppclass = "_cppclass"
local SetPrefix = "LuaSet_"
local function __newindexcpp(t, k, v)
	local SetFuncKey = SetPrefix..tostring(k)
	local f = rawget(t, SetFuncKey)
	if f then f(t, v); return end

	local class = getmetatable(t)

	while class do
		local set_f = rawget(class, SetFuncKey)
		if type(set_f) == "function" then 
			rawset(t, SetFuncKey, set_f)
			set_f(t, v) 
			return
		end 
		class = getmetatable(class) 
	end

	local _cppinstance_ = rawget(t, _cppinstance_str)
	if _cppinstance_ then
		local set_f = _cppinstance_[SetFuncKey]
		if type(set_f) == "function" then
			rawset(t, SetFuncKey, set_f)
			set_f(t, v) 
			return
		end
	end

	rawset(t, k, v)
end

function Inherit(parent)
	local TheNewClass = {}
	TheNewClass.__index = __indexcpp
	TheNewClass.__newindex = __newindexcpp
	TheNewClass._parentclass = parent
	-- **********for luahotupdate************
	if not _WITH_EDITOR then
		addfunc(TheNewClass, parent)
	else
		setmetatable(TheNewClass, parent)
	end
	-- **************************************
	rawset(TheNewClass, "_meta_", TheNewClass)
	return TheNewClass
end

Class = Inherit

ObjectBase = Inherit(Object)
ObjectBase.New = Object.NewIns
function ObjectBase:Ctor()
	rawset(self, "_timer_handles_", {})
	rawset(self, "_event_handles_", {})
	rawset(self, "_childs_", {})
end

function ObjectBase:Destroy()
	for i, v in ipairs(self._timer_handles_) do
		v:Destroy()
	end
	for i, v in ipairs(self._event_handles_) do
		v:Destroy()
	end
	for i, v in ipairs(self._childs_) do
		v:Release()
	end
end

function ObjectBase:Timer(...)
	local handle = TimerMgr:Get():On(...):Bound(true)
	table.insert(self._timer_handles_, handle)
	return handle
end

function ObjectBase:On(...)
	local handle = GlobalEvent.On(...)
	table.insert(self._event_handles_, handle)
	return handle
end

function ObjectBase:AddChild(ins)
	if not ins._meta_:IsChildOf(Object) then
		error("not class")
	else
		table.insert(self._childs_, ins)
		return ins
	end
end

Singleton = Inherit(ObjectBase)
function Singleton:Ins(...)
	local meta = self._meta_
	if meta._ins == nil then
		meta._ins = setmetatable({}, self)
	end
	return meta._ins
end

function Singleton:Get(...)
	local meta = self._meta_
	if rawget(meta, "_ins") == nil then
		local ins = setmetatable({}, self)
		rawset(meta, "_ins", ins)
		ins:Initialize(...)
	end
	return meta._ins
end

function Singleton:GetRaw()
	return rawget(self._meta_, "_ins")
end

function Singleton:Release(...)
	local meta = self._meta_
	if rawget(meta, "_ins") then
		InternDestroy(meta, meta._ins, ...)
	end
end

function Singleton:Destroy()
	rawset(self._meta_,"_ins", nil)
end

local map = {
	__indexcpp
}
return map