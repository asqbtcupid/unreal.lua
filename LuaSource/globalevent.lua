GlobalEvent = {}

function GlobalEvent.Init()
	GlobalEvent.SubscriberList = {}
	GlobalEvent.LastEventData = {}
end

function GlobalEvent.ClearLastEventData()
	GlobalEvent.LastEventData = {}
end

local weakmeta = {__mode = "v"}
function SafeCallBack(callBack, ...)
	local parameters = setmetatable({...}, weakmeta)
    local handle = {}
    function handle:GetParam()
    	return parameters
    end
    local len_p = table.maxn(parameters)
    local function f(...)
        local args = {...}
        local len_a = table.maxn(args)
        for i = 1, len_a do
            parameters[i+len_p] = args[i]
        end
        if parameters[1] == nil then
        	return
        end
        handle.result = callBack(unpack(parameters, 1, len_p+len_a))
        return handle.result
    end
    return f, handle
end

function MakeCallBack(callBack, ...)
    local parameters = setmetatable({...}, weakmeta)
    local handle = {}
    function handle:GetParam()
    	return parameters
    end
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
	local par = setmetatable({}, weakmeta)
	local len = table.maxn(parameters) 
	for i = 1, len do
		par[i] = parameters[i]
	end 
	for i = 1, table.maxn(args) do
		par[i + len] = args[i]
	end
	xpcall(function()Callbackfunction(unpack(par, 1, table.maxn(par))) end, errhandle)
end

function GlobalEvent.Subscribe(EventName, Callbackfunction, ...)
	if type(Callbackfunction) ~= "function" then return end
	local parameters = setmetatable({...}, weakmeta)
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

function GlobalEvent.FireEvent(EventName, ...)
	local arg = {...}
	GlobalEvent.FiringEvent = EventName
	GlobalEvent.LastEventData[EventName] = arg
	if GlobalEvent.SubscriberList[EventName] == nil then return end
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

function GlobalEvent.GetEvent()
	return GlobalEvent.FiringEvent
end
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
