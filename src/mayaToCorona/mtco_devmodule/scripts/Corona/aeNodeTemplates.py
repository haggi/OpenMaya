import pymel.core as pm
import logging

log = logging.getLogger("ui")

class BaseTemplate(pm.ui.AETemplate):
    
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
        
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)


class AECoronaNodeTemplate(BaseTemplate):
    def __init__(self, nodeName):
        BaseTemplate.__init__(self,nodeName)
        self.thisNode = None
        self.node = pm.PyNode(self.nodeName)
        self.buildBody(nodeName)
        log.debug("AECoronaNodeTemplate")
        
    def buildDirLightTemplate(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        self.beginLayout("Corona" ,collapse=1)
        self.addControl("mtco_sun_multiplier", label="Intensity Multiplier")
        self.endLayout()

    def buildDisplacementShaderTemplate(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        self.beginLayout("Corona" ,collapse=1)
        self.addControl("mtco_displacementMin", label="Displacement Min")
        self.addControl("mtco_displacementMax", label="Displacement Max")
        self.endLayout()
        
    def buildCameraTemplate(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        self.beginLayout("Corona" ,collapse=1)
        self.addControl("mtco_controls_exposure", label="Controls Exposure")
        self.addControl("mtco_iso", label="Iso")
        self.addControl("mtco_shutterSpeed", label="Shutter Speed")
        self.endLayout()
        
    def buildCoronaTemplates(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        if self.thisNode.type() == "camera":
            log.debug("AECoronaNodeTemplate:build camera AE")            
            self.beginLayout("Corona" ,collapse=1)
            self.endLayout()
        if self.thisNode.type() == "directionalLight":
            self.buildDirLightTemplate(nodeName)
        if self.thisNode.type() == "displacementShader":
            self.buildDisplacementShaderTemplate(nodeName)
        if self.thisNode.type() == "camera":
            self.buildCameraTemplate(nodeName)
            
    def buildBody(self, nodeName):
        self.buildCoronaTemplates(nodeName)

