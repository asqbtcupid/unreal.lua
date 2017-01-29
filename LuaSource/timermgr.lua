local Handle = Inherit(Object)
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
	if self.totalnum and self.totalnum >= self.num then
		self.mgr.deletes[self] = true
	end
	self.callback(...)
	return self
end

function Handle:Destroy()
	self.mgr.deletes[self] = true
end

TimerMgr = Inherit(Singleton)
function TimerMgr:Ctor(isHandle)
	self.handles={}
	self.deletes={}
end

function TimerMgr:Tick(delta)
	for h in pairs(self.handles) do
		if not self.deletes[h] then
			h.passtime = h.passtime+delta*1000
			if h.passtime >= h.interval then
				h.callback(h.passtime)
				h.passtime = h.passtime - h.interval
				h.num = h.num + 1
				if h.totalnum and h.totalnum >= h.num then
					h:Destroy()
				end
			end
		end
	end
	for h in pairs(self.deletes) do
		self.handles[h] = nil
	end
	self.deletes = {}
end

function TimerMgr:On(f, ...)
	local handle = Handle:NewIns()
	handle.passtime = 0
	handle.num = 0
	handle.totalnum = 0
	handle.mgr = self
	handle.callback = MakeCallBack(f,...)
	self.handles[handle] = true
	return handle
end

return TimerMgr