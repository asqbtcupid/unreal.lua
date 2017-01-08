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
	if self.m_ExistObject[inscpp] then return end
	local class = require (classpath)
	class:CtorFromCpp(inscpp, ...)
	self.m_ExistObject[inscpp] = class:NewOn(inscpp, ...)
end

function ActorMgr:BeginPlay(inscpp, classpath, ...)
	local inslua = self:BindActor(inscpp, classpath)
	if inslua.BeginPlaylua then
		inslua:BeginPlaylua(...)
	end
end

return ActorMgr
