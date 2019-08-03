CoroutineUtil = {}

function CoroutineUtil.Delay(time, ...)
	local Handle = CoroutineMgr:Get():GetHandle()
	if Handle and Handle:IsValid() then
		local function f(delta)
			if Handle:IsValid() then
				return Handle:Resume(delta)
			end
		end
		TimerMgr:Get():On(f):Time(time):Num(1)
		return Handle:Yield(...)
	end
end

function CoroutineUtil.MoveToLocation(aicontroller, target_pos, ...)
	local Handle = CoroutineMgr:Get():GetHandle()
	if Handle and Handle:IsValid() then
		local pathresult = aicontroller:MoveToLocation(target_pos, ...)
		if pathresult == EPathFollowingRequestResult.Failed then
			CoroutineUtil.Delay(0.001)
			return CoroutineUtil.MoveToLocation(aicontroller, target_pos, ...)
		elseif pathresult == EPathFollowingRequestResult.AlreadyAtGoal then
		elseif pathresult == EPathFollowingRequestResult.RequestSuccessful then
			local ReceiveMoveCompleted = aicontroller.ReceiveMoveCompleted
			local Index
			local function OnFinishMoved(...)
				Handle.ReceiveMoveCompleted:Remove(Index)
				if Handle:IsValid() then
					return Handle:Resume(...)
				end
			end
			Index = ReceiveMoveCompleted:Add(OnFinishMoved)
			Handle.ReceiveMoveCompleted = ReceiveMoveCompleted
			return Handle:Yield(...)
		end
	end
end

function CoroutineUtil.LoadAsset(Path)
	local ObjectSoftPath = FSoftObjectPath.New()
	local Handle = CoroutineMgr:Get():GetHandle()
	if Handle and Handle:IsValid() then
		local function f()
			if Handle:IsValid() then
				return Handle:Resume(ObjectSoftPath:ResolveObject())
			end
		end
		ObjectSoftPath.AssetPathName = Path 
		Handle.StreamableHandle = UNZLuaHelperLibrary.RequestAsyncLoad(ObjectSoftPath, f)
		return Handle:Yield(Handle.StreamableHandle)
	else
		return GlobalLoadObject(nil, Path)
	end
end

return CoroutineUtil