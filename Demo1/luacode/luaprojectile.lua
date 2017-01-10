local luaprojectile = Inherit(ADemo1Character)
function luaprojectile:CtorCpp()
	local ProjectileMovement = UProjectileMovementComponent.CreateDefaultSubobject(self, "ProjectileComp")
	self.ProjectileMovement = ProjectileMovement
	ProjectileMovement.UpdatedComponent = self.CollisionComp
 	ProjectileMovement.InitialSpeed = 3000
 	ProjectileMovement.MaxSpeed = 3000
 	ProjectileMovement.bRotationFollowsVelocity = true
 	ProjectileMovement.bShouldBounce = true
 	
 	self.InitialLifeSpan = 3
end

function luaprojectile:ReceiveHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit)
	if OtherActor and self._cppinstance_ ~= OtherActor and OtherComp and OtherComp:IsSimulatingPhysics() then
		OtherComp:AddImpulseAtLocation(UKismetMathLibrary.Multiply_VectorFloat(self:GetVelocity(), 100), self:K2_GetActorLocation())

		self:DestroyCpp()
	end
end

return luaprojectile