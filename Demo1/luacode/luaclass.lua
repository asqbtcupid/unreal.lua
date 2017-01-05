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

function Inherit(parent)
	local TheNewClass = {}
	TheNewClass._parentclass = parent
	TheNewClass.__index = TheNewClass
	if parent.__iscppclass then
		TheNewClass._cppclass = parent._cppclass or parent
		function TheNewClass:Bind(userData)
			self._cppclass:cast(userData) 
			self._cppinstance_ = userData 
		end
		TheNewClass.NewIns = Object.NewIns
		TheNewClass.Super = Object.Super
		local newmeta = {}
		newmeta.__index = parent
		newmeta.__newindex = function (t, k, v)
			if parent["Set_"..k] then
				parent[k] = v
			else
				rawset(t, k, v)
			end
		end
		setmetatable(TheNewClass, newmeta)
	else
		setmetatable(TheNewClass, parent)
	end
	return TheNewClass
end



local function InternDestroy(theclass, ins, ...)
	if theclass == nil then return end
	local Destroy = rawget(theclass, "Destroy")
	if Destroy then Destroy(ins, ...) end
	InternDestroy(theclass:Super(), ins, ...)
end

function Object:Release(...)
	InternDestroy(getmetatable(self), self, ...)
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
