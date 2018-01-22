local TestBpClass = require "gameplay.test.testbpclass"
local TestBpClass_Child = Inherit(TestBpClass)

function TestBpClass_Child:BeginPlay( )
	CppObjectBase.BeginPlay(self)
	if _WITH_EDITOR then
		assert(testbpclass.TestOverride(self) == 100)
		assert(self:TestOverride() == 200)
		assert(self:Interface_NativeEvent(200) == 1777)
		assert(self:Interface_BpImplement(1) == true)
	end
end

function TestBpClass_Child:TestInterface(I)
	if _WITH_EDITOR then
		assert(I == self.TestAssetRegistry)
		self.TestAssetRegistry = nil
		assert(self.TestAssetRegistry == nil)
		self.TestAssetRegistry = I
		assert(I == self.TestAssetRegistry)

		assert(self.test_interface_public == nil)
		assert(self.test_interface_private == nil)

		self.test_interface_public = self
		self.test_interface_private = self

		assert(self.test_interface_public == self)
		assert(self.test_interface_private == self)

		self:Test_Param_Interface_public(self)
		self:Test_Param_Interface_private(self)
	end
end

return TestBpClass_Child