ActorMgr = Inherit(Singleton)

function ActorMgr:Ctor()
	-- local meta = {__mode = "k,v"}
	-- self.m_ExistObject = setmetatable({}, meta)
	self.m_ExistObject = {}
	self.m_OnDestroyed = {}
end

function ActorMgr:BindActor(inscpp, classpath)
	self.m_ExistObject[classpath] = self.m_ExistObject[classpath] or {}
	if not self.m_ExistObject[classpath][inscpp] then 
		local ins = require (classpath):NewOn(inscpp)
		self.m_ExistObject[classpath][inscpp] = ins
	end

	return self.m_ExistObject[classpath][inscpp]
end

function ActorMgr:CtorCpp(inscpp, classpath, ...)
	if self.m_ExistObject[classpath] and self.m_ExistObject[classpath][inscpp] then return end
	local class = require (classpath)
	local luains = self:BindActor(inscpp, classpath)
	class:CtorFromCpp(luains, ...)
end

function ActorMgr:CallLuaInsFunc(inscpp, classpath, functionName, ...)
	local luains = self:BindActor(inscpp, classpath)
	if not self.m_OnDestroyed[inscpp] then
		self.m_OnDestroyed[inscpp] = inscpp.OnDestroyed
		local f = MakeCallBack(self.DestroyActor, self)
		self.m_OnDestroyed[inscpp]:Add(f)
	end
	return luains[functionName](luains, ...)
end

function ActorMgr:DestroyActor(ins)
	local inscpp = ins
	if type(ins) == "table" then
		inscpp = ins._cppinstance_
	end
	for k, v in pairs( self.m_ExistObject) do
		v[inscpp] = nil
	end
	self.m_OnDestroyed[inscpp] = nil
end

function ActorMgr:GetIns(inscpp, classpath)
	if classpath then
		return self:BindActor(inscpp, classpath)
	else
		return inscpp
	end
end

return ActorMgr
