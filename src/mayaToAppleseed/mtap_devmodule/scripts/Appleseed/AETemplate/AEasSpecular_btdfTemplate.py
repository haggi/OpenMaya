import pymel.core as pm
import logging

log = logging.getLogger("ui")

class BaseTemplate(pm.ui.AETemplate):
    
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
        
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)
        

class AEasSpecular_btdfTemplate(BaseTemplate):
    def __init__(self, nodeName):
        BaseTemplate.__init__(self,nodeName)
        log.debug("AEasSpecular_btdfTemplate")
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
        self.addControl("transmittance", label="Transmittance")
        self.addControl("reflectance", label="Reflectance")
        self.addControl("from_ior", label="From Index of Refraction")
        self.addControl("to_ior", label="To Index of Refraction")
        self.addControl("transmittance_multiplier", label="Transmittance Multiplier")
        self.addControl("reflectance_multiplier", label="Reflectance Multiplier")
        self.addSeparator()
        #autoAddEnd
        self.endNoOptimize()
        self.endLayout()
        