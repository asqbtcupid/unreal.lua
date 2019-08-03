local StateClass = Class(Object)
InputMgr = Class(ObjectBase)

function StateClass:Ctor()
	self.bIsPress = false
	self.fHoldTime = 0
	self.vLastPos = nil
end

function InputMgr:Ctor(controller)
	self.m_controller = controller
	local stateins = self:AddGcChild(StateClass:NewIns())
	self.States = {stateins}
end

function InputMgr:Update1(bIsPress, Pos, DeltaTime, bGamePaused)
	local State = self.States[1] 
	if not bIsPress and not State.bIsPress then return end

	State.fHoldTime = State.fHoldTime + DeltaTime
	if State.bIsPress then
		if bIsPress then
			if math.Dis(Pos, State.vLastPos) >= 1 then
				self.m_controller:HandleInput("InputTap_Move", Pos, State.fHoldTime, {Pos[1]-State.vLastPos[1], Pos[2]-State.vLastPos[2]})
			else
				self.m_controller:HandleInput("InputTap_Hold", Pos, State.fHoldTime)
			end
		else
			self.m_controller:HandleInput("InputTap_Release", Pos, State.fHoldTime)
			State.fHoldTime = 0
		end
	else		
		if bIsPress then
			self.m_controller:HandleInput("InputTap_Press", Pos, State.fHoldTime)
		end
	end
	State.bIsPress = bIsPress
	State.vLastPos = Pos
end

function InputMgr:Update(InputState, DeltaTime, bGamePaused)
	self:Update1(InputState[3] ~= 0, {InputState[1], InputState[2]}, DeltaTime, bGamePaused)
end

return InputMgr