import pymel.core as pm
import logging

log = logging.getLogger("ui")

class BaseTemplate(pm.ui.AETemplate):
    
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
        
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)
        

class AEmtap_standinMeshNodeTemplate(BaseTemplate):
    def __init__(self, nodeName):
        BaseTemplate.__init__(self,nodeName)
        log.debug("AEmtap_standinMeshNodeTemplate")
        self.thisNode = None
        self.node = pm.PyNode(self.nodeName)
        self.beginScrollLayout()
        self.buildBody(nodeName)
        self.addExtraControls("ExtraControls")
        self.endScrollLayout()
        
    def buildBody(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        self.beginLayout("StandinData" ,collapse=0)        
        self.addControl("binMeshFile", label="BinaryMesh File")        
        self.addControl("percentDisplay", label="Display Percent")
        self.addControl("polySizeMultiplier", label="Polysize Multiplier")        
        self.endLayout()