import pymel.core as pm
import logging

log = logging.getLogger("ui")

class BaseTemplate(pm.ui.AETemplate):
    
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
        
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)
        

class AEluxClothTemplate(BaseTemplate):
    def __init__(self, nodeName):
        BaseTemplate.__init__(self,nodeName)
        log.debug("AEluxClothTemplate")
        self.thisNode = None
        self.node = pm.PyNode(self.nodeName)
        pm.mel.AEswatchDisplay(nodeName)
        self.beginScrollLayout()
        self.buildBody(nodeName)
        self.addExtraControls("ExtraControls")
        self.endScrollLayout()
        
    def buildBody(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        self.beginLayout("ShaderSettings" ,collapse=0)
        #autoAddBegin
        self.addControl("compo_visible_indirect_material", label="compo_visible_indirect_material")
        self.addControl("compo_override_alpha_value", label="compo_override_alpha_value")
        self.addControl("compo_visible_emission", label="compo_visible_emission")
        self.addControl("compo_override_alpha", label="compo_override_alpha")
        self.addControl("compo_visible_material", label="Compo Visible Material")
        self.addControl("compo_visible_indirect_emission", label="compo_visible_indirect_emission")
        self.addControl("bumpmap", label="Bump Map")
        self.addControl("cloth_repeat_u", label="Number of Lines V")
        self.addControl("cloth_warp_Ks", label="Specular Reflection")
        self.addControl("cloth_warp_Kd", label="Diffuse Reflection")
        self.addControl("cloth_weft_Ks", label="Opposite Specular Reflection")
        self.addControl("cloth_weft_Kd", label="Opposite Diffuse Reflection")
        self.addControl("cloth_presetname", label="Fabric Type")
        #autoAddEnd
        self.endLayout()