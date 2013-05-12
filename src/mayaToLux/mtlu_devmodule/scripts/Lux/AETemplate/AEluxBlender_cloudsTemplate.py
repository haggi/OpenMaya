import pymel.core as pm
import logging

log = logging.getLogger("ui")

class BaseTemplate(pm.ui.AETemplate):
    
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
        
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)
        

class AEluxBlender_cloudsTemplate(BaseTemplate):
    def __init__(self, nodeName):
        BaseTemplate.__init__(self,nodeName)
        log.debug("AEluxBlender_cloudsTemplate")
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
        self.addControl("noisesize", label="Noise Size")
        self.addControl("noisebasis", label="Noise Basis")
        self.addControl("scale", label="Scale")
        self.addControl("noisedepth", label="Noise Depth")
        self.addControl("coordinates", label="Coordinate System")
        self.addControl("bright", label="Noise Brightness")
        self.addControl("rotate", label="Rotate")
        self.addControl("translate", label="Offset")
        self.addControl("contrast", label="Noise Contrast")
        self.addControl("noisetype", label="Noise type")
        self.addSeparator()
        #autoAddEnd
        self.endNoOptimize()
        self.endLayout()
        