import pymel.core as pm
import logging

log = logging.getLogger("ui")

class BaseTemplate(pm.ui.AETemplate):
    
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
        
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)
        

class AEappleseedSurfaceShaderTemplate(BaseTemplate):
    def __init__(self, nodeName):
        BaseTemplate.__init__(self,nodeName)
        log.debug("AEappleseedSurfaceShaderTemplate")
        self.thisNode = None
        self.node = pm.PyNode(self.nodeName)
        pm.mel.AEswatchDisplay(nodeName)
        self.beginScrollLayout()
        self.buildBody(nodeName)
        self.addExtraControls("ExtraControls")
        self.endScrollLayout()
    
    
    def updateUi(self, args=None):
        #shader = self.thisNode.shaderType.get()
        # ambient occusion
        #if shader == 0:
        #    self.dimControl(self.thisNode, "matte_reflectance", True)
        #    self.dimControl(self.thisNode, "matte_reflectance_multiplier", True)
        #    self.dimControl(self.thisNode, "specular_reflectance", True)
        #    self.dimControl(self.thisNode, "specular_reflectance_multiplier", True)
            
        return
        
        
    def buildBody(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        self.beginLayout("Surface" ,collapse=0)
        #self.addControl("shaderType", label="Surface Shader Type", changeCommand=self.updateUi)
        #self.addControl("shaderType", label="Surface Shader Type")
        self.addControl("bsdf", label="BSDF Type")
        self.addSeparator()
        self.addControl("matte_reflectance", label="Matte Reflectance")
        self.addControl("matte_reflectance_multiplier", label="Matte Refl Multiplier")
        self.addSeparator()
        self.addControl("specular_reflectance", label="Specular Reflectance")
        self.addControl("specular_reflectance_multiplier", label="Spec Refl Multiplier")
        self.addControl("shininess_u", label="Shininess U")        
        self.addControl("shininess_v", label="Shininess V")        
        self.addControl("roughness", label="Roughness")
        self.endLayout()
        
        self.beginLayout("BTDF" ,collapse=0)
        self.addControl("transmittance", label="Transmittance")        
        self.addControl("transmittance_multiplier", label="Transmittance Multiplier")        
        self.addControl("from_ior", label="From Ior")        
        self.addControl("to_ior", label="To Ior")        
        self.endLayout()
        self.beginLayout("EDF" ,collapse=0)
        self.addControl("emitLight", label="emitLight")        
        self.addControl("exitance", label="Exitance")        
        self.endLayout()

        self.updateUi()