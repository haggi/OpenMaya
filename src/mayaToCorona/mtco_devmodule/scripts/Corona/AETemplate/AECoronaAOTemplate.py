import pymel.core as pm
import logging

log = logging.getLogger("ui")

class BaseTemplate(pm.ui.AETemplate):
    
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
        
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)
        

class AECoronaAOTemplate(BaseTemplate):
    def __init__(self, nodeName):
        BaseTemplate.__init__(self,nodeName)
        log.debug("AECoronaLightTemplate")
        self.thisNode = None
        self.node = pm.PyNode(self.nodeName)
        pm.mel.AEswatchDisplay(nodeName)
        self.beginScrollLayout()
        self.buildBody(nodeName)
        allAttributes = self.node.listAttr()
        allowedAttributes = ["maxDist", "maxSamplesCount", "phongExponent", "mixExponent", "invert", "excludeMode", "useOffset", "offset"]
        for att in allAttributes:
            att = att.split(".")[-1]
            if not att in allowedAttributes:
                self.suppress(att)
        self.addExtraControls("ExtraControls")
        self.endScrollLayout()
        
    def updateUI(self, nodeName=None):
        self.thisNode = pm.PyNode(nodeName)
        self.dimControl(nodeName, "offset", True)
        if self.thisNode.useOffset.get():
            self.dimControl(nodeName, "offset", False)
        
    def buildBody(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        self.beginLayout("Ambient Occlusion" ,collapse=0)
        self.beginNoOptimize()
        self.addControl("maxDist", label="Max Dist")
        self.addControl("maxSamplesCount", label="Max Samples")
        self.addControl("phongExponent", label="Phong Exponent")
        self.addControl("mixExponent", label="Mix Exponent")
        self.addControl("invert", label="Invert")
        self.addControl("excludeMode", label="Exclude Node")
        self.addControl("useOffset", label="Use Offset", changeCommand=self.updateUI)
        self.addControl("offset", label="Offset")
        self.endNoOptimize()
        self.endLayout()
        self.updateUI(self.thisNode)
                