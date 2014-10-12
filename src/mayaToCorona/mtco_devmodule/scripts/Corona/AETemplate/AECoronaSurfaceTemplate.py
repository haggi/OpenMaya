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
        self.addControl("diffuse", label="Diffuse Color")
        self.addSeparator()
        self.addControl("translucency", label="Translucency")
        self.addControl("alphaMode", label="Alpha Mode")
        self.addControl("opacity", label="Opacity")
        self.endLayout()
        self.beginLayout("Round Corners" ,collapse=0)
        self.addControl("roundCornersRadius", label="Radius")
        self.addControl("roundCornersSamples", label="Samples")
        self.endLayout()
        self.beginLayout("Reflection" ,collapse=0)
        self.addControl("reflectivity", label="Reflectivity")
        self.addControl("brdfType", label="BRDF Type")
        self.addControl("reflectionGlossiness", label="Glossiness")
        self.addControl("fresnelIor", label="Fresnel Value")
        self.addControl("anisotropy", label="Anisotropy")
        self.addControl("anisotropicRotation", label="Aniso Rotation")
        self.endLayout()
        self.beginLayout("Refraction" ,collapse=0)
        self.addControl("refractivity", label="Refractivity")
        self.addControl("refractionIndex", label="Refraction Index")
        self.addControl("refractionGlossiness", label="Glossiness")
        self.addControl("glassMode", label="Glass Mode")
        self.endLayout()
        self.beginLayout("Volume" ,collapse=0)
        self.addControl("attenuationColor", label="Attenuation Color")
        self.addControl("attenuationDist", label="Attenuation Distance")
        self.addControl("volumeScatteringAlbedo", label="Volume Scattering Albedo")
        self.addControl("volumeMeanCosine", label="Mean Cosine")
        self.addControl("volumeSSSMode", label="Use SSS Mode")
        self.addSeparator()
        self.addControl("volumeEmissionColor", label="Emission Color")
        self.addControl("volumeEmissionDist", label="Emission Distance")
        self.endLayout()
        self.beginLayout("Emission" ,collapse=0)
        self.beginNoOptimize()
        self.addControl("emissionColor", label="Emission Color")
        #self.addControl("emissionIesProfile", label="IES Profile")
        self.addControl("mtco_mat_iesProfile", label="IES Profile")
        self.addControl("emissionDisableSampling", label="Disable Sampling")
        self.addControl("emissionSharpnessFake", label="Sharpness Fake")
        self.addControl("emissionSharpnessFakePoint", label="Sharpness Fake Point")
        self.endNoOptimize()
        self.endLayout()
        
        #self.beginLayout("Hardware Texturing" ,collapse=0)
        #pm.mel.eval('AEhardwareTextureTemplate "%s"' % self.nodeName + r'("diffuse emissionColor ")')
        #self.endLayout()
        