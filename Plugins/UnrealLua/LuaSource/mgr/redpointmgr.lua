--[[
	红点数据本质上是个树结构，父节点是否红点取决于它有几个子节点，这是一个递归过程
	而子节点自身数据变化的时候，需要顺着往根节点的方向依次通知父亲节点。

	结合以上两个特点，利用GlobalEvent全局事件和lua的元表，做出这个会自动广播的树结构。

	创建一个根节点：RedPointMgr:Get():Create("名字")
	获取一个根节点：RedPointMgr:Get():GetData("名字")
	继承于ObjectBase的实例，有简化api，self:Red("名字") == RedPointMgr:Get():GetData("名字")，

	创建叶子节点：
	local ParentRedData = RedPointMgr:Get():GetData("ParentRedData")
	ParentRedData[somekey] = value
	value可以是bool类型，可以是整数。
	
	当value是table类型时，会创建非叶子节点，非叶子节点可以继续添加子节点

	当红点数据变化时，会在下一帧以"名字"为key广播一次自己的数据，这一帧多次数据变化，都只会在下一帧
	广播一次。

	每一个非叶子节点，都有一个api是Count(Deep)，Deep默认是1000
	例如ParentRedData:Count(deep)查询从自己算起高度为deep，总共有几个处于红点状态的叶子节点，
	true代表一个红点，整数表示n个红点
--]]

local function TotalCount(t, deep)
	if type(t) == "table" then
		local Total = 0
		for k, v in pairs(t.__rawdata) do
			Total = Total + TotalCount(v, deep - 1)
		end
		if deep > 0 then
			return Total
		else 
			return Total > 0 and 1 or 0
		end
	elseif type(t) == "number" then
		return t
	elseif type(t) == "function" then
		return t()
	elseif t == true then
		return 1
	else
		return 0
	end
end

local function Broadcast(t)
	if t then
		if not t.__pendingbroadcast then
			local function TrueBroadcast()
				t.__pendingbroadcast = false
				if rawget(t, "__name") then
					-- 以自己的名字广播自己的数据
					GlobalEvent.Fire(t.__name, t)
				end
			end
			-- 下一帧再广播，因为这一帧接下来的运算还会导致红点数据变化
			TimerMgr:Get():On(TrueBroadcast):Time(0.0001):Num(1)
			t.__pendingbroadcast = true
			Broadcast(t.__parent)
		end
	end
end

local NewData
local meta = {}
function meta.__index(t, k)
	if k == "__parent" then
		return nil
	elseif k == "__rawdata" then
		return rawget(t, k)
	end
	local rawdata = t.__rawdata
	-- 不需要初始化
	-- 简化Data[100][100][200] = true
	-- 不需要：
	-- Data[100]={}
	-- Data[100][100] = {}
	-- Data[100][100][200] = {}
	if rawdata[k] == nil then
		rawdata[k] = NewData(nil, t)
	end
	return rawdata[k]
end

function meta.__newindex(t, k, v)
	local old = t.__rawdata[k]
	if type(v) == "table" then
		if getmetatable(v) ~= meta then
			local raw_v = v
			v = NewData(nil, t)
			rawset(v, "__rawdata", raw_v)
		end
	end
	t.__rawdata[k] = v
	if old ~= v then
		Broadcast(t)
	end
end

NewData = function(Name, Parent)
	local t = {}
	t.__rawdata = {}
	t.__name = Name
	t.__parent = Parent
	t.__pendingbroadcast = false
	function t:Count(Deep)
		Deep = Deep or 10000
		return TotalCount(self, Deep)
	end
	return setmetatable(t, meta)
end 

RedPointMgr = Inherit(Singleton)
function RedPointMgr:Ctor()
	self.m_Reds = {}
end

function RedPointMgr:Create(RedName)
	if not self.m_Reds[RedName] then
		local NewOne = NewData(RedName)
		self.m_Reds[RedName] = NewOne 
	end
end

function RedPointMgr:GetData(RedName)
	-- 若不存在，则创建一个
	if not self.m_Reds[RedName] then
		self:Create(RedName)
	end
	return self.m_Reds[RedName]
end

return RedPointMgr