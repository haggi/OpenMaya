import pymel.core as pm
import logging

log = logging.getLogger("ui")

class BaseTemplate(pm.ui.AETemplate):
    
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
        
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)


class AEFujiNodeTemplate(BaseTemplate):
    def __init__(self, nodeName):
        BaseTemplate.__init__(self,nodeName)
        self.thisNode = None
        self.node = pm.PyNode(self.nodeName)
        self.buildBody(nodeName)
        log.debug("AEFujiNodeTemplate")
        
    def buildFujiTemplates(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        if self.thisNode.type() == "camera":
            log.debug("AEFujiNodeTemplate:build camera AE")            
            self.beginLayout("Fuji" ,collapse=1)
            self.addControl("someFancyCameraAttribute", label="A very cool Camera Attribute")
            self.endLayout()

        if self.thisNode.type() == "mesh":
            log.debug("AEFujiNodeTemplate:build mesh AE")            
            self.beginLayout("Fuji" ,collapse=1)
            self.addControl("someFancyMeshAttribute", label="A very cool Mesh Attribute")
            self.endLayout()

        if self.thisNode.type() == "areaLight":
            log.debug("AEFujiNodeTemplate:build areaLight AE")            
            self.beginLayout("Fuji" ,collapse=1)
            self.addControl("sample_count", label="Sample Count")
            self.addControl("double_sided", label="Double Sided")
            self.endLayout()
    
    def buildBody(self, nodeName):
        self.buildFujiTemplates(nodeName)

