local CMCharacterBase = Inherit(CppObjectBase, ACMCharacterBase)

function CMCharacterBase:Ctor()
	-- if G_GameStatics.GameMode then
	-- 	G_GameStatics.GameMode:AddPlayerController(self)
	-- end
end

function CMCharacterBase:Tick(DeltaSeconds)
	
end

local skillObjects ={}
local sKillPrefix = "/Game/skillanim/"
function CMCharacterBase:GetMontage(path)
	if not skillObjects[path] then
		skillObjects[path] = UAnimMontage.LoadObject(self, sKillPrefix..path)
	end
	return skillObjects[path]
end

function CMCharacterBase:SkillFsm(name)
    local fsmins =  require ("gameplay.fsm."..name):NewIns(self, self.m_CurrentSkillInfo, self.m_AllSkillInfoMap[self.m_CurrentSkillInfo.id].Level, self.m_TargetActor)
	self.m_SkillFsm = fsmins
end

function CMCharacterBase:BeginPlay()
	if self.Role == 1 then
		G_SimulateProxy.Character[self] = true
		self.m_OnDestroyed = self.OnDestroyed
		local function OnDestroy()
			G_SimulateProxy[self] = false
		end
		self.m_OnDestroyed:Add(OnDestroy)
	end
end

function CMCharacterBase:PostInitializeComponents()
	
end

return CMCharacterBase
