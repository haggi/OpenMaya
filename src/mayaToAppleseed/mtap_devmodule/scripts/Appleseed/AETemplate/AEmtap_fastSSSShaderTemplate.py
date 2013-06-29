import pymel.core as pm
import logging

log = logging.getLogger("ui")

class BaseTemplate(pm.ui.AETemplate):
    
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
        
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)
        

class AEmtap_fastSSSShaderTemplate(BaseTemplate):
    def __init__(self, nodeName):
        BaseTemplate.__init__(self,nodeName)
        log.debug("AEmtap_fastSSSShaderTemplate")
        self.thisNode = None
        self.node = pm.PyNode(self.nodeName)
        pm.mel.AEswatchDisplay(nodeName)
        self.beginScrollLayout()
        self.buildBody(nodeName)
        self.addExtraControls("ExtraControls")
        self.endScrollLayout()
        
    def buildBody(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        self.beginLayout("Subsurface Scatter" ,collapse=0)
        self.addControl("albedo", label="Albedo")        
        self.addControl("ambient_sss", label="Ambient SSS")        
        self.addControl("diffuse", label="Diffuse")        
        self.addControl("distortion", label="Distortion")        
        self.addControl("light_samples", label="Light Samples")        
        self.addControl("occlusion_samples", label="Occlusion Samples")        
        self.addControl("power", label="Power")        
        self.addControl("scale", label="Scale")        
        self.addControl("view_dep_sss", label="View Depedent sss")       
        self.endLayout()