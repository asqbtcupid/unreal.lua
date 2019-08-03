local LogDisplay = a_
local function __index(t, k)
	local TheArgs = rawget(t, "_cppinstance_")
	if type(k) == "table" and rawget(k, "__done") then
		TheArgs(k.__done())
		return t
	else
		local function SetParamFunc(self, ...)
			local UsePointCall = self ~= t
			if TheArgs[k] then
				if UsePointCall then
					TheArgs[k](TheArgs, self, ...)
				else
					TheArgs[k](TheArgs, ...)
				end
			else
				LogDisplay("Snew error, function doesn't exist: WidgetArgType:"..t.__GetWidgetArgTypeName().." Func:"..k)
			end
			return t
		end
		return SetParamFunc
	end
end

local function __call(t)
	return t.__done()
end

local function __add(t, SlotProxy)
	local TheArgs = rawget(t, "_cppinstance_")
	local TheSlot = rawget(SlotProxy, "_cppinstance_")
	TheArgs:__addslot(TheSlot)
	return t
end

local meta = 
{
__index = __index, 
__call = __call,
__add = __add,
}



local function NewProxyForArgs(ArgsIns, SlateWidgetType, ArgsType, CtorArgs, ExposeAs)
	local t = setmetatable({}, meta)
	rawset(t, "_cppinstance_", ArgsIns)
	function t.__done()
		local SharedRef_Widget = ArgsType.MakeFromArgs(ArgsIns, unpack(CtorArgs))
		if ExposeAs then
			ExposeAs:Set(SharedRef_Widget)
		end
		return SharedRef_Widget
	end
	function t.__GetWidgetArgTypeName()
		return ArgsType.classname
	end
	return t
end

local function FixSLotFunc(SlateWidgetType, ArgsType, SlotType)
	if SlateWidgetType.Slot then
		return
	end
	local NewSlotFunc = SlateWidgetType.__Slot or ArgsType.__Slot
	local function NewProxyForSlot()
		local TheSLot = NewSlotFunc()
		local t = setmetatable({}, meta)
		rawset(t, "_cppinstance_", TheSLot)
		function t.__GetWidgetArgTypeName()
			return SlotType.classname
		end
		return t
	end
	SlateWidgetType.Slot = NewProxyForSlot
end

local function CreateProxyForArgs(SlateWidgetType, CtorArgs, ExposeAs)
	local ArgsTypeName = SlateWidgetType.classname .. "_FArguments"
	local SlotTypeName = SlateWidgetType.classname .. "_FSlot"
	local ArgsType = _G[ArgsTypeName]
	local SlotType = _G[SlotTypeName]
	FixSLotFunc(SlateWidgetType, ArgsType, SlotType)
	local ArgsIns = ArgsType.New()
	return NewProxyForArgs(ArgsIns, SlateWidgetType, ArgsType, CtorArgs, ExposeAs)
end

function SNew(SlateWidgetType, ...)
	local CtorArgs = {...}
	return CreateProxyForArgs(SlateWidgetType, CtorArgs)
end

function SAssignNew(ExposeAs, SlateWidgetType, ...)
	local CtorArgs = {...}
	return CreateProxyForArgs(SlateWidgetType, CtorArgs, ExposeAs)
end