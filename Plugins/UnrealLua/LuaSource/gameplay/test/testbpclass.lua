
local TestBpClass = Inherit(CppObjectBase)
local function FloatEqual(a, b)
	return math.abs(a-b)<0.01
end
function TestBpClass:EndPlay(Reason)
	CppObjectBase.EndPlay(self)
end
local HasRestart = false
function TestBpClass:RestartLevel( )
	if not HasRestart then
		HasRestart  = true
		local World = self:GetWorld()
		local GameInstance = World:GetGameInstance()
		local function f()
			a_("now try to restart, test the reference problem during gc")
			UGameplayStatics.OpenLevel(GameInstance, "TestMap", true, "")
		end
		TimerMgr:Get():On(f):Time(0.01):Num(1)
	end
end

function TestBpClass:BeginPlay( )
	CppObjectBase.BeginPlay(self)
	-- do return end	
	self:TestNativePublicPropertyReadWrite()
	self:TestNativePrivatePropertyReadWrite()
	self:TestCppFunction()
	self:TestBpPropertyReadWrite()
	self:TestBpPropertyArr()
	self:TestArrParam()
	self:TestBpPropertyArr1()
	self:TestReturnStruct()
	self:TestArrInnerStruct()
	self:TestDelegate()
	self:TestMapProperty()
	self:TestMapRefFunc()
	self:TestMapRetFunc()
	self:TestSetProperty()
	self:TestSetFunc()
	self:TestEnum()
	self:TestBpStructType()
	self:TestInterface()
	self:TestCoroutinue()
	self:TestFistMemOfStruct()
	self:TestPushStructStack()
	self:TestStaticThing()
	self:TestCallTestRefStruct()
	self:TestUserDefineStruct()
	self:TestContainerReturn()
	self:TestGlueArr()
	self:TestGlueMap()
	self:TestGlueSet()
	self:TestVoidPtr()
	-- self:TestSingleDelegateLua()
	self:TestCppDelegateLua()
	self:TestLuaFunctor()
	self:TestDefaultParam()
	self:TestCallDeadFunctor()
	self:TestFunctionInject()
	self:TestCallParentVirtualFunc()
	self:TestStructPointer()
	self:TestStaticProperty()
	self:TestPassRef()
	self:TestArrRef()
	self:TestCustomNew()
	self:TestDefaultAug()
	self:TestCustomFun()
	self:TestTwoPartsGlue()
	self:SomeBug()
	self:TestPassArrValueRefToFunction()
	self:TestFirstMemStructGlue()
	self:TestDelegateRetAndRef()
	self:TestSharedPtr()
	self:TestSharedRef()
	self:TestDefaultOperator()
	self:TestAlias()
	self:TestNoExprtFlag()
	self:TestBasesBase()
	self:TestNewStaticProperty()
	self:TestCallOverload()
	self:TestGlueCtor()
	self:TestGlueBpStruct()
	self:TestTemp()
	self:BenchMark()
	self:TestReenterUFunc()
	a_("pass")

end

function TestBpClass:CallOverload(functionmame)
	a_(functionmame .. " pass")
end



function TestBpClass:TestOverrideUFunction()

	self.__TestOverrideUFunction=1

	ATestCaseActor.OverrideUFunction("TestOverrideFunction",function(Obj)
		Obj.__TestOverrideUFunction = 2
	end)
	ATestCaseActor.OverrideUFunction("TestOverrideFunction1",function(Obj,InVec)
		Obj.__TestOverrideUFunction = 3
		InVec.X = 100
		InVec.Y = 100
		InVec.Z = 100
	end)

	ATestCaseActor.OverrideUFunction("TestOverrideFunction2",function(Obj,InVec)
		Obj.__TestOverrideUFunction = 4
		InVec.X = 401
		InVec.Y = 402
		InVec.Z = 403
	end)

	ATestCaseActor.OverrideUFunction("TestOverrideFunction3",function(Obj,InA,InB)
		--a_("void TestOverrideFunction3(FVector & InA, FVector & InB)")
		Obj.__TestOverrideUFunction = 5
		InA.X = 500
		InB.Y = -500
	end)

	ATestCaseActor.OverrideUFunction("TestOverrideFunction4",function(Obj,InA,InB)
		--a_("void TestOverrideFunction3(FVector & InA, FVector & InB)")
		Obj.__TestOverrideUFunction = 6
		InA.X = 600
		InB.Y = -600
	end)

	ATestCaseActor.OverrideUFunction("TestOverrideFunction5",function(Obj,InA,InB)
		--a_("void TestOverrideFunction3(FVector & InA, FVector & InB)")
		Obj.__TestOverrideUFunction = 7
		InA.X = 700
		InB.Y = -700
	end)

	ATestCaseActor.OverrideUFunction("TestOverrideFunction6",function(Obj,InA,InB)
		--a_("void TestOverrideFunction3(FVector & InA, FVector & InB)")
		Obj.__TestOverrideUFunction = 8
		InA.X = 800
		InB.Y = -800
	end)

	
	
	assert(self.__TestOverrideUFunction == 1)
	
	self:TestOverrideFunction()
	assert(self.__TestOverrideUFunction == 2,"TestOverrideFunction")

	local a = FVector.New(1,2,3)
	self:TestOverrideFunction1(a)
	assert(self.__TestOverrideUFunction == 3,"TestOverrideFunction")
	assert(a.X == 100)
	assert(a.Y == 100)
	assert(a.Z == 100)

	self:TestOverrideFunction2(a)
	assert(self.__TestOverrideUFunction == 4,"TestOverrideFunction")
	assert(a.X == 401)
	assert(a.Y == 402)
	assert(a.Z == 403)

	local b = FVector.New(3,2,1)

	self:TestOverrideFunction3(a,b)
	assert(self.__TestOverrideUFunction == 5,"TestOverrideFunction")
	assert(a.X == 500)
	assert(b.Y == -500)


	self:TestOverrideFunction4(a,b)
	assert(self.__TestOverrideUFunction == 6,"TestOverrideFunction")
	assert(a.X == 600)
	assert(b.Y == -600)

	self:TestOverrideFunction5(a,b)
	assert(self.__TestOverrideUFunction == 7,"TestOverrideFunction")
	assert(a.X == 700)
	assert(b.Y == -700)

	self:TestOverrideFunction6(a,b)
	assert(self.__TestOverrideUFunction == 8,"TestOverrideFunction")
	assert(a.X == 800)
	assert(b.Y == -800)
end

function TestBpClass:TestTemp()
	local v = FVector.Temp(1,2,3)
	v.X = v.X + 1
	v.Y = v.Y + 1
	v.Z = v.Z + 1
	assert(v.X == 2)
	assert(v.Y == 3)
	assert(v.Z == 4)
end

function TestBpClass:TestInterface( )
	assert(self:Interface_NativeEvent(200) == 1999)
end

function TestBpClass:TestBpStructType()
	local struct = self.test_bpstruct_type
	assert(_G["test_bp_struct_type"] ~= nil)
	assert(struct.inter == 199)
	assert(struct.vector.X == 2)
	assert(struct.vector.Y == 3)
	assert(struct.vector.Z == 4)
	assert(struct.enum == 1)
	assert(struct.arr:Num() == 3)
	assert(struct.arr[1] == true)
	assert(struct.arr[2] == false)
	assert(struct.arr[3] == true)
	local struct1 = struct:Copy()
	assert(struct1.inter == 199)
	assert(struct1.vector.X == 2)
	assert(struct1.vector.Y == 3)
	assert(struct1.vector.Z == 4)
	assert(struct1.enum == 1)
	assert(struct1.arr:Num() == 3)
	assert(struct1.arr[1] == true)
	assert(struct1.arr[2] == false)
	assert(struct1.arr[3] == true)

	local newstruct = test_bp_struct_type.New()
	self.test_bp_struct_type = newstruct
	local nowstruct = self.test_bp_struct_type
	assert(nowstruct.inter == newstruct.inter)
	assert(nowstruct.vector.X == newstruct.vector.X)
	assert(nowstruct.vector.Y == newstruct.vector.Y)
	assert(nowstruct.vector.Z == newstruct.vector.Z)
	assert(nowstruct.enum == newstruct.enum)
	assert(nowstruct.arr:Num() == newstruct.arr:Num())
end

function TestBpClass:TestSetFunc( )
	local function f(name)
		local s = self[name]	 
		local s1, s2 = self:FuncReturnSet_public(s)
		assert(s2 == s)
		for v in pairs(s1) do
			assert(v.X == 9)
			assert(v.Y == 8)
			assert(v.Z == 7)
		end
		assert(self[name][FVector.New(99,88,77)])

		local s1, s2 = self:FuncReturnSet_private(s)
		assert(s2 == s)
		for v in pairs(s1) do
			assert(v.X == 9)
			assert(v.Y == 8)
			assert(v.Z == 7)
		end
		assert(s2[FVector.New(99,88,77)])

		local s1, s2 = self:bp_func_test_set(s)
		assert(s2 == s)
		for v in pairs(s1) do
			assert(v.X == 9)
			assert(v.Y == 8)
			assert(v.Z == 7)
		end
		assert(s2[FVector.New(99,88,77)])
	end
	f("test_set_struct_public")
	f("test_set_struct_private")
	f("test_set_struct_bp")
end

function TestBpClass:TestSetProperty( )
	local function f(name)
		self[name][20] = true
		self[name][30] = true
		assert(self[name][20])
		assert(self[name][30])
		self[name]:Reset()
		assert(not self[name][20])
		assert(not self[name][30])
		local t = {}
		t[1] = true
		t[2] = true
		self[name] = t
		assert(self[name][1])
		assert(self[name][2])
	end
	f("test_set_public")
	f("test_set_private")
	f("test_set_bp")

	local function f1(name)
		self[name]:Reset()
		self[name][FVector.New(1,2,3)] = true
		assert(self[name][FVector.New(1,2,3)])
		local t = self[name]:Table()
		for v in pairs(t) do
			assert(v.X == 1)
			assert(v.Y == 2)
			assert(v.Z == 3)
			v.X = 2
			v.Y = 3
			v.Z = 4
		end
		assert(not self[name][FVector.New(1,2,3)] )
		assert(self[name][FVector.New(2,3,4)])
		t = {}
		t[FVector.New(4,5,6)] = true
		self[name] = t
		assert(not self[name][FVector.New(2,3,4)])
		assert(self[name][FVector.New(4,5,6)])
	end
	f1("test_set_struct_public")
	f1("test_set_struct_private")
	f1("test_set_struct_bp")

	local function f2(name)
		self[name]:Reset()
		self[name] = self.test_set_copy_public
		local t = self[name]:Table()
		for v in pairs(t) do
			assert(v.X == 7)
			assert(v.Y == 8)
			assert(v.Z == 9)
		end
		self[name]:Reset()
		self[name] = self.test_set_copy_private
		local t = self[name]:Table()
		for v in pairs(t) do
			assert(v.X == 7)
			assert(v.Y == 8)
			assert(v.Z == 9)
		end
		self[name]:Reset()
		self[name] = self.test_set_copy_bp
		local t = self[name]:Table()
		for v in pairs(t) do
			assert(v.X == 7)
			assert(v.Y == 8)
			assert(v.Z == 9)
		end
	end
	f2("test_set_struct_public")
	f2("test_set_struct_private")
	f2("test_set_struct_bp")

end

function TestBpClass:TestMapProperty( )
	local function f(name)
		local t = {test1 =2, test2 = 1}
		self[name]:Reset()
		assert(self[name]:Num() == 0)
		self[name].test1 = 1
		self[name].test2 = 2
		assert(self[name]:Num() == 2)
		assert(self[name].test1 == 1)
		assert(self[name].test2 == 2)
		self[name].test2 = 3
		assert(self[name].test2 == 3)
		self[name]:Reset()
		assert(self[name]:Num() == 0)
		self[name] = t
		assert(self[name].test1 == 2)
		assert(self[name].test2 == 1)
	end
	f("test_map_public")
	f("test_map_private")
	f("test_map_bp")

	local function f1(name)
		self[name].test1 = FVector.New(1,2,3)
		assert(self[name].test1.X == 1)
		assert(self[name].test1.Y == 2)
		assert(self[name].test1.Z == 3)
		local v = self[name].test1
		v.X = 2
		v.Y = 3
		v.Z = 4
		assert(self[name].test1.X == 2)
		assert(self[name].test1.Y == 3)
		assert(self[name].test1.Z == 4)
	end
	f1("test_map_struct_private")
	f1("test_map_struct_public")
	f1("test_map_struct_bp")

	local function f2(name)
		self[name]:Reset()
		self[name] = self.test_map_copy_public
		assert(self[name]:Num() == self.test_map_copy_public:Num())
		local v = self[name].haha
		assert(v.X == 1)
		assert(v.Y == 2)
		assert(v.Z == 3)
		local v = self[name].jiji
		assert(v.X == 2)
		assert(v.Y == 3)
		assert(v.Z == 4)


		self[name]:Reset()
		self[name] = self.test_map_copy_private
		assert(self[name]:Num() == self.test_map_copy_private:Num())
		local v = self[name].haha
		assert(v.X == 1)
		assert(v.Y == 2)
		assert(v.Z == 3)
		local v = self[name].jiji
		assert(v.X == 2)
		assert(v.Y == 3)
		assert(v.Z == 4)

		self[name]:Reset()
		self[name] = self.test_map_copy_bp
		assert(self[name]:Num() == self.test_map_copy_bp:Num())
		local v = self[name].haha
		assert(v.X == 1)
		assert(v.Y == 2)
		assert(v.Z == 3)
		local v = self[name].jiji
		assert(v.X == 2)
		assert(v.Y == 3)
		assert(v.Z == 4)

		local t = self[name]:Table()
		for k, v in pairs(t) do
			assert(v == self[name][k])
		end
	end
	f2("test_map_struct_private")
	f2("test_map_struct_public")
	f2("test_map_struct_bp")
end

function TestBpClass:TestMapRefFunc( )
	local test_map = {}
	self.test_map_public.xixi = 100
	self.test_map_private.xixi = 100
	self.test_map_bp.xixi = 100
	test_map.xixi = 100

	self:FuncReturn_ParamMap(self.test_map_public)
	assert(self.test_map_public:Num() == 1)
	assert(self.test_map_public.hehe == 100)
	self:FuncReturn_ParamMap(self.test_map_private)
	assert(self.test_map_private:Num() == 1)
	assert(self.test_map_private.hehe == 100)
	self:FuncReturn_ParamMap(self.test_map_bp)
	assert(self.test_map_bp:Num() == 1)
	assert(self.test_map_bp.hehe == 100)
	self:FuncReturn_ParamMap(test_map)
	assert(test_map.xixi == nil)
	assert(test_map.hehe == 100)

	self.test_map_public.xixi = 100
	self.test_map_private.xixi = 100
	self.test_map_bp.xixi = 100
	test_map.xixi = 100
	self:FuncReturn_ParamMap_private(self.test_map_public)
	assert(self.test_map_public:Num() == 1)
	assert(self.test_map_public.hehe == 100)
	self:FuncReturn_ParamMap_private(self.test_map_private)
	assert(self.test_map_private:Num() == 1)
	assert(self.test_map_private.hehe == 100)
	self:FuncReturn_ParamMap_private(self.test_map_bp)
	assert(self.test_map_bp:Num() == 1)
	assert(self.test_map_bp.hehe == 100)
	self:FuncReturn_ParamMap_private(test_map)
	assert(test_map.xixi == nil)
	assert(test_map.hehe == 100)
	
	self.test_map_public.xixi = 100
	self.test_map_private.xixi = 100
	self.test_map_bp.xixi = 100
	test_map.xixi = 100
	self:bp_test_map_ref(self.test_map_public)
	assert(self.test_map_public:Num() == 1)
	assert(self.test_map_public.hehe == 100)
	self:bp_test_map_ref(self.test_map_private)
	assert(self.test_map_private:Num() == 1)
	assert(self.test_map_private.hehe == 100)
	self:bp_test_map_ref(self.test_map_bp)
	assert(self.test_map_bp:Num() == 1)
	assert(self.test_map_bp.hehe == 100)
	self:bp_test_map_ref(test_map)
	assert(test_map.xixi == nil)
	assert(test_map.hehe == 100)

	local t1 = {}
	self:FuncReturn_ParamMap_struct(t1)
	assert(t1.test1.X == 1)
	assert(t1.test1.Y == 2)
	assert(t1.test1.Z == 3)
	assert(t1.test2.X == 4)
	assert(t1.test2.Y == 5)
	assert(t1.test2.Z == 6)
	local t2 = {}
	self:FuncReturn_ParamMap_struct_private(t2)
	assert(t2.test1.X == 1)
	assert(t2.test1.Y == 2)
	assert(t2.test1.Z == 3)
	assert(t2.test2.X == 4)
	assert(t2.test2.Y == 5)
	assert(t2.test2.Z == 6)
	local t2 = {}
	self:FuncReturn_ParamMap_struct_bp(t2)
	assert(t2.test1.X == 1)
	assert(t2.test1.Y == 2)
	assert(t2.test1.Z == 3)
	assert(t2.test2.X == 4)
	assert(t2.test2.Y == 5)
	assert(t2.test2.Z == 6)
end

function TestBpClass:TestMapRetFunc( )
	local t = self:FuncReturnMap()
	assert(t.lala1.X == 100)
	assert(t.lala1.Y == 200)
	assert(t.lala1.Z == 300)

	assert(t.lala2.X == 222)
	assert(t.lala2.Y == 333)
	assert(t.lala2.Z == 444)

	t = self:FuncReturnMap_private()
	assert(t.lala1.X == 100)
	assert(t.lala1.Y == 200)
	assert(t.lala1.Z == 300)

	assert(t.lala2.X == 222)
	assert(t.lala2.Y == 333)
	assert(t.lala2.Z == 444)

	t = self:bp_func_return_map()
	assert(t.test1.X == 22)
	assert(t.test1.Y == 33)
	assert(t.test1.Z == 44)

	assert(t.test2.X == 12)
	assert(t.test2.Y == 23)
	assert(t.test2.Z == 34)
end

function TestBpClass:TestArrParam()
	local t = {}
	self:FuncReturn_ParamArr(t)
	assert(#t == 2)
	assert(t[1] == 100)
	assert(t[2] == 200)
	self:FuncReturn_ParamArr(self.Test_ParamArr)
	assert(self.Test_ParamArr:Num() == 2)
	assert(self.Test_ParamArr[1] == 100)
	assert(self.Test_ParamArr[2] == 200)

	self:FuncReturn_ParamArr_private(t)
	assert(#t == 3)
	assert(t[1] == 300)
	assert(t[2] == 400)
	assert(t[3] == 500)

	self:FuncReturn_ParamArr_private(self.Test_ParamArr)
	assert(self.Test_ParamArr:Num() == 3)
	assert(self.Test_ParamArr[1] == 300)
	assert(self.Test_ParamArr[2] == 400)
	assert(self.Test_ParamArr[3] == 500)
end

function TestBpClass:TestNativePublicPropertyReadWrite( )
	assert(self.test_bool_public == true)
	self.test_bool_public = false
	assert(self.test_bool_public == false)

	assert(self.test_int_public == 3423)
	self.test_int_public = 23123
	assert(self.test_int_public == 23123)

	assert(self.test_int64_public == 82374)
	self.test_int64_public = 898123
	assert(self.test_int64_public == 898123)

	self.test_uint64_public = 96712738121222
	assert(self.test_uint64_public == 96712738121222)

	self.test_uint64_private = 43021928139121
	assert(self.test_uint64_private == 43021928139121)

	assert(self.test_byte_public == 22)
	self.test_byte_public = 33
	assert(self.test_byte_public == 33)

	assert(math.abs(self.test_float_public - 34.23) <0.01)
	self.test_float_public = 8987.87
	assert(math.abs(self.test_float_public - 8987.87) <0.01)

	assert(math.abs(self.test_double_public - 283814.3) <0.01)
	self.test_double_public = 75612384.28
	assert(math.abs(self.test_double_public - 75612384.28) <0.01)	

	assert(self.test_string_public == "nativestring")
	self.test_string_public = "nativestringoiuqweiu"
	assert(self.test_string_public == "nativestringoiuqweiu")

	assert(tostring(self.test_text_public) == "nativetext")
	self.test_text_public = "nativetextaksjdlaks"
	assert(tostring(self.test_text_public) == "nativetextaksjdlaks")

	assert(self.test_name_public == "nativename")
	self.test_name_public = "nativenameasdoiquweio"
	assert(self.test_name_public == "nativenameasdoiquweio")

	assert(self.test_vector_public.X == 12)
	assert(self.test_vector_public.Y == 34)
	assert(self.test_vector_public.Z == 56)
	local v = self.test_vector_public:Copy()
	self.test_vector_public.X = 99
	self.test_vector_public.Y = 987
	self.test_vector_public.Z = 192899
	assert(self.test_vector_public.X == 99)
	assert(self.test_vector_public.Y == 987)
	assert(self.test_vector_public.Z == 192899)

	self.test_vector_public = v
	assert(self.test_vector_public.X == 12)
	assert(self.test_vector_public.Y == 34)
	assert(self.test_vector_public.Z == 56)

	assert(self.test_object_public == self)
	self.test_object_public = nil
	assert(self.test_object_public == nil)

	assert(self.test_class_public == ATestCaseActor:Class())
	self.test_class_public = AActor:Class()
	assert(self.test_class_public == AActor:Class())

	assert(self.test_weakptr_public == self)
	self.test_weakptr_public = nil
	assert(self.test_weakptr_public == nil)

	assert(self.test_arr_public:Num() == 3)
	assert(self.test_arr_public[1] == 2)
	assert(self.test_arr_public[2] == 3)
	assert(self.test_arr_public[3] == 7)

	self.test_arr_public[1] = 3
	assert(self.test_arr_public[1] == 3)

	self.test_arr_public[4] = 10
	assert(self.test_arr_public:Num()==4)
	assert(self.test_arr_public[4]==10)

	self.test_arr_public:Add(20)
	assert(self.test_arr_public:Num()==5)
	assert(self.test_arr_public[5]==20)

	local v = self.test_arr_public:Pop()
	assert(v == 20)
	assert(self.test_arr_public:Num() == 4)

	local v1 = self.test_arr_public[1]
	v = self.test_arr_public:Remove(1)
	assert(v == v1)
	assert(self.test_arr_public:Num() == 3)

	v1 = 10
	self.test_arr_public:Insert(1, v1)
	assert(self.test_arr_public:Num() == 4)
	assert(self.test_arr_public[1] == 10)
end

function TestBpClass:TestNativePrivatePropertyReadWrite( )
	assert(self.test_bool_private == true)
	self.test_bool_private = false
	assert(self.test_bool_private == false)

	assert(self.test_int_private == 3423)
	self.test_int_private = 23123
	assert(self.test_int_private == 23123)

	assert(self.test_int64_private == 82374)
	self.test_int64_private = 898123
	assert(self.test_int64_private == 898123)

	assert(self.test_byte_private == 22)
	self.test_byte_private = 33
	assert(self.test_byte_private == 33)

	assert(math.abs(self.test_float_private - 34.23) <0.01)
	self.test_float_private = 8987.87
	assert(math.abs(self.test_float_private - 8987.87) <0.01)

	assert(math.abs(self.test_double_private - 283814.3) <0.01)
	self.test_double_private = 75612384.28
	assert(math.abs(self.test_double_private - 75612384.28) <0.01)	

	assert(self.test_string_private == "nativestring")
	self.test_string_private = "nativestringoiuqweiu"
	assert(self.test_string_private == "nativestringoiuqweiu")

	assert(tostring(self.test_text_private) == "nativetext")
	self.test_text_private = "nativetextaksjdlaks"
	assert(tostring(self.test_text_private) == "nativetextaksjdlaks")

	assert(self.test_name_private == "nativename")
	self.test_name_private = "nativenameasdoiquweio"
	assert(self.test_name_private == "nativenameasdoiquweio")

	assert(self.test_vector_private.X == 12)
	assert(self.test_vector_private.Y == 34)
	assert(self.test_vector_private.Z == 56)
	local v = self.test_vector_private:Copy()
	self.test_vector_private.X = 99
	self.test_vector_private.Y = 987
	self.test_vector_private.Z = 192899
	assert(self.test_vector_private.X == 99)
	assert(self.test_vector_private.Y == 987)
	assert(self.test_vector_private.Z == 192899)

	self.test_vector_private = v
	assert(self.test_vector_private.X == 12)
	assert(self.test_vector_private.Y == 34)
	assert(self.test_vector_private.Z == 56)

	assert(self.test_object_private == self)
	self.test_object_private = nil
	assert(self.test_object_private == nil)

	assert(self.test_class_private == ATestCaseActor:Class())
	self.test_class_private = AActor:Class()
	assert(self.test_class_private == AActor:Class())

	assert(self.test_weakptr_private == self)
	self.test_weakptr_private = nil
	assert(self.test_weakptr_private == nil)

	assert(self.test_arr_private:Num() == 3)
	assert(self.test_arr_private[1] == 2)
	assert(self.test_arr_private[2] == 3)
	assert(self.test_arr_private[3] == 7)

	self.test_arr_private[1] = 3
	assert(self.test_arr_private[1] == 3)

	self.test_arr_private[4] = 10
	assert(self.test_arr_private:Num()==4)
	assert(self.test_arr_private[4]==10)

	self.test_arr_private:Add(20)
	assert(self.test_arr_private:Num()==5)
	assert(self.test_arr_private[5]==20)

	local v = self.test_arr_private:Pop()
	assert(v == 20)
	assert(self.test_arr_private:Num() == 4)

	local v1 = self.test_arr_private[1]
	v = self.test_arr_private:Remove(1)
	assert(v == v1)
	assert(self.test_arr_private:Num() == 3)

	v1 = 10
	self.test_arr_private:Insert(1, v1)
	assert(self.test_arr_private:Num() == 4)
	assert(self.test_arr_private[1] == 10)
end

function TestBpClass:TestCppFunction( )
	local test_arr = {2,8,8}
	local test_set = {}
	test_set[9] = true
	test_set[8] = true
	test_set[1] = true

	local test_map ={}
	test_map.test1 = 99
	test_map.test2 = 999
	test_map.test3 = 9999

	assert(self:TestNativeFunction(true, 1234, 567895678, 31, 81723.87, 981723879412.89, "str", "txt", "name", 
		FVector.New(3,2,1), self, AActor:Class(), test_arr, test_set, test_map) == 812738174)

	local test_v = FVector.New(3,2,1)
	local returnvalue, test_bool, test_int, test_int64, test_byte, test_float ,test_double,test_string,test_text,test_name,test_struct,test_obj,test_class = self:TestNativeFunction_Ref(true, 1234, 567895678, 31, 81723.87, 981723879412.89, "str", "txt", "name", 
		test_v, self, AActor:Class())
	assert(returnvalue == 712738174)
	assert(test_bool == false)
	assert(test_int == 4321)
	assert(test_int64 == 8765431)
	assert(test_byte == 32)
	assert( math.abs(test_float - 85555.87) < 0.01 )
	assert( math.abs(test_double - 8981723879412.89) < 0.01 )
	assert(test_string == "strstr")
	assert(tostring(test_text) == "txttxt")
	assert(test_name == "namename")
	assert(test_struct == test_v)
	assert(test_struct.X == 44)
	assert(test_struct.Y == 55)
	assert(test_struct.Z == 66)
	assert(test_obj == nil)
	assert(test_class == ATestCaseActor:Class())

	local test_v = FVector.New(3,2,1)
	local returnvalue, test_bool, test_int, test_int64, test_byte, test_float ,test_double,test_string,test_text,test_name,test_struct,test_obj,test_class = self:TestNativeFunction_Ref_private(true, 1234, 567895678, 31, 81723.87, 981723879412.89, "str", "txt", "name", 
		test_v, self, AActor:Class())
	assert(returnvalue == 712738174)
	assert(test_bool == false)
	assert(test_int == 4321)
	assert(test_int64 == 8765431)
	assert(test_byte == 32)
	assert( math.abs(test_float - 85555.87) < 0.01 )
	assert( math.abs(test_double - 8981723879412.89) < 0.01 )
	assert(test_string == "strstr")
	assert(tostring(test_text) == "txttxt")
	assert(test_name == "namename")
	assert(test_struct == test_v)
	assert(test_struct.X == 44)
	assert(test_struct.Y == 55)
	assert(test_struct.Z == 66)
	assert(test_obj == nil)
	assert(test_class == ATestCaseActor:Class())
end

function TestBpClass:TestBpPropertyReadWrite()
	assert(self.test_property_bool == true)	
	self.test_property_bool = false
	assert(self.test_property_bool == false)	

	assert(self.test_property_int == 1233)
	self.test_property_int = 3321
	assert(self.test_property_int == 3321)

	assert(self.test_property_byte == 144)
	self.test_property_byte = 244
	assert(self.test_property_byte == 244)

	assert( math.abs(self.test_property_float - 124.33) < 0.01 )
	self.test_property_float = 7788.88
	assert( math.abs(self.test_property_float - 7788.88) < 0.01 )

	assert(self.test_property_fname == "namename")
	self.test_property_fname = "namename222"
	assert(self.test_property_fname == "namename222")

	assert(self.test_property_fstring == "fstring")
	self.test_property_fstring = "fstringfstring"
	assert(self.test_property_fstring == "fstringfstring")

	assert(tostring(self.test_property_text) == "ftext")
	self.test_property_text = "ftextftext"
	assert(tostring(self.test_property_text) == "ftextftext")

	assert(self.test_property_vector.X == 11)
	assert(self.test_property_vector.Y == 22)
	assert(self.test_property_vector.Z == 33)
	self.test_property_vector = FVector.New(33,44,11)
	assert(self.test_property_vector.X == 33)
	assert(self.test_property_vector.Y == 44)
	assert(self.test_property_vector.Z == 11)

	local lala = self.test_property_uobject
	local lala1 = UCurveBase.LoadObject(nil, "CurveFloat'/Game/Git/testcase/testobj.testobj'")
	assert(self.test_property_uobject == UCurveBase.LoadObject(nil, "CurveFloat'/Game/Git/testcase/testobj.testobj'"))
	self.test_property_uobject = UCurveBase.LoadObject(nil, "CurveFloat'/Game/Git/testcase/testobj_2.testobj_2'")
	assert(self.test_property_uobject == UCurveBase.LoadObject(nil, "CurveFloat'/Game/Git/testcase/testobj_2.testobj_2'"))

	assert(self.test_property_aactor_class == AActor:Class())
end

function TestBpClass:TestBpPropertyArr()
	assert(self.test_arr_int:Num()==3)
	assert(self.test_arr_int[1] == 2)
	assert(self.test_arr_int[2] == 4)
	assert(self.test_arr_int[3] == 6)

	self.test_arr_int[1] = 3
	assert(self.test_arr_int[1] == 3)

	self.test_arr_int[4] = 10
	assert(self.test_arr_int:Num()==4)
	assert(self.test_arr_int[4]==10)

	self.test_arr_int:Add(20)
	assert(self.test_arr_int:Num()==5)
	assert(self.test_arr_int[5]==20)

	local v = self.test_arr_int:Pop()
	assert(v == 20)
	assert(self.test_arr_int:Num() == 4)

	local v1 = self.test_arr_int[1]
	v = self.test_arr_int:Remove(1)
	assert(v == v1)
	assert(self.test_arr_int:Num() == 3)

	v1 = 10
	self.test_arr_int:Insert(1, v1)
	assert(self.test_arr_int:Num() == 4)
	assert(self.test_arr_int[1] == 10)

	local num = self.test_arr_int_copy:Num()
	self.test_arr_int = self.test_arr_int_copy	
	assert(self.test_arr_int:Num() == num)
	for i = 1, num do
		assert(self.test_arr_int[i] == self.test_arr_int_copy[i])
	end

	for i, v in ipairs(self.test_arr_int:Table()) do
		assert(v == self.test_arr_int[i])
	end

	self.test_arr_int:Empty()
	assert(self.test_arr_int:Num() == 0)

	local t = {}
	local t1 = {}
	local t2
	local t3
	self.test_arr_int, t2, t3 = self:Get_test_arr_int_copy(t, t1)
	assert(t2 == t)
	assert(t3 == t1)
	assert(#t==2)
	assert(t[1] ==9999)
	assert(t[2] ==7777)
	assert(#t1==11)
	assert(t1[11]==888)
	local bb = self.test_arr_int
	assert(self.test_arr_int:Num() == num)
	for i = 1, num do
		assert(self.test_arr_int[i] == self.test_arr_int_copy[i])
	end
end

function TestBpClass:TestBpPropertyArr1()
	self:Get_test_arr_int_copy(self.test_arr_pass_to_bpfunc_1, self.test_arr_pass_to_bpfunc_2)
	assert(self.test_arr_pass_to_bpfunc_1:Num()==2)
	assert(self.test_arr_pass_to_bpfunc_1[1] ==9999)
	assert(self.test_arr_pass_to_bpfunc_1[2] ==7777)
	assert(self.test_arr_pass_to_bpfunc_2:Num()==11)
	assert(self.test_arr_pass_to_bpfunc_2[11]==888)

	self:FuncReturn_ParamArr(self.test_arr_pass_to_bpfunc_1)
	assert(self.test_arr_pass_to_bpfunc_1:Num() == 2)
	assert(self.test_arr_pass_to_bpfunc_1[1] == 100)
	assert(self.test_arr_pass_to_bpfunc_1[2] == 200)
	

	self:FuncReturn_ParamArr_private(self.test_arr_pass_to_bpfunc_1)
	assert(self.test_arr_pass_to_bpfunc_1:Num() == 3)
	assert(self.test_arr_pass_to_bpfunc_1[1] == 300)
	assert(self.test_arr_pass_to_bpfunc_1[2] == 400)
	assert(self.test_arr_pass_to_bpfunc_1[3] == 500)

	self:Get_test_arr_int_copy(self.Test_ParamArr, self.Test_ParamArr_private)
	assert(self.Test_ParamArr:Num()==2)
	assert(self.Test_ParamArr[1] ==9999)
	assert(self.Test_ParamArr[2] ==7777)
	assert(self.Test_ParamArr_private:Num()==11)
	assert(self.Test_ParamArr_private[11]==888)

	local num = self.test_arr_int_copy:Num()
	self.Test_ParamArr = self:Get_test_arr_int_copy({}, {})
	assert(self.Test_ParamArr:Num() == num)
	for i = 1, num do
		assert(self.Test_ParamArr[i] == self.test_arr_int_copy[i])
	end

	self.Test_ParamArr_private = self:Get_test_arr_int_copy({}, {})
	assert(self.Test_ParamArr_private:Num() == num)
	for i = 1, num do
		assert(self.Test_ParamArr_private[i] == self.test_arr_int_copy[i])
	end
end

function TestBpClass:TestReturnStruct()
	local v = self:FuncReturnStruct()
	assert(v.X == 11)
	assert(v.Y == 22)
	assert(v.Z == 12)

	v = self:FuncReturnStruct_private()
	assert(v.X == 77)
	assert(v.Y == 88)
	assert(v.Z == 99)
end

function TestBpClass:TestArrInnerStruct( )
	self.test_arr_struct_public:Empty()
	self.test_arr_struct_public:Set(1, FVector.New(1,2,3))
	local v = self.test_arr_struct_public:Get(1)
	assert(v.X == 1)
	assert(v.Y == 2)
	assert(v.Z == 3)
	v.X = 2
	v.Y = 3
	v.Z = 4
	v = self.test_arr_struct_public:Get(1)
	assert(v.X == 2)
	assert(v.Y == 3)
	assert(v.Z == 4)

	self.test_arr_struct_private:Empty()
	self.test_arr_struct_private:Set(1, FVector.New(1,2,3))
	local v = self.test_arr_struct_private:Get(1)
	assert(v.X == 1)
	assert(v.Y == 2)
	assert(v.Z == 3)
	v.X = 2
	v.Y = 3
	v.Z = 4
	v = self.test_arr_struct_private:Get(1)
	assert(v.X == 2)
	assert(v.Y == 3)
	assert(v.Z == 4)

	self.test_arr_bp_struct:Empty()
	self.test_arr_bp_struct:Set(1, FVector.New(1,2,3))
	local v = self.test_arr_bp_struct:Get(1)
	assert(v.X == 1)
	assert(v.Y == 2)
	assert(v.Z == 3)
	v.X = 2
	v.Y = 3
	v.Z = 4
	v = self.test_arr_bp_struct:Get(1)
	assert(v.X == 2)
	assert(v.Y == 3)
	assert(v.Z == 4)

	self.test_arr_struct_public:Add(FVector.New(7,8,9))
	local v = self.test_arr_struct_public:Remove(self.test_arr_struct_public:Num())
	assert(v.X == 7)
	assert(v.Y == 8)
	assert(v.Z == 9)
end

function TestBpClass:TestDelegate( )
	self.m_Delegate1 = self.Delegate1
	local hasfire = false
	local function f(test_bool,test_int,test_int64,test_byte,test_float,test_double,test_string,test_text,test_name)
		assert(test_bool == true)
		assert(test_int == 1234)
		assert(test_int64 == 2234)
		assert(test_byte == 12)
		assert(math.abs(test_float-12.12)<0.01)
		assert(math.abs(test_double-128371123.12)<0.01)
		assert(test_string == "str")
		assert(tostring(test_text) == "text")
		assert(test_name == "wtestname")
		hasfire = true
	end
	self.m_Delegate1:Add(f)
	self.m_Delegate1:Fire(true, 1234, 2234, 12, 12.12, 128371123.12, "str", "text", "wtestname")
	assert(hasfire)
	hasfire = false
	self.Delegate1_private:Add(f)
	self.Delegate1_private:Fire(true, 1234, 2234, 12, 12.12, 128371123.12, "str", "text", "wtestname")
	assert(hasfire)
	-- self.Delegate3:Add(f)
	-- hasfire = false
	-- self.Delegate3:Fire(true, 1234, 2234, 12, 12.12, 128371123.12, "str", "text", "wtestname")
	-- assert(hasfire)

	local function f1(seflObj, test_struct,test_obj,test_class,test_arr,test_arrstruct, test_set)
		assert(test_struct.X == 1)
		assert(test_struct.Y == 2)
		assert(test_struct.Z == 3)
		assert(test_obj == seflObj)
		assert(test_class == ATestCaseActor:Class())
		assert(test_arr[1]==1)
		assert(test_arr[2]==2)
		assert(test_arr[3]==3)
		assert(test_arrstruct[1].X==2)
		assert(test_arrstruct[1].Y==2)
		assert(test_arrstruct[1].Z==2)
		assert(test_arrstruct[2].X==3)
		assert(test_arrstruct[2].Y==3)
		assert(test_arrstruct[2].Z==3)

		assert(test_set[10] == true)
		assert(test_set[11] == true)
		hasfire = true
	end
	local set = {}
	set[10] = true
	set[11] = true
	self.m_Delegate2 = self.Delegate2
	self.m_Delegate2_private = self.Delegate2_private
	self.m_Delegate2:Add(MakeCallBack(f1, self))
	hasfire = false
	self.m_Delegate2:Fire(FVector.New(1,2,3), self, ATestCaseActor:Class(), {1,2,3}, {FVector.New(2,2,2), FVector.New(3,3,3)}, set)
	assert(hasfire)
	self.m_Delegate2_private:Add(MakeCallBack(f1, self))
	hasfire = false
	self.m_Delegate2_private:Fire(FVector.New(1,2,3), self, ATestCaseActor:Class(), {1,2,3}, {FVector.New(2,2,2), FVector.New(3,3,3)}, set)
	assert(hasfire)
end

function TestBpClass:TestCallLua( test_bool_public, test_int_public, test_int64_public, test_byte_public, test_float_public, test_double_public, test_string_public,test_text_public, test_name_public, test_vector_public, test_object_public,test_class_public, test_weakptr_public, test_arr_public, test_arr_struct_public, test_set_public, test_map_public)
	assert(test_bool_public == true)
	assert(test_int_public == 3423)
	assert(test_int64_public == 82374)
	assert(test_byte_public == 22)
	assert(math.abs(test_float_public - 34.23) <0.01)
	assert(math.abs(test_double_public - 283814.3) <0.01)
	assert(test_string_public == "nativestring")
	assert(tostring(test_text_public) == "nativetext")
	assert(test_name_public == "nativename")
	assert(test_vector_public.X == 12)
	assert(test_vector_public.Y == 34)
	assert(test_vector_public.Z == 56)
	assert(test_object_public == self)
	assert(test_class_public == ATestCaseActor:Class())
	assert(test_weakptr_public == self)
	assert(#test_arr_public == 3)
	assert(test_arr_public[1] == 2)
	assert(test_arr_public[2] == 3)
	assert(test_arr_public[3] == 7)
	assert(test_set_public[9] == true)
	assert(test_set_public[99] == true)
	assert(test_set_public[199] == true)
	assert(test_map_public.test1 == 100)
	assert(test_map_public.test2 == 200)
end

function TestBpClass:TestEnum()
	assert(self.test_enum_bp == 1)
	self.test_map_enum_public.test1 = 1
	self.test_map_enum_public.test2 = 0
	assert(self.test_map_enum_public.test1 == 1)
	assert(self.test_map_enum_public.test2 == 0)

	self.test_map_enum_public = {test1=0,test2=1}	
	assert(self.test_map_enum_public.test1 == 0)
	assert(self.test_map_enum_public.test2 == 1)

	assert(self.test_bpenum_type == 2)
end

function TestBpGlobalCall(param1,param2,param3,param4,param5,param6,param7)
    return param1,param2,param3,param4,param5,param6,param7
end	

function TestBpClass:TestBpCall(param1,param2,param3,param4,param5,param6,param7)
    return param1,param2,param3,param4,param5,param6,param7
end

function TestBpClass:RunCo2()
	local DelayTime = 2
	local PassTime = CoroutineUtil.Delay(DelayTime)
	assert(PassTime >= DelayTime)
end	

function TestBpClass:RunCo1()
	self:Coroutine(self.RunCo2, self)
	local DelayTime = 1
	local PassTime = CoroutineUtil.Delay(DelayTime, 1, 2, 3)
	assert(PassTime >= DelayTime)
end	

function TestBpClass:TestCoroutinue()
	local handle, IsYield, a, b, c = self:Coroutine(self.RunCo1, self)
	assert(handle:GetOwner() == self)
	assert(handle:GetStatus() == "suspended")
	assert(IsYield)
	assert(a ==1)
	assert(b ==2)
	assert(c ==3)
end

function TestBpClass:TestFistMemOfStruct()
	self:Test_FirstMem_Struct()
end

function TestBpClass:TestFistMemOfStruct_Internal(t, t1, t2)
	-- after ue4 build, Capital become chaos
	local Key = "value"
	assert(t[Key] == t[Key])
	t[Key].X=1
	t[Key].Y=2
	t[Key].Z=3
	assert(t[Key].X==1)
	assert(t[Key].Y==2)
	assert(t[Key].Z==3)

	local a = t1[Key]
	local b = t1[Key]
	assert(t1[Key] == t1[Key])
	assert(t1[Key][Key] == t1[Key][Key])
	assert(t1[Key][Key].X ~= nil)

	assert(t2[Key].X ~= nil)
	assert(t2[Key].Y ~= nil)
	assert(t2[Key].Z ~= nil)
	assert(t2[Key] == t2[Key])
	-- Q_(getmetatable(self.bp_testfirstmemstruct["value"]))
	assert(self.bp_testfirstmemstruct["value"].X == 1)
	assert(self.bp_testfirstmemstruct["value"].Y == 2)
	assert(self.bp_testfirstmemstruct["value"].Z == 3)
	assert(self.bp_testfirstmemstruct["value"] == self.bp_testfirstmemstruct["value"])
end

function TestBpClass:TestPushStructStack( )
	self:Test_PushStack_Struct()
end

function TestBpClass:PushStackStruct(v1, v2)
	assert(v1 == v2)
	assert(v1.X == 1)
	assert(v1.Y == 2)
	assert(v1.Z == 3)
	assert(v2.X == 1)
	assert(v2.Y == 2)
	assert(v2.Z == 3)
end

function TestBpClass:TestStaticThing( )
	assert(ATestCaseActor.TestStaticFunction_public(2831978))
	assert(ATestCaseActor.TestStaticFunction_private(192831982))
end

function TestBpClass:TestCallTestRefStruct()
	local before, before1, before2, before3 = FHitResult.New(), FHitResult.Temp(), FHitResult.New(), FHitResult.Temp()
	local out, out1, out2 = self:TestRefStruct(before, before1, before2, before3 )
	assert(out == before)
	assert(out1 == before1)
	assert(out.Actor == self)
	assert(out1.Actor == self)
end

function TestBpClass:TryCallTheSingleDelegate(Delegate)
	Delegate:Fire(true, 2147483647,876.65,"asd",FVector.New(1.1,2.2,3.3),self)
	assert(self.SingleDelegateTestPass == true)
end

function TestBpClass:TestUserDefineStruct( )
	local ins = SomeStruct.New()
	-- test property read write
	assert(ins.Field1 == 8197238)
	assert(FloatEqual(ins.Field2, 885.4))
	assert(ins.Field3.X == 99)
	assert(ins.Field3.Y == 88)
	assert(ins.Field3.Z == 77)
	assert(ins.Field4 == nil)
	assert(ins.Field5 == 12838148213898)
	assert(ins.Field6 == 22838148213898)
	assert(ins.Field7 == "aksdjiasdo")
	assert(tostring(ins.Field8) == ins.Field7)
	assert(#ins.Field10 == 2)
	assert(ins.Field10[1] == 2)
	assert(ins.Field10[2] == 4)

	assert(#ins.Field11 == 2)
	assert(ins.Field11[1].X == 1)
	assert(ins.Field11[1].Y == 2)
	assert(ins.Field11[1].Z == 3)
	assert(ins.Field11[2].X == 2)
	assert(ins.Field11[2].Y == 4)
	assert(ins.Field11[2].Z == 6)

	assert(ins.Field12.uu == true)
	assert(ins.Field12.bb == true)

	assert(ins.Field13.keykey1 == 2389)
	assert(ins.Field13.keykey2 == 9887)

	local arr = ins.Field14.k
	-- assert(type(arr) == "userdata")
	assert(#arr == 2)
	assert(arr[1] == 2)
	assert(arr[2] == 4)
	arr[1] = 10
	assert(arr[1] == 10)

	ins.Field1 = 888878
	ins.Field2 = 123142.4
	ins.Field3 = FVector.New(87,98,23)
	ins.Field4 = self
	ins.Field5 = 12831624871
	ins.Field6 = 9074351198273
	ins.Field7 = "testestset"
	ins.Field8 = ins.Field7
	ins.Field9 = ins.Field7

	ins.Field10 = {1,2,3,4}
	ins.Field11 = {FVector.New(4,5,6),FVector.New(7,8,9)}
	ins.Field12 = {als=true,sodfi=true,alskjd=true}
	ins.Field13 = {aksjdkl=1231411,oqiwue=829374}

	assert(ins.Field1 == 888878)
	assert(FloatEqual(ins.Field2, 123142.4))
	assert(ins.Field3.X == 87)
	assert(ins.Field3.Y == 98)
	assert(ins.Field3.Z == 23)
	assert(ins.Field4 == self)
	assert(ins.Field5 == 12831624871)
	assert(ins.Field6 == 9074351198273)
	assert(ins.Field7 == "testestset")
	assert(tostring(ins.Field8) == ins.Field7)

	assert(#ins.Field10 == 4)
	assert(ins.Field10[1] == 1)
	assert(ins.Field10[4] == 4)

	assert(#ins.Field11 == 2)
	assert(ins.Field11[1].X == 4)
	assert(ins.Field11[1].Y == 5)
	assert(ins.Field11[1].Z == 6)
	assert(ins.Field11[2].X == 7)
	assert(ins.Field11[2].Y == 8)
	assert(ins.Field11[2].Z == 9)

	assert(ins.Field12.als == true)
	assert(ins.Field12.sodfi == true)
	assert(ins.Field12.alskjd == true)

	assert(ins.Field13.aksjdkl == 1231411)
	assert(ins.Field13.oqiwue == 829374)

	ins.BasicField1 = 834672
	assert(ins.BasicField1 == 834672)

	-- test func
	local r1, r2 = SomeStruct.StaticFunc(999)
	assert(r1 == 999 and r2 == 1000)
	-- SomeStruct.StaticFunc1()
	assert(ins:GetField1() == 888878)

	local param1 = 4772
	local arr = {FVector.New(823,743,8839)}
	local set = {quwoie = true, qilk = true}
	local m = {}
	m[81238] = 888
	m[99887] = 888
	local r3, r4, r5, r6 =ins:InsFunc(4772, arr, set, m)
	assert(r4 == arr and r5 == set and r6 == m)
	assert(r3 == 100)
	assert(r4[2].X == 1)
	assert(r4[2].Y == 2)
	assert(r4[2].Z == 3)
	assert(r5.bbbb == true)
	assert(r6[81238] == nil)
	ins:BasicFunc1()
	assert(ins:VirtualFunc() == 33)
	-- can call base class's virtual function 
	assert(SomeBasic.VirtualFunc(ins) == 22)
	function TestBpClass:LuaTestPushSomeStruct(var, var_arr)
		local i=0 
	end
	self:TestPushSomeStruct()

	local v = ins:TestReturnRef()
	local v1 = ins:TestReturnRef()
	assert(v == v1)
	assert(v == ins.Field3)
	ins.Field16 = 0
	assert(ins.Field16 == 0)
	ins.Field16 = 1
	assert(ins.Field16 == 1)
end

function TestBpClass:LuaTestContainerReturnFromLua()
	self.m_testreturncount = self.m_testreturncount or 0
	self.m_testreturncount = self.m_testreturncount + 1
	if self.m_testreturncount == 1 then
		return {1,3}
	elseif self.m_testreturncount == 2 then
		self.Test_ParamArr_private:Empty()
		self.Test_ParamArr_private:Add(1)
		self.Test_ParamArr_private:Add(3)
		return self.Test_ParamArr_private
	elseif self.m_testreturncount == 3 then
		local arr = TArray_int.New()
		arr:Add(1)
		arr:Add(3)
		return arr
	end
end

function TestBpClass:TestContainerReturn()
	self:TestContainerReturnFromLua()
	self:TestContainerReturnFromLua()
	self:TestContainerReturnFromLua()
end

function TestBpClass:TestGlueArr( )
	local arr = TArray_int.New()
	-- pass arr to ref
	self:FuncReturn_ParamArr(arr)
	assert(#arr == 2)
	assert(arr[1] == 100)
	assert(arr[2] == 200)

	arr = {99,88,77}
	assert(#arr==3)
	assert(arr[1] == 99)
	assert(arr[3] == 77)

	arr = self:TestFuncReturnArr()
	assert(#arr == 4)
	assert(arr[1] == 634)
	assert(arr[4] == 12398)

	local arr_arr = TArray_TArray_int.New()
	arr_arr:Add(arr)
	arr_arr:Add(arr)
	assert(#arr_arr == 2)
	assert(#arr_arr[1] == 4)
	assert(arr_arr[1][1] == 634)
	assert(arr_arr[1][4] == 12398)

	local arr_vector = TArray_FVector.New()
	arr_vector:Add(FVector.New(1,2,3))
	arr_vector:Add(FVector.New(4,5,6))
	assert(arr_vector[1].X == 1)
	assert(arr_vector[1].Y == 2)
	assert(arr_vector[1].Z == 3)
	assert(arr_vector[2].X == 4)
	assert(arr_vector[2].Y == 5)
	assert(arr_vector[2].Z == 6)

	local arr_somestruct = TArray_SomeStruct.New()
	arr_somestruct:Add(SomeStruct.New())

	local arr_param = TArray_int.New()
	local firstreturn, secondreturn = SomeStruct.TestReturnArrForGlueStruct(arr_param)
	assert(type(firstreturn) == "userdata")
	assert(#firstreturn == 4)
	assert(firstreturn[1] == 5)
	assert(firstreturn[4] == 8)
	assert(arr_param == secondreturn)
	assert(#arr_param == 4)
	assert(arr_param[1] == 1)
	assert(arr_param[4] == 4)
end

function TestBpClass:TestGlueMap( )
	local map1 = TMap_int_int.New()

	map1:Add(100, 200)	
	map1:Remove(100)

	local map2 = TMap_FString_int.New()
	map2:Add("jiu", 8932)
	map2:Add("ding", 81273)
	self.UsedForTestGlueTMap = map2
	assert(self.UsedForTestGlueTMap["jiu"] == 8932)
	assert(self.UsedForTestGlueTMap["ding"] == 81273)
	
	map2:Remove("jiu")
	map2:Remove("ding")

	self.UsedForTestGlueTMap = map2

	assert(self.UsedForTestGlueTMap["jiu"] == nil)
	assert(self.UsedForTestGlueTMap["ding"] == nil)

	self.UsedForTestGlueTMap["gogo"] = 999

	map2 = TMap_FString_int.New(self.UsedForTestGlueTMap)
	assert(map2["gogo"] == 999)
	local map3 = TMap_FString_int.New({key1 = 11, key2 = 100})
	local map4 = TMap_FString_int.New({key3 = 22, key4 = 99})
	local map5, map6 = self:FuncForTestGlueTMap(map3, map4)
	-- assert(type(map5) == "userdata")

-- a_()
	assert(map4 == map6)
	assert(map4["ohoh"] == 88)
	assert(map4["key1"] == nil)
	assert(map4["jiujiu"] == 66)

	assert(map5["lala"] == 2)
	assert(map5["didi"] == 99)

	local structins = SomeStruct.New()
	structins.Field13 = {buibui = 99, qweqwe =2883}
	assert(structins.Field13["buibui"] == 99)
	assert(structins.Field13["qweqwe"] == 2883)

	structins.Field13 = map4
	assert(structins.Field13["ohoh"] == 88)
	assert(structins.Field13["key1"] == nil)
	assert(structins.Field13["jiujiu"] == 66)
	structins.Field13 = self.UsedForTestGlueTMap
	assert(structins.Field13["gogo"] == 999)

	TMap_FString_int.New({key1 = 11, key2 = 100})
	local map7 = TMap_FString_int.New({key3 = 22, key4 = 99})
	local map8, map9 = SomeStruct.FuncForTestGlueTMap(map3, map7)

	assert(map7 == map9)
	assert(map7["ohoh"] == 88)
	assert(map7["key1"] == nil)
	assert(map7["jiujiu"] == 66)

	assert(map8["lala"] == 2)
	assert(map8["didi"] == 99)
end

function TestBpClass:TestGlueSet( )
	local set1 = TSet_FString.New()

	set1:Add("key1")	
	set1:Remove("key1")

	local set2 = TSet_FString.New()
	set2:Add("jiu")
	set2:Add("ding")
	self.UsedForTestGlueTSet = set2
	assert(self.UsedForTestGlueTSet["jiu"] == true)
	assert(self.UsedForTestGlueTSet["ding"] == true)
	
	set2:Remove("jiu")
	set2:Remove("ding")

	self.UsedForTestGlueTSet = set2

	assert(self.UsedForTestGlueTSet["jiu"] == nil)
	assert(self.UsedForTestGlueTSet["ding"] == nil)

	self.UsedForTestGlueTSet["gogo"] = true

	set2 = TSet_FString.New(self.UsedForTestGlueTSet)
	assert(set2["gogo"] == true)
	local set3 = TSet_FString.New({key1 = true, key2 = true})
	local set4 = TSet_FString.New({key3 = true, key4 = true})
	local set5, set6 = self:FuncForTestGlueTSet(set3, set4)

-- a_()
	assert(set4 == set6)
	assert(set4["ohoh"] == true)
	assert(set4["key1"] == nil)
	assert(set4["jiujiu"] == true)

	assert(set5["lala"] == true)
	assert(set5["didi"] == true)

	local structins = SomeStruct.New()
	structins.Field12 = {buibui = true, qweqwe =true}
	assert(structins.Field12["buibui"] ==true)
	assert(structins.Field12["qweqwe"] ==true)

	structins.Field12 = set4
	assert(structins.Field12["ohoh"] ==true)
	assert(structins.Field12["key1"] == nil)
	assert(structins.Field12["jiujiu"] == true)
	structins.Field12 = self.UsedForTestGlueTSet
	assert(structins.Field12["gogo"] == true)

	TSet_FString.New({key1 = true, key2 = true})
	local set7 = TSet_FString.New({key3 = true, key4 = true})
	local set8, set9 = SomeStruct.FuncForTestGlueTSet(set3, set7)

	assert(set7 == set9)
	assert(set7["ohoh"] == true)
	assert(set7["key1"] == nil)
	assert(set7["jiujiu"] == true)

	assert(set8["lala"] == true)
	assert(set8["didi"] == true)
end

function TestBpClass:TestVoidPtr()
	function TestBpClass:LuaFuncForTestPushVoidPtr( int_i, float_i, double_i, bool_i, FString_i, int64_i, vec_i, arr_i, this)
		assert(int_i:To_int32() == 9887)
		assert(FloatEqual(float_i:To_float(), 81723.4))
		assert(FloatEqual(double_i:To_double(), 812739.8))
		assert(bool_i:To_bool() == true)
		assert(FString_i:To_FString() == "lalala")
		-- a_("lalala", int64_i:To_int64())
		assert(int64_i:To_int64() == 2371984298172)
		-- local vec = vec_i:To_FVector()
		-- assert(vec.X == 1)
		-- assert(vec.Z == 3)

		local vec1 = vec_i:To_Struct("FVector")
		assert(vec1.X == 1)
		assert(vec1.Z == 3)

		local thisptr = this:To_UObject()
		assert(thisptr == self)

		local arr = arr_i:To_Struct("TArray_int")
	end
	self:FuncForTestPushVoidPtr()
end

function TestBpClass:TestSingleDelegateLua()
	local function f(i, v)
		assert(i == 100)
		assert(v.X == 1)
		assert(v.Y == 2)
		assert(v.Z == 3)
	end
	self:TestSingleDelegate(f)
end

function TestBpClass:TestCppDelegateLua()
	local function f(i, v)
		assert(i == 100)
		assert(v.X == 1)
		assert(v.Y == 2)
		assert(v.Z == 3)
	end
	self.CppDelegate:Bind(f)
	self:TestCppDelegate()
	local function f1(i, v)
		assert(i == 100)
		assert(v.X == 2)
		assert(v.Y == 3)
		assert(v.Z == 4)
	end
	local delegate = FDelegateTest4.New(f1)
	self:TestPassCppDelegate(delegate)
	local function f2(i, v)
		assert(i == 222)
		assert(v.X == 88)
		assert(v.Y == 99)
		assert(v.Z == 77)
		return 10000
	end
	self.HasRetValueDelegate:Bind(f2)
	delegate = FDelegateTest5.New(f2)
	self:TestPassRetCppDelegate(delegate)
end

function TestBpClass:TestLuaFunctor()
	local function f(p1,p2,p3,p4,p5)
		assert(p1 == 1)
		assert(p2 == 2)
		assert(p3 == 3)
		assert(p4 == self)
		assert(p5.X == 1)
		assert(p5.Y == 2)
		assert(p5.Z == 3)
	end
	-- self:TestFunctor1(f)
	local function f1()
		return 332
	end
	self:TestFunctor2(f, f1)
end

function TestBpClass:TestDefaultParam()
	assert(self:TestReturnValue(200) == 200)
	-- assert(self:TestReturnValue() == 100)
end

function TestBpClass:TestCallDeadFunctor()
	local function f()
	end
	-- self.CallDeadFunctor(f)
end

function TestBpClass:TestNativeEvent( )
	return 2000
end

function TestBpClass:testluainjectbpfunction()
	return 2003
end

function TestBpClass:TestFunctionInject( )
	-- assert(self:TestNativeEvent()==2000)
end

function TestBpClass:TestCallParentVirtualFunc( )
	-- local d = UTestCallSuperVirtual_Derived.NewObject()
	-- local b = UTestCallSuperVirtual_Base.NewObject()
	-- a_("lalala", d:TheTestVirtualFunc(), b:TheTestVirtualFunc(), UTestCallSuperVirtual_Base.TheTestVirtualFunc(d), d:Test())
end

function TestBpClass:TestStructPointer( )
	local ins = SomeStruct.New()
	local ins1 = UTestCallSuperVirtual_Derived.testPopStructPointer(ins)
	ins.Field1 = 9999999
	ins1.Field1 = 888888.88
	-- todo: actually they should be the same 
	assert(not rawequal(ins, ins1))
	assert(ins.Field1 == ins1.Field1)
end

function TestBpClass:TestStaticProperty()
	-- assert(ATestCaseActor.StaticDelegateTest:IsBound() == false)
	ATestCaseActor.StaticDelegateTest:Bind(function () end)
	assert(ATestCaseActor.StaticDelegateTest:IsBound() == true)
	ATestCaseActor.StaticDelegateTest:Execute(100, FVector.New(1,2,3))
	ATestCaseActor.StaticInt = 100
	assert(ATestCaseActor.staticseestatic() == 100)
	assert(ATestCaseActor.StaticInt == 100)
end

function TestBpClass:TestPassRef()
	local ins = SomeStruct.New()
	local v = FVector.New(1,2,3)
	local bpvar = FTestFirstMemStruct_Nest.New()
	local somebasic = SomeBasic.New()
	somebasic.BasicField1 = 100
	local p1, p2, p3 = ins:TestPassRef(v, somebasic, bpvar)
	local isEqual = rawequal(p1, v) and rawequal(p2, somebasic) and rawequal(p3, bpvar)
	assert(isEqual)
	assert(v.X == 22)
	assert(v.Y == 33)
	assert(v.Z == 44)
	assert(somebasic.BasicField1 == 2000)
	assert(bpvar.TestPassRefNum == 3999)
	ins:TestPassRef2(p1, p2, p3)
	ins:TestPassRef3(p1, p2, p3)
	ins:TestPassRef4("TestText")
	-- ins:TestPassRef1(v, somebasic, FLinearColor.New(), bpvar)
	-- a_(p1, p2, p3, p4)
end

function TestBpClass:TestArrRef()
	self.UseForTestArrRef:Add(3)
	self.UseForTestArrRef:Add(2)
	self.UseForTestArrRef:Add(1)
	local t1 = {3,2,1}
	local t2 = self.UseForTestArrRef
	local t3 = TArray_int.New()
	local Augs = {t1,t2,t3}
	local funcs = {1,3,4}
	t3:Add(3)
	t3:Add(2)
	t3:Add(1)
	local ins = SomeStruct.New()
	-- test read
	for i, v in ipairs(funcs) do
		for _, aug in ipairs(Augs) do
			local result = ins["TestArrRef"..v](ins, aug)
		end
	end
	-- test pop
	local tt1 = {}
	local tt2 = self.UseForTestArrRef
	local tt3 = TArray_int.New()

	local Augs2 = {tt1,tt2,tt3}
	for i, v in ipairs(Augs2) do
		ins:TestArrRef2(t1, v)	
		assert(#v == 3)
		assert(v[1] == 9)
		assert(v[2] == 8)
		assert(v[3] == 7)
	end
	local HasCome = false
	for i,v in pairs(tt2) do
		assert(v == 9 and i == 1)
		HasCome = true
		break
	end
	assert(HasCome)
	HasCome = false
	for i,v in pairs(tt3) do
		assert(v == 9 and i == 1)
		HasCome = true
		break
	end
	assert(HasCome)
end

function TestBpClass:TestCustomNew( )
	local ins = TestnewStruct.new1(1234)
	assert(getmetatable(ins).__gc ~= nil)
	assert(ins.IntData == 1234)
	assert(FloatEqual(ins.FloatData, 1000))
	local ins = TestnewStruct.new1(4321, 2.0)
	assert(ins.IntData == 4321)
	assert(FloatEqual(ins.FloatData, 2.0))
	ins = TestnewStruct.new2(12.34)
	assert(FloatEqual(ins.FloatData, 12.34))
	ins = TestnewStruct.new3(FVector.New(22,33,44))
	assert(ins.StructData.X == 22)
	assert(ins.StructData.Y == 33)
	assert(ins.StructData.Z == 44)
	ins = TestnewStruct.new4(1234,12.34,FVector.New(22,33,44))
	assert(ins.IntData == 1234)
	assert(FloatEqual(ins.FloatData, 12.34))
	assert(ins.StructData.X == 22)
	assert(ins.StructData.Y == 33)
	assert(ins.StructData.Z == 44)

	ins = TestnewStruct.Temp1(1234)
	assert(getmetatable(ins).__gc == nil)
	assert(ins.IntData == 1234)
	assert(FloatEqual(ins.FloatData, 1000))
	local ins = TestnewStruct.Temp1(4321, 2.0)
	assert(ins.IntData == 4321)
	assert(FloatEqual(ins.FloatData, 2.0))
	ins = TestnewStruct.Temp2(12.34)
	assert(FloatEqual(ins.FloatData, 12.34))
	ins = TestnewStruct.Temp3(FVector.New(22,33,44))
	assert(ins.StructData.X == 22)
	assert(ins.StructData.Y == 33)
	assert(ins.StructData.Z == 44)
	ins = TestnewStruct.Temp4(1234,12.34,FVector.New(22,33,44))
	assert(ins.IntData == 1234)
	assert(FloatEqual(ins.FloatData, 12.34))
	assert(ins.StructData.X == 22)
	assert(ins.StructData.Y == 33)
	assert(ins.StructData.Z == 44)
end

function TestBpClass:TestDefaultAug( )
	local ins = TestDefaultAug.New()
	ins:test(1)
	ins:test_name(1)
	ins:test1_1()
	ins:test1_2()
	ins:test3()
	ins:test4()
	ins:test5()
	ins.TestStaticClassFunc()
	assert(ins.TestStaticClassFunc_Ret() == 3)
	ins.TestStaticDefault()
	ins:test_virtual()
end

function TestBpClass:TestCustomFun( )
	local ins  = TestNoCustom.New()
	ins.Data = 5
	ins:Test1(99)
	TestNoCustom.Test2(99)
	TestNoCustom.Test3(99)
end

function TestBpClass:TestTwoPartsGlue()
	-- local ins = StructTwoParts.New()
	-- local ins1 = StructTwoParts1
end

function TestBpClass:SomeBug( )
	-- ipairs Tarray
	for i, v in ipairs(self.test_arr_public) do
	end
	local j = TArray_int.New()
	j:Add(100)
	for i, v in ipairs(j) do
	end
	-- end

	local ins = TestBeginNameBug1.Temp()
	ins.i = 100
	local j = ins.i
	local ins1 = ins:Copy()
	ins1.i = 100
	local j1 = ins1.i

	for k,v in pairs(self.test_map_public) do
	end

	for k,v in pairs(self.test_set_public) do
	end
end

function TestBpClass:TestPassArrValueRefToFunction( )
	local t = {}
	local t1 = SomeStruct2.TestArrPassByRefAndValue(t)
	assert(t == t1)
	assert(t[1].i == 3)
	assert(t[1].j == 4)
	assert(t[2].i == 5)
	assert(t[2].j == 6)

	local t = SomeStruct3.NoGlueArr({})
	local tt = SomeStruct3.GlueArr()
	SomeStruct3.NoGlueArrRefParam({})
	SomeStruct3.NoGlueArrTestPassFromUProperty({FSomeStruct4.New()})
	self.test_pass_to_glue:Add(FSomeStruct4.New())
	SomeStruct3.NoGlueArrTestPassFromUProperty(self.test_pass_to_glue)
	assert(self.test_pass_to_glue:Num() == 1)
	SomeStruct3.NoGlueArrTestPassFromUProperty1(self.test_pass_to_glue)
	assert(self.test_pass_to_glue:Num() == 0)
	self.test_pass_to_glue_map:Add(2, FSomeStruct4.New())
	SomeStruct3.NoGlueArrTestPassFromUProperty2(self.test_pass_to_glue_map)
	assert(self.test_pass_to_glue_map:Num() == 0)

	local set1 = {}
	local TheStruct = FSomeStruct4.New()
	TheStruct.i = 100
	set1[TheStruct] = true
	TheStruct = FSomeStruct4.New()
	TheStruct.i = 200
	set1[TheStruct] = true
	local t2 = SomeStruct3.NoGlueArrTestPassFromUProperty3(set1)
	local t3 = SomeStruct3.NoGlueArrTestPassFromUProperty3(self.test_pass_to_glue_set)
	local t4 = SomeStruct3.NoGlueArrTestPassFromUProperty4(set1)
	local t5 = SomeStruct3.NoGlueArrTestPassFromUProperty4(self.test_pass_to_glue_set)
	assert(type(t5) == "userdata")
end

function TestBpClass:TestFirstMemStructGlue()
	local ParentStruct = SomeStruct6.New()
	local ChillStruct = ParentStruct.InnerStruct
	local GrandChildStruct = ChillStruct.InnerStruct
	assert(ChillStruct.classname == "SomeStruct5")
	assert(GrandChildStruct.classname == "SomeStruct7")
	assert(ParentStruct == ChillStruct)
	assert( ChillStruct == GrandChildStruct)
end

function TestBpClass:TestDelegateRetAndRef()
	local Ins = SomeStruct8.New()
	local HasCall = false
	local function f(i, j)
		assert(i == 100)
		assert(j.X == 1)
		assert(j.Y == 2)
		assert(j.Z == 3)
		HasCall = true
	end
	Ins.Delegate:Bind(f)
	local DelegateRet = Ins:Test_DelegateRet()
	DelegateRet:ExecuteIfBound(100, FVector.New(1,2,3))
	assert(HasCall)
	Ins:Test_DelegateRef(Ins.Delegate)
	assert(Ins.Delegate:IsBound() == false)
	Ins:Test_DelegateRef(nil)
end

function TestBpClass:TestSharedPtr()
	local Ins = SomeStruct8.New()
	local Struct2 = SomeStruct2.New()
	Struct2.i = 100
	local sharedStruct2 = TSharedPtr_SomeStruct2.New(Struct2)
	assert(sharedStruct2:GetSharedReferenceCount() == 1)
	local sharedstruct2_1 = TSharedPtr_SomeStruct2.New()
	sharedstruct2_1:Set(sharedStruct2)
	assert(sharedStruct2:GetSharedReferenceCount() == 2)
	local sharedstruct2_2 = TSharedPtr_SomeStruct2.New(sharedstruct2_1)
	assert(sharedStruct2:GetSharedReferenceCount() == 3)
	local SharedRet = Ins:GetSomeStruct2Shared1(sharedStruct2)
	local StructRet = SharedRet:Get()
	assert(StructRet.i == 100)
	SharedRet:Set(nil)
	local i = SharedRet:Get()
	assert(i == nil)
	SharedRet:Set(Struct2)
	i = SharedRet:Get()
	Struct2.i = 101
	assert(i.i == 100)
	Ins:GetSomeStruct2Shared2(SharedRet)
	Struct2 = SharedRet:Get()
	assert(Struct2.i == 200)
end

function TestBpClass:TestSharedRef()
	local Ins = SomeStruct8.New()
	local Struct2 = SomeStruct2.New()
	Struct2.i = 100
	local sharedStruct2 = TSharedRef_SomeStruct2.New(Struct2)
	assert(sharedStruct2:GetSharedReferenceCount() == 1)
	local sharedStruct2_1 = TSharedRef_SomeStruct2.New(sharedStruct2)
	assert(sharedStruct2:GetSharedReferenceCount() == 2)
	local SharedRet = Ins:GetSomeStruct2Shared3(sharedStruct2)
	local StructRet = SharedRet:Get()
	assert(StructRet.i == 100)
	local Struct3 = SomeStruct2.New()
	Struct3.i = 200
	SharedRet:Set(Struct3)
	local i = SharedRet:Get()
	assert(i.i == 200)
	Struct3.i = 101
	assert(i.i == 200)

	local j = SomeStruct2.New()
	j.i = 100
	local i = TSharedRef_SomeStruct2.New(j)
	assert(i.i==100)
	i.i = 200
	assert(i.i==200)

	local j = SomeStruct2.New()
	j.i = 100
	local i = TSharedPtr_SomeStruct2.New(j)
	assert(i.i==100)
	i.i = 200
	assert(i.i==200)
end

function TestBpClass:TestDefaultOperator()
	local a = StructTestOperator.New()
	local b = StructTestOperator.New()
	local c = StructTestOperator.New()
	c.i = 11
	assert(a+b == 20)
	assert(a-b == 0)
	assert(a==b)
	assert(a~=c)
	assert(a() == 10)
end

function TestBpClass:TestAlias( )
	local ins = TestDelegateRename.New()
	local HasCall = false
	local function f()
		HasCall = true
	end
	local NewDelgate = FDelegateRename2.New()
	NewDelgate:Bind(f)
	ins.Delegate1 = NewDelgate
	ins.Delegate1:ExecuteIfBound()
	assert(HasCall)
	local ins1 = TestTmpFunc.New()
	ins1:Test()
end

function TestBpClass:TestNoExprtFlag( )
	assert(TestNoCtorSharedType.New == nil)
	assert(TestNoCtorSharedType.Temp == nil)
end

function TestBpClass:TestBasesBase( )
	-- todo
	-- local t = base3
	-- local t = base4
end

function TestBpClass:TestNewStaticProperty()
	local t = TestStaticProperty
	t.i = 100
	assert(t.staticsee() == 100)
	assert(t.i == 100)
	local tt = TestStaticProperty.New()
	assert(tt.i == 100)
	assert(tt:see() == 100)
	assert(tt.staticsee() == 100)
end

function TestBpClass:BenchMark( )
	local BenchMarkObject = require "gameplay.test.benchmarkclass":NewOn(UBenchmarkActor.NewObject())
	BenchMarkObject:Start()
end

function TestBpClass:TestCallOverload( )
	local a = TestOverloadFunction.New()
	local v = FVector.New()
---[[
	assert(a:t() == 1)
	assert(a:t(1) == 1)
	assert(a:t(1.1) == 2)
	assert(a:t(v) == 3)
	assert(a:t(self) == 4)
	assert(a:t(v, 1) == 5)
	assert(a:t(v, 1.1) == 6)
	assert(a:t(v, v) == 7)
	assert(a:t(v, self) == 8)
	assert(a:t(TArray_FVector.New()) == 9)
	assert(a:t(self.test_arr_struct_public) == 9)
	assert(a:t(TArray_int.New()) == 10)
	assert(a:t(self.test_arr_public) == 10)
	assert(a:t(TArray_float.New()) == 11)
	assert(a:t("FText"), 12)
	assert(a:t(FText.New()), 12)
	assert(a:t(TMap_int_FString.New()) == 13)
	assert(a:t(TSet_int.New()) == 14)
	local s_s1 = TSharedPtr_SomeStruct.New()
	local s_s2 = TSharedPtr_SomeBasic.New()
	assert(a:t(s_s2) == 15)
	assert(a:t(s_s1) == 15)

	local s_sr1 = TSharedRef_SomeStruct.New(SomeStruct.New())
	local s_sr2 = TSharedRef_SomeBasic.New(SomeStruct.New())
	assert(a:t(s_sr2) == 16)
	assert(a:t(s_sr1) == 16)
	local ss = SomeBasic.New()
	local ss1 = SomeStruct.New()
	assert(a:t(ss) == 17)
	assert(a:t(ss1) == 17)
--]]
-- out function
	assert(a.O_t() == 1)
	assert(a.O_t(1) == 1)
	assert(a.O_t(1.1) == 2)
	assert(a.O_t(FVector.New()) == 3)
	assert(a.O_t(self) == 4)

	assert(a.Data == 0)
	local a_ins = TestOverloadFunction.New(1)
	assert(a_ins.Data == 1)
	a_ins = TestOverloadFunction.New(1.1)
	assert(a_ins.Data == 2)
	a_ins = TestOverloadFunction.New(v)
	assert(a_ins.Data == 3)
	a_ins = TestOverloadFunction.New(self)
	assert(a_ins.Data == 4)

	a_ins = TestOverloadFunction.Temp()
	assert(a_ins.Data == 0)
	a_ins = TestOverloadFunction.Temp(1)
	assert(a_ins.Data == 1)
	a_ins = TestOverloadFunction.Temp(1.1)
	assert(a_ins.Data == 2)
	a_ins = TestOverloadFunction.Temp(v)
	assert(a_ins.Data == 3)
	a_ins = TestOverloadFunction.Temp(self)
	assert(a_ins.Data == 4)

	a = self
	---[[
	assert(a:t() == 1)
	assert(a:t(1) == 1)
	assert(a:t(1.1) == 2)
	assert(a:t(v) == 3)
	assert(a:t(self) == 4)
	assert(a:t(v, 1) == 5)
	assert(a:t(v, 1.1) == 6)
	assert(a:t(v, v) == 7)
	assert(a:t(v, self) == 8)
	assert(a:t(TArray_FVector.New()) == 9)
	assert(a:t(self.test_arr_struct_public) == 9)
	assert(a:t(TArray_int.New()) == 10)
	assert(a:t(self.test_arr_public) == 10)
	assert(a:t(TArray_float.New()) == 11)
	assert(a:t("FText") == 12)
	assert(a:t(FText.New()) == 12)
	assert(a:t(TMap_int_FString.New()) == 13)
	assert(a:t(TSet_int.New()) == 14)
	local s_s1 = TSharedPtr_SomeStruct.New()
	local s_s2 = TSharedPtr_SomeBasic.New()
	assert(a:t(s_s2) == 15)
	assert(a:t(s_s1) == 15)

	local s_sr1 = TSharedRef_SomeStruct.New(SomeStruct.New())
	local s_sr2 = TSharedRef_SomeBasic.New(SomeStruct.New())
	assert(a:t(s_sr2) == 16)
	assert(a:t(s_sr1) == 16)
	local ss = SomeBasic.New()
	local ss1 = SomeStruct.New()
	assert(a:t(ss) == 17)
	assert(a:t(ss1) == 17)
	--]]
end

function TestBpClass:TestGlueCtor()
	local b = FVector.New(1,2,3)
	local c = FVector.Temp(1,2,3)
	assert(b.x == 1)
	assert(b.y == 2)
	assert(b.z == 3)

	assert(c.x == 1)
	assert(c.y == 2)
	assert(c.z == 3)
end

function TestBpClass:testluainjectbpfunction2(In1, In2, In3, In4)
	assert(In1 == FVector.Temp(1,20,3))
	assert(In2 == "dididi")
	assert(In3 == self)
	assert(In4 == 2999)
	return In1, In2, In3, In4
end

function TestBpClass:Error(ErrString)
	assert(false)
	a_(ErrString)
end

function TestBpClass:TestGlueBpStruct()
	local function f(TheClass)
		local BpStructTypeIns = TheClass.New() 
		BpStructTypeIns.Glue_ReadWriteInt = 100
		assert(BpStructTypeIns.Glue_ReadWriteInt == 100)
		BpStructTypeIns.Glue_ReadWriteVector = FVector.New(1,2,3)
		assert(BpStructTypeIns.Glue_ReadWriteVector.X == 1)
		assert(BpStructTypeIns.Glue_ReadWriteVector.Y == 2)
		assert(BpStructTypeIns.Glue_ReadWriteVector.Z == 3)

		TheClass.Glue_StaticReadWriteInt = 100
		assert(TheClass.Glue_StaticReadWriteInt == 100)
		TheClass.Glue_StaticReadWriteVector = FVector.New(1,2,3)
		assert(TheClass.Glue_StaticReadWriteVector.X == 1)
		assert(TheClass.Glue_StaticReadWriteVector.Y == 2)
		assert(TheClass.Glue_StaticReadWriteVector.Z == 3)
	end
	f(FTestGlueExBp)
	f(TestGlueEx)
end

function TestBpClass:TestReenterUFunc()
	local ResultToTest = FVector.Temp()
	self:TestReEnterUFunction(true, ResultToTest)
	assert(ResultToTest.X == 1)
	assert(ResultToTest.Y == 2)
	assert(ResultToTest.Z == 3)
end

function TestBpClass:ReenterAgain( )
	local ResultToTest = FVector.Temp()
	self:TestReEnterUFunction(false, ResultToTest)
	assert(ResultToTest.X == 2)
	assert(ResultToTest.Y == 3)
	assert(ResultToTest.Z == 4)
end

return TestBpClass