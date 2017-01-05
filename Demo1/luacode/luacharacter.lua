local Character_lua = Inherit(ADemo1Character)

function Character_lua:Ctor()
	GlobalEvent.On("Input_Forward", self.MoveForward, self)
	GlobalEvent.On("Input_Right", self.MoveRight, self)
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

return Character_lua