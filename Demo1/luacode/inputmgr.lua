function Input_Forward(v)
    GlobalEvent.Fire("Input_Forward", v)
end

function Input_Right(v)
    GlobalEvent.Fire("Input_Right", v)
end

function PressFire(isTrue)
	GlobalEvent.Fire("PressFire", isTrue)
end

function PressJump(isTrue)
	GlobalEvent.Fire("Jump", isTrue)
end

function Turn(v)
	GlobalEvent.Fire("Turn", v)
end

function Look(v)
	GlobalEvent.Fire("LookUpDown", v)
end