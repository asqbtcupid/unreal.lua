local SimpleDlg = require "simpledlg"
local testUmg = Inherit(SimpleDlg, UUserWidget)
testUmg:DynamicLoad("test")
function testUmg:Ctor(controller)
	-- self:Wnd("btn_clear"):Event("OnClicked", controller.ClearAllCharacter, controller)
	self:Wnd("play"):Event("OnClicked", self.PlayAnim, self)
	self.controller = controller
	-- self.Anim = UAnimMontage.FObjectFinder("/Game/Mannequin/Animations/NewAnimMontage")
	-- local anim = UAnimMontage.LoadObject(self, "/Game/Mannequin/Animations/NewAnimMontage")
	-- A_(self.Anim)Material'/Game/Git/mt_fog.mt_fog'
	local MaterialFather = UMaterial.LoadObject(self, "/Game/Git/mt_fog.mt_fog")
	self.MID = UKismetMaterialLibrary.CreateDynamicMaterialInstance(self, MaterialFather)

	self.MID:SetTextureParameterValue("tx_fog", self.controller.m_FogMgr.Tx_Fog)
	-- A_(self.MID:K2_GetTextureParameterValue("tx_fog"))
	-- A_(self.controller.m_FogMgr.Tx_Fog)
	-- self.m_TestMesh:SetMaterial(0, MID)
	-- A_(MID)
end
function testUmg:PlayAnim()
	A_("lalala")
	-- local AnimIns = self.controller:GetAnimIns()
	-- local anim = UAnimMontage.LoadObject(self, "/Game/Mannequin/Animations/NewAnimMontage")
	-- AnimIns:Montage_Play(anim, 0.5)
end

function testUmg:Txt1(content)
	self:Wnd("txt1"):SetText(tostring(content))
end

function testUmg:Txt2(content)
	self:Wnd("txt2"):SetText(tostring(content))
	-- self:Wnd("img_fog"):SetBrushFromMaterial(self.MID)
	self:Wnd("img_fog"):SetBrushFromTexture(self.controller.m_FogMgr.Tx_Fog)
end

function testUmg:Txt3(content)
	self:Wnd("txt3"):SetText(tostring(content))
end

return testUmg
