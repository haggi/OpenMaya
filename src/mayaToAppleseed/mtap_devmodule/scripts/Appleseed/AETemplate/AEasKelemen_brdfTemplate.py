import pymel.core as pm
import logging

log = logging.getLogger("ui")

class BaseTemplate(pm.ui.AETemplate):
    
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
        
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)
        

class AEasKelemen_brdfTemplate(BaseTemplate):
    def __init__(self, nodeName):
        BaseTemplate.__init__(self,nodeName)
        log.debug("AEasKelemen_brdfTemplate")
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
        self.addControl("matte_reflectance_multiplier", label="Matte Reflectance Multiplier")
        self.addControl("specular_reflectance_multiplier", label="Specular Reflectance Multiplier")
        self.addControl("specular_reflectance", label="Specular Reflectance")
        self.addControl("matte_reflectance", label="Matte Reflectance")
        self.addControl("roughness", label="Roughness")
        self.addSeparator()
        #autoAddEnd
        self.endNoOptimize()
        self.endLayout()
        