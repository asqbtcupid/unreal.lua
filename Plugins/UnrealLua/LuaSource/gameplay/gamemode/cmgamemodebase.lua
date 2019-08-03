local CMGameModeBase = Inherit(CppObjectBase, ACMGameModeBase)

function CMGameModeBase:Ctor()
end

function CMGameModeBase:BeginPlay()
	CppObjectBase.BeginPlay(self)
end

function CMGameModeBase:SetDefaultPawn(defaultPawn)
end

function CMGameModeBase:Tick(Delta)
end

return CMGameModeBase
