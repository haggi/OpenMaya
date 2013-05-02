import pymel.core as pm
import logging

log = logging.getLogger("ui")

class BaseTemplate(pm.ui.AETemplate):
    
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
        
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)
        

class AEluxGlassTemplate(BaseTemplate):
    def __init__(self, nodeName):
        BaseTemplate.__init__(self,nodeName)
        log.debug("AEluxGlassTemplate")
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
        self.addControl("glass_filmindex", label="Film Refraction Index")
        self.addControl("glass_architectural", label="Transmission Refraction Off")
        self.addControl("glass_film", label="Film Thickness")
        self.addControl("glass_cauchyb", label="Cauchy B Coefficient")
        self.addControl("glass_Kt", label="Transmission")
        self.addControl("glass_Kr", label="Reflectivity")
        self.addControl("glass_index", label="Refraction Index")
        #autoAddEnd
        self.endLayout()