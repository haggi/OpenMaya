import pymel.core as pm
import logging

log = logging.getLogger("ui")

class BaseTemplate(pm.ui.AETemplate):
    
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
        
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)


class AEmantraNodeTemplate(BaseTemplate):
    def __init__(self, nodeName):
        BaseTemplate.__init__(self,nodeName)
        self.thisNode = None
        self.node = pm.PyNode(self.nodeName)
        self.buildBody(nodeName)
        log.debug("AEmantraNodeTemplate")
        
    def buildMantraTemplates(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        if self.node.type() == 'lambert':
            pm.editorTemplate(beginLayout="Mantra Raytrace Options")
            self.addControl("mtm_refrBlur", "Refraction Blur")
            self.addControl("mtm_refrSamples", "Refraction Samples")
            pm.editorTemplate(endLayout=True)
        if self.node.type() in ['phong', 'phongE', 'blinn']:
            pm.editorTemplate(beginLayout="Mantra Raytrace Options")
            self.addControl("mtm_refrBlur", "Refraction Blur")
            self.addControl("mtm_refrSamples", "Refraction Samples")
            self.addControl("mtm_reflBlur", "Reflection Blur")
            self.addControl("mtm_reflSamples", "Reflection Samples")
            self.addControl("mtm_oren", "Use OrenNayar diffuse")
            self.addControl("mtm_fresnel", "Fresnel Value")
            pm.editorTemplate(endLayout=True)
        if self.node.type() in ['spotLight']:
            pm.editorTemplate(beginLayout="Mantra SpotLight Options")
            self.addControl("mtm_deepShadowMap", "Use Deep Shadow Maps")
            self.addControl("mtm_spread", "Shadow Spread")
            self.addControl("mtm_bias", "Shadow Bias")
            self.addControl("mtm_blur", "Shadow Blur")
            self.addControl("mtm_shadowQuality", "Shadow Quality")
            self.addControl("mtm_samplingQuality", "Sampling Quality")
            self.addControl("mtm_activeRadius", "Active Radius")
            self.addControl("mtm_surfaceShaders", "Use SurfaceShaders")
            self.addControl("mtm_fastShadows", "Use SimpleShadows")
            pm.editorTemplate(endLayout=True)
            
        if self.node.type() in ['pointLight', 'directionalLight']:
            pm.editorTemplate(beginLayout="Mantra Light Options")
            self.addControl("mtm_samplingQuality", "Sampling Quality")
            self.addControl("mtm_activeRadius", "Active Radius")
            self.addControl("mtm_surfaceShaders", "Use SurfaceShaders")
            self.addControl("mtm_fastShadows", "Use SimpleShadows")
            pm.editorTemplate(endLayout=True)

        if self.node.type() in ['areaLight']:
            pm.editorTemplate(beginLayout="Mantra Light Options")
            self.addControl("mtm_samplingQuality", "Sampling Quality")
            self.addControl("mtm_normalizeArea", "Normalize Area")
            self.addControl("mtm_activeRadius", "Active Radius")
            self.addControl("mtm_areaType", "Area Light Shape")
            self.addControl("mtm_useLightGeometry", "Use Geometry")
            self.addControl("mtm_lightGeometry", "Light Geometry")
            self.addControl("mtm_renderLightGeometry", "Render Light Geometry")
            self.addControl("mtm_surfaceShaders", "Use SurfaceShaders")
            self.addControl("mtm_fastShadows", "Use SimpleShadows")
            pm.editorTemplate(endLayout=True)
            
        if self.node.type() == 'mesh':
            pm.editorTemplate(beginLayout="Mantra Mesh Options")
            #self.addControl("mtm_renderAsSubdiv", "Render As Subdiv")
            pm.editorTemplate("mtm_renderAsSubdiv", ac=True, label="Render As Subdiv")
            pm.editorTemplate(endLayout=True)

        if self.node.type() == 'nurbsCurve':
            pm.editorTemplate(beginLayout="Mantra NurbsCurve Options")
            pm.editorTemplate("primaryVisibility", ac=True, label="Primary Visibility")
            pm.editorTemplate(endLayout=True)
    
    def buildBody(self, nodeName):
        self.buildMantraTemplates(nodeName)


