local luaprojectile = Inherit(ADemo1Character)
function luaprojectile:CtorCpp()
	local com = USphereComponent.CreateDefaultSubobject(self, "SphereComp")
	com.SphereRadius = 5
	

end

function luaprojectile:Ctor()
	
end

function luaprojectile:BeginPlayLua()
	
end

return luaprojectile