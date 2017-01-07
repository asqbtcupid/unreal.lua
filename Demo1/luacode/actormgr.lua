ActorMgr = Inherit(Singleton)

function ActorMgr:Ctor()
	self.m_ExistActor = {}
end

function ActorMgr:BindActor(v, classpath)
	if not self.m_ExistActor[v] then 
		local ins = require (classpath):NewIns()
		ins:Bind(v)
		self.m_ExistActor[v] = ins
	end
	return self.m_ExistActor[v]
end

function ActorMgr:CtorCpp(this, classpath, ...)
	local class = require (classpath)
	class._cppclass:cast(this)
	class:CtorFromCpp(this, ...)
end

return ActorMgr