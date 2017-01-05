-- CallBack Function
local function MakeCallBack(callBack, ...)
    local parameters = {...}
    local handle = {}
    local len_p = table.maxn(parameters)
    local function f(...)
        local args = {...}
        local len_a = table.maxn(args)
        for i = 1, len_a do
            parameters[i+len_p] = args[i]
        end
        handle.result = callBack(unpack(parameters, 1, len_p+len_a))
        return handle.result
    end
    return f, handle
end

local ProxyMeta = {}
function ProxyMeta.__index(t, k)
    local wnd = t._wnd_
    if ProxyMeta[k] then return ProxyMeta[k]  
    elseif wnd[k] then
        if type(wnd[k]) == "function" then
            local function f(_, ...)
                return wnd[k](wnd, ...)
            end
            return f
        else
            return wnd[k]
        end
    -- 使得xxx:EventName(...)变成xxx:subscriberEvent("EventName", ...)
    -- 例如xxx:MouseClick(self.Click, self)变成xxx:subscriberEvent("MouseClick", self.Click, self)
    -- 省了写subscriberEvent这几个字，
    -- 同时，不仅仅可以回调一个参数self，可以多个，例如xxx:MouseClick(self.Click, self, a, b, c, ...)，请看ProxyMeta:On
    elseif k:sub(1,1) ~= "_" then
        local function f(_,...)
            return t:On(k,...)
        end
        return f
    end
end
--获得原生窗口，就是 winMgr:getWindow
function ProxyMeta:Raw()
    return self._wnd_
end

function ProxyMeta:Bind(wnd, opposite)
    local Visible = wnd:Visible()
    if opposite then
        Visible = not Visible
    end
    self:Show( Visible )
    wnd:Hidden(self.Hide, self, not opposite)
    wnd:Shown(self.Show, self, not opposite)
    return self
end
--类似于setText接口，color是加颜色，颜色参数具体看simple.color
function ProxyMeta:Txt(str, color)
    if color ~= nil then
        str = self._ins_:Color(color)..str
    end
    if self._str_ ~= str then
        self._str_ = str
        self:setText(str)
    end
    return self
end
--
function ProxyMeta:Str(id, ...)
    return self:Txt(self._ins_:Str(id, ...))
end
--类似于原生接口subscriberEvent，但是可以回调多个参数
--例如xxx:subscriberEvent("MouseClick", self.Click, self)可以写成xxx:On("MouseClick", self.Click, self)
--但是可以回调不止一个参数了，例如xxx:On("MouseClick", self.Click, self, id)，则回调self:Click(id)
function ProxyMeta:On(event, callBack, ...)
    if type(event) ~= "string" or type(callBack) ~= "function" then
        error("subscribeEvent error in simpledlg")
    end
    if event == "LongPress" then
        self:EnableUserNeedUpdate()
        self:SetNotTrigMouseClickAfterLongPress(true)
        self._EnableUserNeedUpdate_ = true
    end
    local proxyCallBack = MakeCallBack(callBack, ...)
    self:subscribeEvent(event, proxyCallBack)
    return self
end
-- 调用On之后，直接调用一次回调函数
function ProxyMeta:OnFire(event, f, ...)
    self:On(event, f, ...)
    f(...)
    return self
end
function ProxyMeta:ReOn(event, ...)
    self:removeEvent(event)
    return self:On(event, ...)
end

function ProxyMeta:GetImgByNum(num)
    if type(num) == "number" then
        if num < 10000 then
            return true, UI_UTIL.GetPicPathByID(num)
        elseif num >= 10000 then
            return true, ITEM_UTIL.GetItemIconPath(num)
        end
    end
    return false
end
-- 类似于xxx:setProperty("Image", imgstr),简化接口，可以看到Button类的这个函数复写了
-- 同一了该接口
function ProxyMeta:Img(imgstr, isShow)
    local isTrue, imgfromnum = self:GetImgByNum(imgstr)
    if isTrue then imgstr = imgfromnum end
    if self._img_str_ == imgstr then return self end
    self._img_str_ = imgstr
    isShow = isShow or (imgstr ~= nil and imgstr ~= "")
    self:setProperty("Image", imgstr)
    self:Show(isShow)
    return self
end

function ProxyMeta:Enable(isTrue)
    self:setEnabled(isTrue == true)
    return self
end
-- cellName是要添加的cell的lua路径，
-- data是数据，必须是一个数组，按照这个顺序往控件里塞cell
-- CallbackName是cell的回调函数的名字，第i个cell就会用data[i]，{i, wnd, ins}来回调
-- layout的定义可以看UI_UTIL.Layout的setting参数
function ProxyMeta:AddCells(cellName, data, CallbackName, layout, ...)
    local extrainfo = {...}
    CallbackName = CallbackName or "SetInfo"
    if self._Cells_ then 
        return self:RefreshCells(data, CallbackName, ...) 
    end
    if #data == 0 then
        return
    end
    local pos
    local cells = {}
    self._Cells_ = cells
    local function Add(starIndex, endIndex, ...)
        for cellIndex = starIndex, endIndex do
            local newCell = require(cellName):GetInstance(self, cellIndex) 
            if not pos then
                layout = layout or {marginx=1,marginy=1,intervalx=1,intervaly=1}
                pos = UI_UTIL.Layout(self, newCell:GetMainFrame(), layout)
            end
            newCell:GetMainFrame():setPosition(pos[cellIndex])
            if CallbackName then
                newCell[CallbackName](newCell, data[cellIndex], {cellIndex, self, self._ins_}, ...)
            end
            cells[cellIndex] = newCell
        end
    end
    Add(#cells+1, #data, ...)
    --该函数不要碰，数据有变也调用addcells就可以了
    function ProxyMeta:RefreshCells(newData, funcname, ...)
        funcname = funcname or CallbackName 
        data = newData
        for i, v in ipairs(data) do
            if cells[i] then
                cells[i]:Show(true)
                if funcname then
                    cells[i][funcname](cells[i], v, {i, self, self._ins_}, ...)
                end
            else
                Add(i, #data, ...)
                return self
            end
        end
        for i = #data+1, #cells do
            cells[i]:Show(false)
        end
        return self
    end
    -- 获得所有的cell
    function ProxyMeta:GetCells()
        return self._Cells_
    end
    return self
end
-- 是setVisible的升级版
function ProxyMeta:Show(visible)
    visible = visible == true
    if self._visible_ ~= visible then
        self._visible_ = visible
        self:setVisible(visible)
    end
    return self
end

function ProxyMeta:Hide(isTrue)
    return self:Show(not isTrue)
end
-- 是isVisible替代品，经常调用的话，这个效率应该会高一点
function ProxyMeta:Visible()
    if self._visible_ == nil then
        self._visible_ = self:isVisible()
    end
    return self._visible_
end
-- 元表技巧，XXX:Delay(time):SomeFunc(a,b,c,d)实现了
-- 在time时间之后调用XXX:SomeFunc(a,b,c,d)，实例销毁则会自动取消计时不会回调
function ProxyMeta:Delay(time)
    local functor = {}
    function functor.__index(_, k)
        local function f(_, ...)
            self._ins_:Timer(time, MakeCallBack(self[k], self, ...), nil, nil, 1)
            return self
        end
        return f
    end
    setmetatable(functor, functor)
    return functor
end
-- 类似于UI_UTIL.AddModelToWnd，稍微简单一点点   
function ProxyMeta:AddModel(modelId, setting, otherParameter, slaveMap, skills, heroid)
    if setting == nil or setting:find("%-skill") == nil and skills == nil then
        skills = {{"stand2", "stand1"}, {1, -1}, false}
        setting = setting or ""
        setting = setting.." ".."-skill"
    end
    self._actor_ = UI_UTIL.AddModelToWnd(modelId, self, otherParameter, slaveMap, setting, skills, heroid)
    self._model_id_ = modelId
    return self
end
-- 类似于UI_UTIL.AttachSlave
function ProxyMeta:AttachSlave(slaveMap, isInbattle, isNpc)
    UI_UTIL.AttachSlave(self._actor_, slaveMap, isInbattle, self._model_id_ , isNpc)
    return self
end
-- 加主角的model，预览装备，和AddModel类似，equip是需要预览的装备，可以是一个itemid或者{itemid1,itemid2,...}
function ProxyMeta:PreviewEquip(equip, setting, paramaters, skills)
    if equip == nil then equip = {}  
    elseif type(equip) == "number" then equip = {equip} end
    local heroInfo = HeroMgr:GetInstance():GetMainHeroInfo()
    local equips = Clone(heroInfo.equipnews)
    for k, v in pairs(equip) do
        equips[self._ins_:Cfg("item.citemattrlua", v).itemtypeid] = {showid = v}
    end
    local actorId = HERO_UTILS.GetHeroModelID(heroInfo.heroid, heroInfo.elementattrtype)
    self:AddModel(actorId, setting, paramaters, equips, skills, heroInfo.heroid)
    return self
end
-- numSet是{{data1, data2, data3}, {data1, data2, data3},...}这种结构，numSet[1]对应的就是$parameter1$，依次类推
function ProxyMeta:NumUp(strs, numSet, callBack, ...)
    if callBack then
        UI_UTIL.NumIncreaseEffect(self, strs, numSet, self._ins_, MakeCallBack(callBack, ...))
    else
        UI_UTIL.NumIncreaseEffect(self, strs, numSet, self._ins_)
    end
    return self
end
-- 同一个groupID的一系列窗口调用Show时，会自动移位，效果参考UI_UTIL.AutoLayout(wndGroup)
function ProxyMeta:AutoLayout(groupID)
    groupID = groupID or 0
    local ins = self._ins_
    if self._is_AutoLayout_ then return self end
    self._is_AutoLayout_ = true
    ins._Auto_Pos_ = ins._Auto_Pos_ or {}
    ins._Auto_Wnd_ = ins._Auto_Wnd_ or {}
    ins._Auto_visible_ = ins._Auto_visible_ or {}
    ins._Auto_Pos_[groupID] = ins._Auto_Pos_[groupID] or {}
    ins._Auto_Wnd_[groupID] = ins._Auto_Wnd_[groupID] or {}
    ins._Auto_visible_[groupID] = ins._Auto_visible_[groupID] or {}
    local pos = self:getPosition()
    pos = {{pos.x.scale, pos.x.offset}, {pos.y.scale, pos.y.offset}}
    local index = #ins._Auto_Wnd_[groupID] + 1
    ins._Auto_Wnd_[groupID][index] = self
    ins._Auto_Pos_[groupID][index] = pos
    ins._Auto_visible_[groupID][index] = self:Visible()
    self.Show = function(self, isVisible)
        ProxyMeta.Show(self, isVisible)
        ins._Auto_visible_[groupID][index] = isVisible == true
        local emptyIndex = 1
        for i = 1, #ins._Auto_Wnd_[groupID] do
            if ins._Auto_visible_[groupID][i] == true then
                local setpos = ins._Auto_Pos_[groupID][emptyIndex]
                ins._Auto_Wnd_[groupID][i]:setPosition(CEGUI.UVector2(CEGUI.UDim(setpos[1][1], setpos[1][2]), CEGUI.UDim(setpos[2][1], setpos[2][2])))        
                emptyIndex = emptyIndex + 1
            end
        end
        return self
    end
    return self
end
--长按弹出道具tips
function ProxyMeta:PopItem(itemid)
    if self._popitemid_ == itemid then return self end
    self._popitemid_ = itemid
    self:setID(itemid)
    ITEM_UTIL.BindItemCellEvent(self)
    self:SetNotTrigMouseClickAfterLongPress(true)
    return self
end
-- 播放动画，如果name是nil则停止播放
function ProxyMeta:Anim(name)
    if name then
        if self._Anim_Ins_Name == name and self._Anim_Ins_ and self._Anim_Ins_:isRunning() then
        else
            if self._Anim_Ins_ then
                self._Anim_Ins_:getEventSender():removeEvent("AnimationStopped")
            end
            self:setAnimationName(name)
            self:startAnimation()
            self._Anim_Ins_ = self:getAnimationInstance()
            self._Anim_Ins_Name = name
        end
    elseif self._Anim_Ins_ then
        self._Anim_Ins_:getEventSender():removeEvent("AnimationStopped")
        self._Anim_Ins_:stop()
        self._Anim_Ins_ = nil
        self._Anim_Ins_Name = nil
    end
    return self
end
-- 动画停止的回调
function ProxyMeta:AnimStop(callBack, ...)
    if self._Anim_Ins_ then
        local proxyCallBack = MakeCallBack(callBack, ...)
        self._Anim_Ins_:getEventSender():subscribeEvent("AnimationStopped", proxyCallBack)
    end
    return self
end
-- 参考Eff，但是会检查一下这个effectid跟上一个是否一样，不一样才调用Eff
function ProxyMeta:EffCheck(effectid, ...)
    if self._Has_Add_Effect_ ~= effectid then
        return self:Eff(effectid, ...)
    end
    return self
end
--播放特效，特效ID是effectid
function ProxyMeta:Eff(effectid, ...)
    if effectid then
        self:Eff()
        self._Has_Add_Effect_ = effectid
        local effTime = LuaGetGameUIManager():AddUIEffectFromLua(UI_UTIL.GetEffectPathById(effectid), self, ...)
        self._effect_time_ = effTime * 1000 + GetServerTime()
    elseif self._Has_Add_Effect_ then
        self._Has_Add_Effect_ = nil
        self._effect_time_ = nil
        if self._EffStopHandle_ then self._EffStopHandle_:Destroy() end
        LuaGetGameUIManager():RemoveUIEffectFromLua(self)
        if self._EnableUserNeedUpdate_ then self:EnableUserNeedUpdate() end
    end
    return self
end
--特效结束回调
function ProxyMeta:EffStop(callBack, ...)
    if self._EffStopHandle_ then self._EffStopHandle_:Destroy() end
    if self._effect_time_ then
        local leftTime = self._effect_time_ - GetServerTime()
        if leftTime > 0 then
            self._EffStopHandle_ = self._ins_:Timer(leftTime, MakeCallBack(callBack, ...), nil, nil, 1)
        end
    end
    return self
end
-- 点击穿透setMousePassThroughEnabled的替代
function ProxyMeta:Pass(setting)
    self:setMousePassThroughEnabled(setting)
    return self
end
-- getPixelSize的替代，有缓存多次调用效率更佳
function ProxyMeta:Size()
    if self._size_ == nil then
        self._size_ = self:getPixelSize()
    end
    return self._size_
end
-- getPixelSize().width的替代，带缓存
function ProxyMeta:W()
    if self._width_ == nil then
        self._width_ = self:Size().width
    end
    return self._width_
end
-- getPixelSize().height的替代，带缓存
function ProxyMeta:H()
    if self._height_ == nil then
        self._height_ = self:Size().height
    end
    return self._height_
end
-- 加子窗口，name是ui路径，实例销毁会自动销毁
function ProxyMeta:AddChild(name)
    if self.__ChildDlgs_NameMapIndex_[name] then return self end
    local index = #self._ChildDlgs_+1
    self.__ChildDlgs_NameMapIndex_[name] = index
    self._ChildDlgs_[index] = require(name)
    return self
end
-- 获得子窗口的实例
function ProxyMeta:GetChild(indexOrName)
    local index = tonumber(indexOrName) or self.__ChildDlgs_NameMapIndex_[indexOrName]
    if not index then return end
    local data = self._ChildDlgs_[index]
    if not data then return end
    local dlgins = data:Ins()
    if not dlgins then self:ShowChild(indexOrName) end
    return data:Ins()        
end
-- 显示子窗口
function ProxyMeta:ShowChild(indexOrName)
    local index = tonumber(indexOrName) or self.__ChildDlgs_NameMapIndex_[indexOrName]
    if not index then return end
    local data = self._ChildDlgs_[index]
    if not data then return end
    local dlgins = data:Ins()
    if not dlgins then
        dlgins = data:GetInstance(self, index)
    end
    dlgins:Show(true)
    return dlgins
end
-- 隐藏子窗口
function ProxyMeta:HideChild(indexOrName)
    local index = tonumber(indexOrName) or self.__ChildDlgs_NameMapIndex_[indexOrName]
    if not index then return end
    local data = self._ChildDlgs_[index]
    if not data then return end
    data:Exist():Show(false)
    return self
end
-- 显示其中一个子窗口，隐藏其余
function ProxyMeta:ShowOne(indexOrName)
    local index = tonumber(indexOrName) or self.__ChildDlgs_NameMapIndex_[indexOrName]
    if not index then return end
    local ins
    for i, dlg in ipairs(self._ChildDlgs_) do
        if i == index then
            ins = self:ShowChild(i)
        else
            self:HideChild(i)
        end
    end
    return ins
end
-- getPosition的替代
function ProxyMeta:GetPos()
    local pos = self:getPosition()
    return {x = pos.x.offset, y = pos.y.offset}
end
function ProxyMeta:Pos()
    local pos = self:GetScreenPos()
    return {x = pos.x, y = pos.y}
end
function ProxyMeta:CPos()
    local pos = self:GetScreenPosOfCenter()
    return {x = pos.x, y = pos.y}
end
function ProxyMeta:TLPos()
    local pos = self:GetTopLeftPosOnParent()
    return {x = pos.x, y = pos.y}
end
function ProxyMeta:Top(isTrue)
    self:setAlwaysOnTop(isTrue == true)
    return self
end
function ProxyMeta:SetX(pos)
    pos = CEGUI.UDim(0, pos)
    self:setXPosition(pos)
    return self
end
function ProxyMeta:SetY(pos)
    pos = CEGUI.UDim(0, pos)
    self:setYPosition(pos)
    return self
end
function ProxyMeta:SetPos(pos, adapt)
    local nowPos = self:GetPos()
    pos.x = pos.x or nowPos.x
    pos.y = pos.y or nowPos.y
    if adapt then
        local screenw = LuaGetGameUIManager():GetLogicWidth()
        local screenh = LuaGetGameUIManager():GetLogicHeight()
        if adapt == true then
            if pos.x + self:W() > screenw then
                pos.x = screenw - self:W()
            end
            if pos.x < 0 then
                pos.x = 0
            end
            if pos.y + self:H() > screenh then
                pos.y = screenh - self:H()
            end
            if pos.y < 0 then
                pos.y = 0
            end
        elseif adapt == 1 then
            if pos.y >screenh/2 then
                pos.y = pos.y - self:H()
            end
            if pos.x > screenw/2 then
                pos.x = pos.x - self:W()
            end
        end
    end
    pos = CEGUI.UVector2(CEGUI.UDim(0, pos.x), CEGUI.UDim(0, pos.y))
    self:setPosition(pos)
    return self
end
function ProxyMeta:SetScale(scale)
    scale = {x = scale.x or 1, y = scale.y or 1, z = 1}
    self._scale_ = scale 
    scale = CEGUI.Vector3(scale.x, scale.y, scale.z)
    self:setScale(scale)
    return self
end
function ProxyMeta:GetScale()
    if self._scale_ == nil then self._scale_ = {x=1,y=1,z=1} end
    return self._scale_
end
function ProxyMeta:GetRoation()
    local r = self:getRotation()
    return {x=r.x, y=r.y, z=r.z}
end
function ProxyMeta:SetRotation(r)
    local now = self:GetRoation()
    local rotatetion = CEGUI.Vector3(r.x or now.x, r.y or now.y, r.z or now.z)
    self:setRotation(rotatetion)
    return self
end
-- 可以创造一系列的ui动画
function ProxyMeta:AnimHelper(setFunc, ...)
    local function startNewAnim(setFunc, start, endp, time, alpha, relative)
        time = time or 1000
        alpha = alpha or 1
        if type(start) == "function" then
            start = start(self)
        end
        if relative then 
            for k, v in pairs(endp) do
                if start[k] then
                    endp[k] = endp[k]+start[k]
                end
            end
        end 
        local PassTime = 0
        local handle
        local function update(delta)
            PassTime = PassTime + delta
            local Now = self._ins_:Lerp(start, endp, 0, time^alpha, PassTime^alpha)
            setFunc(self, Now)
            if handle.TickFunc then
                local function tick()
                    handle.TickFunc(now)
                end
                xpcall(tick, XpcallErrorHandle)
            end
            if PassTime >= time then
                if handle.StopCall then
                    xpcall(handle.StopCall, XpcallErrorHandle)
                end
                handle:Destroy()
            end
        end
        handle = self._ins_:Timer(1, update)
        local baseDestroy = handle.Destroy
        function handle.Destroy()
            baseDestroy()
            table.remove(self._anim_handles_[setFunc], 1)     
            local paramaters = self._anim_handles_[setFunc][1]
            if paramaters and not self._ins_._Has_Destory_ then
                paramaters.handle.timerhandle = startNewAnim(unpack(paramaters, 1, 6))
            end
        end
        -- 结束回调
        function handle.Stop(_, callBack, ...)
            handle.StopCall = MakeCallBack(callBack, ...)
            return handle
        end
        --每帧回调
        function handle.Tick(_, callBack, ...)
            handle.TickFunc = MakeCallBack(callBack, ...)
            return handle
        end
        return handle
    end
    local handle
    self._anim_handles_[setFunc] = self._anim_handles_[setFunc] or {}
    local isEmpty = #self._anim_handles_[setFunc] == 0
    if not isEmpty then
        local parameters = {setFunc, ...}
        table.insert(self._anim_handles_[setFunc], parameters)
        handle = {}
        function handle.Destroy()
            if handle.timerhandle == nil then
                for i, v in ipairs(self._anim_handles_[setFunc]) do
                    if v == parameters then
                        table.remove(self._anim_handles_[setFunc], i)
                    end
                end
            else
                handle.timerhandle.Destroy()
            end
        end
        -- 结束回调
        function handle.Stop(_, callBack, ...)
            handle.StopCall = MakeCallBack(callBack, ...)
            return handle
        end
        --每帧回调
        function handle.Tick(_, callBack, ...)
            handle.TickFunc = MakeCallBack(callBack, ...)
            return handle
        end
        parameters.handle = handle
    else
        handle = startNewAnim(setFunc, ...)
        self._anim_handles_[setFunc][1] = handle
    end
    return handle
end
-- pos是移动到的位置{x=横轴位置，y=纵轴位置}， time是整个过程需要的时间，alpha大于1则加速，等于1则匀速，小于一则减速
function ProxyMeta:MoveTo(pos, time, alpha)
    return self:AnimHelper(self.SetPos, self.Pos, pos, time, alpha) 
end
function ProxyMeta:MoveTo1(param)
    return self:AnimHelper(self.SetPos, self.GetPos, param.value, param.time, param.alpha, param.relative) 
end
function ProxyMeta:ScaleTo(scale, time, alpha)
    return self:AnimHelper(self.SetScale, self.GetScale, scale, time, alpha) 
end
function ProxyMeta:AlphaTo(param)
    return self:AnimHelper(self.setAlpha, self.getAlpha, param.value, param.time, param.alpha, param.relative) 
end
function ProxyMeta:RotateTo(param)
    return self:AnimHelper(self.SetRotation, self.GetRoation, param.value, param.time, param.alpha, param.relative) 
end


local SingletonDialog = require "ui.singletondialog"
local SimpleMeta = {}
SimpleMeta.__index = SimpleMeta
-- 新的继承方式，将类的内存开销降到最低
function SimpleMeta:__call(classname, layoutname)
    -- local TheClass = class(classname, self)
    local TheClass = {}
    function TheClass.New(...)
        local ins = setmetatable({}, TheClass)
        SimpleDlg.Ctor(ins, ...)
        return ins
    end
    TheClass.__index = TheClass
    setmetatable(TheClass, self)
    if layoutname then
        TheClass.SetVisible = self.SetVisible
        TheClass._PRIORITY_LAYOUT_NAME_ = layoutname
        TheClass.GetLayoutFileName = function () return layoutname..".layout" end 
    end
    return TheClass
end
SimpleDlg = class("SimpleDlg", SingletonDialog)
setmetatable(SimpleDlg, SimpleMeta)
SimpleDlg.__index = SimpleDlg
SimpleDlg._Ins_ = {}
SimpleMeta.MakeCallBack = MakeCallBack

-- 可以替代Ctor，在UI窗口创建前调用
-- function SimpleDlg:BeforeInit()
-- end
-- 不用看
function SimpleMeta:BeforeCreate(parentwindow, nameprefix)
    if self.GetLayoutFileName() == "" and self._PRIORITY_LAYOUT_NAME_ then
        self.GetLayoutFileName = function () return self._PRIORITY_LAYOUT_NAME_..".layout" end 
    end
    self._name_prefix       = tostring(self)..(nameprefix or "")
    self._wnd_Mapto_Proxy   = {}
    self._Desotry_Stuff_    = {}
    self._Timers_           = {}
    SimpleDlg._Ins_[self]   = true
    if self.BeforeInit then
        local function f() 
            self:BeforeInit()
        end
        xpcall(f, XpcallErrorHandle)
    end
    if parentwindow and parentwindow._ins_ then
        parentwindow._ins_:AddDestroyStuff(self)
    end
end

function SimpleMeta:AfterCreate(parentwindow, nameprefix)
    if parentwindow then
        self:MainWnd():SetPos({x=0.1, y=0.1})
    end
end

-- 不用看
function SimpleDlg:OnCreate(parentwindow, nameprefix)
    if self._Has_Init_ then return end
    self._Has_Init_ = true
    SimpleMeta.BeforeCreate(self, parentwindow, nameprefix)
    if self.GetLayoutFileName() ~= "" then
        Dialog.OnCreate(self, parentwindow, self._name_prefix)
    end
    SimpleMeta.AfterCreate(self, parentwindow, nameprefix)
    if self.Init then
        local function f() 
            self:Init(parentwindow, self._name_prefix)
        end
        xpcall(f, XpcallErrorHandle)
    end
end

-- 不用看
function SimpleMeta:CleanDestroyStuff()
    self._Has_Destory_ = true
    SimpleDlg._Ins_[self] = nil
    for k, v  in ipairs(self._Desotry_Stuff_) do
        local function f()
            v:Destroy()
        end
        xpcall(f, XpcallErrorHandle)
    end
end

-- 不用看
function SimpleDlg:Destroy(...)
    local ins = self:GetParseInstance()
    if not ins or ins._Has_Destory_ then return end
    if ins.Release then
        ins:Release(...)
    end
    ins:CleanDestroyStuff()
    SingletonDialog.Destroy(ins)
end

-- 不用看
function SimpleMeta.CleanAllIns()
    local cleanList = {} 
    for ins in pairs(SimpleDlg._Ins_) do
        table.insert(cleanList, ins)
    end
    for _, ins in ipairs(cleanList) do
        xpcall(function () ins:Destroy() end, XpcallErrorHandle)
    end
end

-- 不用看
function SimpleMeta:AddDestroyStuff(stuff)
    self._Desotry_Stuff_[#self._Desotry_Stuff_+1] = stuff
    return stuff
end
-- Destroy时调用一下，如果实例销毁时要做什么，就复写这个方法
-- function SimpleDlg:Release( ... )
-- end

-- GetInstance时创建UI窗口后会调用，如果有初始化的逻辑可以写在这里
-- function SimpleDlg:Init( ... )
-- end

function SimpleMeta:NewIns(...)
    self:Exist():Destroy()
    return self:GetInstance(...)
end
function SimpleMeta:MultiIns(...)
    if require "common.priorityprocedure".GetPriorityByName(self._PRIORITY_LAYOUT_NAME_) then
        self._instance = nil
    end
    return self:GetInstance(...)
end
function SimpleMeta:Ins()
    return self._instance
end
--SetVisible替代
function SimpleMeta:Show(isTrue)
    self:SetVisible(isTrue == true)
end

function SimpleMeta:StrTime(leftTime, ...)
    if leftTime < 0 then leftTime = 0 end
    return TIME_UTIL.intToTimeDHMOther(leftTime/1000, ...)
end
-- 可以用itemid来调用
function SimpleMeta:Color(color)
    if type(color) == "string" then
        if not color:find("tl:") then
            return "[colrect='tl:"..color.." tr:"..color.." bl:"..color.." br:"..color.."]"
        else
            return "[colrect='"..color.."]"
        end
    elseif type(color) == "number" then
        if color < 20 then
            return self:Color(self:Cfg("item.citemnamecolor", colour).colorid)
        else
            return self:Color(self:Cfg("item.citemnamecolor", self:Cfg("item.citemattrlua", color).colour).colorid)
        end
    end
end
--基于Timer，但是可以有多参数回调
function SimpleMeta:TimerS(delay, func, ...)
    local proxyCallBack = MakeCallBack(func, ...)
    local handle = self:Timer(delay, proxyCallBack)
    --只回调那么多次
    function handle:Times(times)
        if self.times then
            self.runcount = self.runcount + (times - self.times)
        else
            self.runcount = times
        end
        self.times = times
        return self
    end
    --只运行time那么久，blastCall表示当超时销毁timer时那次是否还回调
    function handle:Time(time, blastCall)
        self._runTime_ = time
        self._lastcall_ = blastCall
        self.endtime = self._runTime_ + GetServerTime()
        return self
    end
    function handle:Stop(callBack, ...)
        self._stopCall = MakeCallBack(callBack, ...)
        return self
    end
    function handle:Fire(...)
        proxyCallBack(...)
        return self
    end
    return handle
end
--与require "common.timermanager".Schedule的调用方式相同，但是不用手动UnSchedule了，如果想手动，则调用返回物的Destroy()方法
--例如 local handle = self:Timer(1000, self.CallBack, self)
--手动停止则handle:Destroy()
--调用handle:Fire()则立马回调一次
--调用Destroy之后，可以调用handle:Start()重启，如果已经重启再调用handle:Start()也是安全的
function SimpleMeta:Timer(delay, func, ins, obj, times)
    if self._Has_Destory_ then return end
    local t = {}
    t.times = times
    local function update(...)
        if not self._Has_Destory_ and t._newTimer_ then
            if t.endtime and t.endtime < GetServerTime() then
                if not t._lastcall_ then
                    t:Destroy(true)
                    return
                else
                    t._lastcall_ = nil
                end
            end
            func(...)
            if t.runcount then
                t.runcount = t.runcount - 1
                if t.runcount <= 0 then
                    t:Destroy(true)
                end
            end
        end
    end
    function t.Start()
        if t._newTimer_ and self._Timers_[t._newTimer_] then return t end
        t.runcount = t.times
        t._newTimer_ = require "common.timermanager".Schedule(delay, update, ins, obj)
        self._Timers_[t._newTimer_] = true
        if self._runTime_ then
            self.endtime = self._runTime_ + GetServerTime()
        end
        return t
    end
    function t.Destroy(_, isStopCall)
        if t._newTimer_ and self._Timers_[t._newTimer_] then 
            require "common.timermanager".UnSchedule(t._newTimer_)
            self._Timers_[t._newTimer_] = nil
            t._newTimer_ = nil
            if isStopCall and not self._Has_Destory_ and t._stopCall then
                t._stopCall()
            end
        end
        return t
    end
    function t.Fire()
        if ins then
            func(ins, 0, obj)
        else
            func(0, obj)
        end
        return t
    end
    function t:Restart()
        self:Destroy()
        self:Start()
        return self
    end
    t.Start()
    return self:AddDestroyStuff(t)
end

local ExistFunctor = {}
function ExistFunctor.__index(t, k)
    local function f(_, ...)
        local ins = rawget(ExistFunctor, "ins")
        if ins then
            return ins[k](ins, ...)
        end
    end
    return f
end
setmetatable(ExistFunctor, ExistFunctor)

-- 元表技巧，调用xxx:Exist():SomeFunc(a,b,c,d)只有在xxx存在实例时才会调用，否则什么也没发生返回nil
--例如require "ui.wing.skin.wingshowdlg":Exist():IsVisible()如果wingshowdlg有实例则会调用它的IsVisible，否则返回了nil
--其实是简化了这个逻辑:
-- local ins = require "ui.wing.skin.wingshowdlg":GetInstanceNoCreate()
-- if ins then 
--    return ins:IsVisible() 
-- end
function SimpleMeta:Exist()
    ExistFunctor.ins = self._instance
    return ExistFunctor
end

local NoExistFunctor = {}
function NoExistFunctor.__index(t, k)
    local function f(_, ...)
        local ins = rawget(NoExistFunctor, "ins")
        if not ins:Ins() then
            ins = ins:GetInstance(unpack(rawget(NoExistFunctor, "param")))
            return ins[k](ins, ...)
        end
    end
    return f
end
setmetatable(NoExistFunctor, NoExistFunctor)

function SimpleMeta:NoExist(...)
    NoExistFunctor.param = {...}
    NoExistFunctor.ins = self
    return NoExistFunctor
end

-- 元表技巧，XXX:Delay(time):SomeFunc(a,b,c,d)实现了
-- 在time时间之后调用XXX:SomeFunc(a,b,c,d)，实例销毁则会自动取消计时不会回调
function SimpleMeta:Delay(time)
    local DelayFunctor = {}
    function DelayFunctor.__index(t, k)
        local function f(_, ...)
            self:Timer(time, MakeCallBack(self[k], self, ...), nil, nil, 1)
            return self
        end
        return f
    end
    setmetatable(DelayFunctor, DelayFunctor)
    return DelayFunctor
end

local OnceFunctor = {}
function OnceFunctor.__index(t, k)
    local delay = t._delay_
    local ins = t._ins_
    local funcall = ins[k]
    local function f(_, ...)
        local timerhandle = rawget(t, funcall)
        if timerhandle then
            timerhandle:Destroy()
        end
        t[funcall] = ins:TimerS(delay, funcall, ins, ...):Times(1)
        return ins 
    end
    t[k] = f
    return f
end

-- 元表技巧，不管期间调用几次XXX:Once(time):SomeFunc(a,b,c,d)，最终过了time只会调用一次XXX:SomeFunc(a,b,c,d)
-- 处于性能考虑，对于某些函数不想频繁触发则可以用这个方法
function SimpleMeta:Once(delay)
    if not self._once_func_ then
        self._once_func_ = setmetatable({}, OnceFunctor)
        self._once_func_._ins_ = self
    end
    self._once_func_._delay_ = delay or 1
    return self._once_func_
end
-- 遍历表获得相应的index和v
function SimpleMeta:Get(t, key, target)
    if key then
        for k,v in pairs(t) do
            if v[key] == target then
                return k, v
            end
        end
    else
        for k,v in pairs(t) do
            if v == target then
                return k, v
            end
        end
    end
end

function SimpleMeta:MGet(t, key, target)
    local result = {}
    if key then
        for k,v in pairs(t) do
            if v[key] == target then
                table.insert(result, {k, v})
            end
        end
    else
        for k,v in pairs(t) do
            if v == target then
                table.insert(result, {k, v})
            end
        end
    end
    return result
end
--一系列方便弹tips的方法，STRING_UTIL.AddMessageTipByID名字太长总是记不住
function SimpleMeta:Tips(msg, ...)
    if type(msg) == "number" then
        STRING_UTIL.AddMessageTipByID(msg, ...)
    elseif type(msg) == "string" then
        STRING_UTIL.AddMessageTipByMsg(msg, ...)
    elseif type(msg) == "function" then
        STRING_UTIL.AddMessageTipByMsg(msg(), ...)
    end
    return self
end
--类似于上一个
function SimpleMeta:Str(msgid, ...)
    if type(msgid) == "string" then
        if msgid:find("if#&") then
            return STRING_UTIL.GetStringFromBook(msgid)
        else
            return STRING_UTIL.get_stringWithParam(msgid, ...)
        end
    elseif msgid < 10000 then
        return STRING_UTIL.GetResString(msgid, ...)
    else
        return STRING_UTIL.get_stringWithParam(STRING_UTIL.GetClientString(msgid), ...)
    end
end

function SimpleMeta:Sound(id)
    AudioMgr.PlaySkillEffectAudio(id)
    return self
end

function SimpleMeta:Split(str, sep)
    return StringBuilder.SplitStringBySeps(str, sep)
end

function SimpleMeta:MousePos()
    local pos = CEGUI.MouseCursor:getSingleton():getPosition()
    return {x = pos.x, y = pos.y}
end

function SimpleMeta:Lamp(min, max, x)
    if x < min then return min end
    if x > max then return max end
    return x
end
-- 简单的线性插值
function SimpleMeta:Lerp(from, to, min, max, x)
    local result
    x = self:Lamp(min, max, x)
    if type(from) == "table" then
        result = {}
        for k, v in pairs(from) do
            if to[k] == nil then 
                result[k] = from[k] 
            else
                result[k] = from[k] + (to[k]-from[k])*(x-min)/(max-min)
            end
        end
    else
        result = from + (to-from)*x/(max-min)
    end
    return result
end
-- 就是GlobalEvent.Subscribe，但是不用手动释放了
function SimpleMeta:On(...)
    if self._Has_Destory_ then return end
    return self:AddDestroyStuff(GlobalEvent.On(...))
end

function SimpleMeta:OnFire(event, f, ...)
    return self:On(event, f, ...):Fire()
end

function SimpleMeta:Fire( ... )
    GlobalEvent.Fire(...)
    return self
end

function SimpleMeta:Data( ... )
    return GlobalEvent.Data(...)
end

function SimpleMeta:Clear( ... )
    GlobalEvent.Clear(...)
    return self
end
-- TABLE_UTIL的简单写法,省去写knight.gsp.这几个字
function SimpleMeta:Cfg(name, id)
    if name:find("%.") == nil then
        name = "knight.gsp.game."..name
    elseif name:find("knight%.gsp") == nil then
        name = "knight.gsp."..name
    end
    if id == nil then 
        return TABLE_UTIL.getBeanALLRecord(name)
    else 
        return TABLE_UTIL.getLuaBean(name, id)
    end
end

function SimpleMeta:TodaySecond()
    return math.floor(math.mod(GetServerTime()/1000, 86400) + 3600*8)
end

function SimpleMeta:GetSecond(strTime)
    local iScale = 3600
    local iTotalSecond = 0
    for w in string.gmatch(strTime, "%d+") do
        iTotalSecond = iTotalSecond + tonumber(w)*iScale 
        iScale = iScale/60
    end
    return iTotalSecond
end
--协议发送的简单写法,不用再写protocoldef.knight.gsp这个几个字了
function SimpleMeta:SendAct(name, values)
    local sendAct = require ("protocoldef.knight.gsp."..name).Create()
    if values then
        for k, v in pairs(values) do
            sendAct[k] = v
        end
    end
    local meta = {}
    meta.__index = sendAct
    local t = setmetatable({}, meta)
    t.__sendAct__ = sendAct
    -- 协议的发送，省去写LuaProtocolManager.GetInstance()这几个字了
    function t.Send()
        self:Send(sendAct)
        return t
    end
    meta.__newindex = sendAct
    return t
end

function SimpleMeta:Send(protocol)
    protocol = protocol.__sendAct__ or protocol
    LuaProtocolManager.GetInstance():Send(protocol) 
end

function SimpleMeta:MsgBox(func, msgid, ...)
    MsgBoxMgr.PopBusyStyleMsg(msgid, nil, nil, func, ...)
end

function SimpleMeta:GetValue(key)
    local str = STRING_UTIL.GetValue(key)
    return tonumber(str) or str
end
--获得根窗口
function SimpleMeta:MainWnd()
    if self._mainwnd_ == nil then
        self._mainwnd_ = self:ProxyWnd(self.m_pMainFrame)
    end
    return self._mainwnd_
end
--写文件
function SimpleMeta:SetIni(section, p, v)
    FileConfigManager.WriteValue(LoginMgr.ACCOUNT_INI_NAME, tostring(section)..LoginMgr.g_nRoleID, tostring(p), tostring(v))
    return self
end
--读文件
function SimpleMeta:GetIni(section, p)
    return FileConfigManager.GetValue(LoginMgr.ACCOUNT_INI_NAME, tostring(section)..LoginMgr.g_nRoleID, tostring(p))
end

--红点的数据结构，给红点提供方便逻辑支持
function SimpleMeta:RedData(EventName, initTable)
    local RedStructMeta = {}
    RedStructMeta._parent_ = {}
    RedStructMeta._EventName = EventName
    function RedStructMeta.__index(t, k)
        return RedStructMeta[k] or false
    end
    function RedStructMeta.__newindex(t, k, v)
        if RedStructMeta[k] == v then
            t:_hasChange(false)
        else
            RedStructMeta[k] = v
            if type(v) == "table" then
                table.insert(v._parent_, t)
            end
            t:_hasChange(true)
        end
    end
    function RedStructMeta:_hasChange(changed)
        if changed then
            RedStructMeta._cache_ = nil
            if not RedStructMeta._timerhandle then
                RedStructMeta._timerhandle = require "common.timermanager".Schedule(1, RedStructMeta._TimerFire, self, nil, 1)
            end
        end
        RedStructMeta._changed_ = changed
        for _, parent in ipairs(RedStructMeta._parent_) do
            parent:_hasChange(changed)
        end
    end

    function RedStructMeta:_TimerFire()
        RedStructMeta._timerhandle = nil
        GlobalEvent.Fire(self._EventName, self)
    end
    -- api start
    function RedStructMeta:Change()
        return RedStructMeta._changed_ or false
    end

    -- 判断表里是否至少有一个值为true
    function RedStructMeta:True()
        if RedStructMeta._cache_ then return RedStructMeta._cache_ end
        for k, v in pairs(RedStructMeta) do
            if (type(k) ~= "string" or k:sub(1,1) ~= "_") then 
                if v == true then
                    RedStructMeta._cache_ = true
                    return true
                end
                if type(v) == "table" and v.True() then
                    RedStructMeta._cache_ = true
                    return true
                end
            end
        end
        RedStructMeta._cache_ = false
        return false
    end
    function RedStructMeta:Fire()
        if RedStructMeta._timerhandle then
            require "common.timermanager".UnSchedule(RedStructMeta._timerhandle)
            RedStructMeta._timerhandle = nil
        end
        GlobalEvent.Fire(RedStructMeta._EventName, self)
    end

    -- 得到原始数据
    function RedStructMeta:Raw()
        local result = {}
        for k, v in pairs(RedStructMeta) do
            if (type(k) ~= "string" or k:sub(1,1) ~= "_") and type(v) ~= "function" then
                if type(v) == "table" then
                    result[k] = v.GetData()
                else
                    result[k] = v
                end
            end
        end
        return result
    end
    -- api end
    local proxy = setmetatable({}, RedStructMeta)
    if initTable then
        for k, v in pairs(initTable) do
            RedStructMeta[k] = true      
        end
        proxy:Fire()
    end
    return proxy
end

function SimpleMeta:Unmarshal(path, data)
    --do return data end
    if type(data) == "table" then
        local DataBean
        if path:find("%.") == nil then
            DataBean = {}
            function DataBean:New() return self end
            function DataBean:Unmarshal(_os_)
                self.data = ""
                self.data = _os_["unmarshal_"..path](_os_, self.data)
            end
        else
            DataBean = require ("protocoldef.rpcgen.knight.gsp."..path)
        end
        local result = {}
        for k, v in pairs(data) do
            local rankdata = DataBean:New()
            local _os_ = GNET.Marshal.OctetsStream:new(v)
            rankdata:Unmarshal(_os_)
            _os_:delete()
            result[k] = rankdata
        end
        return result
    else
        return self:Unmarshal(path, {data})[1]
    end
end

local winMgr = LuaGetWindowManager()
function SimpleMeta:ProxyWnd(name, cast)
    local wnd 
    if type(name) == "string" then
        wnd = winMgr:getWindow(self._name_prefix..name)
    else
        wnd = name
    end
    if cast then wnd = cast(wnd) end
    if self._wnd_Mapto_Proxy[wnd] then
        return self._wnd_Mapto_Proxy[wnd]
    end
    local t = setmetatable({}, ProxyMeta)
    tolua.inherit(t, wnd)
    t._parent_ = ProxyMeta
    t._wnd_ = wnd
    t._ChildDlgs_ = {} 
    t.__ChildDlgs_NameMapIndex_ = {}
    t._anim_handles_ = {}
    t._ins_ = self
    self._wnd_Mapto_Proxy[wnd] = t
    return t
end
--获得一个winMgr:getWindow类的窗口
function SimpleMeta:Wnd(name)
    return self:ProxyWnd(name)
end

--获得一个winMgr:getWindow类的窗口
function SimpleMeta:Txt(name)
    local Txt = self:ProxyWnd(name)
    function Txt:Color(colorstr)
        self:setProperty("TextColours", colorstr)
        return self
    end
    function Txt:Border(colorstr)
        self:setProperty("BorderColour", colorstr)
        return self
    end
    return Txt
end

--获得一个CEGUI.toPushButton类的窗口
function SimpleMeta:Btn(name)
    local Btn = self:ProxyWnd(name, CEGUI.toPushButton)
    Btn:EnableClickAni(true)
    function Btn:Img(imgstr)
        local isTrue, imgfromnum = self._parent_:GetImgByNum(imgstr)
        if isTrue then imgstr = imgfromnum end
        self:setProperty("HoverImage", imgstr)
        self:setProperty("NormalImage", imgstr)
        self:setProperty("PushedImage", imgstr)
        self:setProperty("DisabledImage", imgstr)
        return self
    end
    return Btn
end

function SimpleMeta:Img(name)
    return self:ProxyWnd(name)
end

--获得一个CEGUI.toGroupButton类的窗口
function SimpleMeta:Btns(name)
    local Btns = self:ProxyWnd(name, CEGUI.toGroupButton)
    Btns:AutoLayout("_btns_")
    function Btns:Img(imgstr)
        local isTrue, imgfromnum = self._parent_:GetImgByNum(imgstr)
        if isTrue then imgstr = imgfromnum end
        self:setProperty("HoverImage", imgstr)
        self:setProperty("NormalImage", imgstr)
        self:setProperty("PushedImage", imgstr)
        self:setProperty("DisabledImage", imgstr)
        return self
    end
    function Btns:Set(isTrue)
        self:setSelected(isTrue == true)
        return self
    end
    function Btns:Is()
        return self:isSelected()
    end
    return Btns
end

--获得一个CEGUI.toScrollablePane类的窗口，主要复写了AddCells方法，带动态加载能力
function SimpleMeta:Pane(name)
    local ins = self
    local Pane = self:ProxyWnd(name, CEGUI.toScrollablePane)
    Pane._isHorizontal_ = false
    -- 调用方式与普通窗口的AddCells一样
    function Pane:AddCells(cellName, data, CallbackName, layout, ...)
        local extrainfo = {...}
        CallbackName = CallbackName or "SetInfo"
        if self._Cells_ then 
            return self:RefreshCells(data, ...) 
        end
        if #data == 0 then
            return
        end
        local pos, numrow, step, bar
        local cells = {}
        self._Cells_ = cells
        self._Data_ = data
        self._Data_Len_ = #data
        -- 获得所有的cell
        function Pane:GetCells()
            return self._Cells_
        end
        bar = self:GetBar()
        local newCell = require(cellName):GetInstance(self, 1) 
        local newCellWnd = newCell:MainWnd()
        layout = layout or {marginx=1,marginy=1,intervalx=1,intervaly=1}
        pos, numrow = UI_UTIL.Layout(self, newCellWnd, layout)
        newCell:GetMainFrame():setPosition(pos[1])
        step = math.ceil(self:H() / newCellWnd:H())*numrow
        newCell[CallbackName](newCell, self._Data_[1], {1, self, ins}, ...)
        cells[1] = newCell
        if self._HitBottom_callBack_list_ then 
            self:HitBottomCall() 
        end
        local function Add(starIndex, endIndex, ...)  
            for cellIndex = starIndex, endIndex do
                local newCell = require(cellName):GetInstance(self, cellIndex) 
                newCell:GetMainFrame():setPosition(pos[cellIndex])
                newCell[CallbackName](newCell, self._Data_[cellIndex], {cellIndex, self, ins}, ...)
                cells[cellIndex] = newCell
            end
            if self._HitBottom_callBack_list_ and endIndex >= starIndex then 
                self:HitBottomCall() 
            end
        end
        local function nextPage(firstTime, ...)
            local starIndex = #cells+1
            local endIndex = math.min(#cells+step, #self._Data_)
            local barPos = bar:GetPos()
            bar:Stop()
            if firstTime and endIndex < #self._Data_ then
                endIndex = endIndex - 1
            end
            Add(starIndex, endIndex, ...)
            bar:SetPos(barPos)
        end
        self:NextPage(nextPage, false, ...)
        nextPage(true, ...)
        function Pane:RefreshCells(newData, ...)
            local bNeedNextPage = false
            if #newData > self._Data_Len_ then bNeedNextPage = true end
            self._Data_ = newData
            self._Data_Len_ = #newData
            local data = newData
            for i, v in ipairs(data) do
                if cells[i] then
                    cells[i]:SetVisible(true)
                    cells[i][CallbackName](cells[i], data[i], {i, self, ins}, ...)
                else
                    if bNeedNextPage then
                        nextPage(false, ...)
                    end
                    return self
                end
            end
            for i = #data+1, #cells do
                cells[i]:SetVisible(false)
            end
            return self
        end
        --把所有cell都加载了
        function Pane:FlushCells(index)
            if index == nil then index = #self._Data_ end
            index = math.min(#self._Data_, index)
            Add(#cells+1, index, unpack(extrainfo))
            return self
        end
        --重新布局
        function Pane:Layout(layout)
            if self._Cells_ then 
                local cellWnd = self._Cells_[1]:MainWnd()
                pos, numrow = UI_UTIL.Layout(self, cellWnd, layout)
                step = math.ceil(self:H() / cellWnd:H())*numrow
                for i, v in ipairs(self._Cells_) do
                    v:GetMainFrame():setPosition(pos[i])
                end
            end
            return self
        end
        -- 把pane移到第i个cell的位置
        function Pane:Focus(i)
            if not self._Cells_ then return end
            i = math.min(i, self._Data_Len_)
            while #self._Cells_ < i do
                nextPage(false, unpack(extrainfo))
            end
            local barpos = math.max(0, pos[i].y.offset - (self:H() - self._Cells_[1]:MainWnd():H())*0.5)
            self:GetBar():SetPos(barpos)
            return self
        end
        return self
    end
    --设成水平滑动的
    function Pane:SetH(isTrue)
        self._isHorizontal_ = isTrue
        if isTrue then
            self:EnableHorzScrollBar(true)
        else
            self:EnableVertScrollBar(true)
        end
        return self
    end
    --获得滑动的Scrollbar
    function Pane:GetBar()
        if not self._bar_ then
            local bar
            if self._isHorizontal_ then
                bar = self:getHorzScrollbar()
            else
                bar = self:getVertScrollbar()
            end
            local bart = ins:ProxyWnd(bar)
            function bart:GetPos()
                return self:getScrollPosition()
            end
            function bart:SetPos(pos)
                return self:setScrollPosition(pos)
            end
            self._bar_ = bart
        end
        return self._bar_
    end
    --使之能够一页一页的滑动
    function Pane:Slide(CallBack, ...)
        if CallBack then
            CallBack = MakeCallBack(CallBack, ...)
        end
        self:FlushCells()
        self._page_index_ = 1
        local cell = self._Cells_[1]
        if self._isHorizontal_ then
            self._page_length_ = cell:MainWnd():W()
        else
            self._page_length_ = cell:MainWnd():H()
        end
        local bar = self:GetBar()
        local function ScrollChanged()
            if GlobalEvent.Data("Touch") then
                local nowClock = os.clock()
                if self._last_set_time == nil or nowClock - self._last_set_time > 0.5 then
                    local ChangeOffset = 50
                    local barPos = bar:GetPos()
                    if barPos < 0 or barPos > (#self._Cells_-1) * self._page_length_ then return end
                    if barPos < (self._page_index_-1) * self._page_length_ - ChangeOffset then
                        self._page_index_ = self._page_index_ - 1
                        if CallBack then
                            CallBack(self._page_index_)
                        end
                        self._last_set_time = nowClock
                        GlobalEvent.Fire("Touch", false)
                    elseif barPos > (self._page_index_-1) * self._page_length_ + ChangeOffset then
                        self._page_index_ = self._page_index_ + 1
                        if CallBack then
                            CallBack(self._page_index_)
                        end
                        self._last_set_time = nowClock
                        GlobalEvent.Fire("Touch", false)
                    end
                end
            end
        end
        bar:ScrollPosChanged(ScrollChanged)
        local function tick(delta)
            if not GlobalEvent.Data("Touch") then
                bar:Stop()
                local barPos = bar:GetPos()
                local destinatePos = (self._page_index_ - 1) * self._page_length_
                local offset = delta*1.5
                if barPos < destinatePos then
                    barPos = barPos + offset
                    if barPos > destinatePos then
                        barPos = destinatePos
                    end
                elseif barPos > destinatePos then
                    barPos = barPos - offset
                    if barPos < destinatePos then
                        barPos = destinatePos
                    end
                end
                bar:SetPos(barPos)
            end
        end
        -- 滑到第page页
        function Pane:SetPage(page)
            if page <= 0 or page > #self._Cells_ then return self end
            self._page_index_ = page
            if CallBack then CallBack(self._page_index_) end 
            return self
        end
        --滑到上一页
        function Pane:NextPage()
            self.SetPage(self._page_index_ + 1)
        end
        --滑到下一页
        function Pane:LastPage()
            self.SetPage(self._page_index_ - 1)
        end
        ins:Timer(1, tick)
        if CallBack then CallBack(self._page_index_) end
        return self
    end
    local baseOn = Pane.On
    function Pane:On(...)
        baseOn(self, ...)
        local contentPane = ins:ProxyWnd(self:getScrolledContainer())
        contentPane:On(...)
        return self
    end
    --是否触底
    function Pane:IsHitBottom()
        if not self._Cells_ then
            return true
        elseif #self._Cells_ < #self._Data_ then
            return false
        elseif self:GetBar():getDocumentSize()-self:GetBar():GetPos()<self:H() then
            return true
        else
            return false
        end
    end
    --触底回调true，否则回到false，只要状态改变才回调
    function Pane:HitBottom(callBack, ...)
        if self._HitBottom_callBack_list_ == nil then
            self._HitBottom_callBack_list_ = {}    
            function Pane:HitBottomCall(...)
                if self._ins_._Has_Destory_ then return end
                local state = self:IsHitBottom()
                if self._last_Bottom_state ~= state then
                    self._last_Bottom_state = state
                    for k, v in pairs(self._HitBottom_callBack_list_) do
                        v(state, ...)
                    end
                end
            end
            self:GetBar():ScrollPosChanged(self.HitBottomCall, self)
        end
        callBack = MakeCallBack(callBack, ...)
        self._HitBottom_callBack_list_[#self._HitBottom_callBack_list_] = callBack
        self._last_Bottom_state = self:IsHitBottom()
        callBack(self._last_Bottom_state)
        return self
    end
    return Pane
end
-- 获得CEGUI.toItemCell类窗口
function SimpleMeta:Item(name)
    local Item = self:ProxyWnd(name, CEGUI.toItemCell)
    Item._isPop_ = true
    -- 设置cell的信息
    function Item:SetItem(id, num, nameWnd, ...)
        if id == nil then
            self:Show(false)
            return
        end
        num = num or 1
        if self._id_ ~= id*num then
            self._id_ = id*num
            self:Show(true)
            nameWnd = nameWnd or self._Item_Namewnd_
            if nameWnd then self._Item_Namewnd_ = nameWnd end
            ITEM_UTIL.SetItemCellInfo(self, id, num, nameWnd, ...)
            if self._isPop_ then
                ITEM_UTIL.BindItemCellEvent(self)
            end
        end
        return self
    end
    -- 绑定Name窗口
    function Item:Name(nameWnd)
        self._Item_Namewnd_ = nameWnd
        return self
    end
    -- 去掉长按弹出事件
    function Item:Nopop(isTrue)
        self._isPop_ = isTrue == false
        return self
    end
    function Item:Img(imgstr)
        local isTrue, imgfromnum = self._parent_:GetImgByNum(imgstr)
        if isTrue then imgstr = imgfromnum end
        self:SetImage(CEGUI.PropertyHelper:stringToImage(imgstr))
        return self
    end
    function Item:Color(imgstr)
        local isTrue, imgfromnum = self._parent_:GetImgByNum(imgstr)
        if isTrue then imgstr = imgfromnum end
        self:SetColorImage(CEGUI.PropertyHelper:stringToImage(imgstr))
        return self
    end
    return Item
end
-- 获得CEGUI.toRichEditbox窗口
function SimpleMeta:RTxt(name)
    local ins = self
    local RTxt = self:ProxyWnd(name, CEGUI.toRichEditbox)
    -- 清除并设置文字
    function RTxt:Txt(strtext, ...)
        UI_UTIL.SetRichEditBoxText(strtext, self, ...)
        return self
    end
    -- 加上文字并换行
    function RTxt:Txts(str, color)
        UI_UTIL.SetRichEditBoxText(str, self, color, false)
        self:AppendBreak()
        return self
    end

    function RTxt:Str(id, ...)
        return self:Txt(ins:Str(id, ...))
    end
    function RTxt:Strs(id, ...)
        return self:Txts(ins:Str(id, ...))
    end
    --添加一个link，返回该link，该link具备Wnd类的所有功能
    function RTxt:Link(name, color)
        local linkComponent 
        if color then
            color = COLOR_UTIL.StringToColour(color)
            linkComponent = self:AppendNameLink(1, name, 1, 1, 1, color) 
            linkComponent:SetNameColorType(2)
        else
            linkComponent = self:AppendNameLink(1, name, 1, 1, 1) 
        end
        self:Refresh()
        return ins:ProxyWnd(linkComponent)
    end
    function RTxt:Break()
        self:AppendBreak()
        return self
    end
    function RTxt:IsHitBottom()
        local fScrollBarPos = self:getVertScrollbar():getScrollPosition()
        local fTextHeight = self:GetTextHeight()
        return fTextHeight - fScrollBarPos < self:H()
    end
    function RTxt:HitBottom(callBack, ...)
        if self._callbacks_ == nil then
            function RTxt:srollchange()
                if self._hitbottom_state_ ~= self:IsHitBottom() then
                    self._hitbottom_state_ = not self._hitbottom_state_
                    self._callbacks_(self._hitbottom_state_)
                end
            end
            self:getVertScrollbar():subscribeEvent("ScrollPosChanged", self.srollchange, self)
        end
        self._callbacks_ = MakeCallBack(callBack, ...)
        self._hitbottom_state_ = self:IsHitBottom()
        self._callbacks_(self._hitbottom_state_)
    end
    return RTxt
end
-- 获得CEGUI.Window.toProgressBar
function SimpleMeta:Bar(name)
    local Bar = self:ProxyWnd(name, CEGUI.Window.toProgressBar)
    function Bar:Set(progress, ...)
        self:setProgress(progress)
        return self
    end
    function Bar:SetTo(param)
        return self:AnimHelper(Bar.Set,  Bar.getProgress, param.value, param.time, param.alpha, param.relative)
    end
    return Bar
end
-- todo
function SimpleMeta:Rank(name)
    local Rank = self:ProxyWnd(name, CEGUI.toMultiColumnList)
    Rank._rows_ = {}
    function Rank:AddData()
    end
    return Rank
end

function SimpleMeta:Cho(name)
    local Cho = self:ProxyWnd(name, CEGUI.Window.toCheckbox)
    function Cho:Check(isTrue)
        self:setSelected(isTrue==true)
        return self
    end
    return Cho
end

return SimpleDlg