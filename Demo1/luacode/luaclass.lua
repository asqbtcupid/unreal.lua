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

Object = {}
Object.__index = Object
function Object:Ctor(...)
end

function Object:Super()
	return self._parentclass
end

local function CtorRecursively(theclass, ins, ...)
	local super = theclass.Super and theclass:Super()
	if super then
		CtorRecursively(super, ins, ...)
	end
	local Ctor = rawget(theclass, "Ctor")
	if Ctor then Ctor(ins, ...) end
end

function Object:NewIns(...)
	local newIns = setmetatable({}, self)
	CtorRecursively(self, newIns, ...)
	return newIns
end

local function InternDestroy(theclass, ins, ...)
	if theclass == nil then return end
	local Destroy = rawget(theclass, "Destroy")
	if Destroy then Destroy(ins, ...) end
	local super = theclass.Super and theclass:Super()
	if super then
		InternDestroy(super, ins, ...)
	end
end

function Object:Release(...)
	InternDestroy(getmetatable(self), self, ...)
end


local function __indexcpp(t, k)
	local class = getmetatable(t)
	while class do
		local v = rawget(class, k)
		if v then return v end 
		v = rawget(class, "Get_"..k)
		if v then return v(t) end
		class = class.Super and class:Super() 
	end
end

local function __newindexcpp(t, k, v)
	local class = getmetatable(t)
	while class do
		local f = class["Set_"..k]
		if f then f(t, v); return end
		class = class.Super and class:Super() 
	end
	rawset(t, k, v)
end

local function CtorCppRecursively(theclass, inscpp, ...)
	local super = theclass.Super and theclass:Super()
	if super then
		CtorRecursively(super, inscpp, ...)
	end
	local CtorCpp = rawget(theclass, "CtorCpp")
	if CtorCpp then CtorCpp(inscpp, ...) end
end

local function CtorFromCpp(theclass, inscpp, ...)
	theclass._cppclass:cast(inscpp)
	CtorCppRecursively(theclass, inscpp, ...)
end

local function NewInsCpp(self, ...)
	self._cppinstance_ = self._cppclass.New()
	self:NewIns(...)
end

local function Bind(self, inscpp)
	self._cppclass:cast(inscpp) 
	self._cppinstance_ = inscpp 
end

local function NewOn(self, inscpp, ...)
	local newIns = setmetatable({}, self)
	self.Bind(newIns, inscpp)
	CtorRecursively(self, newIns, ...)
	return newIns
end

local function DestroyCpp(self)
	Object.Release(self)
	self:K2_DestroyActor()
	ActorMgr:Get():DestroyActor(self)
end

function Inherit(parent)
	local TheNewClass = {}
	TheNewClass._parentclass = parent
	if parent.__iscppclass then
		TheNewClass._cppclass = parent._cppclass or parent
		TheNewClass.Bind = Bind
		TheNewClass.NewOn = NewOn
		TheNewClass.NewInsCpp = NewInsCpp
		TheNewClass.NewIns = Object.NewIns
		TheNewClass.CtorFromCpp = CtorFromCpp
		TheNewClass.DestroyCpp = DestroyCpp
		TheNewClass.Super = Object.Super
		TheNewClass.__index = __indexcpp
		TheNewClass.__newindex = __newindexcpp
		setmetatable(TheNewClass, parent)
	else
		TheNewClass.__index = TheNewClass
		setmetatable(TheNewClass, parent)
	end
	return TheNewClass
end

Singleton = Inherit(Object)
function Singleton:Get(...)
	if self._ins == nil then
		local newins = self:NewIns(...)
		self._ins = newins
	end
	return self._ins
end

function Singleton:Destroy()
	getmetatable(self)._ins = nil
end
