ActorMgr = Inherit(Singleton)

function ActorMgr:Ctor()
	local meta = {__mode = "k,v"}
	self.m_ExistObject = setmetatable({}, meta)
end

function ActorMgr:BindActor(inscpp, classpath)
	if not self.m_ExistObject[inscpp] then 
		local ins = require (classpath):NewOn(inscpp)
		self.m_ExistObject[inscpp] = ins
	end
	return self.m_ExistObject[inscpp]
end

function ActorMgr:CtorCpp(inscpp, classpath, ...)
	local class = require (classpath)
	class:CtorFromCpp(inscpp, ...)
	self.m_ExistObject[inscpp] = class:NewOn(inscpp, ...)
end

function ActorMgr:CallLuaInsFunc(inscpp, classpath, functionName, ...)
	local luains = self:BindActor(inscpp, classpath)
	luains[functionName](luains, ...)
end

function ActorMgr:DestroyActor(ins)
	if type(ins) == "table" then
		self.m_ExistObject[ins._cppinstance_] = nil
	else
		self.m_ExistObject[ins] = nil
	end
end

return ActorMgr
