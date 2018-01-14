local TestBpClass = require "gameplay.test.testbpclass"
local TestBpClass_Child = Inherit(TestBpClass)

function TestBpClass_Child:BeginPlay( )
	CppObjectBase.BeginPlay(self)
	assert(testbpclass_C.TestOverride(self) == 100)
	assert(self:TestOverride() == 200)
end

return TestBpClass_Child