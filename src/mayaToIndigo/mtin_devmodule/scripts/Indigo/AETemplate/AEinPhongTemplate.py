import pymel.core as pm
import logging

log = logging.getLogger("ui")

class BaseTemplate(pm.ui.AETemplate):
    
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
        
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)
        

class AEinPhongTemplate(BaseTemplate):
    def __init__(self, nodeName):
        BaseTemplate.__init__(self,nodeName)
        log.debug("AEinPhongTemplate")
        self.thisNode = None
        self.node = pm.PyNode(self.nodeName)
        pm.mel.AEswatchDisplay(nodeName)
        self.beginScrollLayout()
        self.buildBody(nodeName)
        self.addExtraControls("ExtraControls")
        self.endScrollLayout()
        
    def updateUI(self, dummy):
        print "updateUI", dummy
        pass
    
    def buildBody(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        self.beginLayout("ShaderSettings" ,collapse=0)
        self.beginNoOptimize()
        #autoAddBegin
        self.addControl("displacement", label="Displacement")
        self.addControl("layer", label="Layer")
        self.addControl("albedo", label="Albedo")
        self.addSeparator()
        self.addControl("emission", label="Emission")
        self.addControl("base_emission", label="Emission Multiplier")
        self.addControl("iesProfile", label="IES Profile")
        self.addSeparator()
        self.addControl("exponent", label="Exponent")
        self.addControl("ior", label="IOR")
        self.addControl("specular_reflectivity", label="Reflectivity")
        self.addControl("nk_data", label="NK Data", changeCommand=self.updateUI)
        self.addSeparator()
        self.addControl("bump", label="Bump")
        #autoAddEnd
        self.endNoOptimize()
        self.endLayout()
        