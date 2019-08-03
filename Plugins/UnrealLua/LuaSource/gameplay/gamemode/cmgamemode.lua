local CMGameModeBase = require "gameplay.gamemode.cmgamemodebase"
local CMGameMode = Inherit(CMGameModeBase, ACatchMeGameMode)

function CMGameMode:Ctor()
	self.PlayerControllerClass = ACatchMePlayerController.Class()
	-- self.DefaultPawnClass = APawn.FClassFinder("/Game/TopDownCPP/Blueprints/TopDownCharacter")
	self.DefaultPawnClass = ACMSpectatorPawn.FClassFinder("/Game/Git/bp_spectator")
end

function CMGameMode:BeginPlay()
	CMGameModeBase.BeginPlay(self)
end

return CMGameMode