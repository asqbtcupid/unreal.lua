ActivityRewardWidget = Inherit(NZLuaSimpleViewClass)
ActivityRewardWidget.BpClassPath = '/Game/UI/UIBP/FrontEnd/ActivityUI/Lua/ActivityReward.ActivityReward_C'
requirecpp "FActivityProp"

function ActivityRewardWidget:LuaSetPropInfoAsAward(Data, Index)
	local PropInfo = FActivityProp.Temp()
	PropInfo.ItemId = Data.Id
	if Data.AgingType == EPropAgingType.kPropAgingPermanent then
		PropInfo.num = Data.Num
		PropInfo.time = 0
	else
		PropInfo.num = 1
		PropInfo.time = Data.Time
	end
	self:SetPropInfoAsAward(PropInfo)
end

function ActivityRewardWidget:ExchangeSetPropInfoAsItem(Data, Index)
	local PropInfo = FActivityProp.Temp()
	PropInfo.ItemId = Data.GoodsID
	if Data.AgingType == EPropAgingType.kPropAgingPermanent then
		PropInfo.num = Data.GoodsNum
		PropInfo.time = 0
	else
		PropInfo.num = 1
		PropInfo.time = Data.Time
	end
	self:SetPropInfoAsItem(PropInfo)
end

function ActivityRewardWidget:ExchangeSetPropInfoAsAward(Data, Index)
	local PropInfo = FActivityProp.Temp()
	PropInfo.ItemId = Data.GoodsID
	if Data.AgingType == EPropAgingType.kPropAgingPermanent then
		PropInfo.num = Data.GoodsNum
		PropInfo.time = 0
	else
		PropInfo.num = 1
		PropInfo.time = Data.AgintTime
	end
	self:SetPropInfoAsAward(PropInfo)
end

return ActivityRewardWidget