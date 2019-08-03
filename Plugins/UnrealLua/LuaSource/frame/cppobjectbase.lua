require "luaclass"
CppObjectBase = Class(ObjectBase)
CppSingleton = Class(CppObjectBase)
addfunc(CppSingleton, Singleton)
local setexisttable = _setexisttable 
local LinkAgainstGC = {}
local LevelActors = {}
local Test = {}
local HasWatchLevelActors
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
	LinkAgainstGC[self] = nil
	LevelActors[self] = nil
end

function CppObjectBase:BeginPlay()
	if not HasWatchLevelActors and _WITH_EDITOR then
		HasWatchLevelActors = true
		LuaVarWatcher:Get():AddNode("LevelActors", LevelActors)
	end
	if not self.m_HasBeginPlay then
		self.m_HasBeginPlay = true
		-- actor
		self.OnEndPlay:Add(InsCallBack(self.EndPlay, self))
		if UActorComponent.Cast(self) then
			self:Link(self:GetOwner())
		end
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
	rawset(self, "_PS", {})
	rawset(self, "_PG", {})
	if self.IsObject and AActor.Cast(self) then
		LevelActors[self] = true
	end
end

function CppObjectBase:GC(obj)
	if obj then
		self._gc_list[obj] = true
	end
end

function CppObjectBase:Link(Actor)
	if Actor and AActor.Cast(Actor) then
		if type(Actor) == "table" then
			Actor:GC(self)
		else
			LinkAgainstGC[self] = true
			local destroy_delegate = Actor.OnEndPlay
			self:GC(destroy_delegate)
			local function f(ins)
				ins:Release()
			end
			destroy_delegate:Add(InsCallBack(f, self))
		end
	else
		-- error("can only link to Actor")
	end
end

function CppObjectBase:TryLinkOuter()
	local Outer = self:GetOuter()
	while Outer do
		if type(Outer) == "table" then
			Outer:GC(self)
			return true
		elseif AActor.Cast(Actor) then
			LinkAgainstGC[self] = true
			local destroy_delegate = Actor.OnEndPlay
			self:GC(destroy_delegate)
			local function f(ins)
				ins:Release()
			end
			destroy_delegate:Add(InsCallBack(f, self))
			return true
		else
			Outer = Outer:GetOuter()
		end
	end
	return false
end

function CppObjectBase:Property(property)
	return rawget(self._meta_, property)	
end

function CppObjectBase:NewOn(inscpp, ...)
	local ins = self:Ins()
	rawset(ins, "_cppinstance_", inscpp)
	rawset(ins, "_cppinstance_meta_", getmetatable(inscpp))

	setexisttable(inscpp, ins)
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

function OnWorldCleanup(World, bSessionEnded, bCleanupResources)
	local ActorToDelete = {}
	for Actor in pairs(LevelActors) do
		local ActorWorld = ULuautils.GetActorWorld(Actor)
		if not ActorWorld or ActorWorld == World then
			table.insert(ActorToDelete, Actor)
		end
	end
	for i, Actor in ipairs(ActorToDelete) do
		Actor:Release()
		LevelActors[Actor] = nil
	end
	ActorToDelete = nil
	World = nil
end

return CppObjectBase