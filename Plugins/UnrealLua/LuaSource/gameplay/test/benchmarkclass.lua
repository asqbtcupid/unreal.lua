local BenchmarkBpClass = Inherit(CppObjectBase)

function BenchmarkBpClass:Start()
	collectgarbage("stop")
	self.m_TestResult = {}
-- GlueFunc
	a_("************Start UObject Type Bench****************")
	self:TestBPReflection()
	self:TestGlueFunction()
	-- do return end
	
	a_("************Start Struct Type Bench****************")
	local StructTypeIns = TestGlueEx.New() 
	self.TestGlueFunction(StructTypeIns)
	a_("************Start BpStruct Type Bench****************")
	local BpStructTypeIns = FTestGlueExBp.New() 
	BenchmarkBpClass.TestReadWrite(BpStructTypeIns, "ReadWriteInt", 1)
	BenchmarkBpClass.TestReadWrite(BpStructTypeIns, "ReadWriteVector", FVector.New(1,2,3))
	self.TestGlueFunction(BpStructTypeIns)
	a_("************Start common struct Bench****************")
	self:CommonStructBench(TestGlueEx)
	-- self:TestDynamicMultiCast()
	collectgarbage("restart")
end

local CallCountForBench = 10000
local NsTimeFactor = 1000000000/CallCountForBench
local function Clock()
	return FPlatformTime.Seconds() * NsTimeFactor
end

function BenchmarkBpClass:TestBPReflection( )
	self:TestFunc("TestCallFunc_0param")
	self:TestFunc("TestCallFunc_1param_int", 1)
	self:TestFunc("TestCallFunc_1param_int_ref", 1)
	self:TestFunc("TestCallFunc_1param_FVector", FVector.New(1,2,3))
	self:TestFunc("TestCallFunc_1param_FVector_ref", FVector.New(1,2,3))
	self:TestFunc("TestCallFunc_3param", i, self, FVector.New(1,2,3))
	self:TestFunc("TestCallFunc_Ret_int")
	self:TestFunc("TestCallFunc_Ret_FVector" )

	self:TestReadWrite("ReadWriteInt", 1)
	self:TestReadWrite("ReadWriteVector", FVector.New(1,2,3))
end

function BenchmarkBpClass:TestGlueFunction( )
	BenchmarkBpClass.TestFunc(self, "Glue_TestCallFunc_0param")
	BenchmarkBpClass.TestFunc(self, "Glue_TestCallFunc_1param_int", 1)
	BenchmarkBpClass.TestFunc(self, "Glue_TestCallFunc_1param_int_ref", 1)
	BenchmarkBpClass.TestFunc(self, "Glue_TestCallFunc_1param_FVector", FVector.New(1,2,3))
	BenchmarkBpClass.TestFunc(self, "Glue_TestCallFunc_1param_FVector_ref", FVector.New(1,2,3))
	BenchmarkBpClass.TestFunc(self, "Glue_TestCallFunc_3param", i, self, FVector.New(1,2,3))
	BenchmarkBpClass.TestFunc(self, "Glue_TestCallFunc_Ret_int")
	BenchmarkBpClass.TestFunc(self, "Glue_TestCallFunc_Ret_FVector" )

	BenchmarkBpClass.TestReadWrite(self, "Glue_ReadWriteInt", 1)
	BenchmarkBpClass.TestReadWrite(self, "Glue_ReadWriteVector", FVector.New(1,2,3))

	local cppins = self._cppinstance_ or self
	local meta = getmetatable(cppins)

	a_("static self part1 ")
	BenchmarkBpClass.TestReadWrite(self, "Glue_StaticReadWriteInt", 1)
	BenchmarkBpClass.TestReadWrite(self, "Glue_StaticReadWriteVector", FVector.New(1,2,3))	
	a_("static meta part2 ")
	BenchmarkBpClass.TestReadWrite(meta, "Glue_StaticReadWriteInt", 1, true)
	BenchmarkBpClass.TestReadWrite(meta, "Glue_StaticReadWriteVector", FVector.New(1,2,3), true)	
	a_("****static end****")

end

function BenchmarkBpClass:TestDynamicMultiCast()
	local CallCount = 10000
	local function f()
	end
	self.BenchDelegate_2Param:Add(InsCallBack(f, self))
	local t = Clock()
	self:Call_BenchDelegate_2Param(CallCount)
	local t1 = Clock()
	self.BenchDelegate_2Param:RemoveAll()
	self.BenchDelegate_2Param:Add(f)
	local t2 = Clock()
	self:Call_BenchDelegate_2Param(CallCount)
	local t3 = Clock()
	a_(string.format("BenchDelegate_2Param %.2f %.2f", t1-t, t3-t2))
end
function BenchmarkBpClass:TestFunc(FuncName, ...)
	-- self.m_TestResult[FuncName] = self.m_TestResult[FuncName] or {}
	local TestResult = ""
	local TestCount = CallCountForBench
	local cppins = self._cppinstance_ or self
	local TestFunc = self[FuncName]
	local t = Clock()
	for i = 1, TestCount do
		-- local tt = Clock()
		self[FuncName](self, ...)
		-- add = add + Clock()-tt
	end
	local t1 = Clock()
	for i = 1, TestCount do
		cppins[FuncName](cppins, ...)
	end
	local t2 = Clock()
	for i = 1, TestCount do
		TestFunc(cppins, ...)
	end
	local t3 = Clock()
	a_(string.format(FuncName..": %.2f %.2f %.2f", t1-t, t2-t1, t3-t2))

end

function BenchmarkBpClass:TestReadWrite(PropertyName, Data, bIsStatic)
	bIsStatic = bIsStatic or false
	local TestCount = CallCountForBench
	local cppins = self._cppinstance_ or self
	local readfunc = self["LuaGet_"..PropertyName]
	local writefunc = self["LuaSet_"..PropertyName]
	local t = Clock()
	for i = 1, TestCount do
		local x = self[PropertyName]
	end
	local t1 = Clock()
	for i = 1, TestCount do
		local x = cppins[PropertyName]
	end

	local t2 = Clock()
	if readfunc then
		if not bIsStatic then
			for i = 1, TestCount do
				local x = readfunc(self)
			end
		else
			-- for i = 1, TestCount do
			-- 	local x = readfunc(nil)
			-- end
		end
	end
	local t3 = Clock()

	a_(string.format(PropertyName.." Read: %.2f %.2f %.2f", t1-t, t2-t1, t3-t2))

	local t4 = Clock()
	for i = 1, TestCount do
		self[PropertyName] = Data
	end
	local t5 = Clock()
	for i = 1, TestCount do
		cppins[PropertyName] = Data
	end
	local t6 = Clock()
	if writefunc then
		if not bIsStatic then
			for i = 1, TestCount do
				writefunc(self, nil, Data)
			end
		else
			-- for i = 1, TestCount do
			-- 	writefunc(nil, nil, Data)
			-- end
		end
	end
	local t7 = Clock()
	a_(string.format(PropertyName.." Write: %.2f %.2f %.2f", t5-t4, t6-t5, t7-t6))
end

function BenchmarkBpClass:CommonStructBench(TheClass)
	local TestCount = CallCountForBench
	local t = Clock()
	for i = 1, TestCount do
		TheClass.New()
	end
	local t1 = Clock()
	for i = 1, TestCount do
		TheClass.Temp()
	end
	local t2 = Clock()
	a_(string.format(" New And Temp: %.2f %.2f ", t1-t, t2-t1))
end

return BenchmarkBpClass