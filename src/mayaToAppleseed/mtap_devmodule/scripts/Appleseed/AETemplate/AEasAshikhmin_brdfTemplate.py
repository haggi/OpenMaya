import pymel.core as pm
import logging

log = logging.getLogger("ui")

class BaseTemplate(pm.ui.AETemplate):
    
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
        
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)
        

class AEasAshikhmin_brdfTemplate(BaseTemplate):
    def __init__(self, nodeName):
        BaseTemplate.__init__(self,nodeName)
        log.debug("AEasAshikhmin_brdfTemplate")
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
        self.addControl("glossy_reflectance", label="Glossy Reflectance")
        self.addControl("shininess_v", label="Shininess V")
        self.addControl("shininess_u", label="Shininess U")
        self.addControl("glossy_reflectance_multiplier", label="Glossy Reflectance Multiplier")
        self.addControl("diffuse_reflectance_multiplier", label="Diffuse Reflectance Multiplier")
        self.addControl("fresnel_multiplier", label="Fresnel Multiplier")
        self.addControl("diffuse_reflectance", label="Diffuse Reflectance")
        self.addSeparator()
        #autoAddEnd
        self.endNoOptimize()
        self.endLayout()
        