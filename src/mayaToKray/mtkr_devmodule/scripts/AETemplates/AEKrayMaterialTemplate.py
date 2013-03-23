import pymel.core as pm
import logging

log = logging.getLogger("ui")

class BaseTemplate(pm.ui.AETemplate):
    
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
        
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)
        

class AEKrayMaterialTemplate(BaseTemplate):
    def __init__(self, nodeName):
        BaseTemplate.__init__(self, nodeName)
        log.debug("AEKrayMaerialTemplate")
        self.thisNode = None
        self.node = pm.PyNode(self.nodeName)
        pm.mel.AEswatchDisplay(nodeName)
        self.beginScrollLayout()
        self.buildBody(nodeName)
        self.addExtraControls("ExtraControls")
        self.endScrollLayout()
        
    def buildBody(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        self.beginLayout("Common Material Attributes" , collapse=0)
        self.addControl("color", label="Color")
        self.addControl("transparency", label="Transparency")
        self.addControl("texture_bump", label="Bump Mapping")
        self.addControl("diffuse", label="Diffuse")
        self.addControl("diffuseMode", label="Diffuse Mode")
        self.addControl("translucency", label="Translucency")        
        self.endLayout()
        self.beginLayout("Specular Shading" , collapse=0)
        self.addControl("specularity", label="Specularity")        
        self.addControl("glossiness", label="Glossiness")
        self.addControl("reflection", label="Reflectivity")
        self.addControl("fresnel", label="Fresnel Factor")
        self.addSeparator()
        self.addControl("reflectionBlur", label="Reflection Blur")
        self.addControl("reflectionBlurLights", label="Reflection Blur Lights")
        self.addSeparator()
        self.addControl("ior", label="Refractive Index")
        self.addControl("refractionBlur", label="Refraction Blur")
        self.endLayout()
        self.beginLayout("Blur Rays" , collapse=0)
        self.addControl("blurMaxRays", label="Blur Max Rays")
        self.addControl("minRays", label="Min Rays")
        self.addControl("maxRays", label="Max Rays")
        self.addControl("noiseTolerance", label="Noise Tolerance")
        self.addControl("blurTolerance", label="Blur Tolerance")
        self.endLayout()
        self.beginLayout("Blabla" , collapse=0)
        
        self.addControl("backSide", label="Back Side Shader")

#MObject  KrayMaterial::bumpDepth;
#MObject  KrayMaterial::;
#MObject  KrayMaterial::;
#MObject  KrayMaterial::edgeDetectSlot;
#MObject  KrayMaterial::flags;
#MObject  KrayMaterial::forceColor;
#MObject  KrayMaterial::;
#MObject  KrayMaterial::;
#MObject  KrayMaterial::;
#MObject  KrayMaterial::light;
#MObject  KrayMaterial::lightGroup;
#MObject  KrayMaterial::passiveLem;
#MObject  KrayMaterial::;
#MObject  KrayMaterial::;
#MObject  KrayMaterial::;
#MObject  KrayMaterial::;
#MObject  KrayMaterial::;
#MObject  KrayMaterial::texture_color;
#MObject  KrayMaterial::texture_diffuse;
#MObject  KrayMaterial::texture_light;
#MObject  KrayMaterial::texture_reflection;
#MObject  KrayMaterial::texture_specularity;
#MObject  KrayMaterial::texture_translucency;
#MObject  KrayMaterial::texture_transparency;
#MObject  KrayMaterial::;
#MObject  KrayMaterial::transparencyAffects;
        
        self.endLayout()
