import pymel.core as pm
import logging

log = logging.getLogger("ui")

class BaseTemplate(pm.ui.AETemplate):
    
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
        
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)


class AEluxNodeTemplate(BaseTemplate):
    def __init__(self, nodeName):
        BaseTemplate.__init__(self,nodeName)
        self.thisNode = None
        self.node = pm.PyNode(self.nodeName)
        self.buildBody(nodeName)
        log.debug("AEluxNodeTemplate")
        
    def buildLuxTemplates(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        if self.thisNode.type() == "camera":
            self.beginLayout("Lux" ,collapse=1)
            self.endLayout()
    
    def buildBody(self, nodeName):
        self.buildLuxTemplates(nodeName)

