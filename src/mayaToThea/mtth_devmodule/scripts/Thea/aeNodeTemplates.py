import pymel.core as pm
import logging

log = logging.getLogger("ui")

class BaseTemplate(pm.ui.AETemplate):
    
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
        
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)


class AETheaNodeTemplate(BaseTemplate):
    def __init__(self, nodeName):
        BaseTemplate.__init__(self,nodeName)
        self.thisNode = None
        self.node = pm.PyNode(self.nodeName)
        self.buildBody(nodeName)
        log.debug("AETheaNodeTemplate")

    def updateFileTemplate(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        print "updateFileTemplate node", nodeName, self.thisNode
#         outputs = self.thisNode.outColor.outputs()
#         self.dimControl(nodeName, "mtth_file_iblIntensity", True)
#         self.dimControl(nodeName, "mtth_file_iblRotation", True)
#         self.dimControl(nodeName, "mtth_file_iblWrapping", True)
#         if len(outputs) == 0:
#             return
#         if outputs[0].type() == "theaGlobals":
        self.dimControl(nodeName, "mtth_file_iblIntensity", False)
        self.dimControl(nodeName, "mtth_file_iblRotation", False)
        self.dimControl(nodeName, "mtth_file_iblWrapping", False)
        self.dimControl(nodeName, "mtth_file_interpolation", False)            
        
        
    def buildTheaTemplates(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        if self.thisNode.type() == "camera":
            self.beginLayout("Thea" ,collapse=1)
            self.addControl("mtth_camera_projection", label="Projection Type")
            self.addControl("mtth_diaphragm_blades", label="Num Blades")
            self.addControl("mtth_diaphragm_type", label="Diaphragm Type")
            self.addControl("mtth_shutter_speed", label="Shutter Speed (1/x)sec")
            self.endLayout()
        if self.thisNode.type() == "file":
            self.beginLayout("Thea" ,collapse=1)
            self.addControl("mtth_file_iblIntensity", label="IBL Intensity", changeCommand = self.updateFileTemplate)
            self.addControl("mtth_file_iblRotation", label="IBL Rotation", changeCommand = self.updateFileTemplate)
            self.addControl("mtth_file_iblWrapping", label="IBL Wrapping", changeCommand = self.updateFileTemplate)
            self.addControl("mtth_file_interpolation", label="Interpolation", changeCommand = self.updateFileTemplate)
            self.endLayout()
    
    def buildBody(self, nodeName):
        self.buildTheaTemplates(nodeName)

