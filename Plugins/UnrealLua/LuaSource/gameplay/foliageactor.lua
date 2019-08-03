local FoliageActor = Inherit(ObjectBase)

function FoliageActor:Ctor(Component, Index)
	self.m_Component = UInstancedStaticMeshComponent.Cast(Component)
	self.m_Index = Index
	local TransForm = FTransform.New()
	self.m_Component:GetInstanceTransform(Index, TransForm, true)
	self.m_Pos = TransForm:Break()
	self.m_Pos.Z = 200
end

function FoliageActor:K2_GetActorLocation()
	return self.m_Pos
end

function FoliageActor:OnDamage(DamageData)
	DamageData:GetFrom():GetPlayerController():RemoveFoliage(self.m_Component, self.m_Index)
	-- :RemoveFoliage(self.m_Component, Index)
	-- A_(, DamageData:GetAmount())
end
return FoliageActor
