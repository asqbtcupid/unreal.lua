require "ui.basicwidget"
NZLuaBaseViewClass = Inherit(BasicWidget)

function NewUILuaClass(str)
	str = str or "NZLuaBaseViewClass"
	return Inherit(_G[str])
end

function NZLuaBaseViewClass:Ctor()
	self.m_MessageHandles = {}
end

function NZLuaBaseViewClass:Destroy( )
	for i, v in ipairs(self.m_MessageHandles) do
		v:Release()
	end
	self.m_MessageHandles = nil
end

function NZLuaBaseViewClass:OnInited()
end

function NZLuaBaseViewClass:NativeDestroy()
end

function NZLuaBaseViewClass:Show()
	self:Super_Show()
end

function NZLuaBaseViewClass:Hide(Collapsed)
	self:Super_Hide(Collapsed)
end

function NZLuaBaseViewClass:Listen(MessageId, ...)
	local Handle = NZUIMgr:Get():ListenNZMessage(MessageId, self, -1, ...)
	table.insert(self.m_MessageHandles, Handle)
	return Handle
end

function NZLuaBaseViewClass:Notify(MessageId, ... )
	NZUIMgr:Get():LuaNotifyNZMessage(self, MessageId, ...)
end

function NZLuaBaseViewClass:SendMessage( MessageId, ...)
	NZUIMgr:Get():LuaNotifyNZMessage(self, MessageId, ...)
end

function NZLuaBaseViewClass:SendUIMessage(MessageId, ...)
	MessageId = "UI_"..tostring(MessageId)
	self:SendMessage(MessageId, ...)
end

function NZLuaBaseViewClass:GetDataServer(DataClassName)
	requirecpp(DataClassName)
	local GameIns = UGameplayStatics.GetGameInstance(self)
	assert(GameIns ~= nil)
	local Mgr = UNZPIESingletonManager.Get(GameIns)
	local Data = Mgr:GetNZPIESingleton(_G[DataClassName].Class())
	return Data
end

return NZLuaBaseViewClass