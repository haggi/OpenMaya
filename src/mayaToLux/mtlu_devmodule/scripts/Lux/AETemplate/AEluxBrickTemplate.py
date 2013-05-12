import pymel.core as pm
import logging

log = logging.getLogger("ui")

class BaseTemplate(pm.ui.AETemplate):
    
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
        
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)
        

class AEluxBrickTemplate(BaseTemplate):
    def __init__(self, nodeName):
        BaseTemplate.__init__(self,nodeName)
        log.debug("AEluxBrickTemplate")
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
        self.addControl("scale", label="Scale")
        self.addControl("brickbevel", label="Brick Bevel")
        self.addControl("brickbond", label="Brick pattern")
        self.addControl("mortarsize", label="Mortar Size")
        self.addControl("brickwidth", label="Brick Width")
        self.addControl("bricktex", label="Brick face Color")
        self.addControl("brickdepth", label="Brick Depth")
        self.addControl("coordinates", label="Coordinate System")
        self.addControl("brickmodtex", label="Brick Face Modulation")
        self.addControl("rotate", label="Rotate")
        self.addControl("motartex", label="Mortar Color")
        self.addControl("brickrun", label="Linear Offset")
        self.addControl("translate", label="Offset")
        self.addControl("brickheight", label="Brick Height")
        self.addSeparator()
        #autoAddEnd
        self.endNoOptimize()
        self.endLayout()
        