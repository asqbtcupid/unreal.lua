function Input_Forward(v)
    GlobalEvent.Fire("Input_Forward", v)
end

function Input_Right(v)
    GlobalEvent.Fire("Input_Right", v)
end

function PressFire(isTrue)
	GlobalEvent.Fire("PressFire", isTrue)
end