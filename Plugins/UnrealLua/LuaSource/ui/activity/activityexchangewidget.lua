ActivityExchangeWidget = Inherit(NZLuaSimpleViewClass)
ActivityExchangeWidget.BpClassPath = '/Game/UI/UIBP/FrontEnd/ActivityUI/Lua/ExchangeActivityItem.ExchangeActivityItem_C'

function ActivityExchangeWidget:Ctor(Index)
	self:Wnd("Exchange"):Event("OnClicked", self.ClickExchange, self)
end

function ActivityExchangeWidget:ClickExchange()
	local t = pb.CSActvDoExchangeReq:New()
	t.RuleID = self.m_Rule.RuleID
	t.GroupID = self.m_ActvDesc.GroupID
	local prop_list = {}
	local StoreDataServer = GetCppSingleton(ProtoMgr:Get():GetWorld(), "UNZStoreDataServer")
	local PropArr = TArray_FStoreProp.New()

	for i, v in ipairs(self.m_Rule.ExchangeGoods) do
		prop_list.id = v.GoodsID
		StoreDataServer:GetCachePropByID(Good.GoodsID, PropArr)
		prop_list.gid = PropArr[1].gid
	end
	t.prop_list = prop_list
	t:Send()
end

function ActivityExchangeWidget:Refresh(Data, Index, Desc)
	self.m_ActvDesc = Desc
	self.m_Rule = Data
	self:Wnd("TextSwitch"):SetActiveWidgetIndex(Desc.ExchangeCycleInDays > 1 and 1 or 0)
	
	local NeedProp = {}
	local Awards = {}
	for i, v in ipairs(Data.ExchangeGoods) do
		table.insert(NeedProp, v)
	end
	for i, v in ipairs(Data.GetGoods) do
		table.insert(Awards, v)
	end

	self:Wnd("ItemContainer"):Bind(require "ui.activity.activityrewardwidget")
		:Func("ExchangeSetPropInfoAsItem")
		:Data(NeedProp)
		:Refresh()

	self:Wnd("AwardContainer"):Bind(require "ui.activity.activityrewardwidget")
		:Func("ExchangeSetPropInfoAsAward")
		:Data(Awards)
		:Refresh()

	local CurrentNum = Data.ExchangeLimit - Data.RemainExchangeCnt
	if CurrentNum == Data.ExchangeLimit then
		self:Wnd("BtnSwitch"):SetActiveWidgetIndex(2)
	else
		if Data.bCanExchange then
			self:Wnd("BtnSwitch"):SetActiveWidgetIndex(1)
		else
			self:Wnd("BtnSwitch"):SetActiveWidgetIndex(0)
		end		
	end
	self:Wnd("ActivityCurrentGot"):SetText(CurrentNum);
	self:Wnd("ActivityMaxStock"):SetText(Data.ExchangeLimit);

end

return ActivityExchangeWidget