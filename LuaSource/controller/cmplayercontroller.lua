local CMPlayerControllerBase = require "controller.cmplayercontrollerbase"
local CMPlayerController = Inherit(CMPlayerControllerBase)

function CMPlayerController:Ctor( )
	self.SpawnActors = {}
	self.Count = 0
	self.bAttachToPawn = true
	self.m_DefaultPawnClass = APawn.FClassFinder("/Game/Git/TopDownCharacter")
end

function CMPlayerController:HandleInput(name, ...)
	self[name](self, ...)
end

function CMPlayerController:BeginPlay( )
	CMPlayerControllerBase.BeginPlay(self)
	if self:IsAuth() then
		self:Timer(self.SpawnPlayer, self):Time(1):Num(1)
	else
	end
	if self:IsLocalPlayerController() then
		self:InitFogMgr()
		self.TestUI = require "ui.test":NewCpp(self, self)
	end
	self:GetFoliageActor()
end

function CMPlayerController:InitFogMgr()
	self.m_FogMgr = UCMFogMgr.New(self, "FogMgr")
	self:GC(self.m_FogMgr)
	self.m_FogMgr:LuaInit(self)
end

function CMPlayerController:GetFoliageActor()
	self.m_FoliageMeshs  = {}
	for k, v in ipairs(Cfg("foliage")) do
		self.m_FoliageMeshs[k] = UStaticMesh.LoadObject(self, v.Path)
	end
	local actors = UGameplayStatics.GetAllActorsOfClass(self, AActor.Class(), {})
	self.m_FoliageComponents = {}
	for k, v in ipairs(actors) do
		if ULuautils.GetName(v):find("Foliage") then
			for _, component in ipairs(v:GetComponentsByClass(UInstancedStaticMeshComponent.Class())) do
				component = UInstancedStaticMeshComponent.Cast(component)
				for index, StaticMesh in ipairs(self.m_FoliageMeshs) do
					if component.StaticMesh == StaticMesh then
						self.m_FoliageComponents[component] = index
						self.m_FoliageComponents[index] = component
						if self:IsLocalPlayerController() then
							component:SetMaterial(0, self.m_FogMgr:GetMaterial(Cfg("foliage")[index].Material))
						end
						break
					end
				end
			end
		end
	end
end

function CMPlayerController:SpawnPlayer()
	local SpawnLocation = FVector.New(0, 0, 300)
	local SpawnRotation = FRotator.New(0,0,0)
	local transfrom = UKismetMathLibrary.MakeTransform(SpawnLocation, SpawnRotation, FVector.New(1, 1, 1))
	local spawnActor = UGameplayStatics.BeginDeferredActorSpawnFromClass(self, self.m_DefaultPawnClass, transfrom, ESpawnActorCollisionHandlingMethod.AlwaysSpawn, self)
	spawnActor = UGameplayStatics.FinishSpawningActor(spawnActor, transfrom)
	self.PlayCharacter = spawnActor
	self.m_PlayCharacter = spawnActor
	spawnActor:InitBaseInfo(1, 1)
	spawnActor:SetPlayerController(self)
end

function CMPlayerController:RemoveFoliage(FoliageComponent, Index)
	local FoliageIndex = self.m_FoliageComponents[FoliageComponent]
	if not self:IsAuth() then
		self:S_RemoveFoliage(FoliageIndex, Index)
	end
	self:S_RemoveFoliage_Imp(FoliageIndex, Index)
end

function CMPlayerController:S_RemoveFoliage_Imp(FoliageIndex, Index)
	local FoliageComponent 
	if self.m_FoliageComponents then
		FoliageComponent = UInstancedStaticMeshComponent.Cast(self.m_FoliageComponents[FoliageIndex])
	end
	local function Remove()
		if FoliageComponent then
			FoliageComponent:RemoveInstance(Index)
			ULuautils.UpdateNav(FoliageComponent)
		end
	end
	if self:IsAuth() then
		Remove()
		local actors = UGameplayStatics.GetAllActorsOfClass(self, ACatchMePlayerController.Class(), {})
		for k, v in pairs(actors) do
			v:C_RemoveFoliage(FoliageIndex,Index)
		end
	else
		local TransForm = FTransform.Temp()
		FoliageComponent:GetInstanceTransform(Index, TransForm, true)
		local Pos = TransForm:Break()
		if self.m_FogMgr:CanSeeNow(Pos) then
			Remove()
		else
			A_("no")
		end
	end
end

function CMPlayerController:InputTap_Press(Pos)
	self.m_Pawn:StartPress(Pos)
end	

function CMPlayerController:InputTap_Release(Pos, HoldTime)
	if not self.m_bHasMoveScreen then
		local Hit = FHitResult.Temp()
		if self:GetHitResult(Pos[1], Pos[2], Hit, ECollisionEnabled.QueryOnly) then
			local actor = Hit.Actor
			if actor and actor.m_CanAttacked and actor.m_Visible then
				self:S_TapActor(actor)
			else
				if ULuautils.GetName(actor):find("Foliage") then
					-- Hit.Component
					-- A_(self.m_FoliageComponents[FoliageComponent ])
					-- FoliageComponent = UInstancedStaticMeshComponent.Cast(FoliageComponent)
					-- A_(FoliageComponent.StaticMesh)
					-- A_(UStaticMesh.LoadObject(self, "StaticMesh'/Game/StarterContent/Shapes/Shape_Cylinder.Shape_Cylinder'"))
					-- A_(ULuautils.GetName(actor))

					-- self:RemoveFoliage(Hit.Component, Hit.Item)

					-- A_(Hit.Distance)
					-- self:S_TapFloor(Hit.ImpactPoint)
					
					self:S_TapFoliage(Hit.Component, Hit.Item)
				else
					self:S_TapFloor(Hit.ImpactPoint)
					-- self:S_TapFloor_Imp(Hit.ImpactPoint)
				end
			end
		end
	end
	self.m_bHasMoveScreen = false
end		

function CMPlayerController:InputTap_Hold(Pos, HoldTime)
end		

function CMPlayerController:InputTap_Move(Pos, HoldTime, change)
	self.m_Pawn:Move(Pos)
	if math.abs(change[1]+change[2]) > 10 then
		self.m_bHasMoveScreen = true
	end
end

function CMPlayerController:GetAnimIns()
	local Character = self.Character
	if Character then
		local Mesh = Character.Mesh
		if Mesh then
			return Mesh:GetAnimInstance()
		end
	end
end

function CMPlayerController_GetLifetimeReplicatedProps()
	local t = {}
	table.insert(t, FReplifetimeCond.NewItem("PlayCharacter", ELifetimeCondition.COND_AutonomousOnly))
	return t
end

function CMPlayerController:S_TapFloor_Imp(Pos,haha)
	if self.m_PlayCharacter then
		self.m_PlayCharacter:TapFloor(Pos)
	end
end

function CMPlayerController:S_TapActor_Imp(Actor)
	if self.m_PlayCharacter then
		self.m_PlayCharacter:TapActor(Actor)
	end
end

function CMPlayerController:S_TapFoliage_Imp(Component, Index)
	if self.m_PlayCharacter then

		local FoliageClass = require "gameplay.foliageactor"
		local newone = FoliageClass:New(Component, Index)
		self.m_PlayCharacter:TapActor(newone)
	end
end

function CMPlayerController:Visible(character)
	if not self.PlayCharacter then 
		return false
	end
	if self.PlayCharacter == character then
		return true
	else
		local StartPos = self.PlayCharacter:K2_GetActorLocation()
		local EndPos = character:K2_GetActorLocation() 
		local Hit = FHitResult.Temp()
		if UKismetSystemLibrary.LineTraceSingle(self.PlayCharacter, StartPos, EndPos, ETraceTypeQuery.TraceTypeQuery1, true, {}, EDrawDebugTrace.None, Hit, true) then
			if Hit.Actor == character then
				return true
			else
				return false
			end
		end
	end
end

return CMPlayerController