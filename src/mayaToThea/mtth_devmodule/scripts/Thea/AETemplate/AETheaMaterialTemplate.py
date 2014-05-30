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
        self.beginLayout("ShaderSettings" ,collapse=0)
        self.beginNoOptimize()
        #autoAddBegin
        self.addControl("globalEmitter", label="GlobalEmitter")
        self.addControl("bsdf", label="BSDF")
        self.addControl("emitterAccuracy", label="EmitterAccuracy")
        self.addControl("emitterMinRays", label="EmitterMinRays")
        self.addControl("causticEmitter", label="CausticEmitter")
        self.addControl("twosided", label="Twosided")
        self.addControl("emitterMaxRays", label="EmitterMaxRays")
        self.addControl("dirtLevel", label="DirtLevel")
        self.addControl("tracingDepth", label="TracingDepth")
        self.addControl("passiveEmitter", label="PassiveEmitter")
        self.addControl("repaintable", label="Repaintable")
        self.addControl("blurredMinSubdivs", label="BlurredMinSubdivs")
        self.addControl("perceptualLevel", label="PerceptualLevel")
        self.addControl("ambientEmitter", label="AmbientEmitter")
        self.addControl("blurredMaxSubdivs", label="BlurredMaxSubdivs")
        self.addControl("activeDirt", label="ActiveDirt")
        self.addControl("ambientLevel", label="AmbientLevel")
        self.addControl("shadowCatcher", label="ShadowCatcher")
        self.addSeparator()
        #autoAddEnd
        self.endNoOptimize()
        self.endLayout()
        