local TestBpClass = Inherit(CppObjectBase, AStaticMeshActor)
function TestBpClass:EndPlay(Reason)
	CppObjectBase.EndPlay(self)
end

function TestBpClass:BeginPlay( )
	CppObjectBase.BeginPlay(self)
end

return TestBpClass