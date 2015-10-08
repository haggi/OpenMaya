import pymel.core as pm
import logging

log = logging.getLogger("ui")

class BaseTemplate(pm.ui.AETemplate):
    
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
        
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)
        

class AECoronaSkyTemplate(BaseTemplate):
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
    
    def updateUI(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        self.dimControl(self.thisNode, "pSkyPreethamTurb", True)
        self.dimControl(self.thisNode, "pSkyZenith", True)
        self.dimControl(self.thisNode, "pSkyHorizon", True)
        self.dimControl(self.thisNode, "pSkySunGlow", True)
        self.dimControl(self.thisNode, "pSkySunSideGlow", True)
        self.dimControl(self.thisNode, "pSkySunBleed", True)
        self.dimControl(self.thisNode, "pSkySunFalloff", True)
        if (self.thisNode.pSkyModel.get() == 0) or (self.thisNode.pSkyModel.get() == 2): #preetham or hosek
            self.dimControl(self.thisNode, "pSkyPreethamTurb", False)
        if self.thisNode.pSkyModel.get() == 1: 
            self.dimControl(self.thisNode, "pSkyZenith", False)
            self.dimControl(self.thisNode, "pSkyHorizon", False)
            self.dimControl(self.thisNode, "pSkySunGlow", False)
            self.dimControl(self.thisNode, "pSkySunSideGlow", False)
            self.dimControl(self.thisNode, "pSkySunBleed", False)
            self.dimControl(self.thisNode, "pSkySunFalloff", False)
            
    def buildBody(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        self.beginLayout("Sky" ,collapse=0)
        self.beginNoOptimize()
        self.addControl("pSkyModel", label="Model", changeCommand=self.updateUI)
        self.addSeparator()
        self.addControl("pSkyMultiplier", label="Multiplier")
        self.addControl("pSkyHorizBlur", label="Horizon Blur")
        self.addControl("pSkyGroundColor", label="Ground Color")
        self.addControl("pSkyAffectGround", label="Affect Ground")
        self.addControl("sunSizeMulti", label="Sun Size Multiplier")
        self.beginLayout("Rawafake" ,collapse=0)
        self.addControl("pSkySunFalloff", label="Sun Falloff")
        self.addControl("pSkyZenith", label="Zenit Color")
        self.addControl("pSkyHorizon", label="Horizon Color")
        self.addControl("pSkySunGlow", label="Sun Glow")
        self.addControl("pSkySunSideGlow", label="Sun Side Glow")
        self.addControl("pSkySunBleed", label="Sun Bleed")
        self.endLayout()
        self.beginLayout("Preetham/Hosek" ,collapse=0)
        self.addControl("pSkyPreethamTurb", label="Turbidity")
        self.endLayout()
                
        self.endNoOptimize()
        self.endLayout()
        
        #self.beginLayout("Hardware Texturing" ,collapse=0)
        #pm.mel.eval('AEhardwareTextureTemplate "%s"' % self.nodeName + r'("diffuse emissionColor ")')
        #self.endLayout()
        