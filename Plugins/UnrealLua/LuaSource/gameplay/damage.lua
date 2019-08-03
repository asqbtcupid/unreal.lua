local Damage = Inherit(Object)

function Damage:Ctor(Amount, FromActor)
	self.m_Amount = Amount
	self.m_FromActor = FromActor
end

function Damage:GetAmount( )
	return self.m_Amount
end

function Damage:GetFrom( )
	return self.m_FromActor
end

return Damage