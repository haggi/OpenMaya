import pymel.core as pm
import logging

log = logging.getLogger("ui")

class BaseTemplate(pm.ui.AETemplate):
    
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
        
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)


class AEIndigoNodeTemplate(BaseTemplate):
    def __init__(self, nodeName):
        BaseTemplate.__init__(self,nodeName)
        self.thisNode = None
        self.node = pm.PyNode(self.nodeName)
        self.buildBody(nodeName)
        log.debug("AEIndigoNodeTemplate")
    
    def updateMeshUI(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        
        self.dimControl(self.thisNode, "mtin_mesh_subdivMaxSubdiv", True)  
        self.dimControl(self.thisNode, "mtin_mesh_subdivSmooth", True)
        self.dimControl(self.thisNode, "mtin_mesh_subdivCurvatureThreshold", True)
        self.dimControl(self.thisNode, "mtin_mesh_subdivErrorThreshold", True)
        self.dimControl(self.thisNode, "mtin_mesh_subdivViewDependent", True)
        self.dimControl(self.thisNode, "mtin_mesh_subdivPixelThreshold", True)
        
        if self.thisNode.mtin_mesh_subdivUse.get():
            self.dimControl(self.thisNode, "mtin_mesh_subdivMaxSubdiv", False)  
            self.dimControl(self.thisNode, "mtin_mesh_subdivSmooth", False)
            self.dimControl(self.thisNode, "mtin_mesh_subdivCurvatureThreshold", False)
            self.dimControl(self.thisNode, "mtin_mesh_subdivErrorThreshold", False)
            self.dimControl(self.thisNode, "mtin_mesh_subdivViewDependent", False)
            
            if self.thisNode.mtin_mesh_subdivViewDependent.get():
                self.dimControl("mtin_mesh_subdivPixelThreshold", False)
            
            
    def buildCameraTemplate(self, nodeName):
        self.beginLayout("Indigo" ,collapse=1)
        self.addControl("mtin_lensRadius", label="Lens Radius")            
        self.endLayout()
    
    def buildMeshTemplate(self, nodeName):
        self.beginLayout("Indigo" ,collapse=1)
        self.addControl("mtin_mesh_subdivUse", label="Create Subdivision Surface", changeCommand=self.updateMeshUI)            
        self.addControl("mtin_mesh_subdivMaxSubdiv", label="Max Subdivisions")            
        self.addControl("mtin_mesh_subdivSmooth", label="Smooth Subdivisions")            
        self.addControl("mtin_mesh_subdivCurvatureThreshold", label="Subdiv Curvature Threshold")            
        self.addControl("mtin_mesh_subdivErrorThreshold", label="Subdiv Error Threshold")            
        self.addControl("mtin_mesh_subdivViewDependent", label="View Dependent Subdivision")            
        self.addControl("mtin_mesh_subdivPixelThreshold", label="Subdiv Pixel Threshold")            
        self.endLayout()
    
    def buildIndigoTemplates(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        if self.thisNode.type() == "camera":
            self.buildCameraTemplate(nodeName)
        if self.thisNode.type() == "mesh":
            self.buildMeshTemplate(nodeName)
    
    def buildBody(self, nodeName):
        self.buildIndigoTemplates(nodeName)

