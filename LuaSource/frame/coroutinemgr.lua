local Handle = Inherit(Object)

local weakmeta = {__mode = "v"}
function Handle:Ctor(mgr, newcoroutine)
	self.mgr = mgr
	self.coroutine = newcoroutine
	self.OwnerTable = setmetatable({}, weakmeta)
end

function Handle:Resume( ... )
	if self.mgr:Exist(self) then
		local result = {coroutine.resume(self.coroutine, ...)}
		if coroutine.status(self.coroutine) == "dead" then
			self:Destroy()
		end
		ensure(result[1], result[2])
		return unpack(result, 1, 10)
	end
end

function Handle:Yield( ... )
	-- if self.mgr:Exist(self) then
		return coroutine.yield(...)
	-- end
end

function Handle:GetStatus()
	ensure(self.coroutine ~= nil, "self.coroutine is nil")
	return coroutine.status(self.coroutine)
end

function Handle:IsDead()
	return self:GetStatus() == "dead"
end

function Handle:SetOwner(ObjectBaseOnwer)
	self.OwnerTable[1] = ObjectBaseOnwer
end

function Handle:GetOwner()
	return self.OwnerTable[1]
end

function Handle:Destroy()
	self.m_NotValid = true
	self.mgr:DestroyHandle(self)
	local Owner = self.OwnerTable[1]
	if Owner then
		Owner:RemoveCoHandle(self)
	end
end

function Handle:IsValid()
	return self.m_NotValid ~= true
end

CoroutineMgr = Inherit(Singleton)
function CoroutineMgr:Ctor()
	self.handles={}
end

function CoroutineMgr:DestroyHandle(handle)
	self.handles[handle] = nil
	self.handles[handle.coroutine] = nil
end

function CoroutineMgr:Exist(co_or_handle)
	return self.handles[co_or_handle] ~= nil
end

function CoroutineMgr:GetHandle(co)
	local co = coroutine.running()
	return co and self.handles[co]
end

function CoroutineMgr:GetHandleOfCo(co)
	ensure(co ~= nil, "co is nil")
	return co and self.handles[co]
end


function CoroutineMgr:Create(func)
	local newcoroutine = coroutine.create(func)
	local newhandle = Handle:NewIns(self, newcoroutine)
	self.handles[newcoroutine] = newhandle
	self.handles[newhandle] = newcoroutine
	return newhandle
end

return CoroutineMgr