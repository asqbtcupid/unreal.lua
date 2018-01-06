local CMSpectatorPawn = Inherit(CppObjectBase, ACMSpectatorPawn)

function CMSpectatorPawn:Ctor()
	self:Timer(self.Tick, self):Time(0.001)
	-- self.m_HitTemp = FHitResult.New()
end

function CMSpectatorPawn:Tick( )
	local UI = self:GetController() and self:GetController().TestUI
	if UI then
		local x = self:K2_GetActorLocation().X /100+64
		local y = self:K2_GetActorLocation().Y /100+64
		UI:Txt2(self:K2_GetActorLocation())
		-- UI:Txt2(tostring(x.."  "..y))
		-- UI:Txt3(self:K2_GetActorRotation())
	end
	self.m_Player = self.m_Player or (self:GetController() and self:GetController().PlayCharacter)
	if self.m_Player then
		self:K2_SetActorLocation(self.m_Player:K2_GetActorLocation(), false, FHitResult.Temp(), true)
	end
end

function CMSpectatorPawn:StartPress(Pos)
	self.m_PressPos = Pos
	self.m_PawnPos = self:K2_GetActorLocation()
end

local factor = 2
function CMSpectatorPawn:Move(Pos)
	do return end
	local Rotator = self.Controller.PlayerCameraManager:GetCameraRotation():Vector()
	local ForwardVector = FVector.New(Rotator.X, Rotator.Y,0)
	local PressPos = self.m_PressPos
	local x = (Pos[1] - PressPos[1])*factor
	local y = (Pos[2] - PressPos[2])*factor
	local PawnPos = self.m_PawnPos
	PawnPos = PawnPos + ForwardVector:Normal()*y + ForwardVector:Cross(FVector.Up):Normal()*x
	self:K2_SetActorLocation(PawnPos, false, FHitResult.New(), true)
end

return CMSpectatorPawn