local TcpConnection = Inherit(ObjectBase)
UnrealLuaSocketMgr = Inherit(ObjectBase)
local json = require "util.json".createJson()

local function Log(...)
	a_(...)
end

local seq = 0
function TcpConnection:Ctor(SocketBind, Name)
	self.m_TheSocketBind = SocketBind
	-- self:SetNonBlocking(true)
	self.m_Buffer = ""
	self.m_Name = Name or ""
	self.m_ClientConnections = {}
end

function TcpConnection:SetIsServer( IsServer)
	self.m_IsServer = IsServer
end

function TcpConnection:Listen(Port)
	self.m_TheSocketBind:Listen(Port)
end

function TcpConnection:Accept()
	local IncommingConnection = self.m_TheSocketBind:Accept("")
	if IncommingConnection then
		-- Log("IncommingConnection", self.m_Name)
		local NewConnection = TcpConnection:NewIns(IncommingConnection, "IncomingOne")
		NewConnection:SetNonBlocking(true)
		NewConnection:SetCallBack(self.m_RecvCallBack)
		NewConnection:SetRpcIns(self.m_RpcIns)
		table.insert(self.m_ClientConnections, NewConnection)
	end
end

function TcpConnection:SetCallBack(f)
	self.m_RecvCallBack = f
end

function TcpConnection:SetRpcIns(Ins)
	self.m_RpcIns = Ins
end

function TcpConnection:TryAccept( )
	local bHasPendingConnection = false
	local IsSucess = false
	IsSucess, bHasPendingConnection = self.m_TheSocketBind:HasPendingConnection()
	if IsSucess and bHasPendingConnection then
		self:Accept()	
	end
end

function TcpConnection:Tick()
	if self.m_IsServer then
		self:TryAccept()
		for _, Connection in ipairs(self.m_ClientConnections) do
			Connection:TryRecv()
		end
	else
		self:TryRecv()
	end
end

local PackHeaderLen = 10
local function NumToStr(Num, Len)
	local NumStr = tostring(Num)
	while #NumStr < PackHeaderLen do
		NumStr = "0"..NumStr
	end
	assert(#NumStr==PackHeaderLen)
	return NumStr
end

function TcpConnection:SendStr(Str)
	local len = #Str
	local LenStr = NumToStr(len, PackHeaderLen)
	local SendContent = LenStr..Str	
	if self.m_TheSocketBind then
		self.m_TheSocketBind:Send(SendContent)
	end
end

function TcpConnection:SendTable(t)
	t.Data = t.Data or {}
	local sendbuffer = json.encode(t)
	self:SendStr(sendbuffer)
end

function TcpConnection:SendTableToAllClient(t)
	if self.m_IsServer then
		local sendbuffer = json.encode(t)
		for i, Con in ipairs(self.m_ClientConnections) do
			Con:SendTable(t)
		end
	end
end

function TcpConnection:ReliableGet(t)
	local TimeoutTime = t.TimeOut or 5
	TimeoutTime = os.clock() + TimeoutTime
	t._Reliable = true
	seq = seq + 1
	t._seq = seq
	self:SendTable(t)

	while os.clock() <= TimeoutTime do
		local RecvData = self:TryRecv()
		if RecvData and RecvData._ReliableRsp and RecvData._seqRsp == t._seq then
			RecvData._ReliableRsp = nil
			RecvData._seqRsp = nil
			return RecvData
		end
	end
end

function TcpConnection:RecvByLen(Len)
	while #self.m_Buffer < Len do
		local HasSome, Data = self.m_TheSocketBind:Recv()
		if HasSome and Data and #Data > 0 then
			self.m_Buffer = self.m_Buffer..Data
		end
	end
	local ReturnData = self.m_Buffer:sub(1, Len)
	self.m_Buffer = self.m_Buffer:sub(Len+1, #self.m_Buffer) or ""

	return ReturnData
end

function TcpConnection:TryToConnect(Ip,Port)
	local SocketSystem = ISocketSubsystem:Get()
	self.m_TRYAddr = UnrealLuaSocketMgr:Get():CreateAddr(Ip, Port)
	local result = self.m_TheSocketBind:Connect(self.m_TRYAddr)
	Log("Connect Result:", result, Ip,Port)
	return result
end 

function TcpConnection:SetNonBlocking(bIstrue)
	self.m_TheSocketBind:SetNonBlocking(bIstrue)
end

function TcpConnection:HasPendingData( )
	if #self.m_Buffer > 0 then
		return true
	else
		return self.m_TheSocketBind:HasPendingData()
	end
end

function TcpConnection:Call(FuncName, ...)
	local Param = {}
	Param._Rpc = true
	Param._Name = FuncName
	Param._Data = {...}
	if self.m_IsServer then
		self:SendTableToAllClient(Param)
	else
		self:SendTable(Param)
	end
end

function TcpConnection:Callr(FuncName, ...)
	local Param = {}
	Param._Rpc_r = true
	-- Param._seq
	Param._Name = FuncName
	Param._Data = {...}
	if self.m_IsServer then
		assert(false)
	else
		self:SendTable(Param)
		local EndClock = os.clock() + 5
		while os.clock() <= EndClock do
			local Data = self:TryRecv()
			if Data then
				return Data
			end
		end
	end
end

function TcpConnection:TryRecv()
	while self:HasPendingData() do
		local PackLen = tonumber(self:RecvByLen(PackHeaderLen))
		local Data = self:RecvByLen(PackLen)
		-- Log("TCP RECV:", self.m_Name, PackLen, Data)
		local RecvData = json.decode(Data)
		if RecvData._Rpc and self.m_RpcIns then
			self.m_RpcIns[RecvData._Name](self.m_RpcIns, unpack(RecvData._Data))
		elseif RecvData._Rpc_r and self.m_RpcIns then
			local Return = self.m_RpcIns[RecvData._Name](self.m_RpcIns, unpack(RecvData._Data))
			local Param = {}
			Param._Rpc_r_rsp = true
			Param._Data = Return
			self:SendTable(Param)
		elseif RecvData._Rpc_r_rsp then
			return RecvData._Data
		else
			if self.m_RecvCallBack then
				self.m_RecvCallBack(RecvData)
			end
		end
		return Data
	end
end

function TcpConnection:BindNextPort(Addr, PortTryCount, PortStep)
	PortTryCount = PortTryCount or 1000
	PortStep = PortStep or 1
	return UnrealLuaSocketMgr:Get():GetSystem():BindNextPort(self.m_TheSocketBind, Addr, PortTryCount, PortStep)
end

function UnrealLuaSocketMgr:Ctor(SocketSystem)
	self.m_SocketSystem = SocketSystem
	self.m_Connections = {}
	self:Timer(self.Tick, self):Time(0.001)
end

local SingletonMgr
function UnrealLuaSocketMgr:Get( )
	if not SingletonMgr then
		local SocketSystem = ISocketSubsystem:Get()
		SingletonMgr = UnrealLuaSocketMgr:NewIns(SocketSystem)
	end
	return SingletonMgr
end

function UnrealLuaSocketMgr:GetSystem( )
	return self.m_SocketSystem
end

function UnrealLuaSocketMgr:CreateAddr(Ip, Port)
	self.m_SharedAddr = self.m_SocketSystem:CreateInternetAddr()
	local Addr = self.m_SharedAddr:Get()
	Addr:SetIp(Ip, false)
	Addr:SetPort(Port)	
	return Addr, self.m_SharedAddr
end

function UnrealLuaSocketMgr:CreateTcpConnect(Name)
	local ConnectSocket = self.m_SocketSystem:CreateSocket("Stream", "Server", false)
	return TcpConnection:NewIns(ConnectSocket, Name or "")
end

function UnrealLuaSocketMgr:Listen(Port)
	self.m_ServerConnection = self:CreateTcpConnect("Server")
	self.m_ServerConnection:SetNonBlocking(true)
	local BindAddr = self:CreateAddr("127.0.0.1", Port)
	self.m_ServerPort = self.m_ServerConnection:BindNextPort(BindAddr)
	self.m_ServerConnection:Listen(10)
	self.m_ServerConnection:SetIsServer(true)
	Log("debugger serverport:", self.m_ServerPort)
	return self.m_ServerConnection, self.m_ServerPort
end

function UnrealLuaSocketMgr:ClientToHost(Port)
	self.m_ClientConnection = self:CreateTcpConnect("Client")
	self.m_ClientConnection:SetNonBlocking(true)
	local ClientUsedPort = 63336
	local BindAddr = self:CreateAddr("127.0.0.1", ClientUsedPort)
	self.m_ClientPort = self.m_ClientConnection:BindNextPort(BindAddr)
	self.m_ClientConnection:TryToConnect("127.0.0.1", self.m_ServerPort)
	return self.m_ClientConnection
end

function UnrealLuaSocketMgr:CreateClient(Port)
	self.m_ClientConnection = self:CreateTcpConnect("Client")
	self.m_ClientConnection:SetNonBlocking(true)
	local BindAddr = self:CreateAddr("127.0.0.1", Port)
	self.m_ClientPort = self.m_ClientConnection:BindNextPort(BindAddr)
	return self.m_ClientConnection
end

function UnrealLuaSocketMgr:Tick()
	if self.m_ServerConnection then
		self.m_ServerConnection:Tick()
	end
	if self.m_ClientConnection then
		self.m_ClientConnection:Tick()
	end
end

function UnrealLuaSocketMgr:Destroy()
end

return UnrealLuaSocketMgr
