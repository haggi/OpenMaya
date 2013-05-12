import pymel.core as pm
import logging

log = logging.getLogger("ui")

class BaseTemplate(pm.ui.AETemplate):
    
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
        
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)
        

class AEluxBilerpTemplate(BaseTemplate):
    def __init__(self, nodeName):
        BaseTemplate.__init__(self,nodeName)
        log.debug("AEluxBilerpTemplate")
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
        self.addControl("v11", label="Value 3")
        self.addControl("v01", label="Value 2")
        self.addControl("v00", label="Value 0")
        self.addControl("vdelta", label="VOffset")
        self.addControl("udelta", label="UDelta")
        self.addControl("mapping", label="Mapping")
        self.addControl("v1", label="Plane V0")
        self.addControl("v2", label="Plane V1")
        self.addControl("uscale", label="UScale")
        self.addControl("v10", label="Value 1")
        self.addControl("vscale", label="VScale")
        self.addSeparator()
        #autoAddEnd
        self.endNoOptimize()
        self.endLayout()
        