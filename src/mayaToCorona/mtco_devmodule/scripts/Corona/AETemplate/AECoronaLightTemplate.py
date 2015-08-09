import pymel.core as pm
import logging

log = logging.getLogger("ui")

class BaseTemplate(pm.ui.AETemplate):
    
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
        
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)
        

class AECoronaLightTemplate(BaseTemplate):
    def __init__(self, nodeName):
        BaseTemplate.__init__(self,nodeName)
        log.debug("AECoronaLightTemplate")
        self.thisNode = None
        self.node = pm.PyNode(self.nodeName)
        pm.mel.AEswatchDisplay(nodeName)
        self.beginScrollLayout()
        self.buildBody(nodeName)
        allAttributes = self.node.listAttr()
        allowedAttributes = ["emissionColor", "emissionColorMultiplier", "opacity", "emitLight", "emissionGlossyness", "doubleSidedEmission", "iesProfile", "emissionSharpnessFake"]
        for att in allAttributes:
            att = att.split(".")[-1]
            if not att in allowedAttributes:
                print "Suppressing", att
                self.suppress(att)
        self.addExtraControls("ExtraControls")
        self.endScrollLayout()
        
    def buildBody(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        self.beginLayout("Emission" ,collapse=0)
        self.beginNoOptimize()
        self.addControl("emissionColor", label="Color")
        self.addControl("emissionColorMultiplier", label="Multiplier")
        self.addControl("opacity", label="Opacity")
        self.addControl("emitLight", label="Emit Light")
        self.addControl("emissionGlossyness", label="Directionality")
        self.addControl("doubleSidedEmission", label="Double Sided")
        self.addSeparator()
        self.addControl("iesProfile", label="IES Profile")
        self.addControl("emissionSharpnessFake", label="Sharp Patterns")
        #self.addControl("emissionDisableSampling", label="Disable Sampling")
        #self.addControl("emissionSharpnessFakePoint", label="Sharpness Fake Point")
        self.endNoOptimize()
        self.endLayout()
        
        #self.beginLayout("Hardware Texturing" ,collapse=0)
        #pm.mel.eval('AEhardwareTextureTemplate "%s"' % self.nodeName + r'("diffuse emissionColor ")')
        #self.endLayout()
        