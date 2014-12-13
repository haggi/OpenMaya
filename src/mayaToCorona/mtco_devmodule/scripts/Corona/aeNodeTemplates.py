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
        
    def buildDirLightTemplate(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        self.beginLayout("Corona" ,collapse=1)
        self.addControl("mtco_sun_multiplier", label="Sun Intensity Multiplier")
        self.endLayout()
        
    def buildAreaLightTemplate(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        self.beginLayout("Corona" ,collapse=1)
        self.addControl("mtco_envPortal", label="Use as Environment Portal")
        self.addControl("mtco_areaVisible", label="Visible Geometry")
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
        self.addControl("textureFilter", label="FilterType")
        self.addControl("textureFilterWidth", label="FilterWidth")
        #self.addControl("textureBlur", label="FilterBlur")
        #self.addControl("textureSblur", label="Filter S Blur")
        #self.addControl("textureTblur", label="Filter T Blur")
        self.endLayout()

    def buildDisplacementShaderTemplate(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        self.beginLayout("Corona" ,collapse=1)
        self.addControl("mtco_displacementMin", label="Displacement Min")
        self.addControl("mtco_displacementMax", label="Displacement Max")
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
            
    def buildBody(self, nodeName):
        self.buildCoronaTemplates(nodeName)

