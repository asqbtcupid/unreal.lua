requirecpp "UUserWidget"
requirecpp "UWidgetBlueprintLibrary"
requirecpp "ESlateVisibility"
requirecpp "FMargin"
require "ui.binddatahandle"
local HU = require "luahotupdate"

BasicWidget = Inherit(CppObjectBase)
BasicWidget.BpClassPath = ""

function BasicWidget:Ctor()
	self.m_Widgets = {}
	self.m_ChildWigets = {}
end

function BasicWidget:Destroy( )
	for k, Ins in pairs(self.m_Widgets) do
		Ins:Release()
	end
	self.m_Widgets = nil

	self:RemoveAllEvent()
	local childarr = {}
	for child in pairs(self.m_ChildWigets) do
		table.insert(childarr, child)
	end
	for i, v in ipairs(childarr) do
		v:Release()
	end
	self.m_ChildWigets = nil
end

function BasicWidget:Txt(str)
	self:SetText(tostring(str))
end

function BasicWidget:Hidden()
	self:SetVisibility(ESlateVisibility.Hidden)
end

function BasicWidget:Collapsed()
	self:SetVisibility(ESlateVisibility.Collapsed)
end

function BasicWidget:Visible()
	self:SetVisibility(ESlateVisibility.Visible)
end

function BasicWidget:SelfHitTestInvisible()
	self:SetVisibility(ESlateVisibility.SelfHitTestInvisible)
end

function BasicWidget:Event(delegateName, callback, ...)
	self.m_delegates = self.m_delegates or {}
	local delegates = self.m_delegates
	if not delegates[delegateName] then
		delegates[delegateName] = self[delegateName] 
	end
	return delegates[delegateName]:Add(InsCallBack(callback, ...))
end

function BasicWidget:RemoveEvent(delegateName)
	self.m_delegates = self.m_delegates or {}
	local delegates = self.m_delegates
	if delegates[delegateName] then
		delegates[delegateName]:RemoveAll()
	end
end

function BasicWidget:RemoveEventByHandle(delegateName, r)
	self.m_delegates = self.m_delegates or {}
	local delegates = self.m_delegates
	if delegates[delegateName] then
		delegates[delegateName]:Remove(r)
	end
end

function BasicWidget:RemoveAllEvent( )
	if self.m_delegates ~= nil then
		for name in pairs(self.m_delegates) do
			self:RemoveEvent(name)
		end
	end	
end

function BasicWidget:RefreshName2Widget(bIsForce)
	if self.Name2Widget == nil or bIsForce then
		self.Name2Widget = TMap_FName_UObject.New()
		UNZLuaUIManager.GetName2Widget(self, self.Name2Widget)
	end
end

local CppClassMapLuaClassPath =
{
	UVerticalBox = "luauverticalbox",
	UHorizontalBox = "luauhorizontalbox",
	UNZUIBaseView = "nzluasimpleview",
}

local function GetLuaWidgetClass(CppIns)
	local LuaClassName = CppClassMapLuaClassPath[CppIns.classname]
	if LuaClassName then
		return require ("ui."..LuaClassName)
	else
		return BasicWidget
	end
end

function BasicWidget:Wnd(Name)
	assert(Name)
	self:RefreshName2Widget()
	if type(Name) == "string" then
		if not self.m_Widgets[Name] then
			local function AddNewWnd()
				local TargetWnd = self[Name]
				if TargetWnd == nil and self.Name2Widget ~= nil then
					 TargetWnd = self.Name2Widget[Name]
					 if TargetWnd == nil then
					 	self:RefreshName2Widget(true)
					 	TargetWnd = self.Name2Widget[Name]
					 end
				end
				if TargetWnd then
					local NewIns
					if type(TargetWnd) == "table" then
						NewIns = TargetWnd
					else
						NewIns = GetLuaWidgetClass(TargetWnd):NewOn(TargetWnd)
					end
					self.m_Widgets[Name] = NewIns
				else
					error(Name.." doesn't exist")
				end
			end
			Xpcall(AddNewWnd)
		end
	elseif type(Name) == "userdata" or type(Name) == "table" then
		if not self.m_Widgets[Name] then
			local TargetWnd = Name
			local NewIns = BasicWidget:NewOn(TargetWnd)
			self.m_Widgets[Name] = NewIns
		end
	end
	if self.m_Widgets[Name] == nil then
		self.m_Widgets[Name] = HU.InitFakeTable()()
		DebugLog("error")
	end
	return self.m_Widgets[Name]
end

function BasicWidget:GroupTab(...)
	self.m_GroupTabs = self.m_GroupTabs or {}
	local Tabs = {...}
	table.insert(self.m_GroupTabs, Tabs)
	local GroupId = #self.m_GroupTabs+1
	local OnCheckFunc = nil
	local TabsHandle = {}
	TabsHandle.Tabs = Tabs
	function TabsHandle:Call(...)
		OnCheckFunc = InsCallBack(...)
		return self
	end
	function TabsHandle:Switch(EventName, HitOperator, OtherOperation)
		TabsHandle.HitOperator = HitOperator
		TabsHandle.OtherOperation = OtherOperation
		local function f(self, IndexChecked)
			TabsHandle:CheckByIndex(IndexChecked)
		end
		for Index, Tab in ipairs(Tabs) do
			Tab:Event(EventName, f, Tab, Index)
		end
		return self
	end
	function TabsHandle:Destroy()
	end
	function TabsHandle:CheckByIndex(IndexChecked, ...)
		for ii, Tab in ipairs(Tabs) do
			if ii == IndexChecked then
				Tab[TabsHandle.HitOperator](Tab)
				if OnCheckFunc then
					OnCheckFunc(IndexChecked, ...)
				end
			else
				Tab[TabsHandle.OtherOperation](Tab)
			end 
		end
	end
	return TabsHandle
end

function BasicWidget:CreateChild(LuaClassPath, ...)
	if type(LuaClassPath) == "string" then
		local Ins = require(LuaClassPath):New(self, ...)
		Ins:SetParent(self)
		return Ins
	elseif type(LuaClassPath) == "table" then
		local Ins = LuaClassPath:New(self, ...)
		Ins:SetParent(self)
		return Ins
	elseif type(LuaClassPath) == "userdata" then
		local BpClass = LuaClassPath
		local inscpp = UWidgetBlueprintLibrary.Create(self, BpClass, nil)
		local Ins = BasicWidget:NewOn(inscpp, ...)
		Ins:SetParent(self)
		return Ins
	end
	assert(false)
end

function BasicWidget:AsynCreateChild(LuaClassPath, ...)
	local CoroutineHandle = CoroutineMgr:Get():GetHandle()
	local Ins
	if CoroutineHandle and CoroutineHandle:IsValid() then
		Ins = require(LuaClassPath):AsynNew(self, ...)
	else
		Ins = require(LuaClassPath):New(self, ...)
	end
	Ins:SetParent(self)
	return Ins
end

function BasicWidget:ChildWidgetDestroy(Ins)
	self.m_ChildWigets[Ins] = nil
end

function BasicWidget:AddChild_CallFromChild(ChildIns)
	self.m_ChildWigets[ChildIns] = ChildIns
end

function BasicWidget:NewInsByBpClass(BpClass, Outer, ... )
	assert(BpClass)
	local inscpp = UWidgetBlueprintLibrary.Create(Outer, BpClass, nil)
	assert(inscpp)
	local ins = self:NewOn(inscpp, ...)
	return ins
end

function BasicWidget:New(Outer, ...)
	assert(type(Outer) == "table" )
	assert(self.BpClassPath and self.BpClassPath ~= "")
	local BpClass = UUserWidget.LoadClass(Outer, self.BpClassPath)
	return self:NewInsByBpClass(BpClass, Outer, ...)
end

function BasicWidget:AsynNew(Outer, ...)
	assert(type(Outer) == "table" )
	assert(self.BpClassPath and self.BpClassPath ~= "")
	local BpClass = CoroutineUtil.LoadAsset(self.BpClassPath)
	return self:NewInsByBpClass(BpClass, Outer, ...)
end

function BasicWidget:SetParent(Parent)
	self.m_Parent = Parent
	Parent:AddChild_CallFromChild(self)
end

function BasicWidget:Bind(WidgetClass)
	if not self.m_BindDataHandle then
		self.m_BindDataHandle = BindDataHandle:NewIns(self, WidgetClass)
	end
	return self.m_BindDataHandle
end

return BasicWidget
