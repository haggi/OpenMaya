import pymel.core as pm
import logging

log = logging.getLogger("ui")

class BaseTemplate(pm.ui.AETemplate):
    
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
        
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)
        

class AEluxCarpaintTemplate(BaseTemplate):
    def __init__(self, nodeName):
        BaseTemplate.__init__(self,nodeName)
        log.debug("AEluxCarpaintTemplate")
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
        self.addControl("carpaint_M3", label="Microfacet roughness 3")
        self.addControl("carpaint_Ks1", label="Spec Component 1")
        self.addControl("carpaint_Ks3", label="Spec Component 3")
        self.addControl("carpaint_Ks2", label="Spec Component 2")
        self.addControl("carpaint_name", label="Carpaint Model")
        self.addControl("carpaint_M1", label="Microfacet roughness 3")
        self.addControl("carpaint_R1", label="Fresnel constant1")
        self.addControl("carpaint_R2", label="Fresnel constant2")
        self.addControl("carpaint_R3", label="Fresnel constant3")
        self.addControl("carpaint_M2", label="Microfacet roughness 3")
        self.addControl("carpaint_Kd", label="Diffuse component")
        #autoAddEnd
        self.endLayout()