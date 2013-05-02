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
        #autoAddBegin
        self.addControl("compo_visible_indirect_material", label="compo_visible_indirect_material")
        self.addControl("compo_override_alpha_value", label="compo_override_alpha_value")
        self.addControl("compo_visible_emission", label="compo_visible_emission")
        self.addControl("compo_override_alpha", label="compo_override_alpha")
        self.addControl("compo_visible_material", label="Compo Visible Material")
        self.addControl("compo_visible_indirect_emission", label="compo_visible_indirect_emission")
        self.addControl("bumpmap", label="Bump Map")
        self.addControl("glossytranslucent_backface_index", label="Backface ReflectionIndex")
        self.addControl("glossytranslucent_backface_Ka", label="Backface Absorption")
        self.addControl("glossytranslucent_Kt", label="Transmission")
        self.addControl("glossytranslucent_Ks", label="Specular Reflection")
        self.addControl("glossytranslucent_onesided", label="Disable backface Coating")
        self.addControl("glossytranslucent_backface_vroughness", label="Backface V Roughness")
        self.addControl("glossytranslucent_d", label="Coating Thickness")
        self.addControl("glossytranslucent_Ka", label="Absorption")
        self.addControl("glossytranslucent_backface_d", label="Backface	Coating Thickness")
        self.addControl("glossytranslucent_Kd", label="Diffuse Reflection")
        self.addControl("glossytranslucent_uroughness", label="U Roughness")
        self.addControl("glossytranslucent_backface_Ks", label="Backface Specular Reflection")
        self.addControl("glossytranslucent_index", label="ReflectionIndex")
        self.addControl("glossytranslucent_vroughness", label="V Roughness")
        self.addControl("glossytranslucent_backface_uroughness", label="Backface U Roughness")
        #autoAddEnd
        self.endLayout()