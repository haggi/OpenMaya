import pymel.core as pm
import logging

log = logging.getLogger("ui")

class BaseTemplate(pm.ui.AETemplate):
    
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
        
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)
        

class AEDiffuseLightTemplate(BaseTemplate):
    def __init__(self, nodeName):
        BaseTemplate.__init__(self,nodeName)
        log.debug("AEDiffuseLightTemplate")
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
        self.addControl("attenuation", label="Attenuation")
        self.addControl("power", label="Power")
        self.addControl("efficacy", label="Efficacy")
        self.addControl("color", label="Color")
        self.addControl("frontSide", label="Front Side")
        self.addControl("backSide", label="Back Side")
        self.addControl("emitter", label="Emitter")
        self.addControl("unit", label="Unit")
        self.addSeparator()
        #autoAddEnd
        self.endNoOptimize()
        self.endLayout()
        