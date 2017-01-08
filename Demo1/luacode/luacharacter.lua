local Character_lua = Inherit(ADemo1Character)
function Character_lua:CtorCpp()
	local CapsuleComponent = self.CapsuleComponent
	CapsuleComponent.CapsuleRadius = 55
	CapsuleComponent.CapsuleHalfHeight = 96
	self.BaseTurnRate = 45
	self.BaseLookUpRate = 45
	local FirstPersonCameraComponent = UCameraComponent.CreateDefaultSubobject(self, "FirstPersonCamera")
	self.FirstPersonCameraComponent = FirstPersonCameraComponent
	FirstPersonCameraComponent.AttachParent = CapsuleComponent
	FirstPersonCameraComponent.RelativeLocation = FVector.New(-39.56, 1.75, 64)
	FirstPersonCameraComponent.bUsePawnControlRotation = true

	local Mesh1P = USkeletalMeshComponent.CreateDefaultSubobject(self, "CharacterMesh1P")
	self.Mesh1P = Mesh1P
	Mesh1P:SetOnlyOwnerSee(true)
	Mesh1P.AttachParent = FirstPersonCameraComponent
	Mesh1P.bCastDynamicShadow = false
	Mesh1P.CastShadow = false
	Mesh1P.RelativeLocation = FVector.New(-0.5, -4.4, -155.7)
	Mesh1P.RelativeRotation = FRotator.New(1.9, -19.19, 5.2)

	local FP_Gun = USkeletalMeshComponent.CreateDefaultSubobject(self, "FP_Gun")
	self.FP_Gun = FP_Gun
	FP_Gun:SetOnlyOwnerSee(true)
	FP_Gun.bCastDynamicShadow = false
	FP_Gun.CastShadow = false
	FP_Gun.AttachParent = self.RootComponent

	local FP_MuzzleLocation = USceneComponent.CreateDefaultSubobject(self, "MuzzleLocation")
	self.FP_MuzzleLocation = FP_MuzzleLocation
	FP_MuzzleLocation.AttachParent = FP_Gun
	FP_MuzzleLocation:K2_SetRelativeLocation(FVector.New(0.2, 48.4, -10.6), false, FHitResult.New(), false)

	self.GunOffset = FVector.New(100.0, 0.0, 10.0)

end

function Character_lua:Ctor()
	GlobalEvent.On("Input_Forward", self.MoveForward, self)
	GlobalEvent.On("Input_Right", self.MoveRight, self)
	GlobalEvent.On("PressFire", self.Fire, self)
	-- GlobalEvent.On("PressFire", self.test, self)
end

function Character_lua:BeginPlayLua()
	self.FP_Gun:K2_AttachToComponent(self.Mesh1P, "GripPoint", EAttachmentRule.SnapToTarget, EAttachmentRule.SnapToTarget, EAttachmentRule.SnapToTarget, true)
	if self.bUsingMotionControllers then
		self.VR_Gun:SetHiddenInGame(false, true)
		self.Mesh1P:SetHiddenInGame(true, true)
	else
		self.VR_Gun:SetHiddenInGame(true, true)
		self.Mesh1P:SetHiddenInGame(false, true)
	end
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
				local scale = FVector.New(1, 1, 1)
				local transfrom = UKismetMathLibrary.MakeTransform(SpawnLocation, SpawnRotation, scale)
				local spawnActor = UGameplayStatics.BeginDeferredActorSpawnFromClass(world, self.ProjectileClass, transfrom, ESpawnActorCollisionHandlingMethod.Undefined, self)
				spawnActor = UGameplayStatics.FinishSpawningActor(spawnActor, transfrom)
			end
		end

		if self.FireSound then
			-- UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		end
	end
end

function Character_lua:test(isTrue)
	if isTrue then
	end
end

return Character_lua