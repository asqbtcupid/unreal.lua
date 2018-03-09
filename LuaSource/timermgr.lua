local Handle = Inherit(Object)

local weakmeta = {__mode = "v"}
function Handle:Ctor(mgr, callback, EventHandle)
	self.passtime = 0
	self.num = 0
	self.totalnum = nil
	self.mgr = mgr
	self.callback = callback
	self.bIsBound = false
	self.EventHandle = EventHandle
	self.OwnerTable = setmetatable({}, weakmeta)
end

function Handle:CallBack( ... )
	if self.bIsBound then
		if self.EventHandle:GetParam()[1] == nil then
			self:Destroy()
			return
		end
	end
	self.callback(...)
	self.passtime = 0
	self.num = self.num + 1
	if self.totalnum and self.totalnum <= self.num then
		self:Destroy()
	end
end

function Handle:Bound(IsTrue)
	self.bIsBound = IsTrue
	return self
end

function Handle:Time(t)
	self.interval = t
	return self
end

function Handle:Num(n)
	self.totalnum = n
	return self
end

function Handle:Fire(...)
	self.passtime = 0
	self.num = self.num + 1
	if self.totalnum and self.totalnum <= self.num then
		self.mgr.deletes[self] = true
	end
	self.callback(0, ...)
	return self
end

function Handle:SetOwner(ObjectBaseOwner)
	self.OwnerTable[1] = ObjectBaseOwner
end

function Handle:Destroy()
	self.mgr.deletes[self] = true
	local Owner = self.OwnerTable[1]
	if Owner then
		Owner:RemoveTimerHandle(self)
	end
end

TimerMgr = Inherit(Singleton)
function TimerMgr:Ctor(isHandle)
	self.handles={}
	self.deletes={}
	self.current_frame_newhandles = {}
	self.bProcessingTick = false
end

function TimerMgr:Tick(delta)
	self.bProcessingTick = true
	self.current_frame_newhandles = {}
	for h in pairs(self.handles) do
		if not self.deletes[h] then
			h.passtime = h.passtime+delta
			if h.passtime >= h.interval then
				h:CallBack(h.passtime)
			end
		end
	end
	self.bProcessingTick = false
	for h in pairs(self.current_frame_newhandles) do
		self.handles[h] = true
	end
	for h in pairs(self.deletes) do
		self.handles[h] = nil
	end
	self.deletes = {}
end

function TimerMgr:On(f, ...)
	local handle = Handle:NewIns(self, MakeCallBack(f,...))
	if self.bProcessingTick then
		self.current_frame_newhandles[handle] = true
	else
		self.handles[handle] = true
	end
	return handle
end

return TimerMgr