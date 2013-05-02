import pymel.core as pm
import logging

log = logging.getLogger("ui")

class BaseTemplate(pm.ui.AETemplate):
    
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
        
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)
        

class AEluxRoughglassTemplate(BaseTemplate):
    def __init__(self, nodeName):
        BaseTemplate.__init__(self,nodeName)
        log.debug("AEluxRoughglassTemplate")
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
        self.addControl("roughglass_Kr", label="Reflectivity")
        self.addControl("roughglass_dispersion", label="Accurate Dispersion")
        self.addControl("roughglass_Kt", label="Transmittion Fraction")
        self.addControl("roughglass_uroughness", label="U Roughness")
        self.addControl("roughglass_cauchyb", label="Cauchy B Coefficient")
        self.addControl("roughglass_index", label="Refraction Index")
        self.addControl("roughglass_vroughness", label="V Roughness")
        #autoAddEnd
        self.endLayout()