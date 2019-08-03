ActivityBtn = Inherit(NZLuaSimpleViewClass)
ActivityBtn.BpClassPath = "WidgetBlueprint'/Game/UI/UIBP/FrontEnd/ActivityUI/ActivityBtn.ActivityBtn_C'"

function ActivityBtn:Ctor(TabType, ActivityDesc)
	self.OnClicked = self:Wnd("Btn").OnClicked
	self.TabType = TabType
	self.m_ActivityDesc = ActivityDesc
end

function ActivityBtn:OnInitEvent()
	self:On("ActivityRed", self.UpdateRed, self):CheckFire()
end

function ActivityBtn:UpdateRed(RedData)
	if self.m_ActivityDesc then
		local ActivityDesc = self.m_ActivityDesc
		self:ShowRedPoint(RedData[self.TabType][ActivityDesc.GroupId] == true)
	end
end

function ActivityBtn:SetChecked()
	self:Wnd("Btn"):SetVisibility(ESlateVisibility.HitTestInvisible)
	self:Wnd("CheckBtn"):SetIsChecked(true)
	self:Wnd("BtnTextSwitch"):SetActiveWidgetIndex(0)
end

function ActivityBtn:SetUnchecked()
	self:Wnd("Btn"):Visible()
	self:Wnd("CheckBtn"):SetIsChecked(false)
	self:Wnd("BtnTextSwitch"):SetActiveWidgetIndex(1)
end

return ActivityBtn

function ActivityBtn:SomeFunc()
	self.m_OnClicked = self:Wnd("Btn").OnClicked
	self.m_OnClicked:Add(InsCallBack(self.ClickCallBack, self))
end