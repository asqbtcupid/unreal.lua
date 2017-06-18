local CMPlayerControllerBase = Inherit(CppObjectBase, ACMPlayerControllerBase)

function CMPlayerControllerBase:Ctor( )
	self.m_InputMgr = InputMgr:NewIns(self)
	G_GameStatics.GameMode:AddPlayerController(self)
end

function CMPlayerControllerBase:BeginPlay()

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
	self.m_InputMgr:Update(self:GetInputState(), DeltaTime, bGamePaused)
end

return CMPlayerControllerBase