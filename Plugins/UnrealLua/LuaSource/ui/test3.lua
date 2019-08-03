local TestUI = NewUILuaClass()

function TestUI:Ctor()
	-- local TextWnd = self:Wnd("TextBlock_29")
	-- TextWnd:SetText("一些文本")
	self:Wnd("Button_48"):Event("OnClicked", self.Click, self)
end

function TestUI:Click( )
	-- local u = self:GetDataServer("UNZActivityDataServer")
	UNZUIManager.Pop(100)
end

return TestUI