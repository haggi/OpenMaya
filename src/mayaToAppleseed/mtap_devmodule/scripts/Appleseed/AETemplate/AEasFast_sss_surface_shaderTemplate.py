import pymel.core as pm
import logging

log = logging.getLogger("ui")

class BaseTemplate(pm.ui.AETemplate):
    
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
        
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)
        

class AEasFast_sss_surface_shaderTemplate(BaseTemplate):
    def __init__(self, nodeName):
        BaseTemplate.__init__(self,nodeName)
        log.debug("AEasFast_sss_surface_shaderTemplate")
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
        self.addControl("ambient_sss", label="Ambient SSS")
        self.addControl("light_samples", label="Light Samples")
        self.addControl("power", label="Power")
        self.addControl("view_dep_sss", label="View-Dependent SSS")
        self.addControl("distortion", label="Normal Distortion")
        self.addControl("scale", label="Geometric Scale")
        self.addControl("diffuse", label="Diffuse Lighting")
        self.addControl("occlusion_samples", label="Occlusion Samples")
        self.addControl("albedo", label="Albedo")
        self.addSeparator()
        #autoAddEnd
        self.endNoOptimize()
        self.endLayout()
        