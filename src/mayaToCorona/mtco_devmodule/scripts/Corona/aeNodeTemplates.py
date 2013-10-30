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
        
    def buildCoronaTemplates(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        if self.thisNode.type() == "camera":
            log.debug("AECoronaNodeTemplate:build camera AE")            
            self.beginLayout("Corona" ,collapse=1)
            self.endLayout()
    
    def buildBody(self, nodeName):
        self.buildCoronaTemplates(nodeName)

