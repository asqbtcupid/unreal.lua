local SkillFsm = require "gameplay.fsm.skillfsm"
local BaseSkillFsm = Inherit(SkillFsm)
local Damage = require "gameplay.damage"

function BaseSkillFsm:Ctor(Owner, SkillInfo, SkillLevel, Target)
	self:Enter("Before")	
end

function BaseSkillFsm:CalDamage()
	local LevelInfo = self.m_LevelInfo
	local DamageAmout = LevelInfo.AttackPower + self.m_Owner:GetBaseAttackPower()*LevelInfo.AttackPowerFactor
	return Damage:NewIns(DamageAmout, self.m_Owner)
end

return BaseSkillFsm