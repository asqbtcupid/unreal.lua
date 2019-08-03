local BenchmarkBpClass = Inherit(CppObjectBase)

function BenchmarkBpClass:Start()
	self.m_TestResult = {}
-- GlueFunc
	self:TestBPReflection()
	self:TestGlueFunction()
	self:TestDynamicMultiCast()
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
	self:TestFunc("Glue_TestCallFunc_0param")
	self:TestFunc("Glue_TestCallFunc_1param_int", 1)
	self:TestFunc("Glue_TestCallFunc_1param_int_ref", 1)
	self:TestFunc("Glue_TestCallFunc_1param_FVector", FVector.New(1,2,3))
	self:TestFunc("Glue_TestCallFunc_1param_FVector_ref", FVector.New(1,2,3))
	self:TestFunc("Glue_TestCallFunc_3param", i, self, FVector.New(1,2,3))
	self:TestFunc("Glue_TestCallFunc_Ret_int")
	self:TestFunc("Glue_TestCallFunc_Ret_FVector" )

	self:TestReadWrite("Glue_ReadWriteInt", 1)
	self:TestReadWrite("Glue_ReadWriteVector", FVector.New(1,2,3))
end

function BenchmarkBpClass:TestDynamicMultiCast()
	local CallCount = 100000
	local function f()
	end
	self.BenchDelegate_2Param:Add(InsCallBack(f, self))
	local t = os.clock()
	self:Call_BenchDelegate_2Param(CallCount)
	local t1 = os.clock()
	self.BenchDelegate_2Param:RemoveAll()
	self.BenchDelegate_2Param:Add(f)
	local t2 = os.clock()
	self:Call_BenchDelegate_2Param(CallCount)
	local t3 = os.clock()
	a_("BenchDelegate_2Param", t1-t, t3-t2)
end

function BenchmarkBpClass:TestFunc(FuncName, ...)
	self.m_TestResult[FuncName] = self.m_TestResult[FuncName] or {}
	local TestResult = ""
	local TestCount = 1000000
	local cppins = self._cppinstance_
	local TestFunc = self[FuncName]
	local t = os.clock()
	for i = 1, TestCount do
		self[FuncName](self, ...)
	end
	local t1 = os.clock()
	for i = 1, TestCount do
		cppins[FuncName](cppins, ...)
	end
	local t2 = os.clock()
	for i = 1, TestCount do
		TestFunc(self, ...)
	end
	local t3 = os.clock()
	a_(FuncName, t1-t, t2-t1, t3-t2)
end

function BenchmarkBpClass:TestReadWrite(PropertyName, Data)
	local TestCount = 1000000
	local cppins = self._cppinstance_
	local readfunc = self["LuaGet_"..PropertyName]
	local writefunc = self["LuaSet_"..PropertyName]
	local t = os.clock()
	for i = 1, TestCount do
		local x = self[PropertyName]
	end
	local t1 = os.clock()
	for i = 1, TestCount do
		local x = cppins[PropertyName]
	end
	local t2 = os.clock()
	if readfunc then
		for i = 1, TestCount do
			local x = readfunc(self)
		end
	end
	local t3 = os.clock()
	a_(PropertyName, "Read", t1-t, t2-t1, t3-t2)

	local t4 = os.clock()
	for i = 1, TestCount do
		self[PropertyName] = Data
	end
	local t5 = os.clock()
	for i = 1, TestCount do
		cppins[PropertyName] = Data
	end
	local t6 = os.clock()
	if writefunc then
		for i = 1, TestCount do
			writefunc(self, Data)
		end
	end
	local t7 = os.clock()
	a_(PropertyName, "Writ", t5-t4, t6-t5, t7-t6)
end

return BenchmarkBpClass