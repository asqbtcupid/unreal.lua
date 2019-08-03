BindDataHandle = Inherit(Object)

function BindDataHandle:Ctor(Wnd, WidgetClass)
	self.m_Wnd = Wnd
	self.m_ChildClass = WidgetClass
	self.m_Childs = {}
	self.m_ChildRefreshFuncName = "Refresh"
	self.m_Visibility = ESlateVisibility.Visible
	-- self.m_AddFuncName 
end

function BindDataHandle:SetVisibility(Visibility)
	self.m_Visibility = Visibility
	return self
end

function BindDataHandle:AddFunc(FuncName)
	self.m_AddFuncName = FuncName
	return self
end

function BindDataHandle:Class(WidgetClass)
	self.m_ChildClass = WidgetClass
	return self
end

function BindDataHandle:Data(NewData)
	self.m_Data = NewData
	for i, child in ipairs(self.m_Childs) do
		child:Release()
	end
	self.m_Childs = {}
	self.m_Wnd:ClearChildren()
	return self
end

function BindDataHandle:Func(FuncName)
	self.m_ChildRefreshFuncName = FuncName
	return self
end

function BindDataHandle:Refresh(...)
	assert(self.m_ChildRefreshFuncName)
	for i, ChildData in ipairs(self.m_Data) do
		local ChildWnd = self.m_Childs[i]
		if ChildWnd == nil then
			local NewChild = self.m_Wnd:CreateChild(self.m_ChildClass, i)
			self.m_Wnd[self.m_AddFuncName](self.m_Wnd, NewChild)
			ChildWnd = NewChild
			self.m_Childs[i] = ChildWnd
		end
		ChildWnd:SetVisibility(self.m_Visibility)
		ChildWnd[self.m_ChildRefreshFuncName](ChildWnd, ChildData, i, ...)
	end

	for i = #self.m_Data + 1, #self.m_Childs do
		self.m_Childs[i]:Collapsed()
	end
	return self
end

return BindDataHandle