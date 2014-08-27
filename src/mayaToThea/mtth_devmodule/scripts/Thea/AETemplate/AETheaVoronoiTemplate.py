import pymel.core as pm
import logging

log = logging.getLogger("ui")

class BaseTemplate(pm.ui.AETemplate):
    
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
        
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)
        

class AETheaVoronoiTemplate(BaseTemplate):
    def __init__(self, nodeName):
        BaseTemplate.__init__(self,nodeName)
        log.debug("AETheaVoronoiTemplate")
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
        self.addControl("cellColor", label="Cell Color")
        self.addControl("ridgeColor", label="Ridge Color")
        self.addControl("plateauColor", label="Plateau Color")
        self.addControl("ridge", label="Ridge")
        self.addControl("plateau", label="Plateau")
        #autoAddEnd
        self.endNoOptimize()
        self.endLayout()
        