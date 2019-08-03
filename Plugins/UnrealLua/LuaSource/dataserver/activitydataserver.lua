ActivityDataServer = Inherit(Singleton)

function ActivityDataServer:Ctor()
	self:On("CSActvDescRes", self.OnCSActvDescRes, self)
	self:On("CSActvListRes", self.OnCSActvListRes, self)
	self:On("CSActvExchangeRes", self.OnCSActvExchangeRes, self)
	self:On("CSActvAwardRes", self.OnCSActvAwardRes, self)
	self:On("CSActvAwardListRes", self.OnCSActvAwardListRes, self)
	self:On("CSActvDoExchangeRes", self.OnCSActvDoExchangeRes, self)
	self:On("CSActvUpdateNtf", self.OnCSActvUpdateNtf, self)
	self:On("CSActvClientNoticeRes", self.OnCSActvClientNoticeRes, self)
	self:On("CSActvGetInCrementDescRes", self.OnCSActvGetInCrementDescRes, self)
	self.m_RedActivity = self:Red("ActivityRed")
	LuaVarWatcher:Get():Add("ActvRed", self.m_RedActivity)
end

local function GetDisplayTime(ServerData)
	local Year = math.floor(ServerData/10000)
	local Month = math.floor((ServerData - Year*10000)/100)
	local Day = ServerData%100
	return Year.."-"..Month.."-"..Day
end

function ActivityDataServer:OnCSActvDescRes(DataTable)
	local Groups = {}
	local ActiveTabType = {}
	local GroupDesc = {}
	local HasAddedGroupIds = {}
	for i, Desc in ipairs(DataTable.desc) do
		Desc.TitleName = StringUtil.ReplaceEscapedCharWithChar(Desc.TitleName)
		Desc.ContentTitle = StringUtil.ReplaceEscapedCharWithChar(Desc.ContentTitle)
		Desc.ContentText = StringUtil.ReplaceEscapedCharWithChar(Desc.ContentText)

		Desc.PanelStartDate = GetDisplayTime(Desc.PanelStartDate)
		Desc.PanelEndDate = GetDisplayTime(Desc.PanelEndDate)
		Groups[Desc.GroupId] = Groups[Desc.GroupId] or {}
		table.insert(Groups[Desc.GroupId], Desc)

		if (Desc.ActvType == ActvTypeEnums.EActvType_NewPlayerReward or
			Desc.ActvType == ActvTypeEnums.EActvType_LoginCumu or
			Desc.ActvType == ActvTypeEnums.EActvType_PatFace or
			Desc.ActvType == ActvTypeEnums.EActvType_Carnival or
			Desc.ActvType == ActvTypeEnums.EActvType_Share or
			Desc.ActvType == ActvTypeEnums.EActvType_IrregularPatFace or
			Desc.ActvType == ActvTypeEnums.EActvType_NewPlayerReward2 or
			Desc.ActvType == ActvTypeEnums.EActvType_NewPlayerPatFace or
			Desc.ActvType == ActvTypeEnums.EActvType_NewPlayerIrregularPatFace or
			Desc.ActvType == ActvTypeEnums.EActvType_Lobby) then
			Desc.ActvTabType = 0
		end

		GroupDesc[Desc.ActvTabType] = GroupDesc[Desc.ActvTabType] or {}
		if not HasAddedGroupIds[Desc.GroupId] then
			HasAddedGroupIds[Desc.GroupId] = true
			table.insert(GroupDesc[Desc.ActvTabType], Desc)
		end

		ActiveTabType[Desc.ActvTabType] = ActiveTabType[Desc.ActvTabType] or {}
		table.insert(ActiveTabType[Desc.ActvTabType], Desc)

		if Desc.ContentPicType == ActvContentPicTypeEnum.EActvContentPicType_Http then
			local Url = Desc.ContentPicId..tostring(Desc.GroupId)
			Desc.PicName = UNZLuaHelperLibrary.MD5(Url)
		elseif Desc.ContentPicType == ActvContentPicTypeEnum.EActvContentPicType_Img then
			Desc.PicName = tostring(Desc.ContentPicId) or tostring(GroupId)
		end

		-- self.m_RedActivity[Desc.ActvTabType] = self.m_RedActivity[Desc.ActvTabType]
		
		local GroupsRed = self.m_RedActivity[Desc.ActvTabType]
		GroupsRed[Desc.GroupId] = true
	end

	self.m_Groups = Groups
	self.m_GroupDesc = GroupDesc
	self.m_ActiveTabType = ActiveTabType
end

function ActivityDataServer:SortAndNotifyActivityChange()
	local HasObtainAward = {}
	for GroupId, Activities in pairs(self.m_Groups) do
		HasObtainAwardCount = 0
		for Index, Activity in ipairs(Activities) do
			local ActvData = self.m_ActvListRes[Activity.ActvId]
			if ActvData then
				if (ActvData.state&ActvInfoStateEnum.EActvState_Award ~= 0 ) and (ActvData.state&ActvInfoStateEnum.EActvState_Finish ~= 0) then
					HasObtainAwardCount = HasObtainAwardCount + 1
				end 
			end
		end
		HasObtainAward[GroupId] = HasObtainAwardCount == #Activities
	end

	local function CmpActivities(a, b)
		if HasObtainAward[a.ActvId] ~= HasObtainAward[b.ActvId] then
			return HasObtainAward[b.ActvId] == true
		end

		if a.GroupId ~= b.GroupId then
			if a.SortId ~= b.SortId then
				return a.SortId < b.SortId
			end
			return a.GroupId < b.GroupId
		end

		return a.ActvId < b.ActvId 
	end

	local function CmpInGroup(a, b)
		if self.m_ActvListRes then
			local a_SortId = self.m_ActvListRes[a.ActvId] and self.m_ActvListRes[a.ActvId].SortId or 0 
			local b_SortId = self.m_ActvListRes[b.ActvId] and self.m_ActvListRes[b.ActvId].SortId or 0 
			if a_SortId ~= b_SortId then
				return a_SortId > b_SortId
			end
		end
		
		return a.ActvId < b.ActvId 
	end

	for i, Activities in pairs(self.m_Groups) do
		table.sort(Activities, CmpInGroup)
	end

	for i, Activities in pairs(self.m_GroupDesc) do
		table.sort(Activities, CmpActivities)
	end
	LuaVarWatcher:Get():AddNode("Group", self.m_Groups)
	LuaVarWatcher:Get():AddNode("GroupDesc", self.m_GroupDesc)
	self:Fire("OnCSActvDescRes", self.m_Groups, self.m_ActiveTabType, self.m_GroupDesc)
end

local function DecideSortId(info)
	info.SortId = 1;
	if (info.state & ActvInfoStateEnum.EActvState_Finish ) ~= 0 then
		info.SortId = 2;
		if (info.state & ActvInfoStateEnum.EActvState_Award) ~= 0 then
			info.SortId = 0;
		end
	end
end

function ActivityDataServer:OnCSActvListRes(DataTable)
	local t = {}
	for i, v in ipairs(DataTable.actv_info) do
		t[v.id] = v
		DecideSortId(v)
	end
	self.m_ActvListRes = t
	self:SortAndNotifyActivityChange()
end

function ActivityDataServer:GetActvList(id)
	return self.m_ActvListRes[id]
end

function ActivityDataServer:GetGroupDesc(GroupId)
	return self.m_GroupDesc[GroupId]
end

function ActivityDataServer:OnCSActvExchangeRes(DataTable)
	self.m_ExchangeRes = DataTable
	LuaVarWatcher:Get():Add("Exchange", self.m_ExchangeRes)

	local GroupDesc = self.m_GroupDesc or {}
	local Groups =  self.m_Groups or {}

	local HasAddedGroupIds = {}
	local StoreDataServer = GetCppSingleton(ProtoMgr:Get():GetWorld(), "UNZStoreDataServer")
	for i, Desc in ipairs(DataTable.ExchangeInfos) do
		Desc.ActvType = ActvTypeEnums.EActvType_Daily

		GroupDesc[Desc.ActvTabType] = GroupDesc[Desc.ActvTabType] or {}
		Desc.GroupId = Desc.GroupID
		if not HasAddedGroupIds[Desc.GroupID] then
			HasAddedGroupIds[Desc.GroupID] = true
			table.insert(GroupDesc[Desc.ActvTabType], Desc)
		end

		local StartDateTime = FDateTime.FromUnixTimestamp(Desc.StartTime + 28800)
		Desc.PanelStartDate = string.format("%d-%d-%d", StartDateTime:GetYear(), StartDateTime:GetMonth(), StartDateTime:GetDay())
		local EndDateTime = FDateTime.FromUnixTimestamp(Desc.EndTime + 28800)
		Desc.PanelEndDate = string.format("%d-%d-%d", EndDateTime:GetYear(), EndDateTime:GetMonth(), EndDateTime:GetDay())
		Desc.TitleName = StringUtil.ReplaceEscapedCharWithChar(Desc.TitleName)
		Desc.ContentTitle = StringUtil.ReplaceEscapedCharWithChar(Desc.ContentTitle)
		Desc.ContentText = StringUtil.ReplaceEscapedCharWithChar(Desc.ContentText)
		Desc.LabelType = ActvSpecialNoteTypeEnums.EActvSpecialNoteType_Exchange
		Desc.bIsExchangeDesc = true
		local Rules = Desc.ExchangeRules
		for _, RulesItem in ipairs(Rules) do
			RulesItem.bCanExchange = true
			for _, Good in pairs(RulesItem.ExchangeGoods) do
				local HasNum = 0
				local PropArr = TArray_FStoreProp.New()
				StoreDataServer:GetCachePropByID(Good.GoodsID, PropArr)
				for i = 1, #PropArr do
					local lala = PropArr[i].num
					HasNum = HasNum + PropArr[i].num
				end
				RulesItem.bCanExchange = RulesItem.bCanExchange and HasNum >= Good.GoodsNum
			end
		end

		Groups[Desc.GroupId] = Groups[Desc.GroupId] or {}
		table.insert(Groups[Desc.GroupId], Desc)
	end

	self.m_GroupDesc = GroupDesc
	self.m_Groups = Groups
end

function ActivityDataServer:OnCSActvAwardRes(DataTable)
end

function ActivityDataServer:OnCSActvAwardListRes(DataTable)
end

function ActivityDataServer:OnCSActvDoExchangeRes(DataTable)
end

function ActivityDataServer:OnCSActvUpdateNtf(DataTable)
	for i, v in ipairs(DataTable.actv_info) do
		self.m_ActvListRes[v.id] = v
		DecideSortId(v)
	end
	self:SortAndNotifyActivityChange()
end

function ActivityDataServer:OnCSActvClientNoticeRes(DataTable)
end

function ActivityDataServer:OnCSActvGetInCrementDescRes(DataTable)
end

function ActivityDataServer:ActvHasPic(GroupId)
	local ActivityDesc = self.m_GroupDesc[GroupId]
	if ActivityDesc.ContentPicType == ActvContentPicTypeEnum.EActvContentPicType_Http then
		return ActivityDataServer.HasActvImgFile(ActivityDesc.PicName)
	elseif ActivityDesc.ContentPicType == ActvContentPicTypeEnum.EActvContentPicType_Img then
		return ActivityDataServer.HasActvImgAsset( ActivityDesc.PicName )
	end
	return false
end

function ActivityDataServer.HasActvImgAsset( ImgName )
	local FilePath = string.format("%sUI/UIAtlas/UITextures/Activity/Frames/%s_png.uasset", FPaths.ProjectContentDir(), ImgName)
	local res = FPaths.FileExists(FilePath)
	return res
end

function ActivityDataServer.HasActvImgFile(ImgName)
	local FilePath = string.format("%sTmpActvImages/%s.png", FPaths.ProjectSavedDir(), ImgName)
	local res = FPaths.FileExists(FilePath)
	return res
end

function ActivityDataServer:HasActvShown(ins, GroupId)
	return ConfigMgr:Get():GetBoolUni(ins, "ActvShowGroup"..GroupId)
end

function ActivityDataServer:SetActvShown(ins, GroupId)
	ConfigMgr:Get():SetBoolUni(ins, "ActvShowGroup"..GroupId, true)
end



return ActivityDataServer
