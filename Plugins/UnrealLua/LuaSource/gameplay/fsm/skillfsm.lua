local SkillFsm = Inherit(ObjectBase)

function SkillFsm:Ctor(Owner, SkillInfo, SkillLevel, Target)
	self.m_Owner = Owner
	self.m_Time = 0
	self.m_BaseInfo = SkillInfo
	self.m_LevelInfo = Cfg("skilllevelinfo")[SkillInfo.id*1000+SkillLevel]
	self.m_Target = Target
	self.m_Frame = 1
end

function SkillFsm:Enter(StateName)
	self[StateName.."_P"](self)
	local TransitionState = StateName.."_T"
	self[TransitionState](self)
	self.m_StateTick = TransitionState
	self.m_State = StateName
end

function SkillFsm:Tick(delta)
	self.m_Time = self.m_Time + delta
	if self.m_StateTick then
		self[self.m_StateTick](self)
	end
end

function SkillFsm:Before_P()
	
end

function SkillFsm:Before_T()
	if self.m_Time >= self.m_BaseInfo.BeforeTime then
		self.m_Time = 0
		self:Enter("Act")
	end
end

function SkillFsm:CalDamage()
end

function SkillFsm:Act_P()
	self.m_Damage = self:CalDamage()
	self.m_Owner:M_PlaySkillMontage(self.m_BaseInfo.id)
end

function SkillFsm:Act_T()
	if self.m_EndTime then
		local Frame = self.m_LevelInfo.AttackFrame[self.m_Frame] 
		if Frame and self.m_Time/self.m_EndTime >= Frame then
			self.m_Target:OnDamage(self.m_Damage)			
			self.m_Frame = self.m_Frame + 1
		elseif self.m_Time >= self.m_EndTime then
			self:Enter("After")
		end
	end
end

function SkillFsm:SetAnimTime(Time)
	self.m_Time = 0
	self.m_EndTime = Time
end

function SkillFsm:After_P()
	self.m_Time = 0
end

function SkillFsm:After_T()
	if self.m_Time >= self.m_BaseInfo.AfterTime then
		self:Enter("Quit")
	end
end

function SkillFsm:Quit_P()
	self:Release()
end

function SkillFsm:Quit_T()
end

function SkillFsm:CanRun()
	return not self.m_State or self.m_State == "Before" or self.m_State == "After"
end

function SkillFsm:Destroy()
	self.m_Owner.m_SkillFsm = nil
end

return SkillFsm