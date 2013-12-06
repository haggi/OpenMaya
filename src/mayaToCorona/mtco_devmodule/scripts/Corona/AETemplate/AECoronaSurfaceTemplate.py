import pymel.core as pm
import logging

log = logging.getLogger("ui")

class BaseTemplate(pm.ui.AETemplate):
    
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
        
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)
        

class AECoronaSurfaceTemplate(BaseTemplate):
    def __init__(self, nodeName):
        BaseTemplate.__init__(self,nodeName)
        log.debug("AECoronaSurfaceTemplate")
        self.thisNode = None
        self.node = pm.PyNode(self.nodeName)
        pm.mel.AEswatchDisplay(nodeName)
        self.beginScrollLayout()
        self.buildBody(nodeName)
        self.addExtraControls("ExtraControls")
        self.endScrollLayout()
        
    def buildBody(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        self.beginLayout("Diffuse" ,collapse=0)
        #self.beginNoOptimize()
        #autoAddBegin
#        self.addControl("refractivity", label="Refractivity")
#        self.addControl("anisotropyRotation", label="Aniso Rotation")
#        self.addControl("attenuationColor", label="Attenuation Color")
#        self.addControl("brdfType", label="BRDF Type")
#        self.addControl("anisotropy", label="Anisotropy")
#        self.addControl("attenuationDist", label="Attenuation Distance")
#        self.addControl("reflectivity", label="Reflectivity")
#        self.addControl("refractionIndex", label="Refraction Index")
#        self.addControl("roundCornersRadius", label="Round Courner Radius")
#        self.addControl("refractionGlossiness", label="Glossiness")
#        self.addControl("roundCornersSamples", label="Round Corners Samples")
#        self.addControl("fresnelIor", label="Fresnel Value")
#        self.addControl("glassMode", label="Glas Mode")
        self.addControl("diffuse", label="Diffuse Color")
        self.addControl("translucency", label="Translucency")
#        self.addControl("reflectionGlossiness", label="Glossiness")
        self.endLayout()
        self.beginLayout("Reflection" ,collapse=0)
        self.endLayout()
        self.beginLayout("Refraction" ,collapse=0)
        self.endLayout()
        self.beginLayout("Emission" ,collapse=0)
        self.addControl("emissionColor", label="Emission Color")
        self.addControl("emissionExponent", label="Emission Exponent")
        self.addControl("emissionDisableSampling", label="Disable Sampling")
        self.addControl("emissionSharpnessFake", label="Sharpness Fake")
#        self.addControl("emissionSharpnessFakePoint", label="Sharpness Fake Point")
        #autoAddEnd
        #self.endNoOptimize()
        self.endLayout()
        