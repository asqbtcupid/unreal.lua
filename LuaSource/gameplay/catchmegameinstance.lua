local CatchMeGameInstance = Inherit(CppObjectBase, UCatchMeGameInstance)

function CatchMeGameInstance:Ctor()
	GameInstance = self
end

return CatchMeGameInstance