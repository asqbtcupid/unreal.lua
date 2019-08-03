requirecpp "UNZAssetManager" 
local MemberFunction = {}

function MemberFunction:AsyncLoad(ObjectSoftPath, ...)
	return RequestAsyncLoad(ObjectSoftPath, InsCallBack(...))
end

-- example
-- local TShared_handle = self:AsyncLoadImage(40600000035, 0, self.Test, self)
-- local handle = TShared_handle:Get()
function MemberFunction:AsyncLoadImage(ResourceId, ResourceType, ...)   
	ResourceId = tostring(ResourceId)
	local AssertPath = UNZAssetManager.Get():GetIconSpritePath(ResourceId, ResourceType)
	return self:AsyncLoad(AssertPath, ...)
end

return MemberFunction