import pymel.core as pm
import logging

log = logging.getLogger("ui")

class BaseTemplate(pm.ui.AETemplate):
    
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
        
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)
        

class AEinGlossyTransparentTemplate(BaseTemplate):
    def __init__(self, nodeName):
        BaseTemplate.__init__(self,nodeName)
        log.debug("AEinGlossyTransparentTemplate")
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
        self.addControl("layer", label="Layer")
        self.addControl("exponent", label="Exponent")
        self.addControl("internal_medium_name", label="Internal Medium")
        self.addControl("bump", label="Bump")
        self.addControl("base_emission", label="Emission Multiplier")
        self.addControl("displacement", label="Displacement")
        self.addControl("emission", label="Emission")
        self.addSeparator()
        #autoAddEnd
        self.endNoOptimize()
        self.endLayout()
        