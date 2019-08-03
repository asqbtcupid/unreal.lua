local ActivityMainUI = NewUILuaClass()

function ActivityMainUI:Ctor()
	self:Wnd("CommonPopBorderUI_WithLabel"):Wnd("CloseBtn"):Event("OnClicked", self.ClickClose, self)
	self:Wnd("ActivityTypeSwitch"):Collapsed()
	self.m_BtnBox = self:Wnd("ActivityBtnScrollBox")
	self.m_TabsHandle = self:GroupTab(self:Wnd("ActivityTab"), self:Wnd("ActivityTab_C_1"), self:Wnd("ActivityTab_C_0"), self:Wnd("ActivityTab_C_2"))
										:Switch("OnChecked", "SetCheckedState", "SetUnCheckedState")
										:Call(self.TabChange, self)

	self.m_ActivityTypeSwitcher = self:Wnd("ActivityTypeSwitch")
	self.m_SwitchIndex = {}
	self:On("OnCSActvDescRes", self.RefreshActivity, self):CheckFire()
	self:On("ActivityRed", self.UpdateTabRedPoint, self):CheckFire()
	self:On("OpenActivity", self.OpenActivity, self)
end

function ActivityMainUI:OpenActivity(GroupId)
	
end

function ActivityMainUI:UpdateTabRedPoint(RedData)
	for i, Tab in ipairs(self.m_TabsHandle.Tabs) do
		Tab:ShowCount(RedData[Tab.TabTypeIndex]:Count())
	end
end

function ActivityMainUI:RefreshActivity(Groups, ActiveTabType, GroupsDesc)
	local VisibleTab = {}
	self.m_Groups = Groups
	self.m_ActiveTabType = ActiveTabType
	self.m_GroupsDesc = GroupsDesc

	local function SetTabVisibility()
		for Index, Tab in pairs(self.m_TabsHandle.Tabs) do
			local TabType = Tab.TabTypeIndex
			if not ActiveTabType[TabType] then
				Tab:Collapsed()
			else
				table.insert(VisibleTab, Index)
				Tab:Visible()
			end
		end
	end
	SetTabVisibility()
	if not self.m_HasInit then
		self.m_HasInit = true
		self.m_NowTabIndex = VisibleTab[1]
	elseif self.m_NowTabIndex > #VisibleTab then
		self.m_NowTabIndex = 1
	end

	self.m_TabsHandle:CheckByIndex(self.m_NowTabIndex, true)
end

function ActivityMainUI:TabChange(TabIndex, bNotFromClick)
	local Tab = self.m_TabsHandle.Tabs[TabIndex]
	local TargetTabType = Tab.TabTypeIndex
	self.m_BtnBox:ClearChildren()
	local Padding = FMargin.Temp()
	Padding.Bottom = -1
	local Btns = {}
	self.m_BtnActivitis = {}
	local HasAddGroup = {}
	
	for _, ActivityDesc in pairs(self.m_GroupsDesc[TargetTabType] or {}) do
		local Continue = false
		if ActivityDesc.ContentPicType == ActvContentPicTypeEnum.EActvContentPicType_Http then
			if not ActivityDataServer.HasActvImgFile(ActivityDesc.PicName) then
				Continue = true
			end
		end
		if not Continue then
			local Btn = self:CreateChild("ui.activity.activitybtn", TargetTabType, ActivityDesc)
			Btn:SetInfo(ActivityDesc.GroupId, ActivityDesc.TitleName)
			Btn:SetLabel(ActivityDesc.LabelType);
			Btn:SetPadding(Padding)
			self.m_BtnBox:AddChild(Btn)
			table.insert(Btns, Btn)
			table.insert(self.m_BtnActivitis, ActivityDesc)
		end
	end	
	self.m_BtnGroup = self:GroupTab(unpack(Btns))
					:Switch("OnClicked", "SetChecked", "SetUnchecked")
					:Call(self.BtnChange, self)
	
	if bNotFromClick then
		self.m_BtnGroup:CheckByIndex(self.m_NowBtnIndex or 1)
	else
		self.m_BtnGroup:CheckByIndex(1)
	end
end

function ActivityMainUI:AsynCallBack(path)
end

function ActivityMainUI:CoroutineAsyn(BtnIndex)
	local Activity = self.m_BtnActivitis[BtnIndex]
	local function LoadPanel(MemberName, ClassPath)
		if not self[MemberName] then
			self[MemberName] = self:AsynCreateChild(ClassPath)
			local SwitchIndex = self.m_ActivityTypeSwitcher:GetChildrenCount()
			self.m_ActivityTypeSwitcher:AddChild(self[MemberName])
			self.m_SwitchIndex[self[MemberName]] = SwitchIndex
			self.m_ActivityTypeSwitcher:SetVisibility(ESlateVisibility.SelfHitTestInvisible)
		end
		self.m_ActivityTypeSwitcher:SetActiveWidgetIndex(self.m_SwitchIndex[self[MemberName]])
		self[MemberName]:Refresh(Activity, self.m_Groups[Activity.GroupId])
	end
	if Activity.ActvType == ActvTypeEnums.EActvType_Notice or Activity.ActvType == ActvTypeEnums.EActvType_NewPlayerNotice then
		LoadPanel("m_pic_activity_panel", "ui.activity.picactivitypanel")
	else
		LoadPanel("m_daily_activity_panel", "ui.activity.dailyactivitypanel")
	end
end

function ActivityMainUI:BtnChange(BtnIndex)
	self.m_NowBtnIndex = BtnIndex
	self:Coroutine(self.CoroutineAsyn, self, BtnIndex)
end

function ActivityMainUI:ClickClose( )
	UNZUIManager.Pop(200)
	-- a_(require "didi")
	-- a_(ULuautils.GetWorld1(ProtoMgr:Get()))
	-- requirecpp "UNZLoginDataServer"
	-- a_(UNZLoginDataServer.GetDefaultObject().LastNickName)
	-- self:Fire("ActivityRed", self:Data("ActivityRed"))
	-- self:Red("ActivityRed")[1] = {}
	-- self:Red("ActivityRed")[1][332] = 2
	-- self:Red("ActivityRed")[3][1234] = 5
	-- self:Red("ActivityRed")[3][12342] = 5
	-- local ins = GetCppSingleton(ProtoMgr:Get():GetWorld(), "UNZStoreDataServer")
	-- a_(ins)
	-- ConfigMgr:Get():SetBoolUni(self, "lala", true)
	-- local value = ConfigMgr:Get():GetBoolUni(self, "lala")
	-- a_(value)
end

function ActivityMainUI:SendSomeProto( )
	local t = pb.CSActvDescReq:New()
	t.register_time = FDateTime.Now():ToUnixTimestamp() - 100
	t:Send()
end

return ActivityMainUI