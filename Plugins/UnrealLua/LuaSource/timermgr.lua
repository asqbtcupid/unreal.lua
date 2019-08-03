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
-- 设置定时器间隔
function Handle:Time(t)
	self.interval = t
	return self
end
-- 设置回调次数n,n次回调后销毁
function Handle:Num(n)
	self.totalnum = n
	return self
end
-- 直接触发一次
function Handle:Fire(...)
	self.passtime = 0
	self.num = self.num + 1
	if self.totalnum and self.totalnum <= self.num then
		self.mgr.deletes[self] = true
	end
	self.callback(0, ...)
	return self
end
-- 配合objectbase实例使用,管理生命周期
function Handle:SetOwner(ObjectBaseOwner)
	self.OwnerTable[1] = ObjectBaseOwner
end

function Handle:SetFrameEnd()
	self.m_FrameEnd = true
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

-- 从main.lua tick过来的,每帧调用
function TimerMgr:Tick(delta)
	self.bProcessingTick = true
	self.current_frame_newhandles = {}
	self.m_DeferredCall = {}

	for h in pairs(self.handles) do
		if not self.deletes[h] then
			h.passtime = h.passtime+delta
			if h.passtime >= h.interval then
				if h.m_FrameEnd then
					self.m_DeferredCall[h] = true
				else
					h:CallBack(h.passtime)
				end
			end
		end
	end

	for h in pairs(self.m_DeferredCall) do
		h:CallBack(h.passtime)
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
-- f是回调函数,...是放在前面的函数,参看GlobalEvent.lua的MakeCallBack的函数
-- 注册之后返回一个handle,可以用这个handle来设置timer定时的时间间隔,和回调次数
-- 回调时实际过去的时间间隔会放在最后一个参数
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