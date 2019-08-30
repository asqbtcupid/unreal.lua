-- 克隆一个table，例如 local same = clone(sometable)
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

function DeepCopy(dst, src)
	for k, v in pairs(src) do
		if type(v) == "table" then
			if type(dst[k]) ~= "table" then
				dst[k] = {}
			end
			DeepCopy(dst[k], v)
		else
			dst[k] = v
		end

	end
end

function addfunc(des_t, src_t)
	for k,v in pairs(src_t) do
		if type(v) == "function" and k ~= "Ctor" and k ~= "Destroy" and not des_t[k] then
			des_t[k] = v
		end
	end
end
-- 类系统里最基本的类
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
	local newIns = setmetatable({nil,{},{}}, self)
	return newIns
end
-- 创建类的一个实例，...参数会传到构造函数Ctor里
function Object:NewIns(...)
	local ins = self:Ins()
	ins:Initialize(...)
	return ins
end
-- 每次生成实例时，会调用这个函数，负责从父类到子类的顺序，依次调用Ctor函数
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
-- 每次release实例时，会调用这个函数，负责从子类到父类的顺序，依次调用Destroy函数
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
-- Release一个对象，需要手动调用，...会当成参数传入Destroy函数
function Object:Release(...)
	if rawget(self, "_meta_") then
		error("not ins")
	end
	if self._has_destroy_ then
		return
	end
	self._has_destroy_ = true
	InternDestroy(self._meta_, self, ...)
end
-- 是否存在某个luafunction
function IsExistLuaFunc(t, functionName)
	local class = getmetatable(t)
	while class do
		local v = rawget(class, functionName)
		if v then 
			if type(v) == "function" then
				return true 
			end
		end 
		class = getmetatable(class) 
	end
    return false
end


local _parentclass = "_parentclass"
local _cppinstance_str = 1
local funcstr = "function"
local strstr = "string"
local GetStr = "LuaGet_"
local Userdatastr = "userdata"
local TableStr = "table"
-- index函数，先找lua类里，再找c++类里的
local function __indexcpp(t, k)
	local PropertyGetFunc = rawget(t, 2)
	local CppGetFunc = PropertyGetFunc and PropertyGetFunc[k]
	if CppGetFunc then
		local GetFunc = CppGetFunc[1]
		if GetFunc then
			return GetFunc(t)
		else
			return CppGetFunc[2]
		end
	end
	local class = getmetatable(t)
	while class do
		local v = rawget(class, k)
		if v then 
			if type(v) == funcstr then
				rawset(t, k, v)
			end
			return v 
		end 
		class = getmetatable(class) 
	end
	
	local _cppinstance_ = rawget(t, _cppinstance_str)
	if _cppinstance_ then 
		local v = _cppinstance_[k] 
		local typeof = type(v)
		if typeof == funcstr then
			rawset(t, k, v)
		else
			if typeof == Userdatastr and not v.IsObject then
				PropertyGetFunc[k] = {nil, v, 1}
			else
				local GetFunc = _cppinstance_[GetStr..tostring(k)]
				if GetFunc then
					PropertyGetFunc[k] = {GetFunc,nil,1}
				end
			end
		end
		return v
	end
end

local trynewindex = __trynewindex
local trynewindexstr = "__trynewindex"
local SetStr = "LuaSet_"
local function __newindexcpp(t, k, v)
	local PropertySetFunc = rawget(t, 3)
	local CppSetFunc = PropertySetFunc and PropertySetFunc[k]
	if CppSetFunc then
		CppSetFunc(t, nil, v)
		return
	end 
	if type(k) == strstr then
		local _cppinstance_ = rawget(t, _cppinstance_str)
		if _cppinstance_  then
			local cppmeta = getmetatable(_cppinstance_)
			if cppmeta then
				local __trynewindex = rawget(cppmeta, trynewindexstr)
				if __trynewindex then
					if __trynewindex(_cppinstance_, k, v) then
						PropertySetFunc[k] = _cppinstance_[SetStr..tostring(k)]
						return
					end
				end
			end
		end
	end
	rawset(t, k, v)
end

-- 继承parent来创建新类
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
addfunc(ObjectBase, require "objectbasefunction")
-- local weakmeta = {__mode = "kv"}
function ObjectBase:Ctor()
	rawset(self, "_timer_handles_", {})
	rawset(self, "_event_handles_", {})
	rawset(self, "_childs_", {})
	-- rawset(self, "_coroutine_", setmetatable({}, weakmeta))
	rawset(self, "_coroutine_", {})
	rawset(self, "_NeedToDestroy_", {})
end

function ObjectBase:Destroy()
	local ItemToDestroy = {}
	local function TravelToAddDestroyItem(Key, bDestroyKey, bDestroyValue)
		for i, v in pairs(self[Key]) do
			if bDestroyKey then
				ItemToDestroy[i] = true
			end
			if bDestroyValue then
				ItemToDestroy[v] = true
			end
		end
		self[Key] = nil
	end
	TravelToAddDestroyItem("_timer_handles_", true, false)
	TravelToAddDestroyItem("_event_handles_", true, false)
	TravelToAddDestroyItem("_childs_", false, true)
	TravelToAddDestroyItem("_coroutine_", true, false)
	TravelToAddDestroyItem("_NeedToDestroy_", true, false)

	for v in pairs(ItemToDestroy) do
		v:Destroy()
	end
end

function ObjectBase:Diewith(Thing)
	self._NeedToDestroy_[Thing] = true
end

function ObjectBase:RemoveTimerHandle(handle)
	if self._timer_handles_ then
		self._timer_handles_[handle] = nil
	end
end

function ObjectBase:RemoveCoHandle(handle)
	if self._coroutine_ then
		self._coroutine_[handle] = nil
	end
end

function ObjectBase:RemoveEventHandle(handle)
	if self._event_handles_ then
		self._event_handles_[handle] = nil
	end
end
-- 创建timer
function ObjectBase:Timer(...)
	local handle = TimerMgr:Get():On(...):Bound(true)
	handle:SetOwner(self)
	self._timer_handles_[handle] = true
	return handle
end

-- 侦听全局事件
function ObjectBase:On(...)
	local handle = GlobalEvent.On(...)
	handle.SetOwner(self)
	self._event_handles_[handle] = true
	return handle
end

function ObjectBase:Data(name)
	return GlobalEvent.Data(name)
end

function ObjectBase:Fire(...)
	return GlobalEvent.Fire(...)
end

function ObjectBase:OnProto(ProtoName, ...)
	ProtoMgr:Get():Listen(ProtoName)
	return self:On(ProtoName, ...)
end

function ObjectBase:SendProto(ProtoName, TheTable, ...)
	ProtoMgr:Get():LuaSendProto(ProtoName, TheTable, ...)
	return self
end

function ObjectBase:DataProto(ProtoName)
	assert(type(ProtoName) == "string")
	return GlobalEvent.Data(ProtoName)
end
-- 以协程的方式进入一个函数，在这个函数里可以调用CoroutineUtil系列的协程函数
function ObjectBase:Coroutine(func, ...)
	local handle = CoroutineMgr:Get():Create(func)
	self._coroutine_[handle] = true
	handle:SetOwner(self)
	return handle, handle:Resume(...)
end

function ObjectBase:AddGcChild(ins)
	if not ins._meta_:IsChildOf(Object) then
		error("not class")
	else
		table.insert(self._childs_, ins)
		return ins
	end
end
-- 获取一个名字为Name的红点数据
function ObjectBase:Red(Name)
	return RedPointMgr:Get():GetData(Name)
end

Singleton = Inherit(ObjectBase)
function Singleton:Ins(...)
	local meta = self._meta_
	if meta._ins == nil then
		meta._ins = setmetatable({}, self)
	end
	return meta._ins
end
-- 第一次调用时会创建一个实例，之后调用返回该实例，实现单例模式
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
-- 销毁单例
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