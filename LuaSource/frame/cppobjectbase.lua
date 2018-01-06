require "luaclass"
CppObjectBase = Class(ObjectBase)
CppSingleton = Class(CppObjectBase)
addfunc(CppSingleton, Singleton)

local LevelActors = {}
local function GcActors()
	LevelActors = ULuautils.GCThisSet(LevelActors)
end
GlobalEvent.On("GC", GcActors)

function CppObjectBase:EndPlay(Reason)
	if not self.m_HasEndPlay then
		self.m_HasEndPlay = true
		self:Release()
	end
end

function CppObjectBase:Destroy()
	for v in pairs(self._gc_list) do
		if v.Release then
			v:Release()
		end
	end
end

function CppObjectBase:BeginPlay()
	if not self.m_HasBeginPlay then
		self.m_HasBeginPlay = true
		local OnEndPlayDelegate = self.OnEndPlay
		self:GC(OnEndPlayDelegate)
		OnEndPlayDelegate:Add(self.EndPlay)
	end
end

function CppObjectBase:GetName()
	return ULuautils.GetName(self)		 
end

function CppObjectBase:NewCpp(...)
	if not rawget(self, "_meta_") then
		error("not class")
	end
	return self._cppclass.New(...)
end

CppObjectBase.New = CppObjectBase.NewCpp
CppObjectBase.__iscppclass = true
function CppObjectBase:Ctor()
	rawset(self, "_gc_list", {})
	if AActor.Cast(self) then
		LevelActors[self] = true
	end
end

function CppObjectBase:GC(obj)
	if obj then
		self._gc_list[obj] = true
	end
end

function CppObjectBase:Property(property)
	return rawget(self._meta_, property)	
end

function CppObjectBase:NewOn(inscpp, ...)
	local ins = self:Ins()
	rawset(ins, "_cppinstance_", inscpp)
	_objectins2luatable[inscpp] = ins
	ins:Initialize(...)
	return ins
end

function CppObjectBase:IsAuth()
	return self.Role == ENetRole.ROLE_Authority
end

function CppSingleton:NewOn(...)
	if self:GetRaw() then
		error("CppSingleton NewOn error")
	else
		local ins =  CppObjectBase.NewOn(self, ...)
		self._meta_._ins = ins
		return ins
	end
end

function CppSingleton:Get(...)
	local meta = self._meta_
	if rawget(meta, "_ins") == nil then
		rawset(meta, "_ins", self:NewCpp(...))
	end
	return meta._ins
end



return CppObjectBase