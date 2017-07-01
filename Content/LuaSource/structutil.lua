function FVector:__add(other)
	return UKismetMathLibrary.Add_VectorVector(self, other)
end
function FVector:__sub(other)
 	return UKismetMathLibrary.Subtract_VectorVector(self, other)
end
function FVector:__mul(other)
 	return UKismetMathLibrary.Multiply_VectorFloat(self, other)
end 
function FVector:__tostring()
	return string.format("X:%.2f  Y:%.2f  Z:%.2f", self.X, self.Y, self.Z)
end
function FVector:__len()
	return UKismetMathLibrary.VSize(self)
end
local FVector_CppNew = FVector.New
function FVector.New(x, y, z)
	local v = FVector_CppNew()
	if x then v.X = x end
	if y then v.Y = y end
	if z then v.Z = z end
	return v
end
function FVector:IsZero()
	return self.X==0 and self.Y == 0 and self.Z == 0
end
function FVector.ZeroVector()
	return FVector.New(0, 0, 0)
end

function FVector:Normal()
	return UKismetMathLibrary.Normal(self)
end

function FVector:Normal2D()
	local f = self:Normal()
	return FVector.New(f.X, f.Y, 0)
end

function FVector:Dot(other)
	return UKismetMathLibrary.Dot_VectorVector(self, other)
end

function FVector:Rotation()
	return UKismetMathLibrary.Conv_VectorToRotator(self)
end

function FVector:Cross(other)
	return UKismetMathLibrary.Cross_VectorVector(self, other)
end
FVector.Up = FVector.New(0,0,1)

local FRotator_CppNew = FRotator.New
function FRotator.New(Pitch, Yaw, Roll)
	local v = FRotator_CppNew()
	if Pitch then v.Pitch = Pitch end
	if Yaw then v.Yaw = Yaw end
	if Roll then v.Roll = Roll end
	return v
end

function FRotator:Vector()
	return UKismetMathLibrary.Conv_RotatorToVector(self)
end

function FRotator:__tostring()
	return string.format("X:%.1f  Y:%.1f  Z:%.1f", self.Pitch, self.Yaw, self.Roll)
end

function FReplifetimeCond.NewItem(name, cond)
	local v = FReplifetimeCond.New()
	v.PropertyName = name
	v.Cond = cond or ELifetimeCondition.COND_None
	return v
end

function FTransform.Make(Location, Rotation, Scale)
	Location = Location or FVector.New() 
	Rotation = Rotation or FRotator.New()
	Scale = Scale or FVector.New(1,1,1)
	return UKismetMathLibrary.MakeTransform(Location, Rotation, Scale)
end

function FTransform:Break()
	local Location = FVector.New()
	local Rotator = FRotator.New()
	local Scale = FVector.New()
	return UKismetMathLibrary.BreakTransform(self, Location, Rotator, Scale)
end

FVector2D.New = ULuautils.FVector2D_New
FColor.New = ULuautils.FColor_New