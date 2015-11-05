import pymel.core as pm
import logging

log = logging.getLogger("ui")

class BaseTemplate(pm.ui.AETemplate):
    
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
        
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)


class AECoronaNodeTemplate(BaseTemplate):
    def __init__(self, nodeName):
        BaseTemplate.__init__(self,nodeName)
        self.thisNode = None
        self.node = pm.PyNode(self.nodeName)
        self.buildBody(nodeName)
        log.debug("AECoronaNodeTemplate")

    def updateSun(self, node = None):   
        thisLight = pm.PyNode(node)
    
        if thisLight.mtco_useAsSun.get():
            otherLights = []
            for light in pm.ls(type="directionalLight"):
                if light.mtco_useAsSun.get() and (light != thisLight):
                    otherLights.append(light)

            if len(otherLights) > 0:                       
                result = pm.confirmDialog(title="Sun Problem", message="Another Light already has sun activated, what should be done?", button=["Use This light", "Use Other Light"])
                if result == "Use This light":
                    for light in otherLights:
                        light.mtco_useAsSun.set(False)
                else:
                    thisLight.mtco_useAsSun.set(False)
                    
    def buildDirLightTemplate(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        self.beginLayout("Corona" ,collapse=1)
        self.addControl("mtco_useAsSun", label="Use as Sun", changeCommand=self.updateSun)
        #self.addControl("mtco_sun_multiplier", label="Sun Intensity Multiplier")
        self.endLayout()
        
    def buildAreaLightTemplate(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        self.beginLayout("Corona" ,collapse=1)
        self.addControl("mtco_envPortal", label="Use as Environment Portal")
        self.addControl("mtco_areaVisible", label="Visible Geometry")
        self.addControl("mtco_doubleSided", label="Double Sided")
        self.addControl("mtco_castShadows", label="Cast Shadows")
        self.addControl("mtco_visibleInReflection", label="Visible in Reflection")
        self.addControl("mtco_visibleInRefraction", label="Visible in Refraction")
        self.addControl("mtco_noOSL", label="No OSL")
        self.endLayout()

    def buildBum2dTemplate(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        self.beginLayout("Corona" ,collapse=1)
        self.addControl("normalMap", label="Normal Map")
        self.endLayout()

    def buildFileTemplate(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        self.beginLayout("Corona" ,collapse=1)
        self.addControl("fromSRGB", label="Convert sRGB->Linear")
        self.addControl("toSRGB", label="Convert Linear->sRGB")
        self.addSeparator() 
        self.addControl("textureFilterWidth", label="FilterWidth")
        self.addControl("textureFilter", label="FilterType")
        self.addControl("environmentMappingType", label="Environment Mapping")
        self.addControl("textureBlur", label="FilterBlur")
        self.addControl("mtco_noOSL", label="No OSL")
        #self.addControl("textureBlur", label="FilterBlur")
        #self.addControl("textureSblur", label="Filter S Blur")
        #self.addControl("textureTblur", label="Filter T Blur")
        self.endLayout()

    def buildDisplacementShaderTemplate(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        self.beginLayout("Corona" ,collapse=1)
        self.addControl("mtco_displacementMin", label="Displacement Min")
        self.addControl("mtco_displacementMax", label="Displacement Max")
        self.addControl("mtco_displacementAdaptive", label="Adaptive")
        self.endLayout()

    def buildMeshTemplate(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        self.beginLayout("Corona" ,collapse=1)
        self.addControl("mtco_visibleInGI", label="Visible in GI")
        self.addControl("mtco_envPortal", label="Use as Environment Portal")
        self.addControl("mtco_shadowCatcherMode", label="Shadow Catcher Mode")
        self.endLayout()

    def updateCameraTemplate(self, nodeName):
        node = pm.PyNode(nodeName)
        self.dimControl(node, "mtco_iso", True)        
        self.dimControl(node, "mtco_shutterSpeed", True)        
        if node.mtco_overrideRenderSettings.get():
            self.dimControl(node, "mtco_iso", False)        
            self.dimControl(node, "mtco_shutterSpeed", False)        
        
        self.dimControl(node, "mtco_circularBlades", True)        
        self.dimControl(node, "mtco_blades", True)        
        self.dimControl(node, "mtco_bladeRotation", True)        
        self.dimControl(node, "mtco_bokehBitmap", True)      
        
        if node.mtco_useBokeh.get():
            self.dimControl(node, "mtco_circularBlades", False)        
            if not node.mtco_circularBlades.get():
                self.dimControl(node, "mtco_blades", False)        
                self.dimControl(node, "mtco_bladeRotation", False)        
                self.dimControl(node, "mtco_bokehBitmap", False)    
        
    def buildCameraTemplate(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        self.beginLayout("Corona" ,collapse=1)
        self.addControl("mtco_cameraType", label="Projection Type")
        self.addSeparator()
        self.addControl("mtco_overrideRenderSettings", label="Override Render Globals", changeCommand=self.updateCameraTemplate)
        self.addControl("mtco_iso", label="Iso")
        self.addControl("mtco_shutterSpeed", label="Shutter Speed")
        self.addSeparator()
        self.addControl("mtco_useBokeh", label="Use Bokeh", changeCommand=self.updateCameraTemplate)
        self.addSeparator() 
        self.addControl("mtco_circularBlades", label="CircularBlades", changeCommand=self.updateCameraTemplate)
        self.addSeparator() 
        self.addControl("mtco_blades", label="Num Blades")
        self.addControl("mtco_bladeRotation", label="Blade Rotation")
        self.addSeparator() 
        self.addControl("mtco_bokehBitmap", label="Bokeh Bitmap")
        self.endLayout()

    def buildObjectSetTemplate(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        self.beginLayout("Corona" ,collapse=1)
        self.addControl("mtco_mtlOverride", label="Material Override")
        #if self.thisNode.hasAttr
        self.endLayout()
        
    def buildCoronaTemplates(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        if self.thisNode.type() == "camera":
            self.buildCameraTemplate(nodeName)
        if self.thisNode.type() == "areaLight":
            self.buildAreaLightTemplate(nodeName)
        if self.thisNode.type() == "directionalLight":
            self.buildDirLightTemplate(nodeName)
        if self.thisNode.type() == "displacementShader":
            self.buildDisplacementShaderTemplate(nodeName)
        if self.thisNode.type() == "bump2d":
            self.buildBum2dTemplate(nodeName)
        if self.thisNode.type() == "file":
            self.buildFileTemplate(nodeName)
        if self.thisNode.type() == "mesh":
            self.buildMeshTemplate(nodeName)
        if self.thisNode.type() == "objectSet":
            self.buildObjectSetTemplate(nodeName)
            
    def buildBody(self, nodeName):
        self.buildCoronaTemplates(nodeName)

