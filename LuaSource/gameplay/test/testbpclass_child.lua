local TestBpClass = require "gameplay.test.testbpclass"
local TestBpClass_Child = Inherit(TestBpClass)

function TestBpClass_Child:BeginPlay( )
	CppObjectBase.BeginPlay(self)
	assert(testbpclass.TestOverride(self) == 100)
	assert(self:TestOverride() == 200)
	assert(self:Interface_NativeEvent(200) == 1777)
	assert(self:Interface_BpImplement(1) == true)
end

return TestBpClass_Child