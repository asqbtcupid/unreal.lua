function testtemplate_call(f, int_1, s, p, float, b, t)
	assert(getmetatable(f).classname == "FVector")
	assert(int_1 == 1)
	assert(s == "test")
	assert(p == nil)
	assert(float == 2.0)
	assert(b == true)
	assert(type(t) == "table")
end
function testtemplate_r_int()
	return 1
end
function testtemplate_r_float()
	return 1.0
end
function testtemplate_r_double()
	return 1.0
end
function testtemplate_r_AActor()
	return nil
end
function testtemplate_r_FVector()
	return FVector.New(1,2,3)
end
function testtemplate_r_TArray()
	return {1,2,3}
end