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
        
    def buildBody(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        self.beginLayout("BRDF" ,collapse=0)
        self.addControl("shaderType", label="Shader Type")
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
        self.beginLayout("Subsurface Controls" ,collapse=0)
        self.addControl("useSSS", label="Use SSS")        
        self.addControl("albedo", label="Albedo")        
        self.addControl("ambient_sss", label="Ambient SSS")        
        self.addControl("diffuse", label="Diffuse")        
        self.addControl("distortion", label="Distortion")        
        self.addControl("light_samples", label="Light Samples")        
        self.addControl("occlusion_samples", label="Occlusion Samples")        
        self.addControl("power", label="Power")        
        self.addControl("scale", label="Scale")        
        self.addControl("view_dep_sss", label="View Depedent sss")       
        self.endLayout()
