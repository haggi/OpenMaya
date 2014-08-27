import pymel.core as pm
import logging

log = logging.getLogger("ui")

class BaseTemplate(pm.ui.AETemplate):
    
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
        
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)
        

class AETheaMaterialTemplate(BaseTemplate):
    def __init__(self, nodeName):
        BaseTemplate.__init__(self,nodeName)
        log.debug("AETheaMaterialTemplate")
        self.thisNode = None
        self.node = pm.PyNode(self.nodeName)
        pm.mel.AEswatchDisplay(nodeName)
        self.beginScrollLayout()
        self.buildBody(nodeName)
        self.addExtraControls("ExtraControls")
        self.endScrollLayout()
        
    def buildBody(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        self.beginLayout("Base Layer" ,collapse=0)
        self.addControl("bsdf", label="BSDF")
        self.endLayout()
        self.beginLayout("Emitter Settings" ,collapse=1)
        self.addControl("emitter", label="Emitter")
        self.addControl("emitterMinRays", label="EmitterMinRays")
        self.addControl("emitterMaxRays", label="EmitterMaxRays")
        self.addControl("emitterAccuracy", label="EmitterAccuracy")
        self.addControl("ambientEmitter", label="AmbientEmitter")
        self.addControl("passiveEmitter", label="PassiveEmitter")
        self.endLayout()
        self.beginLayout("Layers" ,collapse=0)
        self.endLayout()
        self.beginLayout("Material General Settings" ,collapse=0)
        self.beginNoOptimize()
        self.addControl("twosided", label="Twosided")
        self.addControl("shadowCatcher", label="ShadowCatcher")
        self.addControl("activeDirt", label="ActiveDirt")
        self.endNoOptimize()
        self.addControl("perceptualLevel", label="PerceptualLevel")
        self.addControl("ambientLevel", label="AmbientLevel")
        self.addControl("tracingDepth", label="TracingDepth")
        self.addSeparator()
        self.endLayout()
        