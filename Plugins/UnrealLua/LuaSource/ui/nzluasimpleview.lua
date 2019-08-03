require "ui.nzluabaseviewclass"
NZLuaSimpleViewClass = Inherit(NZLuaBaseViewClass)

function NZLuaSimpleViewClass:Ctor( )
	if not self.m_DestroyDelegate then
		self.m_DestroyDelegate = self.DestroyEvent
		self.m_HasRelease = false
		assert(self.m_DestroyDelegate)
		self.m_DestroyDelegate:Add(InsCallBack(self.OnDestroyEvent, self))

		self.m_InitEvent = self.InitEvent
		self.m_InitEvent:Add(InsCallBack(self.OnInitEvent, self))

		self.m_InitedEvent = self.InitedEvent
		self.m_InitedEvent:Add(InsCallBack(self.OnInitedEvent, self))
	end
end

function NZLuaSimpleViewClass:Destroy( )
	self:RemoveFromViewport()
	if self.m_Parent then
		self.m_Parent:ChildWidgetDestroy(self)
	end
end

function NZLuaSimpleViewClass:OnInitEvent( )
end

function NZLuaSimpleViewClass:OnDestroyEvent( )
end

function NZLuaSimpleViewClass:OnInitedEvent( )
end

return NZLuaSimpleViewClass