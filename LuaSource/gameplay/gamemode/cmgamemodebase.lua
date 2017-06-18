local CMGameModeBase = Inherit(CppObjectBase, ACMGameModeBase)

function CMGameModeBase:Ctor()
	G_GameStatics.GameMode = self
	self.PlayerControllers = {}
end

function CMGameModeBase:BeginPlay()
end

function CMGameModeBase:SetDefaultPawn(defaultPawn)
	self.defaultPawn = defaultPawn
end

function CMGameModeBase:AddPlayerController(controller )
	self.PlayerControllers[controller] = true
end

return CMGameModeBase
