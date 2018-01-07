local AnimInstance = Inherit(CppObjectBase)

function AnimInstance:NativeInitializeAnimation()
	local Pawn = self:TryGetPawnOwner()
	self:Link(Pawn)
	self.m_Pawn = Pawn
end

function AnimInstance:NativeUpdateAnimation()
	if self.m_Pawn then
		self.WalkSpeed = #self.m_Pawn:GetVelocity()
		local controller = UGameplayStatics.GetPlayerController(self, 0)
		if controller and controller.PlayCharacter == self.m_Pawn and controller.TestUI then
			controller.TestUI:Txt1(self.WalkSpeed)		
		end
	end
end

return AnimInstance