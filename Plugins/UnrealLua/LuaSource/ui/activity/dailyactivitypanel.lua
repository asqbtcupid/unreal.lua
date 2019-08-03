DailyActivityPanel = Inherit(NZLuaSimpleViewClass)
DailyActivityPanel.BpClassPath = "/Game/UI/UIBP/FrontEnd/ActivityUI/Lua/DailyActivityPanel.DailyActivityPanel_C"

function DailyActivityPanel:Ctor()
	self.m_ActivityTime = self:Wnd("ActivityTime")
	self.m_ActivityDescText = self:Wnd("ActivityDescText")
	self.m_ActivityDetailDescText = self:Wnd("ActivityDetailDescText")
	self:Wnd("DetailBtn"):Event("OnClicked", self.ClickDetail, self)
	self:Wnd("DailyActvContainer"):ClearChildren()
end

function DailyActivityPanel:ClickDetail( )
	if self:Wnd("DailyActivityMainPanel").Visibility == ESlateVisibility.Collapsed then
		self:Wnd("DetailBtnSwitch"):SetActiveWidgetIndex(0);
		self:Wnd("DailyActivityMainPanel"):SetVisibility(ESlateVisibility.Visible);
		self:Wnd("DailyActivityDetailPanel"):SetVisibility(ESlateVisibility.Collapsed);
	else
		self:Wnd("DetailBtnSwitch"):SetActiveWidgetIndex(1);
		self:Wnd("DailyActivityMainPanel"):SetVisibility(ESlateVisibility.Collapsed);
		self:Wnd("DailyActivityDetailPanel"):SetVisibility(ESlateVisibility.Visible);
	end
end

function DailyActivityPanel:Refresh(ActivityDesc, TheGroupBelong)
	self.m_GroupId = ActivityDesc.GroupId
	self.m_ActivityDesc = ActivityDesc
	
	self.m_ActivityTime:SetText(ActivityDesc.PanelStartDate.."~"..ActivityDesc.PanelEndDate)

	self.m_ActivityDescText:SetText(USimpleBBCode.DecodeBB(ActivityDesc.ContentTitle))
	if ActivityDesc.ContentText ~= "" then
		self:Wnd("DetailBtn"):Visible()
		self.m_ActivityDetailDescText:SetText(USimpleBBCode.DecodeBB(ActivityDesc.ContentText))
	else
		self:Wnd("DetailBtn"):Hidden()
	end
	
	if not ActivityDesc.bIsExchangeDesc then
		self:Wnd("DailyActvContainer"):Bind()
			:Class(require "ui.activity.activityitemwidget")
			:Data(TheGroupBelong)
			:Refresh()
	else
		self:Wnd("DailyActvContainer"):Bind()
			:Class(require "ui.activity.activityexchangewidget")
			:Data(ActivityDesc.ExchangeRules)
			:Refresh(ActivityDesc)
	end
end

function DailyActivityPanel:Click( )
	
end

return DailyActivityPanel