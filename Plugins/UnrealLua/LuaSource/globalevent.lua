GlobalEvent = {}

function GlobalEvent.Init()
	GlobalEvent.SubscriberList = {}
	GlobalEvent.LastEventData = {}
end

function GlobalEvent.ClearLastEventData()
	GlobalEvent.LastEventData = {}
end

local weakmeta = {__mode = "kv"}
--[[
包装回调函数，...调用回调时放在前面的参数，
主要服务于继承于ObjectBase的对象，当该对象调用了Release销毁之后
回调不会生效
--]]
function InsCallBack(callBack, Ins, ...)
	assert(callBack)
	if type(Ins) ~= "table" or Ins._meta_ == nil then
		error("InsCallBack error, Ins Must is a class ins")
	end
	local parameters = setmetatable({Ins, ...}, weakmeta)
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
        if parameters[1] == nil or parameters[1]._has_destroy_ then
        	return
        end
        local function trucall()
        	handle.result = callBack(unpack(parameters, 1, len_p+len_a))
        end
        Xpcall(trucall)
        return handle.result
    end
    return f, handle
end
--[[
当...的第一个对象是table时，如果该table被垃圾回收了，那么就不会回调了
--]]
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
        local function trucall()
        	handle.result = callBack(unpack(parameters, 1, len_p+len_a))
        end
        Xpcall(trucall)
        return handle.result
    end
    return f, handle
end
--[[
包装回调的，把...参数包装起来，回调时放在前面传回
--]]
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
        local function trucall()
        	handle.result = callBack(unpack(parameters, 1, len_p+len_a))
        end
        Xpcall(trucall)
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
	Handle.OwnerTable = setmetatable({}, weakmeta)
	-- 与ObjectBase对象配合用的，使得生命周期绑定一起
	function Handle.SetOwner(ObjectBaseOwner)
		Handle.OwnerTable[1] = ObjectBaseOwner
	end
	-- 销毁
	function Handle.Destroy() 
		GlobalEvent.UnSubscribe(Handle)
		local Owner = Handle.OwnerTable[1]
		if Owner then
			Owner:RemoveEventHandle(Handle)
		end 
	end
	-- 用之前的Fire的数据Fire一次
	function Handle.Fire()
		local data = {GlobalEvent.Data(EventName[1])}
		CallBack(Callbackfunction, parameters, data)
		return Handle
	end
	-- 如果之前有Fire过，那么Fire一次
	function Handle.CheckFire()
		local data = {GlobalEvent.Data(EventName[1])}
		if data[1] then
			CallBack(Callbackfunction, parameters, data)
		end
		return Handle
	end
	return Handle
end
-- Fire一次
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
-- 简化API
GlobalEvent.On = GlobalEvent.Subscribe
GlobalEvent.Fire = GlobalEvent.FireEvent
GlobalEvent.Data = GlobalEvent.GetLastEventData
GlobalEvent.Event = GlobalEvent.GetEvent

return GlobalEvent
