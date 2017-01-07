local Character_lua = Inherit(ADemo1Character)
function Character_lua:Ctor()
	GlobalEvent.On("Input_Forward", self.MoveForward, self)
	GlobalEvent.On("Input_Right", self.MoveRight, self)
	GlobalEvent.On("PressFire", self.Fire, self)
	-- GlobalEvent.On("PressFire", self.test, self)
end

function Character_lua:MoveForward(v)
	if v ~= 0 then
		local forwardvector = self:GetActorForwardVector()
        self:AddMovementInput(forwardvector, v, false)
    end
end

function Character_lua:MoveRight(v)
	if v ~= 0 then
		local rightvector = self:GetActorRightVector()
        self:AddMovementInput(rightvector, v, false)
    end
end

function Character_lua:Fire(isTrue)
	if isTrue then
		if self.ProjectileClass then
			local world = UUluautils.GetWorld(self)
			if world then
				local SpawnRotation = self:GetControlRotation()
				local vec_offset = UKismetMathLibrary.GreaterGreater_VectorRotator(self.GunOffset, SpawnRotation) 
				local MuzzleLocation = self.FP_MuzzleLocation:K2_GetComponentLocation()
				local SpawnLocation = UKismetMathLibrary.Add_VectorVector(MuzzleLocation, vec_offset)
				local scale = FVector.New()
				scale.X = 1;scale.Y = 1;scale.Z = 1
				local transfrom = UKismetMathLibrary.MakeTransform(SpawnLocation, SpawnRotation,scale)
				local spawnActor = UGameplayStatics.BeginDeferredActorSpawnFromClass(world, ADemo1Projectile.Class(), transfrom, ESpawnActorCollisionHandlingMethod.Undefined, nil)
				spawnActor = UGameplayStatics.FinishSpawningActor(spawnActor, transfrom)
				A_(spawnActor)
			end
		end
	end
end

function Character_lua:test(isTrue)
	if isTrue then
	end
end

return Character_lua