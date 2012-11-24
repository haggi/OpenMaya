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
        if args:
            self.thisNode = pm.PyNode(args)
        shader = self.thisNode.bsdf.get()
        # ambient occusion
        
        # dim all first
        self.dimControl(self.thisNode, "roughness", True)
        self.dimControl(self.thisNode, "matte_reflectance", True)
        self.dimControl(self.thisNode, "matte_reflectance_multiplier", True)
        self.dimControl(self.thisNode, "specular_reflectance", True)
        self.dimControl(self.thisNode, "specular_reflectance_multiplier", True)
        self.dimControl(self.thisNode, "transmittance", True)
        self.dimControl(self.thisNode, "transmittance_multiplier", True)
        self.dimControl(self.thisNode, "from_ior", True)
        self.dimControl(self.thisNode, "to_ior", True)
        self.dimControl(self.thisNode, "emitLight", True)
        self.dimControl(self.thisNode, "exitance", True)
        self.dimControl(self.thisNode, "shininess_u", True)
        self.dimControl(self.thisNode, "shininess_v", True)
        
        if shader == 0: # lambert
            self.dimControl(self.thisNode, "matte_reflectance", False)
            self.dimControl(self.thisNode, "matte_reflectance_multiplier", False)
        
        if shader == 1: # ashikmin
            self.dimControl(self.thisNode, "matte_reflectance", False)
            self.dimControl(self.thisNode, "matte_reflectance_multiplier", False)
            self.dimControl(self.thisNode, "specular_reflectance", False)
            self.dimControl(self.thisNode, "specular_reflectance_multiplier", False)
            self.dimControl(self.thisNode, "shininess_u", False)
            self.dimControl(self.thisNode, "shininess_v", False)
            
        if shader == 2: # kelemen
            self.dimControl(self.thisNode, "matte_reflectance", False)
            self.dimControl(self.thisNode, "matte_reflectance_multiplier", False)
            self.dimControl(self.thisNode, "specular_reflectance", False)
            self.dimControl(self.thisNode, "specular_reflectance_multiplier", False)
            self.dimControl(self.thisNode, "roughness", False)
            
        if shader == 3: # specular
            self.dimControl(self.thisNode, "matte_reflectance", False)
            self.dimControl(self.thisNode, "matte_reflectance_multiplier", False)
            self.dimControl(self.thisNode, "transmittance", False)
            self.dimControl(self.thisNode, "transmittance_multiplier", False)
            self.dimControl(self.thisNode, "from_ior", False)
            self.dimControl(self.thisNode, "to_ior", False)
            
        if shader == 4: # phong
            self.dimControl(self.thisNode, "matte_reflectance", False)
            self.dimControl(self.thisNode, "matte_reflectance_multiplier", False)
            
        self.dimControl(self.thisNode, "emitLight", False)
        self.dimControl(self.thisNode, "exitance", False)
            
        
    def buildBody(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        self.beginLayout("Surface" ,collapse=0)
        #self.addControl("shaderType", label="Surface Shader Type", changeCommand=self.updateUi)
        #self.addControl("shaderType", label="Surface Shader Type")
        self.addControl("bsdf", label="BSDF Type", changeCommand=self.updateUi)
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