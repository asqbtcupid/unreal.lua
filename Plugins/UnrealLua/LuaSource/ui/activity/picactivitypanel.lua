requirecpp "NavigationType"
requirecpp "NZUIMessage"
PicActivityPanel = Inherit(NZLuaSimpleViewClass)
PicActivityPanel.BpClassPath = "/Game/UI/UIBP/FrontEnd/ActivityUI/Lua/PicActivityPanel.PicActivityPanel_C"
function PicActivityPanel:Ctor()
	self:Wnd("ActivityPicBtn"):Event("OnClicked", self.Click, self)
	self.m_PicWnd = self:Wnd("ActivityContentPic")
end

function PicActivityPanel:Refresh(ActivityDesc)
	self.m_GroupId = ActivityDesc.GroupId
	self.m_ActivityDesc = ActivityDesc

	local Sprite
	if ActivityDesc.ContentPicType == ActvContentPicTypeEnum.EActvContentPicType_Img then
		local Path = string.format("/Game/UI/UIAtlas/UITextures/Activity/Frames/%d_png.%d_png", ActivityDesc.ContentPicId, ActivityDesc.ContentPicId)
		-- Sprite = UPaperSprite.LoadObject(nil, Path)
		Sprite = CoroutineUtil.LoadAsset(Path)
	elseif ActivityDesc.ContentPicType == ActvContentPicTypeEnum.EActvContentPicType_Http then
		Sprite = UNZLuaHelperLibrary.GetImgTextrue2D(ActivityDesc.PicName)
	end

	if Sprite then
		local Brush = FSlateBrush.Temp()
		Brush.ResourceObject = Sprite
		Brush.ImageSize = self.m_PicWnd.Brush.ImageSize
		self.m_PicWnd:SetBrush(Brush)
		self.m_PicWnd:SelfHitTestInvisible()
	else
		self.m_PicWnd:Collapsed()
	end
end

function PicActivityPanel:GetJumpUrl( )
	local JumpUrl = self.m_ActivityDesc.ContentText
	local LoginData = GetCppSingleton(self, "UNZLoginDataServer")
	local areaid = 0
	if LoginData then
		if LoginData:IsWeiXinAccount() then
			areaid = 1
		elseif LoginData:IsQQAccount() then
			areaid = 2
		elseif LoginData:IsGuest() then
			areaid = 3
		end
	end

	local platid = 0
	if UGameplayStatics.GetPlatformName() == "IOS" then
		platid = 2
	else
		platid = 1
	end
	if not string.find(JumpUrl, "%?") then
		JumpUrl = JumpUrl.."?"
	else
		JumpUrl = JumpUrl.. "&"
	end
	JumpUrl = JumpUrl .. string.format("sAreaId=%d&sPartition=1&sPlatId=%d&sRoleId=%d", areaid, platid, LoginData:GetUin())
	return JumpUrl
end

function PicActivityPanel:Click( )
	local JumpUrl = self:GetJumpUrl()
	local JumpId = self.m_ActivityDesc.ContentPicGotoPos
	local BrowserSwitch = self.m_ActivityDesc.BrowserSwitch
	local GotoPos = self.m_ActivityDesc.GotoPos

	if JumpId == NavigationType.EHTML then
		requirecpp "UNZPictureActivityPanelBase"

		local useMSDKBroswer = UNZPictureActivityPanelBase.GetDefaultObject().bUseMSDKBrowser
		if BrowserSwitch == 1 then
			useMSDKBroswer = true
		elseif BrowserSwitch == 2 then
			useMSDKBroswer = false
		end
		self:SendUIMessage(NZUIMessage.OpenPlatformSpecialUrl, JumpUrl, useMSDKBroswer)
	elseif JumpId == NavigationType.EActivity then
		-- todo
		self:Fire("OpenActivity", JumpId)
	elseif JumpId == NavigationType.ERankMatch then
		self:SendUIMessage(NZUIMessage.RankMatch_ReqOpenRankMatchUI)
	else
		UNZUIManager.Push(JumpId)
	end
end

return PicActivityPanel