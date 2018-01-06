local CatchMeGameInstance = Inherit(CppObjectBase)

function CatchMeGameInstance:Ctor()
	GameInstance = self
end

return CatchMeGameInstance