function JustForProtoNew(Proto)
	local t = {}
	function t:Send()
		ProtoMgr:Get():LuaSendProto(Proto.__name, t)		
	end
	return t
end

ProtoMgr = Inherit(CppObjectBase)
requirecpp "UNZLuaProcessProto"

local Mapping = require "mgr.protomaping"
function ProtoMgr:Get( )
	return ProtoMgr._Singleton
end

function ProtoMgr:GetWorld( )
	if not self.m_World then
		self.m_World = ULuautils.GetWorld1(self)
	end
	assert(self.m_World ~= nil)
	return self.m_World
end

function ProtoMgr:Ctor( )
	ProtoMgr._Singleton = self
	self:Init()
	self:On("LuaShutdown", self.Release, self)
	self:InitListen()
end

function ProtoMgr:Destroy( )
	self:Shutdown()
end

function ProtoMgr:Test( ProtoName, TheDataTable)
	local ProtoProcessConfig = Mapping[ProtoName]
	local msgID = ProtoProcessConfig[1]
	local ProtoLuaFile = ProtoProcessConfig[2]
	require ("proto."..ProtoLuaFile)
	local ProtoMsgName = ProtoProcessConfig[3]
	local EncodeFuncName = "msg_"..ProtoMsgName.."Encode"
	local EncodeFunc = pb[EncodeFuncName]
	local TheEncoder = PbEncoder()
	EncodeFunc(TheDataTable, TheEncoder)
	local Buffer = TheEncoder:encode()

	local DecodeFuncName = "msg_"..ProtoMsgName.."Decode"
	local DecodeFunc = pb[DecodeFuncName]
	local BufferTable = DecodeFunc(PbDecoder(Buffer))
	Q_(TheDataTable)
	Q_(BufferTable)
end

function ProtoMgr:ProtoProcess(msgID, InBuffer)
	local t = os.clock()
	local mem = collectgarbage("count")
	local ProtoProcessConfig = Mapping[msgID]
	local ProtoMsgName = ProtoProcessConfig[3]
	local DecodeFuncName = "msg_"..ProtoMsgName.."Decode"
	local DecodeFunc = pb[DecodeFuncName]
	local BufferTable = DecodeFunc(PbDecoder(InBuffer))
	local DecodeTime = os.clock() - t
	local MemCost = collectgarbage("count") - mem
	-- DebugLog("Decode Proto "..ProtoMsgName.." Time:"..tostring(DecodeTime).."s, Mem:"..tostring(MemCost).."KB")
	GlobalEvent.Fire(ProtoMsgName, BufferTable)
	-- a_("lalala", ProtoMsgName)
	-- Q_(BufferTable)
end

function ProtoMgr:LuaSendProto(ProtoName, TheDataTable)
	assert(type(TheDataTable) == "table")
	local ProtoProcessConfig = Mapping[ProtoName]
	local msgID = ProtoProcessConfig[1]
	local ProtoMsgName = ProtoProcessConfig[3]
	local EncodeFuncName = "msg_"..ProtoMsgName.."Encode"
	local EncodeFunc = pb[EncodeFuncName]
	local TheEncoder = PbEncoder()
	EncodeFunc(TheDataTable, TheEncoder)
	local Buffer = TheEncoder:encode()
	assert(Buffer)
	UNZLuaProcessProto.SendProto(self, msgID, Buffer)
end

function ProtoMgr:Listen(ProtoName)
	local ProtoProcessConfig = Mapping[ProtoName]
	local msgID = ProtoProcessConfig[1]
	local ProtoLuaFile = ProtoProcessConfig[2]
	require ("proto."..ProtoLuaFile)
	self:RegisterProtoById(msgID)
end

function ProtoMgr:InitListen()
	for ProtoName in pairs(Mapping) do
		if type(ProtoName) == "string" then 
			self:Listen(ProtoName)
		end
	end
end

require "dataserver.activitydataserver":Get()

return ProtoMgr