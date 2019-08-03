LuaUVerticalBox = Inherit(BasicWidget)

function LuaUVerticalBox:Bind(WidgetClass)
	return BasicWidget.Bind(self, WidgetClass):AddFunc("AddChildToVerticalBox")
end

return LuaUVerticalBox