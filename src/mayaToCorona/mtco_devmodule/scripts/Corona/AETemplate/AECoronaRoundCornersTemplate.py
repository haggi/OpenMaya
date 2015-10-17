import pymel.core as pm
import logging

log = logging.getLogger("ui")

class BaseTemplate(pm.ui.AETemplate):
    
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
        
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)
        

class AECoronaRoundCornersTemplate(BaseTemplate):
    def __init__(self, nodeName):
        BaseTemplate.__init__(self,nodeName)
        log.debug("AECoronaRoundCornersTemplate")
        self.thisNode = None
        self.node = pm.PyNode(self.nodeName)
        pm.mel.AEswatchDisplay(nodeName)
        self.beginScrollLayout()
        self.buildBody(nodeName)
        allAttributes = self.node.listAttr()
        allowedAttributes = ["samples", "radius"]
        for att in allAttributes:
            att = att.split(".")[-1]
            if not att in allowedAttributes:
                self.suppress(att)
        self.addExtraControls("ExtraControls")
        self.endScrollLayout()
                
    def buildBody(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        self.beginLayout("RoundCorners" ,collapse=0)
        self.addControl("samples", label="Samples")
        self.addControl("radius", label="Radius")
        self.endLayout()
                