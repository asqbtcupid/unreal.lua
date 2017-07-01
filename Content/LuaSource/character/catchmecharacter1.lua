local CMCharacterBase = require "character.cmcharacterbase"
local CatchMeCharacter = Inherit(CMCharacterBase, ACatchMeCharacter)
local Damage = require "gameplay.damage"

local ESkillState = 
{
	Idle = 0,
	Before = 1,
	Acting = 2,
	After = 3,
}
function CatchMeCharacter:Ctor()
	local CapsuleComponent = self.CapsuleComponent
	CapsuleComponent.CapsuleRadius = 42
	CapsuleComponent.CapsuleHalfHeight = 96

	self.bUseControllerRotationPitch = false
	self.bUseControllerRotationYaw = false
	self.bUseControllerRotationRoll = false

	local CharacterMovement = self.CharacterMovement 
	-- self:AddCamera()
	CharacterMovement.bOrientRotationToMovement = true
	CharacterMovement.RotationRate = FRotator.New(0, 640, 0)
	CharacterMovement.bConstrainToPlane = true
	CharacterMovement.bSnapToPlaneAtStart = true
	
	local PrimaryActorTick = self.PrimaryActorTick
	PrimaryActorTick.bCanEverTick = true
	PrimaryActorTick.bStartWithTickEnabled = true

	self.m_CanAttacked = true
	self.m_SkillState = ESkillState.Idle
end

function CatchMeCharacter:AddCamera( )
	local CameraBoom = USpringArmComponent.CreateDefaultSubobject(self, "CameraBoom")
	self.CameraBoom = CameraBoom
	local RootComponent = self.RootComponent
	CameraBoom.AttachParent = RootComponent
	CameraBoom.bAbsoluteRotation = true
	CameraBoom.TargetArmLength = 800
	CameraBoom.RelativeRotation = FRotator.New(-60, 0, 0)
	CameraBoom.bDoCollisionTest = false 

	local TopDownCameraComponent = UCameraComponent.CreateDefaultSubobject(self, "TopDownCamera")
	self.TopDownCameraComponent = TopDownCameraComponent
	TopDownCameraComponent.AttachParent = CameraBoom
	TopDownCameraComponent.AttachSocketName = "SpringEndpoint"
	TopDownCameraComponent.bUsePawnControlRotation = false
end

function CatchMeCharacter:InitBPComponents()
	if not self.m_IsInitBPComponents then
		self.m_IsInitBPComponents = true
		local WidgetComponents = self:GetComponentsByClass(UWidgetComponent.Class())
		for k, v  in pairs(WidgetComponents) do
			if ULuautils.GetName(v) == "Widget_HP" then
				self.m_widget = UWidgetComponent.Cast(v)
				self.m_UI_HP = require "ui.uicharacterhp":NewOn(self.m_widget.Widget)
			end
		end
	end
end

function CatchMeCharacter:PostInitializeComponents()
	self:InitBPComponents()
end

function CatchMeCharacter:BeginPlay()
	self:Super().BeginPlay(self)
end

function CatchMeCharacter:Tick(delta)
	self:UpdateSkillState()
end
-- ****** Before Skill****************
function CatchMeCharacter:CanAckSkill(SkillId)
		
end

function CatchMeCharacter:StartBeforeSkill()
	if self.m_SkillState == ESkillState.Idle then
		self.m_SkillState = ESkillState.Before
		self.m_AttackingTarget = self.m_TargetActor
		self.m_BeforeSkillTimerHandle = self:Timer(self.EndBeforeSkill, self):Time(self.m_CurrentSkillInfo.BeforeTime):Num(1)
		self.m_ActtingSkillInfo = self.m_CurrentSkillInfo
	end
end

function CatchMeCharacter:StopBeforeSkill()
	self.m_BeforeSkillTimerHandle:Destroy()
end

function CatchMeCharacter:EndBeforeSkill()
	if self.m_AttackingTarget then
		self:StartActSkill()
	else
		self.m_SkillState = ESkillState.Idle
	end
end
-- **********Act Skill***********************************
function CatchMeCharacter:StartActSkill()
	self.m_ActtingSkillDamage = self:CalDamage()
	self.m_SkillState = ESkillState.Acting
	self:M_PlaySkillMontage(self.m_ActtingSkillInfo.id)
end

function CatchMeCharacter:SkillCost()
	
end

function CatchMeCharacter:M_PlaySkillMontage_Imp(SkillId)
	local cfg = Cfg("skillbaseinfo")[SkillId]
	local MontagePath = cfg.MontagePath
	local SpeedFactor = cfg.SpeedFactor
	local Anim = self:GetMontage(MontagePath)
	local AnimTime = self.Mesh:GetAnimInstance():Montage_Play(Anim, SpeedFactor)
	if self:IsAuth() then
		self.m_ActSkillTimerHandle = self:Timer(self.EndActSkill, self):Time(AnimTime/SpeedFactor):Num(1)
	end
end

function CatchMeCharacter:StopActSkill()
end

function CatchMeCharacter:CalDamage()
	local SkillId = self.m_ActtingSkillInfo.id
	local SkillLevel = self.m_AllSkillInfoMap[SkillId].Level
	local cfg = Cfg("skilllevelinfo")[SkillId*1000+SkillLevel]
	local DamageAmout = cfg.AttackPower + self:GetBaseAttackPower()*cfg.AttackPowerFactor
	return Damage:NewIns(DamageAmout, self)
end

function CatchMeCharacter:EndActSkill()
	self.m_AttackingTarget:OnDamage(self.m_ActtingSkillDamage)
	self:StartAfterSkill()
	if not self.m_CurrentSkillInfo.IsAgain then
		-- self.m_TargetActor
	end
end
-- ******************************************************

function CatchMeCharacter:StartAfterSkill( )
	self.m_SkillState = ESkillState.After
	self:Timer(self.EndAfterSkill, self):Time(self.m_CurrentSkillInfo.AfterTime):Num(1)
end
function CatchMeCharacter:StopAfterSkill( )
	
end
function CatchMeCharacter:EndAfterSkill( )
	self.m_SkillState = ESkillState.Idle
	if not self.m_CurrentSkillInfo.IsAgain then
		self:SetCurrentSkill(self.m_BaseSkillId)
	end
end

function CatchMeCharacter:ActTargetSkill()
	if self.m_TargetActor then
		A_("AT ", self.m_TargetActor:GetName())
		self:StartBeforeSkill() 
	end
end

function CatchMeCharacter:UpdateSkillState()
	if self.m_SkillState == ESkillState.Idle then
		if self.m_TargetActor and self.m_CurrentSkillInfo.HasTarget then
			self:TryActTarget()
		end
	end
end

function CatchMeCharacter:TryActTarget()
	local function IsClose()
		local FaceRotationVector = self.m_TargetActor:K2_GetActorLocation() - self:K2_GetActorLocation() 
		if #(FaceRotationVector) > self.m_CurrentSkillInfo.Distance then
			return false
		-- else
		-- 	local Angle = math.abs(self:K2_GetActorRotation().Yaw - FaceRotationVector:Rotation().Yaw)
		-- 	if Angle > 10 then
		-- 		return false
		-- 	end
		end
		return true
	end
	if not IsClose() then
		self:GetController():MoveToActor(self.m_TargetActor)
	else
		self:GetController():StopMovement()
		self:ActTargetSkill()
	end
end
-- ************************input from controller*****************
function CatchMeCharacter:TapActor(Actor)
	local isEnemy = Actor ~= self
	if isEnemy then
		if self.m_CurrentSkillInfo.HasTarget then
			self.m_TargetActor = Actor
			self:TryActTarget()
		end
	end
end

function CatchMeCharacter:CanRun()
	return self.m_SkillState == ESkillState.Idle or self.m_SkillState == ESkillState.After
end

function CatchMeCharacter:TapFloor(Pos)
	if self.m_CurrentSkillInfo.MoveToFloor then
		self.m_TargetActor = nil
		if self:CanRun() then
			self:GetController():MoveToLocation(Pos)
		end
	end
end
-- **********************************************************

function CatchMeCharacter:GetBaseAttackPower()
	local cfg = Cfg("characterlevelinfo")[self.CharacterId*1000+self.Level]
	return cfg.AttackPower
end

function CatchMeCharacter:InitBaseInfo(CharacterId, Level)
	self.CharacterId = CharacterId
	self.Level = Level
	self.EXP = 0
	self.m_MapHP = self:GetMaxHP()
	self.HP = self.m_MapHP
	self.HPPercent = 1
	self.m_BaseInfo = Cfg("characterbaseinfo")[CharacterId]
	self:SetBaseSkill(self.m_BaseInfo.Skills[1])
	self:SetCurrentSkill(self.m_BaseInfo.Skills[1])
	local BaseSkill = FSkillInfo.New()
	BaseSkill.SkillId = 1
	BaseSkill.Level = 1
	local SkillInfo = {BaseSkill}
	self.SkillInfo = SkillInfo
	self:OnRep_SkillInfo()
end

function CatchMeCharacter:GetMaxHP()
	return Cfg("characterlevelinfo")[self.CharacterId*1000+self.Level].MaxHP
end

function CatchMeCharacter:SetBaseSkill(SkillId)
	self.m_BaseSkillId = SkillId
	self.m_BaseSkillInfo = Cfg("skillbaseinfo")[self.m_BaseSkillId]
end

function CatchMeCharacter:SetCurrentSkill(SkillId)
	self.m_NowSkillId = SkillId
	self.m_CurrentSkillInfo = Cfg("skillbaseinfo")[self.m_NowSkillId]
end

function CatchMeCharacter:OnRep_SkillInfo()
	self.m_AllSkillInfo = self.SkillInfo
	self.m_AllSkillInfoMap = {}
	-- A_(self.m_AllSkillInfo[1].Level)
	for k, v in ipairs(self.m_AllSkillInfo) do
		self.m_AllSkillInfoMap[v.SkillId] = v
	end
end

function CatchMeCharacter:OnRep_HP()
end

function CatchMeCharacter:OnRep_HPPercent( )
	self.m_UI_HP:Wnd("Bar_HP"):SetPercent(self.HPPercent)
end

function CatchMeCharacter:OnRep_MP( )
end

function CatchMeCharacter:OnRep_EXP( )
end

function CatchMeCharacter:OnRep_Level( )
end

function CatchMeCharacter:OnRep_CharacterId()
end

function CatchMeCharacter:IsReplicationPausedForConnection(ConnectionOwnerNetViewer)
	return false
end

function CatchMeCharacter:OnDamage(DamageIns)
	local Amount = DamageIns:GetAmount()
	local NowHP = self.HP 
	NowHP = NowHP - Amount
	self.HP = NowHP
	self.HPPercent = NowHP/self:GetMaxHP()
end

function CatchMeCharacter_GetLifetimeReplicatedProps()
	local t = {}
	table.insert(t, FReplifetimeCond.NewItem("BaseInfo", ELifetimeCondition.COND_None))
	table.insert(t, FReplifetimeCond.NewItem("SkillInfo", ELifetimeCondition.COND_OwnerOnly))
	table.insert(t, FReplifetimeCond.NewItem("HP", ELifetimeCondition.COND_OwnerOnly))
	table.insert(t, FReplifetimeCond.NewItem("HPPercent", ELifetimeCondition.COND_None))
	table.insert(t, FReplifetimeCond.NewItem("MP", ELifetimeCondition.COND_OwnerOnly))
	table.insert(t, FReplifetimeCond.NewItem("EXP", ELifetimeCondition.COND_OwnerOnly))
	table.insert(t, FReplifetimeCond.NewItem("Level", ELifetimeCondition.COND_OwnerOnly))
	table.insert(t, FReplifetimeCond.NewItem("CharacterId", ELifetimeCondition.COND_None))
	return t
end

return CatchMeCharacter