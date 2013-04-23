import pymel.core as pm
import logging

log = logging.getLogger("ui")

class BaseTemplate(pm.ui.AETemplate):
    
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
        
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)


class AELuxNodeTemplate(BaseTemplate):
    def __init__(self, nodeName):
        BaseTemplate.__init__(self,nodeName)
        self.thisNode = None
        self.node = pm.PyNode(self.nodeName)
        self.buildBody(nodeName)
        log.debug("AELuxNodeTemplate")
        
    def buildLuxTemplates(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        if self.thisNode.type() == "camera":
            log.debug("AELuxNodeTemplate:build camera AE")            
            self.beginLayout("Lux" ,collapse=1)
            self.addControl("mtlu_diaphragm_blades", label="Blades")
            self.addControl("mtlu_autofocus", label="Autofocus")
            self.addControl("mtlu_distribution", label="Distribution")
            self.addControl("mtlu_power", label="Exponential Distribution Power")
            self.endLayout()
            
            
        if self.thisNode.type() == "directionalLight":
            log.debug("AELuxNodeTemplate:build dir light AE")            
            self.beginLayout("Lux" ,collapse=1)
            self.addControl("mtlu_light_importance", label="Importance")
            self.addControl("mtlu_dirLight_theta", label="Shadow softness")
            self.endLayout()

        if self.thisNode.type() == "ambientLight":
            log.debug("AELuxNodeTemplate:build ambi light AE")            
            self.beginLayout("Lux" ,collapse=1)
            self.addControl("mtlu_light_importance", label="Importance")
            self.addControl("mtlu_ambientLight_map", label="IBL Map")
            self.addControl("mtlu_ambientLight_samples", label="Light Samples")
            self.endLayout()

        if self.thisNode.type() == "pointLight":
            log.debug("AELuxNodeTemplate:build point light AE")            
            self.beginLayout("Lux" ,collapse=1)
            self.addControl("mtlu_light_importance", label="Importance")
            self.addControl("mtlu_pointLight_power", label="Power")
            self.addControl("mtlu_pointLight_efficacy", label="Efficacy")
            self.addControl("mtlu_pointLight_ies", label="IES File")
            self.endLayout()

        if self.thisNode.type() == "areaLight":
            log.debug("AELuxNodeTemplate:build area light AE")            
            self.beginLayout("Lux" ,collapse=1)
            self.addControl("mtlu_light_importance", label="Importance")
            self.addControl("mtlu_areaLight_samples", label="Power")
            self.addControl("mtlu_areaLight_power", label="Power")
            self.addControl("mtlu_areaLight_efficacy", label="Efficacy")
            self.addControl("mtlu_areaLight_ies", label="IES File")
            self.addControl("mtlu_areaLight_geo", label="Light mesh geometry")
            self.endLayout()
            
                
    def buildBody(self, nodeName):
        self.buildLuxTemplates(nodeName)

