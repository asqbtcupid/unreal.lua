require "editor.snew"

local function CollectGarbage()
	collectgarbage("collect")
end

TimerMgr:Get():On(CollectGarbage):Time(1)
local SharedTextBlock = nil 
local function OnClose(a,b,c )
	SharedTextBlock = nil
	a_(a, b, c, "lalala close")
end

local function SomeText()
	return tostring(math.abs(math.sin(os.clock()))).." ".. tostring(math.abs(math.cos(os.clock()+100)))
end
local function SomeColor()
	return FSlateColor.Temp(math.abs(math.sin(os.clock())), math.abs(math.cos(os.clock()+100)))
end

function SomeSimpleExampleForEditor()
	SharedTextBlock = TSharedPtr_STextBlock.New()
	local NewDock = SNew(SDockTab)
	:TabRole(ETabRole.NomadTab)
	:OnTabClosed(OnClose)
	[
		SNew(SVerticalBox)
		+SVerticalBox.Slot()
		.AutoHeight()
		[
			SAssignNew(SharedTextBlock, STextBlock)
			.Text("lalala")
			.ColorAndOpacity(SomeColor)
		]
		+SVerticalBox.Slot()
		.AutoHeight()
		.Padding(0.0)
		[
			SNew(STextBlock)
			.Text("lalala1")
			.ColorAndOpacity(SomeColor)
		]
		+SVerticalBox.Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+SHorizontalBox.Slot()
			[
				SNew(SCheckBox)
				.Style(FCoreStyle.Get(), "ToggleButtonCheckbox")
				[
					SNew(SBox)
					.VAlign(EHorizontalAlignment.VAlign_Center)
					.HAlign(EHorizontalAlignment.HAlign_Center)
					.Padding(FMargin.New(4.0, 2.0))
					[
						SNew(STextBlock)
						.Text("BOX")
					]
				]
			]
			+SHorizontalBox.Slot()
			[
				SNew(SCheckBox)
			]
		]
		+SVerticalBox.Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+SHorizontalBox.Slot()
			.AutoWidth()
			[
				SNew(SButton)
				.OnClicked(function () a_("click");return FReply.Handled() end)
				[
					SNew(STextBlock)
					.Text("ButtonTest")
				]
			]
			+SHorizontalBox.Slot()
			.AutoWidth()
			[
				SNew(SButton)
				.OnClicked(function () a_("click1");return FReply.Handled() end)
				[
					SNew(STextBlock)
					.Text("ButtonTest1")
				]
			]
			+SHorizontalBox.Slot()
			.AutoWidth()
			[
				SNew(SButton)
				.OnClicked(function () a_("click2");return FReply.Handled() end)
				[
					SNew(STextBlock)
					.Text("ButtonTest2")
				]
			]
		]
	]
	return NewDock()
end