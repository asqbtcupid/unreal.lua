local CMPlayerControllerBase = Inherit(CppObjectBase)

function CMPlayerControllerBase:Ctor( )
	self.m_InputMgr = InputMgr:NewIns(self)
	self.m_InputState = {}
end

function CMPlayerControllerBase:BeginPlay()
	CppObjectBase.BeginPlay(self)
end

function CMPlayerControllerBase:PlayerTick(DeltaTime)
	
end

function CMPlayerControllerBase:PreProcessInput(DeltaTime, bGamePaused)

end

function CMPlayerControllerBase:PostProcessInput(DeltaTime, bGamePaused)

end

function CMPlayerControllerBase:SetPawn(aPawn)
	self.m_Pawn = aPawn
end

function CMPlayerControllerBase:ProcessPlayerInput(DeltaTime, bGamePaused)
	self:GetInputState(self.m_InputState)
	self.m_InputMgr:Update(self.m_InputState, DeltaTime, bGamePaused)
end

return CMPlayerControllerBase