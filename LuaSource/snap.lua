if ULuautils.GetName(actor):find("Foliage") then
	local c = UInstancedStaticMeshComponent.Cast(Hit.Component:Get())
	c:RemoveInstance(Hit.Item)
end


