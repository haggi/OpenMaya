
# this file should be placed in AETemplates, but maya 2014 seems to have a problem with 
# this package name and other-than-ae template scripts. Seems to work if I move it here.

import pymel.core as pm
import logging

log = logging.getLogger("ui")

class BaseTemplate(pm.ui.AETemplate):
    
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
        
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)


class AEappleseedNodeTemplate(BaseTemplate):
    def __init__(self, nodeName):
        BaseTemplate.__init__(self,nodeName)
        self.thisNode = None
        self.node = pm.PyNode(self.nodeName)
        self.buildBody(nodeName)
        log.debug("AEappleSeedNodeTemplate")
        
    def buildAppleSeedTemplates(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        if self.thisNode.type() == "camera":
            log.debug("AEappleSeedNodeTemplate:build camera AE")            
            self.beginLayout("AppleSeed" ,collapse=1)
            self.addControl("mtap_cameraType", label="Camera Type")            
            self.addControl("mtap_diaphragm_blades", label="Diaphragm Blades")            
            self.addControl("mtap_diaphragm_tilt_angle", label="Diaphragm Tilt Angle")                        
            self.endLayout()
    
    def buildBody(self, nodeName):
        self.buildAppleSeedTemplates(nodeName)

