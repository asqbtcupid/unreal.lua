require "frame.debugger.debugsocketmgr"

local SimpleDlg = require "simpledlg"
local testUmg = Inherit(SimpleDlg)
testUmg:DynamicLoad("test")
function testUmg:Ctor(controller)
	self.Controller = controller
	self:Wnd("btn_testcoroutine"):Event("OnClicked", self.ClickCoroutine, self)
	self:Wnd("Play"):Event("OnClicked", self.PlayAnim, self)
	self:Wnd("Button_66"):Event("OnClicked", self.SendSomeThing, self)
	self:Wnd("Button_80"):Event("OnClicked", self.InitClient, self)
	self:Wnd("Button_81"):Event("OnClicked", self.InitServer, self)

	-- local MaterialFather = UMaterial.LoadObject(self, "/Game/Git/mt_fog.mt_fog")
	-- self.MID = UKismetMaterialLibrary.CreateDynamicMaterialInstance(self, MaterialFather)

	-- self.MID:SetTextureParameterValue("tx_fog", controller.m_FogMgr.Tx_Fog)
	-- self:Wnd("img_fog"):SetBrushFromTexture(controller.m_FogMgr.Tx_Fog)
	-- self.lala = 0
	-- local a = jj;
	-- jj:SetText(NSLOCTEXT("jiji", "bibi", "walalala"))
	-- jj:SetText(LOCTEXT("jiji2", "walalaladidi"))
	-- self:SetText()
	-- self:Wnd("txt1"):SetText(FText.NsLocText("jiji", "bibi", "walalala"))
	-- self:Wnd("txt2"):SetText("walalala")
	local format = NSLOCTEXT("jiji", "didi1", "TestForMat {1}/{2}")
	local value1 = NSLOCTEXT("jiji", "didi2", "ohohoh")
	local value2 = NSLOCTEXT("jiji", "didi3", "you are right")
	local result = format:Format(value1, value2)
	self:Wnd("txt1"):SetText(result)
end

local function GetRecvData(Socket)
	local IsContinue = true
	local HasRecvData	
	while IsContinue do
		local HasSome, Data = Socket:Recv()
		if HasSome and Data and #Data > 0 then
			HasRecvData = HasRecvData or ""
			HasRecvData = HasRecvData..Data
			IsContinue = true
		else
			IsContinue = false
		end
	end
	return HasRecvData
end

function testUmg:TestServerSocket()
	local IncommingConnection = self.m_ServerSocket:Accept("lalala")
	if IncommingConnection then
		self.m_ClientSocketIncomming = IncommingConnection
		self.m_ClientSocketIncomming:SetNonBlocking(true)
		a_("ohohoh client comming")
	end
	if self.m_ClientSocketIncomming then
		local Data = GetRecvData(self.m_ClientSocketIncomming)
		if Data then
			a_("server:", Data)
			self.m_ClientSocketIncomming:Send("yes")
		end
	end
end

function testUmg:TestClientSocket( )
	local data = GetRecvData(self.m_ClientSocket)	
	if data then
		a_("client:", data)
	end
end

local ServerUsedPort = 63335
function testUmg:InitServer( )
	if self.m_ServerSocket then
		return
	end
	local mgr = UnrealLuaSocketMgr:Get()
	self.m_ServerSocket, self.m_ServerPort = mgr:Listen(ServerUsedPort)
end

function testUmg:InitClient( )
	if self.m_ClientSocket then
		return 
	end

	self.m_ClientSocket = UnrealLuaSocketMgr:Get():ClientToHost(self.m_ServerPort)
end

local function test1()
	-- OpenDebugger()
	-- UGameplayStatics.OpenLevel(self, "TestMap", true, "")
	-- a_( self:Wnd("Button_81").OnClicked )
	local StructTypeIns = TestGlueEx.New() 
	local f = StructTypeIns["LuaGet_Glue_ReadWriteVector"]
	local t = os.clock()
	for i = 1, 1000000 do
		-- local x = StructTypeIns.ReadWriteVector
		local x = f(StructTypeIns)
	end
	a_(os.clock() - t)
	local Ins = UBenchmarkActor.NewObject()
	f = Ins["LuaGet_ReadWriteVector"]
	local t = os.clock()
	for i = 1, 1000000 do
		-- local x = StructTypeIns.ReadWriteVector
		local x = f(Ins)
	end
	a_(os.clock() - t)
end

local function test2()
	local t = os.clock()
	for i = 1, 1000000 do
		local x = {}
		x.x = 2
		local y = x.x		
	end
	a_(os.clock() - t)
end

local function test3()
	local StructTypeIns = TestGlueEx.New() 
	local t = os.clock()
	for i = 1, 1000000 do
		-- local x = StructTypeIns.ReadWriteVector
		local x = StructTypeIns["Glue_ReadWriteInt"]
	end
	a_(os.clock() - t)
end

function testUmg:PlayAnim()
	-- test2()
	-- test1()
	-- test3()
	-- local t = TestStaticProperty
	-- a_(rawset(t, "i", nil))
	-- -- a_()
	-- t.i = 100
	-- a_(t.staticsee())
	-- a_(t.i)

	-- local b = FVector.New(1,2,3)
	-- local c = FVector.Temp(1,2,3)
	-- assert(b.x == 1)
	-- a_(b.x)
	-- local u = UBenchmarkActor.NewObject()
	-- u:TestCallFunc_0param()
	-- local u = require "gameplay.test.benchmarkclass":NewOn(UBenchmarkActor.NewObject())
	-- a_(u.ReadWriteInt)
	-- a_(u.ReadWriteInt)
	local StructTypeIns = TestGlueEx.New() 
	-- local x = StructTypeIns.Glue_ReadWriteVector
	-- local x = StructTypeIns.Glue_ReadWriteVector
	local x = TestGlueEx.Glue_StaticReadWriteVector
	a_(x)
	local y = TestGlueEx.Glue_StaticReadWriteVector
	a_(y)
end

local function f()
end
local lala = FDelegateTest4.New(f)
local weakmeta = {__mode = "kv"}

function testUmg:ClickCoroutine( )
	local a = UBenchmarkActor.NewObject()
end

function testUmg:SendSomeThing()
	-- self.m_ClientSocket:SendStr("JustSend")
end

function testUmg:Txt1(content)
	-- self:Wnd("txt1"):SetText(FText.NsLocText("jiji", "bibi", "walalala"))
	-- self:Wnd("txt1"):SetText("walalala")
end

function testUmg:Txt2(content)
	-- self:Wnd("txt2"):SetText(tostring(content))
	-- self:Wnd("img_fog"):SetBrushFromMaterial(self.MID)
end

function testUmg:Txt3(content)
	-- self:Wnd("txt3"):SetText(tostring(content))
end

return testUmg