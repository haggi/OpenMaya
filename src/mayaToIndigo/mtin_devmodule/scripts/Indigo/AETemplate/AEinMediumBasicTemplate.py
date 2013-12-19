import pymel.core as pm
import logging

log = logging.getLogger("ui")

class BaseTemplate(pm.ui.AETemplate):
    
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
        
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)
        

class AEinMediumBasicTemplate(BaseTemplate):
    def __init__(self, nodeName):
        BaseTemplate.__init__(self,nodeName)
        log.debug("AEinMediumBasicTemplate")
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
        self.addControl("subsurface_scattering", label="Subsurface Scatter")
        self.addControl("precedence", label="Precedence")
        self.addControl("scattering_coefficient_spectrum", label="Subsurface Color")
        self.addControl("absorption_coefficient_spectrum", label="Absorption")
        self.addControl("ior", label="Ior")
        self.addControl("phase_function_hgg", label="Henyey Greenstein G")
        self.addControl("cauchy_b_coeff", label="B Coeff")
        self.addControl("phase_function", label="Phase Function")
        self.addControl("name", label="Name")
        self.addSeparator()
        #autoAddEnd
        self.endNoOptimize()
        self.endLayout()
        