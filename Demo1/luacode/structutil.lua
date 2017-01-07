local FVector_CppNew = FVector.New
function FVector.New(x, y, z)
	local v = FVector_CppNew()
	if x then v.X = x end
	if y then v.Y = y end
	if z then v.Z = z end
	return v
end

local FRotator_CppNew = FRotator.New
function FRotator.New(Pitch, Yaw, Roll)
	local v = FRotator_CppNew()
	if Pitch then v.Pitch = Pitch end
	if Yaw then v.Yaw = Yaw end
	if Roll then v.Roll = Roll end
	return v
end