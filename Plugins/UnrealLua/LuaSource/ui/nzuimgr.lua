require "ui.nzluabaseviewclass"
require "ui.nzluasimpleview"

NZUIMgr = Inherit(CppObjectBase)

function NZUIMgr:Ctor()
	self.m_ViewIns = {}
	self.m_RegisteredEvents={}
	-- local meta = {__mode = "kv"}
	-- self.m_weak = setmetatable({}, meta)
end

function OnFrontEndInitEvent(FrontEndUIInstance)
	NZUIMgr:Get():OnFrontEndInitEvent(FrontEndUIInstance)
end

function NZUIMgr:OnFrontEndInitEvent(FrontEndUIInstance)
	-- self.m_ActivityBtnOnDelegate = FrontEndUIInstance.ActivityBtn.OnClicked
	-- self.m_ActivityBtnOnDelegate:Clear()
	-- self.m_ActivityBtnOnDelegate:Add(MakeCallBack(a_, "lalala"))
end

function NZUIMgr:Get( )
	if NZUIMgr._Singleton == nil then
		NZUIMgr._Singleton = NZUIMgr:NewOn(UNZLuaUIManager.Get())
	end
	return NZUIMgr._Singleton
end

function NZUIMgr:OnInited(ViewIns)
	if type(ViewIns) ~= "table" then
		error("not a table")
	end
	ViewIns:OnInited()
end

function NZUIMgr:InitView(ViewIns, ...)
	local ClassPath = ViewIns.LuaPath
	if ClassPath == "" then
		ClassPath = "ui.nzluabaseviewclass"
	else
		ClassPath = "ui."..ClassPath
	end
	local ViewLuaClass = require(ClassPath)
	local LuaIns = ViewLuaClass:NewOn(ViewIns, ...)
	self.m_ViewIns[LuaIns] = true
end

function NZUIMgr:NativeDestroy(ViewIns)
	assert(self.m_ViewIns[ViewIns])
	ViewIns:Release()
	self.m_ViewIns[ViewIns] = nil
end

function NZUIMgr:Show(ViewIns)
	ViewIns:Show()
end

function NZUIMgr:Hide(ViewIns, Collapsed)
	ViewIns:Hide(Collapsed)
end

requirecpp "VoidPtrStruct"
local Name2ProcessFunc =
{
	INT8 =  VoidPtrStruct.To_int8,
	UINT8 =  VoidPtrStruct.To_uint8,
	INT16 =  VoidPtrStruct.To_int16,
	UINT16 =  VoidPtrStruct.To_uint16,
	INT32 =  VoidPtrStruct.To_int32,
	UINT32 =  VoidPtrStruct.To_uint32,
	INT64 =  VoidPtrStruct.To_int64,
	UINT64 =  VoidPtrStruct.To_uint64,
	BOOL =  VoidPtrStruct.To_bool,
	FLOAT =  VoidPtrStruct.To_float,
	DOUBLE =  VoidPtrStruct.To_double,
	LUADOUBLE =  VoidPtrStruct.To_double,
	FSTRING =  VoidPtrStruct.To_FString,
	UObject =  VoidPtrStruct.To_UObject,
}

function NZUIMgr:OnNZMessageNotify(Message)
	local Params = Message.Params
	local Vars={}
	local Names = {}
	local HasFunc = {}
	for i = 1, #Params do
		local TypeName = Params[i].TypeName
		table.insert(Names, TypeName)
		HasFunc[i] = Name2ProcessFunc[TypeName] or 0
		local GetValueFunc = Name2ProcessFunc[TypeName]
		if GetValueFunc then
			Vars[i] = GetValueFunc(Params[i].Value)
		else
			Vars[i] = Params[i].Value:To_Struct(TypeName)
		end
	end
	local MgrQueue = self.m_RegisteredEvents[Message.MessageId]
	if MgrQueue then
		for Handle in pairs(MgrQueue) do
			Handle:Call(unpack(Vars))
		end
	end
end

local MessageHandle = Inherit(ObjectBase)

function MessageHandle:Ctor(CallBack, MgrQueue)
	self.m_CallBack = CallBack
	self.m_MgrQueue = MgrQueue
end

function MessageHandle:Destroy( )
	self.m_MgrQueue[self] = nil
end

function MessageHandle:GetCallBack( )
	return self.m_CallBack
end

function MessageHandle:Call(...)
	self.m_CallBack(...)
end

function NZUIMgr:ListenNZMessage(MessageId, UIObject, Times, ...)
	local MgrQueue = self.m_RegisteredEvents[MessageId] 
	if not MgrQueue then
		local RegisterSeq = self:LuaListenNZMessage(UIObject, MessageId, Times)
		MgrQueue = {}
	end
	local CallBack = InsCallBack(...)
	self.m_RegisteredEvents[MessageId] = MgrQueue
	local Handle = MessageHandle:NewIns(CallBack, MgrQueue)
	MgrQueue[Handle] = true
	return Handle
end

function OpenTestUnrealLuaUI( )
	NZUIMgr:Get()
	UNZUIManager.Register(100, "WidgetBlueprint'/Game/pptui.pptui_C'")
	UNZUIManager.Push(100)
end

function InitUI()
	NZUIMgr:Get()
	UNZUIManager.Register(200, "WidgetBlueprint'/Game/UI/UIBP/FrontEnd/ActivityUI/Lua/ActivityMainUI.ActivityMainUI_C'")
	UNZUIManager.Push(200)
end
TestLua = OpenTestUnrealLuaUI

return NZUIMgr