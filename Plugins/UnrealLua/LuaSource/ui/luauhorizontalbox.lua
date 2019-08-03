LuaUHorizontalBox = Inherit(BasicWidget)

function LuaUHorizontalBox:Bind(WidgetClass)
	return BasicWidget.Bind(self, WidgetClass):AddFunc("AddChildToHorizontalBox")
end

return LuaUHorizontalBox