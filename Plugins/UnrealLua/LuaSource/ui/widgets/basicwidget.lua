local BasicWidget = Inherit(CppObjectBase, UWidget)

function BasicWidget:Event(delegateName, callback, ...)
	local lua_delegateName = "m_"..delegateName
	if not self[lua_delegateName] then
		self[lua_delegateName] = self[delegateName] 
	end
	self[lua_delegateName]:Add(MakeCallBack(callback, ...))
end

return BasicWidget