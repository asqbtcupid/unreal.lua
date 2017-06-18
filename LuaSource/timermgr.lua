local Handle = Inherit(Object)

function Handle:Ctor(mgr, callback)
	self.passtime = 0
	self.num = 0
	self.totalnum = 0
	self.mgr = mgr
	self.callback = callback
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
	if self.totalnum and self.totalnum >= self.num then
		self.mgr.deletes[self] = true
	end
	self.callback(0, ...)
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
			h.passtime = h.passtime+delta
			if h.passtime >= h.interval then
				h.callback(h.passtime)
				h.passtime = 0
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
	local handle = Handle:NewIns(self, MakeCallBack(f,...))
	self.handles[handle] = true
	return handle
end

return TimerMgr