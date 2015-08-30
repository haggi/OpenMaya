import pymel.core as pm
import logging
from getpass import getuser

log = logging.getLogger("ui")

class BaseTemplate(pm.ui.AETemplate):
    
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
        
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)
        

class AECoronaSurfaceTemplate(BaseTemplate):
    def __init__(self, nodeName):
        BaseTemplate.__init__(self,nodeName)
        log.debug("AECoronaMTLTemplate")
        self.thisNode = None
        self.bumpCtrl = None
        self.node = pm.PyNode(self.nodeName)
        pm.mel.AEswatchDisplay(nodeName)
        self.beginScrollLayout()
        self.buildBody(nodeName)
        if getuser() != "haggi":
            self.suppress("glassMode")
            self.suppress("emissionSharpnessFake")
            self.suppress("castsShadows")
            self.suppress("volumeEmissionColor")
            self.suppress("volumeEmissionDist")
            self.suppress("roundCornersRadiusMultiplier")
            self.suppress("bgOverride")
            self.suppress("emissionDisableSampling")
            self.suppress("shadowCatcherMode")
            self.suppress("iesProfile")
            self.suppress("translucenceCoeff")
            self.suppress("diffuseReflectivity")
            self.suppress("transparency")
        self.addExtraControls("ExtraControls")        
        self.endScrollLayout()
    
    def bumpNew(self, attribute):
        pm.setUITemplate("attributeEditorTemplate", pushTemplate=True)        
        attName = attribute
        self.bumpCtrl = pm.attrNavigationControlGrp(attribute=attribute, label="Bump Map") 
        pm.setUITemplate("attributeEditorTemplate", popTemplate=True)                
                    
    def bumpReplace(self, attribute):
        if self.bumpCtrl is not None:
            pm.attrNavigationControlGrp(self.bumpCtrl, edit=True, attribute=attribute) 
    
    def buildBody(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        self.beginLayout("Diffuse" ,collapse=0)
        self.addControl("diffuseMultiplier", label="Diffuse Level")
        self.addControl("diffuse", label="Diffuse Color")
        self.callCustom(self.bumpNew, self.bumpReplace,"normalCamera")
        self.addSeparator()
        self.addControl("translucencyFraction", label="Translucency Fraction")
        self.addControl("translucency", label="Translucency")
        self.endLayout()
        self.beginLayout("Reflection" ,collapse=0)
        self.addControl("reflectivityMultiplier", label="Reflectivity Level")
        self.addControl("reflectivity", label="Reflection Color")
        self.addControl("reflectionGlossiness", label="Glossiness")
        self.addControl("fresnelIorMultiplier", label="Fresnel IOR")
        self.addControl("fresnelIor", label="Fresnel Map")
        self.addControl("anisotropy", label="Anisotropy")
        self.addControl("anisotropicRotation", label="Aniso Rotation")
        self.endLayout()
        self.beginLayout("Refraction" ,collapse=0)
        self.addControl("refractivityMultiplier", label="Refractivity Level")
        self.addControl("refractivity", label="Refraction Color")
        self.addControl("refractionIndexMultiplier", label="Refraction IOR Level")
        self.addControl("refractionIndex", label="Refraction IOR Map")
        self.addControl("refractionGlossiness", label="Glossiness")
        self.addControl("glassType", label="Refraction Mode")
        self.endLayout()
        self.beginLayout("Alpha" ,collapse=0)
        self.addControl("opacityMultiplier", label="Opacity Level")
        self.addControl("opacity", label="Opacity")
        self.addControl("alphaMode", label="Alpha Mode")
        self.endLayout()
        self.beginLayout("Volume" ,collapse=0)
        self.addControl("attenuationColor", label="Absorption Color")
        self.addControl("attenuationDist", label="Absorption Distance")
        self.addSeparator()
        self.addControl("volumeScatteringAlbedo", label="Scatter Color")
        self.addControl("volumeMeanCosine", label="Scatter Directionality")
        self.addControl("singleScatteringOnly", label="Single Bounce Only")
#         self.addSeparator()
#         self.addControl("volumeEmissionColor", label="Emission Color")
#         self.addControl("volumeEmissionDist", label="Emission Distance")
#         self.endLayout()
        self.beginLayout("Emission" ,collapse=0)
        self.beginNoOptimize()
        self.addControl("emissionColor", label="Emission Color")
        self.addControl("emissionColorMultiplier", label="Emission Multiplier")
        
        self.beginLayout("Round Corners" ,collapse=0)
        self.addControl("roundCornersRadius", label="Radius")
        self.addControl("roundCornersSamples", label="Samples")
        self.endLayout()
        #self.addControl("iesProfile", label="IES Profile")
        #self.addControl("emissionDisableSampling", label="Disable Sampling")
        #self.addControl("emissionSharpnessFake", label="Sharpness Fake")
        #self.addControl("emissionSharpnessFakePoint", label="Sharpness Fake Point")
        self.endNoOptimize()
        self.endLayout()
        
        #self.beginLayout("Hardware Texturing" ,collapse=0)
        #pm.mel.eval('AEhardwareTextureTemplate "%s"' % self.nodeName + r'("diffuse emissionColor ")')
        #self.endLayout()
        