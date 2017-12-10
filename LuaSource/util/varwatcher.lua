LuaVarWatcher = Inherit(Singleton)

function LuaVarWatcher:Ctor()
	-- only work in editor mode, GameInstance can be other UObject instance
	if GameInstance == nil or UVarNode == nil then
		self.m_NotWork = true
	end
	local weakmeta = {__mode = "kv"}
	self.m_WeakRegistry = setmetatable({}, weakmeta)
	self.m_MaxRegistryIndex = 1
	self.m_IndexGUID = 0
	self.m_Root = {}
	self.m_NeedShowFunction = false
	self:Timer(self.Tick, self):Time(0.00001)
	GlobalEvent.On("LuaShutdown", self.Release, self)
end

function LuaVarWatcher:Destroy( )
	for UNode in pairs(self.m_Root) do
		UNode:Release()
	end
end

function LuaVarWatcher:GetUNodeNewName( )
	self.m_IndexGUID = self.m_IndexGUID + 1
	return "UNode"..tostring(self.m_IndexGUID)
end

function LuaVarWatcher:GetClassName(VarValue)
	if type(VarValue) == "table" or type(VarValue) == "userdata" then
		if VarValue.classname then
			return "("..VarValue.classname..")"
		end
	end
	return ""
end


function LuaVarWatcher:UpdateNodesChildren(UNode, VarValue, bIsKey)
	if UNode:bIsExpanding() or (UNode.HasExpand and not UNode:HasChild()) then
		local function TravelVar(ChildrenMap, key, v, KeyName, bTravelKey)
			local ValueStr = tostring(v)..self:GetClassName(v)
			local ChildNode = ChildrenMap[key]
			if ChildNode then
				ChildNode.Value = ValueStr
			else
				local Index = self:AddToIndexMap(key)
				ChildNode = UVarNode.New(GameInstance, self:GetUNodeNewName())
				ChildNode:Init(KeyName, ValueStr, Index)
				UNode:AddChild(ChildNode, bIsKey)
			end

			if UNode:bIsExpanding() then
				ChildNode.HasExpand = true
			end

			if bTravelKey then
				self:UpdateNodesChildren(ChildNode, key, true)
			end
			self:UpdateNodesChildren(ChildNode, v, false)
		end
		if type(VarValue) == "table" then
			local ChildrenMap = {}
			local ChildrenArr
			if not bIsKey then
				ChildrenArr = UNode.Children
			else
				ChildrenArr = UNode.KeyChildren
			end
			for i, Node in ipairs(ChildrenArr) do
				local Key = self.m_WeakRegistry[Node.LuaIndex]
				if (Key and VarValue[Key]) or getmetatable(VarValue) == Key then
					if not self.m_NeedShowFunction and type(VarValue[Key]) == "function" then
						Node:Release()
					else
						ChildrenMap[Key] = Node
					end
				else
					Node:Release()
				end
			end

			for key, v in pairs(VarValue) do
				if self.m_NeedShowFunction or type(v) ~= "function" then
					local KeyName = tostring(key)
					if bIsKey then
						KeyName = "$Key$ "..KeyName
					end
					KeyName=KeyName..self:GetClassName(key)
					TravelVar(ChildrenMap, key, v, KeyName, true)
				end
			end
			local meta = getmetatable(VarValue)
			if meta then
				local KeyName = "$meta$ "
				if bIsKey then
					KeyName = "$Key$ "..KeyName
				end
				TravelVar(ChildrenMap, meta, meta, KeyName, false)
			end
		elseif type(VarValue) == "userdata" then
			local meta = getmetatable(VarValue)
			if meta then
				local ChildrenMap = {}
				local ChildrenArr
				if not bIsKey then
					ChildrenArr = UNode.Children
				else
					ChildrenArr = UNode.KeyChildren
				end
				for i, Node in ipairs(ChildrenArr) do
					local Key = self.m_WeakRegistry[Node.LuaIndex]
					ChildrenMap[Key] = Node
				end

				for key, v in pairs(meta) do
					if key:find("^LuaGet_") and type(v) == "function" then
						local value = v(VarValue)
						local KeyName = key:match("^LuaGet_(.*)")
						if bIsKey then
							KeyName = "$Key$ "..KeyName
						end
						KeyName=KeyName..self:GetClassName(key)
						TravelVar(ChildrenMap, key, value, KeyName, false)
					end
				end

				local KeyName = "$meta$"
				if bIsKey then
					KeyName = "$Key$ "..KeyName
				end
				TravelVar(ChildrenMap, meta, meta, KeyName, false)
			end
		end
	end
end

function LuaVarWatcher:Tick( )
	if UVarNode.NeedUpdate() then
		self.m_NeedShowFunction = UVarNode.NeedShowFunction()
		local RootToClean = {}
		for UNode, RootNodeIndex in pairs(self.m_Root) do
			local VarValue = self.m_WeakRegistry[RootNodeIndex]
			if VarValue then
				self:UpdateNodesChildren(UNode, VarValue)
			else
				table.insert(RootToClean, UNode)
			end
		end
		for _, UNode in ipairs(RootToClean) do
			UNode:Release()
			self.m_Root[UNode] = nil
		end
	end
end

function LuaVarWatcher:AddToIndexMap(VarValue)
	local Index = self.m_MaxRegistryIndex
	self.m_WeakRegistry[Index] = VarValue
	self.m_MaxRegistryIndex = Index + 1
	return Index
end

function LuaVarWatcher:OpenChild(UNode, VarValue)
	UNode.HasExpand = true
	self:UpdateNodesChildren(UNode, VarValue, false)
end

function LuaVarWatcher:AddNode(VarName, VarValue)
	if self.m_NotWork then
		return
	end
	
	local Index = self:AddToIndexMap(VarValue)
	local NewUNode = UVarNode.New(GameInstance, self:GetUNodeNewName())
	NewUNode:Init(VarName, tostring(VarValue), Index)
	self.m_Root[NewUNode] = Index
	NewUNode:AddToRootArr()
	self:OpenChild(NewUNode, VarValue)
end

return LuaVarWatcher