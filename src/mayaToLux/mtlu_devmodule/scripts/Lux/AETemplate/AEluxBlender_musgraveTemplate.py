import pymel.core as pm
import logging

log = logging.getLogger("ui")

class BaseTemplate(pm.ui.AETemplate):
    
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
        
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)
        

class AEluxBlender_musgraveTemplate(BaseTemplate):
    def __init__(self, nodeName):
        BaseTemplate.__init__(self,nodeName)
        log.debug("AEluxBlender_musgraveTemplate")
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
        self.addControl("outscale", label="IScale")
        self.addControl("scale", label="Scale")
        self.addControl("h", label="H")
        self.addControl("octs", label="Octaves")
        self.addControl("coordinates", label="Coordinate System")
        self.addControl("bright", label="Noise Brightness")
        self.addControl("rotate", label="Rotate")
        self.addControl("lacu", label="Lacunarity")
        self.addControl("gain", label="Gain")
        self.addControl("offset", label="Offset")
        self.addControl("noisebasis", label="Noise Basis")
        self.addControl("translate", label="Offset")
        self.addControl("type", label="Type")
        self.addControl("contrast", label="Noise Contrast")
        self.addSeparator()
        #autoAddEnd
        self.endNoOptimize()
        self.endLayout()
        