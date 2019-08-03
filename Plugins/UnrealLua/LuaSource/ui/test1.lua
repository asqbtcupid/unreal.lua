local TestUI = NewUILuaClass()

function TestUI:Ctor()
	self:Wnd("Button_77"):Event("OnClicked", self.Click, self)
	self:Wnd("Button_54"):Event("OnClicked", self.ChangeText, self)
	self:Wnd("Button_45"):Event("OnClicked", self.TestRegister, self)
	self:Wnd("Button_91"):Event("OnClicked", self.TestFire, self)
end

function TestUI:Click( )
	UNZUIManager.Pop(100)
end

function TestUI:SomeTest( )
	local t = 0
	self:Wnd("test111"):SetText(tostring(t))
	CoroutineUtil.Delay(1)
	t = t + 1
	self:Wnd("test111"):SetText(tostring(t))
	CoroutineUtil.Delay(1)
	t = t + 1
	
	self:Wnd("test111"):SetText(tostring(t))
	CoroutineUtil.Delay(1)
	t = t + 1
	
	self:Wnd("test111"):SetText(tostring(t))
	CoroutineUtil.Delay(1)
	t = t + 1
	self:Wnd("test111"):SetText(tostring(t))
end

function TestUI:ChangeText( )
	-- UTableUtil.DoString("aaa = {}")
	self:Coroutine(self.SomeTest, self)
end

function TestUI:TestRegister(...)
	-- local u = self:GetDataServer("UNZChatDataServer")
end

function TestUI:TestFire()
	-- self:Notify("wahaha", 1,2.3, true, false, "xixi", FVector.New(2,3,4) )
end

return TestUI