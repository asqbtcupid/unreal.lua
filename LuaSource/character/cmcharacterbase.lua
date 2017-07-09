local CMCharacterBase = Inherit(CppObjectBase, ACMCharacterBase)

function CMCharacterBase:Ctor()
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
	
end

function CMCharacterBase:PostInitializeComponents()
	
end

return CMCharacterBase
