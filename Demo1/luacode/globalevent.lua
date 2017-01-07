GlobalEvent = {}

function GlobalEvent.Init()
	GlobalEvent.SubscriberList = {}
	GlobalEvent.LastEventData = {}
end

function GlobalEvent.ClearLastEventData()
	GlobalEvent.LastEventData = {}
end

function MakeCallBack(callBack, ...)
    local parameters = {...}
    local handle = {}
    local len_p = table.maxn(parameters)
    local function f(...)
        local args = {...}
        local len_a = table.maxn(args)
        for i = 1, len_a do
            parameters[i+len_p] = args[i]
        end
        handle.result = callBack(unpack(parameters, 1, len_p+len_a))
        return handle.result
    end
    return f, handle
end


local function CallBack(Callbackfunction, parameters, args)
	args = args or {}
	local par = {}
	local len = table.maxn(parameters) 
	for i = 1, len do
		par[i] = parameters[i]
	end 
	for i = 1, table.maxn(args) do
		par[i + len] = args[i]
	end
	xpcall(function()Callbackfunction(unpack(par, 1, table.maxn(par))) end, errhandle)
end

-- EventName可以是任意类型数据，如果是table则用table的子项订阅，方便同一函数注册多个事件，
-- Callbackfunction为触发事件时的回调方法
-- 返回的东西需要保存，用来取消订阅
-- 如果传入继承于Singleton的实例的话，不需要取消订阅，当实例销毁时，会自动取消订阅
function GlobalEvent.Subscribe(EventName, Callbackfunction, ...)
	if type(Callbackfunction) ~= "function" then return end
	local parameters = {...}
	local Subscriber = {Callbackfunction, parameters}
	if type(EventName) ~= "table" then EventName = {EventName} end
	for _, Event in pairs(EventName) do
		if GlobalEvent.SubscriberList[Event] == nil then
			GlobalEvent.SubscriberList[Event] = {}
		end
		GlobalEvent.SubscriberList[Event][Subscriber] = true
	end
	local Handle = {EventName, Subscriber}
	function Handle.Destroy() GlobalEvent.UnSubscribe(Handle) end
	function Handle.Fire()
		local data = {GlobalEvent.Data(EventName[1])}
		CallBack(Callbackfunction, parameters, data)
		return Handle
	end
	function Handle.CheckFire()
		local data = {GlobalEvent.Data(EventName[1])}
		if data[1] then
			CallBack(Callbackfunction, parameters, data)
		end
		return Handle
	end
	return Handle
end

-- 触发事件函数，EventName如上，将以订阅时的参数...与回调数据...拼接后调用回调函数
function GlobalEvent.FireEvent(EventName, ...)
	local arg = {...}
	GlobalEvent.FiringEvent = EventName
	GlobalEvent.LastEventData[EventName] = arg
	if GlobalEvent.SubscriberList[EventName] == nil then return end
	-- FireList防止了遍历GlobalEvent.SubscriberList的同时修改GlobalEvent.SubscriberList，大忌
	local FireList = {}
	for Subscriber in pairs(GlobalEvent.SubscriberList[EventName]) do
		table.insert(FireList, Subscriber)
	end
	for _, Subscriber in ipairs(FireList) do
		if GlobalEvent.SubscriberList[EventName][Subscriber] then
			CallBack(Subscriber[1], Subscriber[2], arg)
		end
	end
end

-- 用Subscribe的返回值来取消订阅
function GlobalEvent.UnSubscribe(SubscriberInfo)
	if type(SubscriberInfo) ~= "table" then return end
	local EventName = SubscriberInfo[1]
	local Subscriber = SubscriberInfo[2]
	if type(EventName) == "table" and Subscriber then
		for _, Event in pairs(EventName) do
			if Event and GlobalEvent.SubscriberList[Event] then 
				GlobalEvent.SubscriberList[Event][Subscriber] = nil
			end
		end
	end
end

-- 取得正在触发的那个事件，正在FireEvent的那个事件
function GlobalEvent.GetEvent()
	return GlobalEvent.FiringEvent
end
-- 取得EventName事件最后触发时的数据, 即EventName最后一次fireevent的...，如果没有指定EventName，则会拿到最后一次事件的data
function GlobalEvent.GetLastEventData(EventName)
	if EventName == nil then
		if GlobalEvent.FiringEvent then return GlobalEvent.GetLastEventData(GlobalEvent.FiringEvent) end
	elseif type(GlobalEvent.LastEventData[EventName]) == "table" then
		return unpack(GlobalEvent.LastEventData[EventName], 1, table.maxn(GlobalEvent.LastEventData[EventName]))
	end
end

function GlobalEvent.Clear(EventName)
	GlobalEvent.LastEventData[EventName] = nil
end

function GlobalEvent.Save(EventName, ...)
	local arg = {...}
	GlobalEvent.LastEventData[EventName] = arg
end
GlobalEvent.Init()
GlobalEvent.On = GlobalEvent.Subscribe
GlobalEvent.Fire = GlobalEvent.FireEvent
GlobalEvent.Data = GlobalEvent.GetLastEventData
GlobalEvent.Event = GlobalEvent.GetEvent

return GlobalEvent
