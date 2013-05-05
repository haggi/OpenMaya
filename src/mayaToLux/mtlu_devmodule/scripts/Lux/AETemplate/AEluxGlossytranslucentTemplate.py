import pymel.core as pm
import logging

log = logging.getLogger("ui")

class BaseTemplate(pm.ui.AETemplate):
    
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
        
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)
        

class AEluxGlossytranslucentTemplate(BaseTemplate):
    def __init__(self, nodeName):
        BaseTemplate.__init__(self,nodeName)
        log.debug("AEluxGlossytranslucentTemplate")
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
        self.beginNoOptimize()
        #autoAddBegin
        self.addControl("index", label="ReflectionIndex")
        self.addControl("Ka", label="Absorption")
        self.addControl("onesided", label="Disable backface Coating")
        self.addControl("d", label="Coating Thickness")
        self.addControl("Kd", label="Diffuse Reflection")
        self.addControl("backface_d", label="Backface	Coating Thickness")
        self.addControl("vroughness", label="V Roughness")
        self.addControl("uroughness", label="U Roughness")
        self.addControl("backface_Ka", label="Backface Absorption")
        self.addControl("backface_uroughness", label="Backface U Roughness")
        self.addControl("Ks", label="Specular Reflection")
        self.addControl("backface_index", label="Backface ReflectionIndex")
        self.addControl("Kt", label="Transmission")
        self.addControl("backface_Ks", label="Backface Specular Reflection")
        self.addControl("backface_vroughness", label="Backface V Roughness")
        self.addSeparator()
        self.addControl("compo_visible_indirect_material", label="compo_visible_indirect_material")
        self.addControl("compo_override_alpha_value", label="compo_override_alpha_value")
        self.addControl("compo_visible_emission", label="compo_visible_emission")
        self.addControl("compo_override_alpha", label="compo_override_alpha")
        self.addControl("compo_visible_material", label="Compo Visible Material")
        self.addControl("compo_visible_indirect_emission", label="compo_visible_indirect_emission")
        self.addControl("bumpmap", label="Bump Map")
        #autoAddEnd
        self.endNoOptimize()
        self.endLayout()
        