import pymel.core as pm
import logging

log = logging.getLogger("ui")

class BaseTemplate(pm.ui.AETemplate):
    
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
        
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)
        

class AEmtap_smokeShaderTemplate(BaseTemplate):
    def __init__(self, nodeName):
        BaseTemplate.__init__(self,nodeName)
        log.debug("AEmtap_smokeShaderTemplate")
        self.thisNode = None
        self.node = pm.PyNode(self.nodeName)
        pm.mel.AEswatchDisplay(nodeName)
        self.beginScrollLayout()
        self.buildBody(nodeName)
        self.addExtraControls("ExtraControls")
        self.endScrollLayout()
        
    def buildBody(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        self.beginLayout("Smoke" ,collapse=0)
        self.endLayout()