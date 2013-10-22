import pymel.core as pm
import logging

log = logging.getLogger("ui")

class BaseTemplate(pm.ui.AETemplate):
    
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
        
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)
        

class AEasSurface_shader_collectionTemplate(BaseTemplate):
    def __init__(self, nodeName):
        BaseTemplate.__init__(self,nodeName)
        log.debug("AEasSurface_shader_collectionTemplate")
        self.thisNode = None
        self.node = pm.PyNode(self.nodeName)
        pm.mel.AEswatchDisplay(nodeName)
        self.beginScrollLayout()
        self.buildBody(nodeName)
        self.addExtraControls("ExtraControls")
        self.endScrollLayout()
        
    def buildBody(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        self.beginLayout("ShaderSettings" ,collapse=0)
        self.beginNoOptimize()
        #autoAddBegin
        self.addControl("surface_shader2", label="Surface Shader 2")
        self.addControl("surface_shader3", label="Surface Shader 3")
        self.addControl("surface_shader1", label="Surface Shader 1")
        self.addControl("surface_shader6", label="Surface Shader 6")
        self.addControl("surface_shader7", label="Surface Shader 7")
        self.addControl("surface_shader4", label="Surface Shader 4")
        self.addControl("surface_shader5", label="Surface Shader 5")
        self.addControl("surface_shader8", label="Surface Shader 8")
        self.addSeparator()
        #autoAddEnd
        self.endNoOptimize()
        self.endLayout()
        