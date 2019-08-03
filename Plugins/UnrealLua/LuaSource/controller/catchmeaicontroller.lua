local CatchMeAIController = Inherit(CppObjectBase)
function CatchMeAIController:Ctor()
	self:SetReplicates(true)
	-- A_(UKismetSystemLibrary.IsServer(self))
end

function CatchMeAIController:BeginPlay()
	CppObjectBase.BeginPlay(self)
	if self.Pawn then
		self.Pawn:GC(self)
	end
end

function CatchMeAIController:Possess(InPawn)
	InPawn:GC(self)
	self:SetOwner(InPawn:GetOwner())
end

function CatchMeAIController:MoveToActor(actor)
	if actor._cppclass then
		ACatchMeAIController.MoveToActor(self, actor)
	else
		self:MoveToLocation(actor:K2_GetActorLocation())
	end
end

return CatchMeAIController