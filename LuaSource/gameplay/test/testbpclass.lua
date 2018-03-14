local TestBpClass = Inherit(CppObjectBase)

function TestBpClass:EndPlay(Reason)
	CppObjectBase.EndPlay(self)
end

function TestBpClass:BeginPlay( )
	CppObjectBase.BeginPlay(self)
	if _WITH_EDITOR then
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
	end
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

	assert(self.test_text_public == "nativetext")
	self.test_text_public = "nativetextaksjdlaks"
	assert(self.test_text_public == "nativetextaksjdlaks")

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

	assert(self.test_text_private == "nativetext")
	self.test_text_private = "nativetextaksjdlaks"
	assert(self.test_text_private == "nativetextaksjdlaks")

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
	assert(test_text == "txttxt")
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
	assert(test_text == "txttxt")
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

	assert(self.test_property_text == "ftext")
	self.test_property_text = "ftextftext"
	assert(self.test_property_text == "ftextftext")

	assert(self.test_property_vector.X == 11)
	assert(self.test_property_vector.Y == 22)
	assert(self.test_property_vector.Z == 33)
	self.test_property_vector = FVector.New(33,44,11)
	assert(self.test_property_vector.X == 33)
	assert(self.test_property_vector.Y == 44)
	assert(self.test_property_vector.Z == 11)

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
end

function TestBpClass:TestDelegate( )
	self.m_Delegate1 = self.Delegate1
	local function f(test_bool,test_int,test_int64,test_byte,test_float,test_double,test_string,test_text,test_name)
		assert(test_bool == true)
		assert(test_int == 1234)
		assert(test_int64 == 2234)
		assert(test_byte == 12)
		assert(math.abs(test_float-12.12)<0.01)
		assert(math.abs(test_double-128371123.12)<0.01)
		assert(test_string == "str")
		assert(test_text == "text")
		assert(test_name == "wtestname")
	end
	self.m_Delegate1:Add(f)
	self.m_Delegate1:Fire(true, 1234, 2234, 12, 12.12, 128371123.12, "str", "text", "wtestname")
	self.Delegate1_private:Add(f)
	self.Delegate1_private:Fire(true, 1234, 2234, 12, 12.12, 128371123.12, "str", "text", "wtestname")

	local function f1(test_struct,test_obj,test_class,test_arr,test_arrstruct, test_set)
		assert(test_struct.X == 1)
		assert(test_struct.Y == 2)
		assert(test_struct.Z == 3)
		assert(test_obj == self)
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
	end
	local set = {}
	set[10] = true
	set[11] = true
	self.Delegate2:Add(f1)
	self.Delegate2:Fire(FVector.New(1,2,3), self, ATestCaseActor:Class(), {1,2,3}, {FVector.New(2,2,2), FVector.New(3,3,3)}, set)
	self.Delegate2_private:Add(f1)
	self.Delegate2_private:Fire(FVector.New(1,2,3), self, ATestCaseActor:Class(), {1,2,3}, {FVector.New(2,2,2), FVector.New(3,3,3)}, set)
end

function TestBpClass:TestCallLua( test_bool_public, test_int_public, test_int64_public, test_byte_public, test_float_public, test_double_public, test_string_public,test_text_public, test_name_public, test_vector_public, test_object_public,test_class_public, test_weakptr_public, test_arr_public, test_arr_struct_public, test_set_public, test_map_public)
	assert(test_bool_public == true)
	assert(test_int_public == 3423)
	assert(test_int64_public == 82374)
	assert(test_byte_public == 22)
	assert(math.abs(test_float_public - 34.23) <0.01)
	assert(math.abs(test_double_public - 283814.3) <0.01)
	assert(test_string_public == "nativestring")
	assert(test_text_public == "nativetext")
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
	assert(t.value == t.value)
	t.value.X=1
	t.value.Y=2
	t.value.Z=3
	assert(t.value.X==1)
	assert(t.value.Y==2)
	assert(t.value.Z==3)

	assert(t1.value == t1.value)
	assert(t1.value.value == t1.value.value)

	assert(t2.value.X ~= nil)
	assert(t2.value.Y ~= nil)
	assert(t2.value.Z ~= nil)
	assert(t2.value == t2.value)

	assert(self.bp_testfirstmemstruct.value.X == 1)
	assert(self.bp_testfirstmemstruct.value.Y == 2)
	assert(self.bp_testfirstmemstruct.value.Z == 3)
	assert(self.bp_testfirstmemstruct.value == self.bp_testfirstmemstruct.value)
end

function TestBpClass:TestPushStructStack( )
	self:Test_PushStack_Struct()
end

function TestBpClass:PushStackStruct(v1, v2)
	assert(v1 ~= v2)
	assert(v1.X == 1)
	assert(v1.Y == 2)
	assert(v1.Z == 3)
	assert(v2.X == 1)
	assert(v2.Y == 2)
	assert(v2.Z == 3)
end


return TestBpClass