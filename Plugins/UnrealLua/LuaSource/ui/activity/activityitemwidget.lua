ActivityItemWidget = Inherit(NZLuaSimpleViewClass)
ActivityItemWidget.BpClassPath = '/Game/UI/UIBP/FrontEnd/ActivityUI/Lua/TaskActivityItem.TaskActivityItem_C'

function ActivityItemWidget:Ctor(Index)
	self:Wnd("AwardContainer"):ClearChildren()
	self:Wnd("TaskFinishBtn"):Event("OnClicked", self.ClickReqAward, self)
	self:Wnd("TaskBtn"):Event("OnClicked", self.ClickTaskBtn, self)
end

function ActivityItemWidget:ClickReqAward()
	local t = pb.CSActvAwardReq:New()
	t.actv_group = self.m_ActvDesc.GroupId
	t.actv_id = self.m_ActvDesc.ActvId
	t:Send()
end

function ActivityItemWidget:ClickTaskBtn( )
	UNZUIManager.Pop()
	UNZUIManager.Push(self.m_ActvDesc.GotoPos)
end

function ActivityItemWidget:Refresh(Data, Index)
	self:RemoveBtnEvent()
	self.m_ActvDesc = Data
	local ActvData = ActivityDataServer:Get():GetActvList(Data.ActvId) or {data = 0, state = 0, finish_time = 0, award_times = 0}
	if ActvData then
		local TaskProgress = ActvData.data
		if Data.ConditonType == ActvConditionTypeEnum.EActvCdtType_CheckOnlineTime or Data.ConditonType == ActvConditionTypeEnum.EActvCdtType_MatchTime then
			TaskProgress = TaskProgress / 60
		end
		
		self:Wnd("TaskText"):Txt(string.format(Data.ConditionText, TaskProgress))

		local BtnSwitch = self:Wnd("BtnSwitch")
		if (ActvData.state&ActvInfoStateEnum.EActvState_Award ~= 0 ) and (ActvData.state&ActvInfoStateEnum.EActvState_Finish ~= 0) then
			BtnSwitch:SetActiveWidgetIndex(2)
		elseif ActvData.state & ActvInfoStateEnum.EActvState_Finish ~= 0 then
			BtnSwitch:SetActiveWidgetIndex(1)
		else
			 if Data.GotoPos == 0 then
			 	BtnSwitch:SetActiveWidgetIndex(3)
			 else
				BtnSwitch:SetActiveWidgetIndex(0)
			 end
		end

		self:Wnd("AwardContainer"):Bind(require "ui.activity.activityrewardwidget")
			:Func("LuaSetPropInfoAsAward")
			:Data(Data.Award)
			:Refresh()

		if Data.ActvType == ActvTypeEnums.EActvType_Circle then
			self:Wnd("CanGetAwardPanel"):SetVisibility( BtnSwitch.ActiveWidgetIndex == 1 and ESlateVisibility.HitTestInvisible or ESlateVisibility.Collapsed)
			self:Wnd("CanGetAwardNum"):Txt(ActvData.finish_time - ActvData.award_times)
			local circleConditionStr = string.format(Data.CircleActvConditionText, ActvData.finish_time)
			self:Wnd("CircleConditionText"):Txt(circleConditionStr)
			self:Wnd("CircleActvInfoPanel"):HitTestInvisible()
		else
			self:Wnd("CircleActvInfoPanel"):Collapsed()
		end

		-- todo
		if BtnSwitch.ActiveWidgetIndex ~= 2 and Data.IsAwardPropLimit then
			local taskTextIndex = 0
			if bReachedTotalLimit then
				taskTextIndex = 4
			elseif bReachedCircleLimit then
				if limitType == ActvPropLimitResetTypeEnum.EActvPropLimitResetTypeEnum_Day then
					taskTextIndex = 1;
				elseif limitType == ActvPropLimitResetTypeEnum.EActvPropLimitResetTypeEnum_Week then
					taskTextIndex = 2;
				elseif limitType == ActvPropLimitResetTypeEnum.EActvPropLimitResetTypeEnum_Month then
					taskTextIndex = 3;
				else
					taskTextIndex = 1;
				end
			end
			self:Wnd("TaskTextSwitch"):SetActiveWidgetIndex(taskTextIndex)

			if taskTextIndex ~= 0 then
				BtnSwitch:SetActiveWidgetIndex(3);
				self:Wnd("CircleActvInfoPanel"):Collapsed()
			end
		end
	end
end

function ActivityItemWidget:RefreshExchange(Data, Index)
end

return ActivityItemWidget