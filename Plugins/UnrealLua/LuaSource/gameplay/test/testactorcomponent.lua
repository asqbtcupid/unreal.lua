local TestActorComponent = Inherit(CppObjectBase)

function TestActorComponent:BeginPlay()
	CppObjectBase.BeginPlay(self)
	-- test link to actor
	-- self:Timer(self.TestLinkDestroy, self):Time(1)
end

function TestActorComponent:TestLinkDestroy()
end

return TestActorComponent