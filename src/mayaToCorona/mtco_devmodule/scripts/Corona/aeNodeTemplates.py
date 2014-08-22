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
        self.addControl("mtco_sun_multiplier", label="Sun Intensity Multiplier")
        self.endLayout()
        
    def buildAreaLightTemplate(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        self.beginLayout("Corona" ,collapse=1)
        self.addControl("mtco_envPortal", label="Use as Environment Portal")
        self.endLayout()

    def buildBum2dTemplate(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        self.beginLayout("Corona" ,collapse=1)
        self.addControl("normalMap", label="Normal Map")
        self.endLayout()

    def buildFileTemplate(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        self.beginLayout("Corona" ,collapse=1)
        self.addControl("fromSRGB", label="Convert sRGB->Linear")
        self.addControl("toSRGB", label="Convert Linear->sRGB")
        self.endLayout()

    def buildDisplacementShaderTemplate(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        self.beginLayout("Corona" ,collapse=1)
        self.addControl("mtco_displacementMin", label="Displacement Min")
        self.addControl("mtco_displacementMax", label="Displacement Max")
        self.endLayout()

    def buildMeshTemplate(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        self.beginLayout("Corona" ,collapse=1)
        self.addControl("mtco_visibleInGI", label="Visible in GI")
        self.addControl("mtco_envPortal", label="Use as Environment Portal")
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
        if self.thisNode.type() == "areaLight":
            self.buildAreaLightTemplate(nodeName)
        if self.thisNode.type() == "directionalLight":
            self.buildDirLightTemplate(nodeName)
        if self.thisNode.type() == "displacementShader":
            self.buildDisplacementShaderTemplate(nodeName)
        if self.thisNode.type() == "camera":
            self.buildCameraTemplate(nodeName)
        if self.thisNode.type() == "bump2d":
            self.buildBum2dTemplate(nodeName)
        if self.thisNode.type() == "file":
            self.buildFileTemplate(nodeName)
        if self.thisNode.type() == "mesh":
            self.buildMeshTemplate(nodeName)
            
    def buildBody(self, nodeName):
        self.buildCoronaTemplates(nodeName)

